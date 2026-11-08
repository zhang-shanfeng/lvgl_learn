#pragma once

#include "lvgl.h"

/* 🐰 页面入口 🐰 */
struct page_entry_t
{
    lv_obj_t* scr;     /* 页面根对象 */
    lv_group_t* group; /* 该页面专属的按键导航 Group */
    const char* name;  /* 调试用 */
};

void lvgl_app_init(void);
void page_cycle_next(void);

extern bool is_edited_mode;