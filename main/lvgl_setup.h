#pragma once

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"

void lcd_init(void);
lv_display_t *lvgl_init(esp_lcd_panel_io_handle_t io_handle,
                        esp_lcd_panel_handle_t panel_handle);

extern esp_lcd_panel_io_handle_t io_handle;
extern esp_lcd_panel_handle_t panel_handle;