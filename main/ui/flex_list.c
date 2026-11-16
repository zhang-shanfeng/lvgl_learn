#include "flex_list.h"

#include "custom_fonts.h"

static lv_style_t style_list;
static lv_style_t style_header;
static lv_style_t style_button;
static lv_style_t style_button_pressed;
static lv_style_t style_button_focused; /* 新增 */

/* 聚焦反色过渡 */
static const lv_style_prop_t focused_trans_props[] = {LV_STYLE_BG_COLOR, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_INV};
static lv_style_transition_dsc_t focused_trans;

static const lv_style_prop_t default_trans_props[] = {LV_STYLE_BG_COLOR, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_INV};
static lv_style_transition_dsc_t default_trans;

void flex_list_styles_init(void)
{
    static bool inited = false;
    if (inited)
        return;
    inited = true;

    lv_style_transition_dsc_init(&focused_trans, focused_trans_props, lv_anim_path_ease_out, 120, 0, NULL);
    lv_style_set_transition(&style_button_focused, &focused_trans);

    lv_style_transition_dsc_init(&default_trans, default_trans_props, lv_anim_path_ease_out, 120, 0, NULL);
    lv_style_set_transition(&style_button, &default_trans);

    /* 列表背景 */
    lv_style_init(&style_list);
    lv_style_set_pad_ver(&style_list, 0);
    lv_style_set_pad_gap(&style_list, 0);
    lv_style_set_clip_corner(&style_list, true);
    lv_style_set_pad_row(&style_list, 4);

    /* 分组标题 */
    lv_style_init(&style_header);
    lv_style_set_bg_opa(&style_header, LV_OPA_COVER);
    lv_style_set_bg_color(&style_header, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_text_color(&style_header, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_transform_width(&style_header, 16);
    lv_style_set_text_font(&style_header, &ibm_plex_mono_medium_16_2bpp);

    /* 按钮 */
    lv_style_init(&style_button);
    lv_style_set_radius(&style_button, 0);
    lv_style_set_shadow_width(&style_button, 0);
    lv_style_set_bg_opa(&style_button, LV_OPA_COVER);
    lv_style_set_bg_color(&style_button, lv_color_white());
    lv_style_set_text_color(&style_button, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_border_width(&style_button, 1);
    lv_style_set_border_color(&style_button, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_border_side(&style_button, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_all(&style_button, 8);
    lv_style_set_pad_column(&style_button, 8);

    /* 按下反馈 */
    lv_style_init(&style_button_pressed);
    lv_style_set_recolor(&style_button_pressed, lv_color_black());
    lv_style_set_recolor_opa(&style_button_pressed, LV_OPA_20);

    /* 聚焦反馈：背景色反转，不依赖 outline */
    lv_style_init(&style_button_focused);
    lv_style_set_outline_width(&style_button_focused, 0);
    lv_style_set_bg_color(&style_button_focused, lv_palette_darken(LV_PALETTE_GREY, 1));
    lv_style_set_text_color(&style_button_focused, lv_color_white());
    lv_style_set_border_color(&style_button_focused, lv_palette_darken(LV_PALETTE_GREY, 1));
    lv_style_set_outline_width(&style_button_focused, 0);
    lv_style_set_border_width(&style_button_focused, 0);
}

lv_obj_t* flex_list_create(lv_obj_t* parent)
{
    flex_list_styles_init(); /* 防御性调用，幂等 */
    lv_obj_t* list = lv_obj_create(parent);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(list, &style_list, 0);
    return list;
}

lv_obj_t* flex_list_add_header(lv_obj_t* list, const char* txt)
{
    lv_obj_t* label = lv_label_create(list);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_add_style(label, &style_header, 0);
    lv_label_set_text(label, txt);
    return label;
}

lv_obj_t* flex_list_add_button(lv_obj_t* list, const void* icon, const char* txt, lv_event_cb_t cb, void* user_data)
{
    lv_obj_t* btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(btn, &style_button, 0);
    lv_obj_add_style(btn, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &style_button_focused, LV_STATE_FOCUS_KEY); /* 新增 */

    /* 图标可选：icon 为 NULL 时整段跳过，不依赖 LV_USE_IMAGE */
    if (icon)
    {
        lv_obj_t* img = lv_image_create(btn);
        lv_image_set_src(img, icon);
    }

    if (txt)
    {
        lv_obj_t* label = lv_label_create(btn);
        lv_obj_set_style_text_font(label, &ibm_plex_mono_medium_16_2bpp, 0);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if (cb)
    {
        lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, user_data);
    }

    return btn;
}