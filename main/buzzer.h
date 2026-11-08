#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

esp_err_t buzzer_init(void);
void buzzer_set_enabled(bool enabled);
bool buzzer_is_enabled(void);
esp_err_t buzzer_start_with_timeout(uint64_t duration_us);
void buzzer_stop_all(void);
esp_err_t buzzer_start_alarm(uint32_t on_ms, uint32_t off_ms);
void buzzer_stop_alarm(void);
void buzzer_on_any_key(void);