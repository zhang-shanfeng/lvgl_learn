#pragma once

#include "ina226_task.h"
#include "lvgl.h"

struct ina226_ui_t
{
    lv_obj_t *label_voltage_val;
    lv_obj_t *label_current_val;
    lv_obj_t *label_power_val;
    lv_obj_t *current_spinbox;
    lv_obj_t *voltage_spinbox;
    lv_obj_t *power_spinbox;
    lv_obj_t *btn_start;
};

void switch_to_ina226_page(void);
void page_ina226_update_number(struct ina226_value_t values);
void ina226_page_init(lv_obj_t *parent);

extern struct ina226_ui_t ina226_ui;