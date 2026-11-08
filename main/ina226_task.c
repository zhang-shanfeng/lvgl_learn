/**************************************************************************************************
 * @file ina226_task.c
 * @author freerealmshn@gmail.com (xiaotupo@163.com)
 * @brief
 * @version V0.1.0
 * @date 2026-10-16
 * r_shunt 决定物理上能测多大压降（INA226 分流电压量程是 ±81.92mV）
 * max_current 决定软件上校准的量程
 * 按 max_current = 81.92mV / r_shunt 算，比如 r_shunt =0.01Ω，理论上限约 8.19A。
 * 你设的 max_current 不要超过这个值，
 * 否则 INA226 的分流电压会先饱和，测出来不准。
 * @copyright Copyright (c) 2025 by zhang-shanfeng, All Rights Reserved.
 * @website https://zhang-shanfeng.github.io
 *
 *************************************************************************************************/
#include "ina226_task.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "page_ina226.h"
#include "sdkconfig.h"

static const char *TAG = "ina226";

#define INA226_I2C_SDA GPIO_NUM_16
#define INA226_I2C_SCL GPIO_NUM_17

/* A0,A1 引脚接到 3.3V */
#define INA226_I2C_ADDR 0x40
ina226_device_t ina;

static const i2c_master_bus_config_t i2c_bus_config = {
    .i2c_port = I2C_NUM_0,
    .scl_io_num = INA226_I2C_SCL,
    .sda_io_num = INA226_I2C_SDA,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = false,
};

static const i2c_device_config_t ina226_i2c_config = {
    .dev_addr_length = I2C_ADDR_BIT_7,
    .device_address = INA226_I2C_ADDR,
    .scl_speed_hz = 400000,
};

static ina226_config_t ina226_config = {
    .timeout_ms = 100,
    .averages = INA226_AVERAGES_16,
    .bus_conv_time = INA226_BUS_CONV_TIME_1100_US,
    .shunt_conv_time = INA226_SHUNT_CONV_TIME_1100_US,
    .mode = INA226_MODE_SHUNT_BUS_CONT,
    .r_shunt = 0.01f,
    .max_current = 8.129f,
};

void ina226_task(void *pvParameters)
{
    i2c_master_bus_handle_t i2c_bus;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(
        i2c_bus, &ina226_i2c_config, &ina226_config.i2c_dev));

    /* Setup INA226 device. */
    // ESP_ERROR_CHECK(ina226_init(&ina, &ina226_config));

    ESP_LOGI(TAG, "Scanning I2C bus...");
    for (uint8_t addr = 0x03; addr < 0x78; addr++)
    {
        esp_err_t ret = i2c_master_probe(i2c_bus, addr, 100);
        if (ret == ESP_OK) { ESP_LOGI(TAG, "Found device at 0x%02X", addr); }
    }
    ESP_LOGI(TAG, "Scan done");

    esp_err_t err = ina226_init(&ina, &ina226_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "INA226 init failed: %s", esp_err_to_name(err));
        vTaskDelete(NULL);
        return;
    }

    uint16_t manufacturer_id;
    uint16_t die_id;
    ina226_get_manufacturer_id(&ina, &manufacturer_id);
    ina226_get_die_id(&ina, &die_id);
    ESP_LOGI(TAG, "厂商ID:%d, DIE ID:%d", manufacturer_id, die_id);

    // current_alert_value_set(&ina, 0.015);

    struct ina226_value_t ina226_values;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(200));

        // 读取总线电压
        err = ina226_get_bus_voltage(&ina, &ina226_values.voltage);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Read voltage failed");
            continue;
        }

        // 读取电流
        err = ina226_get_current(&ina, &ina226_values.current);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Read current failed");
            continue;
        }

        // 读取功率（注意：INA226 内部会自动算功率，不需要你手动用 V*I）
        // err = ina226_get_power(&ina, &ina226_values.power);
        // if (err != ESP_OK)
        // {
        //     ESP_LOGE(TAG, "Read power failed");
        //     continue;
        // }
        ina226_values.power = ina226_values.voltage * ina226_values.current;

        if (ina226_values.voltage > 1.0f && ina226_values.current == 0.0f &&
            ina226_values.power == 0.0f)
        {
            // 连续几次都这样，就重新初始化
            ina226_init(&ina, &ina226_config);
        }

        // 打印结果
        ESP_LOGI(TAG,
                 "电压: %.3f V | 电流: %.3f A | 功率: %.3f W",
                 ina226_values.voltage,
                 ina226_values.current,
                 ina226_values.power);

        page_ina226_update_number(ina226_values);
    }
}

void voltage_alert_value_set(ina226_device_t *device, const float alert_value)
{
    ina226_set_alert_mask(device, INA226_ALERT_BUS_OVER_VOLTAGE);
    ina226_set_bus_voltage_alert_limit(device, alert_value);
}

void current_alert_value_set(ina226_device_t *device,
                             const float alert_current_a)
{
    // INA226 没有独立的电流超限报警位，电流报警复用分流电压超限(SOL)
    ina226_set_alert_mask(device, INA226_ALERT_SHUNT_OVER_VOLTAGE);

    // 将电流阈值转换为分流电压阈值：V_shunt = I * R_shunt
    // 假设 device->r_shunt_ohms 已在初始化时设置
    float alert_shunt_voltage_v = alert_current_a * device->config->r_shunt;

    // 分流电压寄存器的 LSB 为 2.5uV (0.0000025V)
    // 计算写入 Alert Limit Register 的原始值
    uint16_t alert_limit_raw = (uint16_t)(alert_shunt_voltage_v / 0.0000025f);

    ina226_set_alert_limit_raw(device, alert_limit_raw);
}

void power_alert_value_set(ina226_device_t *device, const float alert_power_w)
{
    // 使能功率超限报警 (POL, Bit 11)
    ina226_set_alert_mask(device, INA226_ALERT_POWER_OVER_LIMIT);

    // 功率寄存器的 LSB 取决于初始化时设置的 CURRENT_LSB
    // 标准关系：Power_LSB = 25 * CURRENT_LSB
    // 假设 device->current_lsb 已在 ina226_init 中根据校准值计算并保存
    float power_lsb = 25.0f * device->current_lsb;

    uint16_t alert_limit_raw = (uint16_t)(alert_power_w / power_lsb);

    ina226_set_alert_limit_raw(device, alert_limit_raw);
}
