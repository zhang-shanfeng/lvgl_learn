#include "utility.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "utility_c.h"

using zsf::utility::FixedString;

static const char *TAG = "utility";

#ifdef CONFIG_ENABLE_FixedString_TEST_FUNC
extern "C" void test(void)
{
    auto name = FixedString<20>("button");

    ESP_LOGI(TAG, "name: %s, len: %d", name.c_str(), name.size());
    vTaskDelay(pdMS_TO_TICKS(500));
    name.set("new str");
    ESP_LOGI(TAG, "name: %s, len: %d", name.c_str(), name.size());
    vTaskDelay(pdMS_TO_TICKS(500));
    name.append("append");
    ESP_LOGI(TAG, "name: %s, len: %d", name.c_str(), name.size());
    vTaskDelay(pdMS_TO_TICKS(500));
}
#endif