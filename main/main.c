/*
 * button Demo
 *
 * `idf.py add-dependency "espressif/button^4.2.1"`
 */
#include "button_task.h"
#include "buzzer.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ina226_task.h"
#include "lvgl_app.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "utility_c.h"

static const char *TAG = "gpio_button_example";

void app_main(void)
{
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
    printf("Internal free: %d, largest block: %d\n", info.total_free_bytes, info.largest_free_block);

    heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
    printf("PSRAM free: %d, largest block: %d\n", info.total_free_bytes, info.largest_free_block);

    esp_err_t err;

    // === 步骤1：初始化 NVS ===
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // nvs 分区已满或版本不兼容，擦除后重试
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    bool buzzer_enabled = true;
    err = buzzer_nvs_load(&buzzer_enabled);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "load buzzer nvs failed: %s", esp_err_to_name(err));
        /* 可以用默认值继续跑 */
    }

    buzzer_set_enabled(buzzer_enabled);

    buzzer_init();

    ina226_task_init();

    button_task_init();

    xTaskCreatePinnedToCore(ina226_task, "ina226_task", 4096, NULL, 0, NULL, 1);

    lvgl_app_init();

    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
    printf("Internal free: %d, largest block: %d\n", info.total_free_bytes, info.largest_free_block);

    heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
    printf("PSRAM free: %d, largest block: %d\n", info.total_free_bytes, info.largest_free_block);

    while (1)
    {
#ifdef CONFIG_ENABLE_FixedString_TEST_FUNC
        test();
#endif

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
