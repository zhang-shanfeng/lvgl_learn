#pragma once

#include "ina226.h"

struct ina226_value_t
{
    float voltage;
    float current;
    float power;
    float max_alarm_current;
};

void ina226_task_init(void);
void ina226_task(void *pvParameters);
void ina226_task_set_current_alert_value(ina226_device_t *device, const float alert_current_a);
void ina226_task_set_voltage_alert_value(ina226_device_t *device, const float alert_value);
void ina226_task_set_power_alert_value(ina226_device_t *device, const float alert_power_w);
void ina226_task_set_max_alarm_current(float alarm_current);
void ina226_task_set_max_current(float current_value);
float ina226_task_get_max_current(void);
esp_err_t ina226_task_nvs_save(const float max_current);
esp_err_t ina226_task_nvs_load(float *max_current);

extern ina226_device_t ina;
extern struct ina226_value_t ina226_values;