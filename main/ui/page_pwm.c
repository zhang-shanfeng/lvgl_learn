#include "page_pwm.h"

void build_page_pwm(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_t *btn = lv_button_create(page);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "PWM");

    lv_group_add_obj(group, btn);
}