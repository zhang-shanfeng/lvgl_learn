#include "page_ina226.h"

#include "custom_fonts.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "lvgl_app.h"
#include "ui.h"
#include "ui_styles.h"
#include "ui_widgets.h"

static const char* TAG = "PAGE_INA226";

static struct ui_kv_row_cfg_t make_ina226_kv_cfg(const char* title, const char* value);

static void main_cont(lv_obj_t* parent);
static void current_spinbox(lv_obj_t* parent);

/* 事件函数 */
static void toggle_btn_event_cb(lv_event_t* e);
static void spinbox_event_handler(lv_event_t* e);

struct ina226_ui_t ina226_ui;

void page_ina226_update_number(struct ina226_value_t values)
{
    if (lvgl_port_lock(100))
    {
        lv_label_set_text_fmt(ina226_ui.label_voltage_val, "%.3fV", values.voltage);
        lv_label_set_text_fmt(ina226_ui.label_current_val, "%.3fA", values.current);
        lv_label_set_text_fmt(ina226_ui.label_power_val, "%.3fW", values.power);

        lvgl_port_unlock();
    }
}

/**************************************************************************************************
 * @brief 工厂函数，用于创造 ui_kv_row_cfg_t 配置
 *
 * @param title
 * @param value
 * @return struct ui_kv_row_cfg_t
 * @date 2026-10-28
 *************************************************************************************************/
static struct ui_kv_row_cfg_t make_ina226_kv_cfg(const char* title, const char* value)
{
    return (struct ui_kv_row_cfg_t){
        .title = title,
        .value = value,
        .title_font = &alibaba_puhuiti_regular_24_4bpp,
        .value_font = &ibm_plex_mono_medium_24_2bpp,
        .height = 0,
        .pad_ver = 4,
        .pad_hor = 0,
        .has_title_color = true,
        .title_color = lv_palette_darken(LV_PALETTE_DEEP_ORANGE, 1),
        .has_value_color = true,
        .value_color = lv_color_white(),
    };
}

void main_cont(lv_obj_t* parent)
{
    /* 创建一个容器 */
    lv_obj_t* left = lv_obj_create(parent);
    lv_obj_remove_style_all(left);
    lv_obj_set_grid_cell(left,                  /* 父容器 */
                         LV_GRID_ALIGN_STRETCH, /* column 对齐 */
                         0,                     /* 列 pos */
                         1,                     /* 列 col_span */
                         LV_GRID_ALIGN_STRETCH, /* 行对齐 */
                         0,                     /* 行 pos */
                         1                      /* 行 row_span */
    );
    lv_obj_set_layout(left, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(left, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    border_style_apply(left);
    lv_obj_set_style_border_color(left, lv_palette_lighten(LV_PALETTE_ORANGE, 1), 0);
    lv_obj_set_style_pad_all(left, 4, 0);
    lv_obj_set_height(left, lv_pct(100));

    /* 电压行参数配置 */
    struct ui_kv_row_cfg_t cfg = make_ina226_kv_cfg("电压", "0.000V");
    /* 创建电压行 */
    ina226_ui.label_voltage_val = ui_create_kv_row(left, &cfg);

    /* 电流行参数配置 */
    cfg = make_ina226_kv_cfg("电流", "0.000A");
    /* 创建电流行 */
    ina226_ui.label_current_val = ui_create_kv_row(left, &cfg);

    /* 功率行参数配置 */
    cfg = make_ina226_kv_cfg("功率", "0.000W");
    /* 创建功率行 */
    ina226_ui.label_power_val = ui_create_kv_row(left, &cfg);

    /* 右侧：放启动按钮 */
    lv_obj_t* right = lv_obj_create(parent);
    lv_obj_remove_style_all(right);
    lv_obj_set_grid_cell(right, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    border_style_apply(right);
    lv_obj_set_height(right, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(right, 8, 0);

    ina226_ui.btn_start = lv_button_create(right);
    lv_obj_align(ina226_ui.btn_start, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_checkable(ina226_ui.btn_start, true);
    lv_obj_set_size(ina226_ui.btn_start, lv_pct(100), LV_SIZE_CONTENT);

    lv_obj_t* btn_start_label = lv_label_create(ina226_ui.btn_start);
    lv_obj_set_style_text_font(btn_start_label, &ibm_plex_mono_medium_18_2bpp, 0);
    lv_label_set_text(btn_start_label, "Start");
    lv_obj_align(btn_start_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(ina226_ui.btn_start, toggle_btn_event_cb, LV_EVENT_VALUE_CHANGED, btn_start_label);
}

/**************************************************************************************************
 * @brief Create a spinbox group object
 * 电流最大报警值
 * 电压最大报警值
 * 功率最大报警值
 * @param parent
 * @date 2026-10-28
 *************************************************************************************************/
static void current_spinbox(lv_obj_t* parent)
{
    static lv_style_t cont_style;
    static bool cont_style_inited = false;

    if (!cont_style_inited)
    {
        lv_style_init(&cont_style);
        lv_style_set_layout(&cont_style, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&cont_style, LV_FLEX_FLOW_ROW);
        lv_style_set_pad_all(&cont_style, 4);
        cont_style_inited = true;
    }

    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_grid_cell(cont,                  /* 父容器 */
                         LV_GRID_ALIGN_STRETCH, /* 列对齐 */
                         0,                     /* 列 pos */
                         2,                     /* 列 col_span */
                         LV_GRID_ALIGN_STRETCH, /* 行对齐 */
                         1,                     /* 行 pos */
                         1                      /* 行 row_span */
    );

    lv_obj_add_style(cont, &cont_style, 0);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    border_style_apply(cont);
    lv_obj_set_style_border_color(cont, lv_palette_darken(LV_PALETTE_BLUE, 1), 0);

    ina226_ui.current_spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(ina226_ui.current_spinbox, 0, 99999);
    lv_spinbox_set_digit_format(ina226_ui.current_spinbox, 4, 1);
    lv_spinbox_step_prev(ina226_ui.current_spinbox);
    lv_obj_set_style_pad_all(ina226_ui.current_spinbox, 3, 0);
    lv_obj_set_style_text_font(ina226_ui.current_spinbox, &ibm_plex_mono_medium_24_2bpp, 0);
    lv_obj_set_style_radius(ina226_ui.current_spinbox, 8, 0);
    lv_obj_set_size(ina226_ui.current_spinbox, 120, LV_SIZE_CONTENT);
    lv_obj_add_style(ina226_ui.current_spinbox, &style_spinbox_null_cursor, LV_PART_CURSOR); /* 默认不显示光标 */

    /* 添加事件 */
    lv_obj_add_event_cb(ina226_ui.current_spinbox, spinbox_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ina226_ui.current_spinbox, spinbox_event_handler, LV_EVENT_CLICKED, NULL);
}

static void spinbox_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* spinbox = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        int32_t raw_value = lv_spinbox_get_value(spinbox);
        float real_value = (float)raw_value / 1000.0f;
        current_alert_value_set(&ina, real_value);
        ESP_LOGE(TAG, "real_value: %f", real_value);
    }

    if (code == LV_EVENT_CLICKED)
    {
        is_edited_mode = !is_edited_mode;

        lv_obj_set_edited(spinbox, is_edited_mode);

        if (!is_edited_mode)
        {
            lv_obj_add_style(spinbox, &style_spinbox_null_cursor, LV_PART_CURSOR);
        }
        else
        {
            lv_obj_remove_style(spinbox, &style_spinbox_null_cursor, LV_PART_CURSOR);
        }
    }
}

// 事件回调函数
static void toggle_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_event_get_user_data(e);  // 从用户数据中获取标签指针

    if (lv_obj_has_state(btn, LV_STATE_CHECKED))
    {
        lv_label_set_text(label, "Stop");
    }
    else
    {
        lv_label_set_text(label, "Start");
    }
}

void build_page_ina226(lv_obj_t* page, lv_group_t* group)
{
    /* Screen 根样式 */
    static lv_style_t style_root;
    static bool style_root_inited = false;
    if (!style_root_inited)
    {
        lv_style_init(&style_root);
        lv_style_set_bg_color(&style_root, lv_color_black());
        lv_style_set_bg_opa(&style_root, LV_OPA_100);
        lv_style_set_size(&style_root, lv_pct(100), lv_pct(100));
        lv_style_set_pad_all(&style_root, 2);
        style_root_inited = true;
    }
    /* 给根容器 设置样式 */
    lv_obj_add_style(page, &style_root, LV_STATE_DEFAULT);

    /*
     * 2列，2行 Grid 容器
     */
    static int32_t col_dsc[] = {
        LV_GRID_FR(2),
        LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST,
    };
    static int32_t row_dsc[] = {
        LV_GRID_FR(1),
        48,
        LV_GRID_TEMPLATE_LAST,
    };

    lv_obj_set_grid_dsc_array(page, col_dsc, row_dsc);
    lv_obj_set_style_pad_row(page, 4, 0);
    lv_obj_set_style_pad_column(page, 4, 0);

    main_cont(page);

    current_spinbox(page);

    if (ina226_ui.btn_start)
    {
        lv_group_add_obj(group, ina226_ui.btn_start);
        ESP_LOGI(TAG, "btn_start 已加入组");
    }

    if (ina226_ui.current_spinbox)
    {
        lv_group_add_obj(group, ina226_ui.current_spinbox);
        ESP_LOGI(TAG, "current_spinbox 已加入组");
    }
}