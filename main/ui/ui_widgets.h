/**************************************************************************************************
 * @file ui_widgets.h
 * @author freerealmshn@gmail.com (xiaotupo@163.com)
 * @brief
 * @version V0.1.0
 * @date 2026-10-27
 *
 * @copyright Copyright (c) 2025 by zhang-shanfeng, All Rights Reserved.
 * @website https://zhang-shanfeng.github.io
 *
 *************************************************************************************************/
#pragma once

#include "lvgl.h"

struct ui_kv_row_cfg_t
{
    const char* title;
    const char* value;
    const lv_font_t* title_font;
    const lv_font_t* value_font;
    lv_color_t title_color;
    bool has_title_color;
    lv_color_t value_color;
    bool has_value_color;
    lv_coord_t height;
    lv_coord_t pad_ver; /* 上下内边距 */
    lv_coord_t pad_hor; /* 左右内边距 */
};

struct ui_spinbox_cfg_t
{
    const lv_font_t* font;
    int32_t min_value;    /* spinbox 最小值 */
    int32_t max_value;    /* spinbox 最大值 */
    uint32_t digit_count; /* 不含小数点和正负号的数字位数 */
    uint32_t sep_pos;     /* 小数点前的数字位数。如果为0，则不显示小数点 */
    int32_t radius;       /* spinbox 圆角大小 */
    int32_t pad_ver;      /* 上下内边距 */
    int32_t pad_hor;      /* 左右内边距 */
    lv_color_t value_color;
    bool has_value_color;
    int32_t width;
    int32_t height;
};

struct ui_label_cfg_t
{
    const char* title;
    const lv_font_t* font;
    lv_color_t color;
    bool has_color;
    int32_t width;
    int32_t height;
    int32_t pad_ver; /* 上下内边距 */
    int32_t pad_hor; /* 左右内边距 */
};

struct ui_kv_row_spinbox_cfg_t
{
    int32_t width;   /* row 宽度 */
    int32_t height;  /* row 高度 */
    int32_t pad_ver; /* row 上下内边距 */
    int32_t pad_hor; /* row 左右内边距 */
    struct ui_label_cfg_t label;
    struct ui_spinbox_cfg_t spinbox;
};

lv_obj_t* ui_create_kv_row(lv_obj_t* parent, const struct ui_kv_row_cfg_t* cfg);
lv_obj_t* ui_create_kv_row_spinbox(lv_obj_t* parent, const struct ui_kv_row_spinbox_cfg_t* cfg);
struct ui_kv_row_spinbox_cfg_t make_row_spinbox_kv_cfg(const char* title);

void ui_header_create(
    lv_obj_t* parent, const char* title, const char* right_text, lv_event_cb_t right_cb, void* user_data);