#pragma once

#include "lvgl.h"

struct page_settings_t
{
    lv_obj_t *switch_btn;
};

void build_page_setting(lv_obj_t *page, lv_group_t *group);

extern struct page_settings_t page_settings;