#include "ui_widgets.h"

#include "color.h"
#include "custom_fonts.h"
#include "page_more.h"

/**************************************************************************************************
 * @brief 创建一行，内部包括2个 Label，如：电压 1.233V
 *
 * @note
 * @param parent
 * @param cfg
 * @return lv_obj_t*
 * @date 2026-11-06
 *************************************************************************************************/
lv_obj_t* ui_create_kv_row(lv_obj_t* parent, const struct ui_kv_row_cfg_t* cfg)
{
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(row, lv_pct(100), cfg->height ? cfg->height : LV_SIZE_CONTENT);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    if (cfg->pad_ver)
    {
        lv_obj_set_style_pad_ver(row, cfg->pad_ver, 0);
    }
    if (cfg->pad_hor)
    {
        lv_obj_set_style_pad_hor(row, cfg->pad_hor, 0);
    }

    lv_obj_t* left = lv_label_create(row);

    if (cfg->title_font)
    {
        lv_obj_set_style_text_font(left, cfg->title_font, LV_STATE_DEFAULT);
    }

    if (cfg->has_title_color)
    {
        lv_obj_set_style_text_color(left, cfg->title_color, 0);
    }
    lv_label_set_text(left, cfg->title);

    /* 右侧动态数据标签 */
    lv_obj_t* right = lv_label_create(row);
    /* 设置一个固定宽度，以解决容器大小不固定 */
    lv_obj_set_width(right, LV_SIZE_CONTENT);
    if (cfg->value_font)
    {
        lv_obj_set_style_text_font(right, cfg->value_font, 0);
    }
    if (cfg->has_value_color)
    {
        lv_obj_set_style_text_color(right, cfg->value_color, 0);
    }
    lv_label_set_text(right, cfg->value);

    return right;
}

/**************************************************************************************************
 * @brief 创建一行，内部放两个控件，左侧 Label,右侧 spinbox
 *
 * @note 备注
 * @param parent
 * @return lv_obj_t*
 * @date 2026-11-04
 *************************************************************************************************/
lv_obj_t* ui_create_kv_row_spinbox(lv_obj_t* parent, const struct ui_kv_row_spinbox_cfg_t* cfg)
{
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, cfg->width, cfg->height);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    if (cfg->pad_ver)
    {
        lv_obj_set_style_pad_ver(cont, cfg->pad_ver, 0);
    }
    if (cfg->pad_hor)
    {
        lv_obj_set_style_pad_hor(cont, cfg->pad_hor, 0);
    }

    lv_obj_t* title = lv_label_create(cont);
    lv_obj_set_style_text_font(title, cfg->label.font, 0);
    if (cfg->label.has_color)
    {
        lv_obj_set_style_text_color(title, cfg->label.color, 0);
    }
    lv_obj_set_size(title, cfg->label.width, cfg->label.height);
    lv_label_set_text(title, cfg->label.title);

    lv_obj_t* spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(spinbox, cfg->spinbox.min_value, cfg->spinbox.max_value);
    lv_spinbox_set_digit_format(spinbox, cfg->spinbox.digit_count, cfg->spinbox.sep_pos);
    lv_spinbox_step_prev(spinbox);
    if (cfg->spinbox.pad_ver)
    {
        lv_obj_set_style_pad_ver(spinbox, cfg->spinbox.pad_ver, 0);
    }
    if (cfg->spinbox.pad_hor)
    {
        lv_obj_set_style_pad_hor(spinbox, cfg->spinbox.pad_hor, 0);
    }
    lv_obj_set_style_text_font(spinbox, &ibm_plex_mono_medium_24_2bpp, 0);
    lv_obj_set_style_radius(spinbox, cfg->spinbox.radius, 0);
    lv_obj_set_size(spinbox, cfg->spinbox.width, cfg->spinbox.height);

    return spinbox;
}

/**************************************************************************************************
 * @brief ui_kv_row_spinbox_cfg_t 结构体工厂函数
 *
 * @note
 * @param title
 * @return struct ui_kv_row_spinbox_cfg_t
 * @date 2026-11-06
 *************************************************************************************************/
struct ui_kv_row_spinbox_cfg_t make_row_spinbox_kv_cfg(const char* title)
{
    return (struct ui_kv_row_spinbox_cfg_t){.width = lv_pct(100),
                                            .height = LV_SIZE_CONTENT,
                                            .pad_hor = 6,
                                            .pad_ver = 6,
                                            .label =
                                                {
                                                    .title = title,
                                                    .width = LV_SIZE_CONTENT,
                                                    .height = LV_SIZE_CONTENT,
                                                    .has_color = true,
                                                    .color = lv_color_white(),
                                                    .pad_hor = 0,
                                                    .pad_ver = 0,
                                                    .font = &alibaba_puhuiti_regular_24_4bpp,
                                                },
                                            .spinbox = {
                                                .min_value = 0,
                                                .max_value = 9999,
                                                .digit_count = 4,
                                                .sep_pos = 1,
                                                .width = LV_SIZE_CONTENT,
                                                .height = LV_SIZE_CONTENT,
                                                .font = &ibm_plex_mono_medium_24_2bpp,
                                                .has_value_color = true,
                                                .value_color = lv_color_black(),
                                                .radius = 8,
                                                .pad_hor = 4,
                                                .pad_ver = 4,
                                            }};
}

#if (0)

void ui_header_create(
    lv_obj_t* parent, const char* title, const char* right_text, lv_event_cb_t right_cb, void* user_data)
{
    /* 创建 root_cont 容器，Layout 为 Column */
    lv_obj_t* root_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(root_cont);
    /* 设置放到 parent Grid 中的位置等参数 */
    lv_obj_set_grid_cell(root_cont, LV_GRID_ALIGN_START, /* 列对齐 */
                         0,                              /* 列位置 */
                         2,                              /* 跨列 */
                         LV_GRID_ALIGN_START,            /* 列对齐 */
                         0,                              /* 行位置 */
                         1                               /* 跨行 */
    );
    /* 设置 root_cont 大小 */
    lv_obj_set_size(root_cont, lv_pct(100), 36);
    lv_obj_set_style_bg_opa(root_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(root_cont, lv_color_hex(HEADER_BG_COLOR), 0);
    lv_obj_set_layout(root_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(root_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(root_cont, LV_FLEX_ALIGN_START, /* 主轴对齐：靠左 */
                          LV_FLEX_ALIGN_CENTER,           /* 交叉轴对齐 */
                          LV_FLEX_ALIGN_CENTER);          /* track 对齐 */

    /* left 容器 */
    lv_obj_t* left_cont = lv_obj_create(root_cont);
    lv_obj_remove_style_all(left_cont);
    lv_obj_set_height(left_cont, lv_pct(100));
    lv_obj_set_flex_grow(left_cont, 1);

    lv_obj_t* label_title = lv_label_create(left_cont);
    lv_obj_remove_style_all(label_title);
    lv_obj_set_style_text_font(label_title, &smiley_sans_oblique_24_4bpp, 0);
    lv_obj_set_style_text_color(label_title, lv_color_white(), 0);
    lv_label_set_text(label_title, title);
    lv_obj_set_align(label_title, LV_ALIGN_CENTER);

    /* right 右侧容器（菜单按钮）*/
    lv_obj_t* right_cont = lv_obj_create(root_cont);
    lv_obj_remove_style_all(right_cont);
    lv_obj_set_size(right_cont, LV_SIZE_CONTENT, 36);
    lv_obj_set_style_pad_hor(right_cont, 4, 0);

    more_btn = lv_button_create(right_cont);
    lv_obj_remove_style_all(more_btn);
    lv_obj_set_size(more_btn, LV_SIZE_CONTENT, lv_pct(85));
    lv_obj_set_style_pad_ver(more_btn, 4, 0);
    lv_obj_set_state_trickle(more_btn, true);

    lv_obj_t* more_btn_label = lv_label_create(more_btn);
    lv_obj_set_style_text_font(more_btn_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(more_btn_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(more_btn_label, right_text);
    lv_obj_set_align(more_btn, LV_ALIGN_CENTER);

    lv_obj_set_style_text_color(more_btn_label, lv_color_hex(0xFB8C00), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(more_btn_label, lv_color_hex(0xE65100), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(more_btn, right_cb, LV_EVENT_CLICKED, user_data);
}

#endif