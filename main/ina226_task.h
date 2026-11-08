#pragma once

#include "ina226.h"

struct ina226_value_t
{
    float voltage;
    float current;
    float power;
};

void ina226_task(void *pvParameters);
void current_alert_value_set(ina226_device_t *device,
                             const float alert_current_a);
void voltage_alert_value_set(ina226_device_t *device, const float alert_value);

extern ina226_device_t ina;