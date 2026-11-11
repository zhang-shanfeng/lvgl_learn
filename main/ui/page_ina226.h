#pragma once

#include "ina226_task.h"
#include "lvgl.h"

struct ina226_ui_t
{
    lv_obj_t *label_voltage_val;
    lv_obj_t *label_current_val;
    lv_obj_t *label_power_val;
    lv_obj_t *current_spinbox;
    lv_obj_t *btn_start;
};

void page_ina226_update_number(struct ina226_value_t values);

void build_page_ina226(lv_obj_t *page, lv_group_t *group);

extern struct ina226_ui_t ina226_ui;