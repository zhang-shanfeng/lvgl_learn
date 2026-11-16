#include "nvs_float.h"

#include <string.h>

// 保存 float
esp_err_t save_float(nvs_handle_t handle, const char *key, float value)
{
    int32_t raw;
    memcpy(&raw, &value, sizeof(raw));
    return nvs_set_i32(handle, key, raw);
}

// 读取 float
esp_err_t load_float(nvs_handle_t handle, const char *key, float *out_value)
{
    if (out_value == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    int32_t raw;
    esp_err_t err = nvs_get_i32(handle, key, &raw);
    if (err == ESP_OK)
    {
        memcpy(out_value, &raw, sizeof(*out_value));
    }
    return err;  // 键不存在时返回 ESP_ERR_NVS_NOT_FOUND，由调用者处理默认值
}