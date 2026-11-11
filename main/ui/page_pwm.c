#include "page_pwm.h"

void build_page_pwm(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_t *cont = lv_obj_create(page);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_COLUMN);

    lv_obj_t *btn = lv_button_create(cont);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "PWM");

    lv_obj_t *btn1 = lv_button_create(cont);
    lv_obj_t *btn_label1 = lv_label_create(btn1);
    lv_label_set_text(btn_label1, "PWM");

    lv_group_add_obj(group, btn);
    lv_group_add_obj(group, btn1);
}