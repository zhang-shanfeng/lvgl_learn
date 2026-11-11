#include "page_home.h"

void build_page_home(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_set_style_bg_color(page, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 1), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);

    lv_obj_t *btn = lv_button_create(page);
    lv_obj_set_size(btn, 120, 44);
    lv_group_add_obj(group, btn);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, "Home");
    lv_obj_center(lbl);
}