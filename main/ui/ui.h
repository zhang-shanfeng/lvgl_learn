#pragma once

#include "lvgl.h"

/* -------- 页面枚举 -------- */
enum page_id_t
{
    PAGE_LIST = 0,
    PAGE_HOME,
    PAGE_INA226,
    PAGE_PWM,
    PAGE_COUNT,
};

/* -------- 全局 UI 句柄  -------- */
struct ui_t
{
    lv_obj_t *screen;
    lv_obj_t *title_label;
    lv_obj_t *content_area;
    lv_obj_t *pages[PAGE_COUNT];
    lv_obj_t *header_right;             /* header 右侧的容器 right_cont */
    lv_obj_t *more_btn;                 /* 全局按钮，不属于任何组 */
    enum page_id_t current_page;        /* 当前页 */
    lv_group_t *groups[PAGE_COUNT];     /* 每页一个组 */
    lv_indev_t *indev;                  /* 仅用于切页时切换组绑定，不参与按键读取 */
    lv_obj_t *last_focused[PAGE_COUNT]; /* 每页最后聚焦的控件 */
};

/* 页面构建函数签名，所有页面文件统一实现这个签名 */
typedef void (*page_builder_t)(lv_obj_t *page, lv_group_t *group);

/* 对外接口：给按键模块等调用 */
void ui_show_page(enum page_id_t id);
void ui_show_list_page(void);

extern bool is_edited_mode;
extern struct ui_t ui;
extern const page_builder_t page_builders[];