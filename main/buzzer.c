#include "buzzer.h"

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "nvs_flash.h"

#define BUZZER_IO 14

#define BUZZER_NVS_NAMESPACE "buzzer"
#define BUZZER_NVS_KEY_ENABLE "enabled"

static const char* TAG = "buzzer";

/* 内部状态 */
static volatile bool s_enabled = true; /* 蜂鸣器总开关，默认开启 */

/* ---- 创建 esp_timer 句柄 ----*/
static esp_timer_handle_t s_off_timer = NULL;   /* 用于单次蜂鸣器响声的 ESP 定时器句柄  */
static esp_timer_handle_t s_alarm_timer = NULL; /* 用于循环报警的 ESP 定时器句柄  */

static volatile bool s_alarm_active = false;   /* 是否处于报警状态 */
static volatile bool s_alarm_phase_on = false; /* true=当处于响相位 */
static uint32_t s_alarm_on_ms = 100;
static uint32_t s_alarm_off_ms = 100;

static StaticSemaphore_t s_lock_buf;
static SemaphoreHandle_t s_lock = NULL; /* 互斥量句柄 */

static void buzzer_start(void);
static void buzzer_stop(void);
static void stop_alarm_internal(void);
static esp_err_t start_with_timeout_internal(uint64_t duration_us);

/**************************************************************************************************
 * @brief 获取锁
 *
 * @date 2026-10-31
 *************************************************************************************************/
static inline void buzzer_lock(void)
{
    if (s_lock)
    {
        xSemaphoreTake(s_lock, portMAX_DELAY);
    }
}

/**************************************************************************************************
 * @brief 释放锁
 *
 * @date 2026-10-31
 *************************************************************************************************/
static inline void buzzer_unlock(void)
{
    if (s_lock)
    {
        xSemaphoreGive(s_lock);
    }
}

/**************************************************************************************************
 * @brief NVS 加载函数
 *
 * @param enabled
 * @return esp_err_t
 * @date 2026-10-31
 *************************************************************************************************/
static esp_err_t buzzer_nvs_load(bool* enabled)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(BUZZER_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return err;

    uint8_t val = 1; /* 默认开启 */
    err = nvs_get_u8(handle, BUZZER_NVS_KEY_ENABLE, &val);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        /* 首次上电，尚未保存过，保持默认 true 并写入初始值 */
        nvs_set_u8(handle, BUZZER_NVS_KEY_ENABLE, val);
        nvs_commit(handle);
        err = ESP_OK;
    }
    nvs_close(handle);

    if (err == ESP_OK)
    {
        *enabled = (val != 0);
    }
    return err;
}

/**************************************************************************************************
 * @brief NVS 报错函数
 *
 * @param enabled
 * @return esp_err_t
 * @date 2026-10-31
 *************************************************************************************************/
static esp_err_t buzzer_nvs_save(bool enabled)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(BUZZER_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_u8(handle, BUZZER_NVS_KEY_ENABLE, enabled ? 1 : 0);
    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

/**************************************************************************************************
 * @brief 定时器回调函数
 *
 * @param arg
 * @date 2026-10-31
 *************************************************************************************************/
static void off_timer_cb(void* arg)
{
    /* 只需要关闭蜂鸣器 */
    buzzer_stop();
}

/**************************************************************************************************
 * @brief 报警定时器回调函数
 *
 * @param arg
 * @date 2026-10-31
 *************************************************************************************************/
static void alarm_timer_cb(void* arg)
{
    /* s_alarm_active 为 false 时，关闭蜂鸣器 */
    if (!s_alarm_active)
    {
        buzzer_stop();
        return;
    }

    if (s_alarm_phase_on)
    {
        /* 响 -> 停 */
        buzzer_stop();
        s_alarm_phase_on = false;
        esp_timer_start_once(s_alarm_timer, (uint64_t)s_alarm_off_ms * 1000ULL);
    }
    else
    {
        /* 停 -> 响 */
        buzzer_start();
        s_alarm_phase_on = true;
        esp_timer_start_once(s_alarm_timer, (uint64_t)s_alarm_on_ms * 1000ULL);
    }
}

/**************************************************************************************************
 * @brief 开启超时定时，内部函数
 * 这个函数实现的就是打开蜂鸣器响，并开启一次性定时器，定时器定时时间到了后执行 `off_timer_cb` 回调函数来关闭蜂鸣器
 * @param duration_us
 * @return esp_err_t
 * @date 2026-10-31
 *************************************************************************************************/
static esp_err_t start_with_timeout_internal(uint64_t duration_us)
{
    if (duration_us == 0)
        return ESP_ERR_INVALID_ARG;
    if (!s_off_timer)
        return ESP_ERR_INVALID_STATE;

    stop_alarm_internal(); /* 提示音打断报警 */

    if (esp_timer_is_active(s_off_timer))
    {
        esp_timer_stop(s_off_timer); /* 重新计时 */
    }

    buzzer_start();
    esp_err_t err = esp_timer_start_once(s_off_timer, duration_us);
    if (err != ESP_OK) /* 如果创建失败则关闭蜂鸣器 */
    {
        buzzer_stop();
    }
    return err;
}

/**************************************************************************************************
 * @brief buzzer IO
 *  配置、创建互斥量（互斥锁）、创建两个定时器：s_off_timer、s_alarm_timer
 *
 * @return esp_err_t **ESP_OK** : 初始化成功
 * @date 2026-10-31
 *************************************************************************************************/
esp_err_t buzzer_init(void)
{
    /* IO 口配置 */
    gpio_config_t buzzer_config = {
        .pin_bit_mask = 1ULL << BUZZER_IO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&buzzer_config);
    gpio_set_level(BUZZER_IO, 0);

    if (s_lock)
        return ESP_OK; /* 互斥锁已经初始化 */

    /* 创建静态互斥信号量 */
    s_lock = xSemaphoreCreateMutexStatic(&s_lock_buf);
    if (!s_lock)
        return ESP_ERR_NO_MEM;

    const esp_timer_create_args_t off_args = {
        .callback = &off_timer_cb,
        .name = "buzzer_off",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&off_args, &s_off_timer), TAG, "create off timer failed");

    const esp_timer_create_args_t alarm_args = {
        .callback = &alarm_timer_cb,
        .name = "buzzer_alarm",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&alarm_args, &s_alarm_timer), TAG, "create alarm timer failed");
    return ESP_OK;
}

/**************************************************************************************************
 * @brief 开启蜂鸣器
 *
 * @note 内部调用 gpio_set_level
 * @date 2026-11-12
 *************************************************************************************************/
static void buzzer_start(void)
{
    gpio_set_level(BUZZER_IO, 1);
}

/**************************************************************************************************
 * @brief 关闭蜂鸣器
 *
 * @note 内部调用 gpio_set_level
 * @date 2026-11-12
 *************************************************************************************************/
static void buzzer_stop(void)
{
    gpio_set_level(BUZZER_IO, 0);
}

/**************************************************************************************************
 * @brief 设置总开关的状态
 *
 * @note 用于控制是否使能 Buzzer 功能，禁能后将全面关闭蜂鸣器
 * @param enabled true: 开启, false: 关闭
 * @date 2026-10-31
 *************************************************************************************************/
void buzzer_set_enabled(bool enabled)
{
    if (!enabled)
    {
        buzzer_stop_all(); /* 确保关闭蜂鸣器 */
    }
    s_enabled = enabled;
}

/**************************************************************************************************
 * @brief 返回总开关的状态
 *
 * @note 直接返回全局的 s_enabled
 * @return true
 * @return false
 * @date 2026-10-31
 *************************************************************************************************/
bool buzzer_is_enabled(void)
{
    return s_enabled;
}

/**************************************************************************************************
 * @brief 启动单次蜂鸣器发声
 *
 * @note 加入了全局总开关变量 s_enabled 的使用，只有总开关为开时才能调用 start_with_timeout_internal() 函数
 * @param duration_us
 * @return
 * - ESP_OK 代表启动成功
 * @date 2026-10-31
 *************************************************************************************************/
esp_err_t buzzer_start_with_timeout(uint64_t duration_us)
{
    if (!s_enabled)
        return ESP_OK; /* 总开关关闭，静默忽略 */
    if (!s_lock)
        return ESP_ERR_INVALID_STATE; /* 未 init */

    buzzer_lock();
    esp_err_t err = start_with_timeout_internal(duration_us);
    buzzer_unlock();

    return err;
}

/**************************************************************************************************
 * @brief 关闭报警定时器，停止报警
 *
 * @date 2026-10-31
 *************************************************************************************************/
static void stop_alarm_internal(void)
{
    s_alarm_active = false; /* 先置标志 */
    s_alarm_phase_on = false;

    /* 应对“回调恰好在 stop 前后重启了自己”的极小窗口 */
    if (s_alarm_timer && esp_timer_is_active(s_alarm_timer))
    {
        esp_timer_stop(s_alarm_timer);
    }
    buzzer_stop();
}

/**************************************************************************************************
 * @brief 开始报警
 *
 * @note
 * @param on_ms
 * @param off_ms
 * @return
 *  - ESP_OK 代表 s_alarm_timer 定时器启动成功
 * @date 2026-10-31
 *************************************************************************************************/
esp_err_t buzzer_start_alarm(uint32_t on_ms, uint32_t off_ms)
{
    if (!s_enabled)
        return ESP_OK; /* 总开关关闭：静默忽略 */
    if (!s_lock || !s_alarm_timer)
        return ESP_ERR_INVALID_STATE;
    if (on_ms == 0 || off_ms == 0)
        return ESP_ERR_INVALID_ARG;

    buzzer_lock();

    /* 先停掉旧的报警和单次提示音，保证从“响”相位重新开始 */
    stop_alarm_internal();
    /* 如果单次提示音定时器处于有效工作状态，则停止 s_off_timer */
    if (esp_timer_is_active(s_off_timer))
    {
        esp_timer_stop(s_off_timer);
    }

    s_alarm_on_ms = on_ms;
    s_alarm_off_ms = off_ms;
    s_alarm_phase_on = true;
    s_alarm_active = true;

    /* ---- 开启蜂鸣器，并开启 s_alarm_timer 定时器（单次模式） ---- */
    buzzer_start();
    esp_err_t err = esp_timer_start_once(s_alarm_timer, (uint64_t)on_ms * 1000ULL);
    if (err != ESP_OK)
    {
        s_alarm_active = false;
        buzzer_stop();
    }

    buzzer_unlock();
    return err;
}

/**************************************************************************************************
 * @brief 关掉报警
 *
 * @date 2026-10-31
 *************************************************************************************************/
void buzzer_stop_alarm(void)
{
    if (!s_lock)
        return;
    buzzer_lock();
    stop_alarm_internal();
    buzzer_unlock();
}

/**************************************************************************************************
 * @brief 关掉所有：报警与单次响声
 *
 * @date 2026-10-31
 *************************************************************************************************/
void buzzer_stop_all(void)
{
    /* 未 init 时至少把硬件关掉 */
    if (!s_lock)
    {
        buzzer_stop();
        return;
    }
    buzzer_lock();
    stop_alarm_internal();
    if (s_off_timer && esp_timer_is_active(s_off_timer))
    {
        esp_timer_stop(s_off_timer);
    }
    buzzer_stop();
    buzzer_unlock();
}

/**************************************************************************************************
 * @brief 按任意键关掉报警
 *
 * @date 2026-10-31
 *************************************************************************************************/
void buzzer_on_any_key(void)
{
    buzzer_stop_alarm();
}
