#pragma once

#include "nvs.h"
#include "nvs_flash.h"

esp_err_t save_float(nvs_handle_t handle, const char *key, float value);
esp_err_t load_float(nvs_handle_t handle, const char *key, float *out_value);