#include "page_pwm.h"

#include "color.h"

void build_page_pwm(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_set_style_bg_color(page, lv_color_hex(BODY_BG_COLOR), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);

    lv_obj_t *cont = lv_obj_create(page);
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_bg_opa(cont, LV_OPA_0, 0);
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