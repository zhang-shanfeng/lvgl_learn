#pragma once

#include "lvgl.h"

void styles_init(void);
void border_style_init(void);
void border_style_apply(lv_obj_t* obj);

extern lv_style_t flex_style;
extern lv_style_t g_border_style;
extern lv_style_t style_spinbox_null_cursor;
extern lv_style_t style_switch_main;
extern lv_style_t style_switch_indicator_checked;
extern lv_style_t style_switch_knob;