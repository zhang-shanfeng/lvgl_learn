#include "page_settings.h"

/**************************************************************************************************
 * @brief 设置页面初始化
 *
 * @note 备注
 * @param parent
 * @date 2026-11-02
 *************************************************************************************************/
void page_setting_init(lv_obj_t *parent)
{
    /* 设置 parent 样式 */
    static lv_style_t parent_style;
    lv_style_init(&parent_style);
    /* 设置背景颜色和背景透明度 */
    lv_style_set_bg_color(&parent_style, lv_color_black());
    lv_style_set_bg_opa(&parent_style, LV_OPA_100);
    lv_obj_add_style(parent, &parent_style, 0);
    /* 设置 parent 布局 */

    /* 页面头部：标题 */

    /* 主内容区：设置项 */
}