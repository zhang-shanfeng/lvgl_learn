#include "ui_styles.h"

lv_style_t flex_style;
lv_style_t g_border_style;
lv_style_t style_spinbox_null_cursor;

void border_style_init(void)
{
    static bool inited = false;
    if (inited) return;

    lv_style_set_border_width(&g_border_style, 2);
    lv_style_set_border_color(&g_border_style, lv_color_white());
    lv_style_set_border_opa(&g_border_style, LV_OPA_100);
    lv_style_set_border_side(&g_border_style, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&g_border_style, 8);

    inited = true;
}

/**************************************************************************************************
 * @brief 应用 border 到指定对象
 * 确保用 border_style_init 初始化了
 *
 * @param obj
 * @date 2026-10-28
 * @details
 * @Call 该函数调用的其他函数
 * @Caller 调用该函数的其他函数
 * @author freerealmshn@gmail.com
 * @note
 *************************************************************************************************/
void border_style_apply(lv_obj_t* obj)
{
    if (obj && lv_obj_is_valid(obj))
    {
        lv_obj_add_style(obj, &g_border_style, 0);
    }
}

/**************************************************************************************************
 * @brief 样式初始化
 *
 * @date 2026-10-18
 * @details
 * @Call 该函数调用的其他函数
 * @Caller main/app_main()
 * @author freerealmshn@gmail.com
 * @note
 *************************************************************************************************/
void styles_init(void)
{
    /* ============== 初始化 flex_style 样式 ============== */
    lv_style_init(&flex_style);
    /* 用于同时设置上、下、左、右四个方向的内边距（padding） */
    lv_style_set_pad_all(&flex_style, 0);
    /* 设置行间距（row gap） */
    lv_style_set_pad_row(&flex_style, 0);
    /* 设置 Flex 布局中列间距（同一行内各个子元素之间的水平间隔）为 0 */
    lv_style_set_pad_column(&flex_style, 0);
    lv_style_set_border_width(&flex_style, 0);
    /* 把背景不透明度设为完全透明 */
    lv_style_set_bg_opa(&flex_style, LV_OPA_TRANSP);

    /* spinbox 光标隐藏样式 */
    lv_style_init(&style_spinbox_null_cursor);
    lv_style_set_bg_opa(&style_spinbox_null_cursor, LV_OPA_0);
    lv_style_set_text_opa(&style_spinbox_null_cursor, LV_OPA_0);
    lv_style_set_border_width(&style_spinbox_null_cursor, 0);
}
