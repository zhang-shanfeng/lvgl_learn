#include "lvgl_app.h"

#include <stdio.h>

#include "button_task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "lvgl.h"
#include "lvgl_setup.h"
#include "page_home.h"
#include "page_ina226.h"
#include "page_more.h"
#include "page_settings.h"
#include "ui_styles.h"
#include "ui_widgets.h"

static const char* TAG = "lcd_lvgl_task";
lv_group_t* ina226_group;

#define PAGE_COUNT 1

static struct page_entry_t pages[PAGE_COUNT];
static uint8_t current_page_index = 0; /* 当前页面索引 */
static lv_indev_t* keypad_indev = NULL;

static lv_group_t* g = NULL;

bool is_edited_mode = false;

/**************************************************************************************************
 * @brief 注册所有页面
 *
 * @note 备注
 * @date 2026-11-01
 *************************************************************************************************/
static void pages_register(void)
{
    /* ---- 页面1: Ina226 ---- */
    pages[0].scr = lv_obj_create(NULL);
    pages[0].group = lv_group_create();
    pages[0].name = "INA226";

    ina226_page_init(pages[0].scr);
    lv_screen_load(pages[0].scr); /* 上电后就加载首页，否则首页会白屏 */

    // lv_group_add_obj(pages[0].group, ina226_ui.current_spinbox);
    // lv_group_add_obj(pages[0].group, ina226_ui.btn_start);
    // lv_indev_set_group(keypad_indev, pages[0].group);
    lv_group_set_editing(pages[0].group, false);
    lv_group_focus_obj(ina226_ui.current_spinbox);

    /* ---- 页面2: 设置页面  ---- */
    // pages[2].scr = lv_obj_create(NULL);
    // pages[2].group = lv_group_create();
    // pages[2].name = "Setting";
    // page_setting_init(pages[2].scr);

    // current_page_index = 0;
    // lv_screen_load(pages[0].scr);
    // lv_indev_set_group(keypad_indev, pages[0].group);
}

/**************************************************************************************************
 * @brief 页面循环切换
 *
 * @note 备注
 * @date 2026-11-01
 *************************************************************************************************/
void page_cycle_next(void)
{
    /* 计算下一个索引（循环）*/
    uint8_t next_index = (current_page_index + 1) % PAGE_COUNT;

    /*
     * 决定动画方向：
     * - 正常前进用 MOVE_LEFT
     * - 从最后一页回绕到第一页时用 MOVE_RIGHT，视觉上更符合"绕回来"的感觉
     */
    lv_screen_load_anim_t anim = (next_index == 0) ? LV_SCR_LOAD_ANIM_MOVE_RIGHT : LV_SCR_LOAD_ANIM_MOVE_LEFT;
    /* 加载新屏幕（auto_del = false，保留旧页面以便来回切换） */
    lv_screen_load_anim(pages[next_index].scr, anim, 200, 0, false);

    /*
     * 切换输入设备的 Group 绑定
     * 清除旧 Group 的焦点，避免视觉残留
     */
    lv_group_focus_obj(NULL);
    lv_indev_set_group(keypad_indev, pages[next_index].group);

    /* 更新当前索引 */
    current_page_index = next_index;
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

    /* 默认组 */
    g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(keypad_indev, g);

    pages_register(); /* 注册所有页面 */
}
