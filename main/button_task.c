/**************************************************************************************************
 * @file button_task.c
 * @author freerealmshn@gmail.com
 * @brief
 * @version V0.1.0
 * @date 2026-10-07
 *
 * @copyright Copyright (c) 2025 by zhang-shanfeng, All Rights Reserved.
 * @website https://zhang-shanfeng.github.io
 *
 *************************************************************************************************/
#include "button_task.h"

#include "button_adc.h"
#include "buzzer.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "iot_button.h"
#include "lvgl_app.h"
#include "ui.h"

static const char* TAG = "button_task";
uint32_t g_current_lv_key = 0;
SemaphoreHandle_t g_key_mutex = NULL;

struct adc_button_range_t
{
    uint16_t min_mv;
    uint16_t max_mv;
};

static const struct adc_button_range_t button_ranges[] = {
    {.min_mv = 500, .max_mv = 650},   /* 2.2K */
    {.min_mv = 720, .max_mv = 820},   /* 3.3K */
    {.min_mv = 980, .max_mv = 1180},  /* 4.7K */
    {.min_mv = 1236, .max_mv = 1436}, /* 6.8K */
    {.min_mv = 1550, .max_mv = 1750}, /* 10K */
    {.min_mv = 1880, .max_mv = 2080}, /* 15K */
    {.min_mv = 2176, .max_mv = 2376}, /* 22K */
    {.min_mv = 2440, .max_mv = 2640}, /* 33K */
};

static void button_event_cb(void* arg, void* usr_data)
{
    int btn_id = (int)(intptr_t)usr_data;
    button_event_t event = iot_button_get_event((button_handle_t)arg);
    uint32_t lv_key = 0;

    /* ---- 全局键：打开功能列表，不写入 g_current_lv_key ---- */
    if (btn_id == 6 && !is_edited_mode)
    {
        if (event == BUTTON_PRESS_DOWN)
        {
            ui_show_list_page(); /* 内部调用 show_page(PAGE_LIST) */
        }
        return;
    }

    switch (btn_id)
    {
        case 1:
            lv_key = is_edited_mode ? LV_KEY_UP : LV_KEY_PREV;
            break;
        case 2:
            lv_key = is_edited_mode ? LV_KEY_DOWN : LV_KEY_NEXT;
            break;
        case 3:
            lv_key = LV_KEY_ENTER;
            break;
        case 4:
            lv_key = LV_KEY_LEFT;
            break;  // 数值增加
        case 5:
            lv_key = LV_KEY_RIGHT;
            break;  // 数值减少
        case 6:
            lv_key = LV_KEY_ESC;
            break;  // 光标左移（选择高位）
        // case 7:
        //     lv_key = LV_KEY_RIGHT;
        //     break;  // 光标右移（选择低位）
        default:
            break;
    }

    if (event == BUTTON_PRESS_DOWN)
    {
        xSemaphoreTake(g_key_mutex, portMAX_DELAY);
        g_current_lv_key = lv_key;
        // buzzer_start_with_timeout(1000 * 50);
        xSemaphoreGive(g_key_mutex);
    }
    else if (event == BUTTON_PRESS_UP || event == BUTTON_PRESS_END)
    {
        xSemaphoreTake(g_key_mutex, portMAX_DELAY);
        if (g_current_lv_key == lv_key)
        {
            g_current_lv_key = 0;
        }
        xSemaphoreGive(g_key_mutex);
    }
}

/**************************************************************************************************
 * @brief 创建单个 ADC 按键
 *
 * @param index
 * @param min_mv
 * @param max_mv
 * @return button_handle_t
 * @date 2026-10-07
 *************************************************************************************************/
static button_handle_t create_adc_button(uint8_t index, uint16_t min_mv, uint16_t max_mv)
{
    // 1. 基础配置，通常留空或设置通用参数
    const button_config_t btn_cfg = {
        .long_press_time = 1500,  // 可选：自定义长按阈值，0 表示使用默认
        .short_press_time = 135,
    };

    // 2. ADC 专属配置
    button_adc_config_t adc_cfg = {
        .unit_id = ADC_UNIT_1,         // ADC1
        .adc_channel = ADC_CHANNEL_4,  // 假设使用 GPIO5
        .button_index = index,         // 按键在通道上的索引，需唯一
        .min = min_mv,                 // 电压下限 (mV)
        .max = max_mv,                 // 电压上限 (mV)
    };

    // 3. 使用 v4.x 专用 API 创建
    button_handle_t btn = NULL;
    esp_err_t ret = iot_button_new_adc_device(&btn_cfg, &adc_cfg, &btn);
    if (ret != ESP_OK || btn == NULL)
    {
        ESP_LOGE(TAG, "Failed to create ADC button %d", index);
        return NULL;
    }

    return btn;
}

/**************************************************************************************************
 * @brief 按键初始化函数，初始化完成后直接交由 button 组件的事件系统自行运行
 *
 * @date 2026-10-07
 *************************************************************************************************/
void button_task_init(void)
{
    /* 创建互斥量 */
    g_key_mutex = xSemaphoreCreateMutex();

    for (int i = 0; i < 8; i++)
    {
        button_handle_t btn = create_adc_button(i, button_ranges[i].min_mv, button_ranges[i].max_mv);

        if (btn && i < 7)
        {
            iot_button_register_cb(btn, BUTTON_PRESS_DOWN, NULL, button_event_cb, (void*)(intptr_t)(i + 1));
            iot_button_register_cb(btn, BUTTON_PRESS_UP, NULL, button_event_cb, (void*)(intptr_t)(i + 1));
            iot_button_register_cb(btn, BUTTON_PRESS_END, NULL, button_event_cb, (void*)(intptr_t)(i + 1));

            iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, button_event_cb, (void*)(intptr_t)(i + 1));
        }
        if (btn && i == 7)
        {
            iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, button_event_cb, (void*)(intptr_t)(i + 1));
            iot_button_register_cb(btn, BUTTON_PRESS_DOWN, NULL, button_event_cb, (void*)(intptr_t)(i + 1));
        }
    }
}

/**************************************************************************************************
 * @brief 给 LVGL 用的函数
 *
 * @param indev
 * @param data
 * @date 2026-10-30
 *************************************************************************************************/
void keypad_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
    xSemaphoreTake(g_key_mutex, portMAX_DELAY);
    if (g_current_lv_key != 0)
    {
        data->key = g_current_lv_key;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    xSemaphoreGive(g_key_mutex);
}