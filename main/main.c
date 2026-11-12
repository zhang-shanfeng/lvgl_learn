/*
 * button Demo
 *
 * `idf.py add-dependency "espressif/button^4.2.1"`
 */
#include "button_task.h"
#include "buzzer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ina226_task.h"
#include "lvgl_app.h"
#include "sdkconfig.h"
#include "utility_c.h"

static const char *TAG = "gpio_button_example";

void app_main(void)
{
    buzzer_init();
    button_task_init();

    lvgl_app_init();

    xTaskCreatePinnedToCore(ina226_task, "ina226_task", 4096, NULL, 0, NULL, 1);

    while (1)
    {
#ifdef CONFIG_ENABLE_FixedString_TEST_FUNC
        test();
#endif

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
