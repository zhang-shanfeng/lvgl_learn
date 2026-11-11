#include "lvgl_app.h"

#include <stdio.h>

#include "button_task.h"
#include "color.h"
#include "custom_fonts.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "lvgl.h"
#include "lvgl_setup.h"
#include "page_home.h"
#include "page_ina226.h"
#include "ui.h"
#include "ui_styles.h"
#include "ui_widgets.h"

static const char* TAG = "lcd_lvgl_task";

static lv_indev_t* keypad_indev = NULL;

static void more_btn_cb(lv_event_t* e)
{
    lv_obj_set_hidden(ui.header_right, true);

    // show_page(PAGE_LIST);
}

static void create_header(lv_obj_t* parent)
{
    lv_obj_t* header = lv_obj_create(parent);
    lv_obj_remove_style_all(header);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);

    lv_obj_set_size(header, lv_pct(100), 36);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(HEADER_BG_COLOR), 0);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, /* 主轴对齐：靠左 */
                          LV_FLEX_ALIGN_CENTER,        /* 交叉轴对齐 */
                          LV_FLEX_ALIGN_CENTER);       /* track 对齐 */

    /* left 容器 */
    lv_obj_t* left_cont = lv_obj_create(header);
    lv_obj_remove_style_all(left_cont);
    lv_obj_set_height(left_cont, lv_pct(100));
    lv_obj_set_flex_grow(left_cont, 1);

    ui.title_label = lv_label_create(left_cont);
    lv_obj_remove_style_all(ui.title_label);
    lv_obj_set_style_text_font(ui.title_label, &ibm_plex_mono_medium_18_2bpp, 0);
    lv_obj_set_style_text_color(ui.title_label, lv_color_white(), 0);
    lv_label_set_text(ui.title_label, "Home");
    lv_obj_set_align(ui.title_label, LV_ALIGN_CENTER);

    /* right 右侧容器（菜单按钮）*/
    ui.header_right = lv_obj_create(header);
    lv_obj_remove_style_all(ui.header_right);
    lv_obj_set_size(ui.header_right, LV_SIZE_CONTENT, 36);
    lv_obj_set_style_pad_hor(ui.header_right, 4, 0);

    ui.more_btn = lv_button_create(ui.header_right);
    lv_obj_remove_style_all(ui.more_btn);
    lv_obj_set_size(ui.more_btn, LV_SIZE_CONTENT, lv_pct(85));
    lv_obj_set_style_pad_ver(ui.more_btn, 4, 0);
    lv_obj_set_state_trickle(ui.more_btn, true);
    lv_obj_add_event_cb(ui.more_btn, more_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(ui.more_btn);

    lv_obj_t* more_label = lv_label_create(ui.more_btn);
    lv_obj_set_style_text_font(more_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(more_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(more_label, LV_SYMBOL_BARS);
    lv_obj_center(more_label);

    lv_obj_set_style_text_color(more_label, lv_color_hex(0xFB8C00), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(more_label, lv_color_hex(0xE65100), LV_PART_MAIN | LV_STATE_PRESSED);
}

/**************************************************************************************************
 * @brief 创建内容区容器
 *
 * @note 备注
 * @param parent
 * @date 2026-11-11
 *************************************************************************************************/
static void create_content_area(lv_obj_t* parent)
{
    ui.content_area = lv_obj_create(parent);
    lv_obj_set_width(ui.content_area, lv_pct(100));
    lv_obj_set_flex_grow(ui.content_area, 1);
    lv_obj_set_style_pad_all(ui.content_area, 0, 0);
    lv_obj_set_style_border_width(ui.content_area, 0, 0);
    lv_obj_set_scrollbar_mode(ui.content_area, LV_SCROLLBAR_MODE_OFF);
}

static lv_obj_t* create_page(lv_obj_t* parent, enum page_id_t id)
{
    lv_obj_t* page = lv_obj_create(parent);
    lv_obj_remove_style_all(page);
    lv_obj_set_size(page, lv_pct(100), lv_pct(100));

    /* 关键：创建后立即隐藏，如果在调用出才隐藏会导致上电是按钮或其它内容闪烁 */
    lv_obj_set_hidden(page, true);

    ui.groups[id] = lv_group_create(); /* 创建组 */

    if (page_builders[id])
    {
        page_builders[id](page, ui.groups[id]);
    }

    return page;
}

void lvgl_app_init(void)
{
    lcd_init();

    lvgl_init(io_handle, panel_handle);

    styles_init();
    border_style_init();

    keypad_indev = lv_indev_create();
    lv_indev_set_type(keypad_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keypad_indev, keypad_read_cb);

    ui.indev = keypad_indev;

    ui.screen = lv_screen_active();
    lv_obj_set_size(ui.screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(ui.screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(ui.screen, 0, 0);
    lv_obj_set_style_bg_color(ui.screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(ui.screen, LV_OPA_COVER, 0);

    /* 将 ui.screen 设置为 flex column 布局方式 */
    lv_obj_set_flex_flow(ui.screen, LV_FLEX_FLOW_COLUMN);

    create_header(ui.screen);
    create_content_area(ui.screen);

    for (int i = 0; i < PAGE_COUNT; i++) { ui.pages[i] = create_page(ui.content_area, i); }

    ui_show_page(PAGE_HOME);
    lcd_set_backlight(true); /* 在页面都创建完成后才开启背光 */
}