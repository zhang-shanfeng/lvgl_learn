#include "ui.h"

#include "page_home.h"
#include "page_ina226.h"
#include "page_list.h"
#include "page_pwm.h"

bool is_edited_mode = false;

struct ui_t ui;

const page_builder_t page_builders[PAGE_COUNT] = {
    [PAGE_LIST] = build_page_list,
    [PAGE_HOME] = build_page_home,
    [PAGE_INA226] = build_page_ina226,
    [PAGE_PWM] = build_page_pwm,
};

/**************************************************************************************************
 * @brief 根据页面 ID 切换页面
 *
 * @note 备注
 * @param id
 * @date 2026-11-08
 *************************************************************************************************/
void ui_show_page(enum page_id_t id)
{
    if (id >= PAGE_COUNT)
        return;

    for (int i = 0; i < PAGE_COUNT; i++)
    {
        if (i == id)
        {
            lv_obj_set_hidden(ui.pages[i], false);
        }
        else
        {
            lv_obj_set_hidden(ui.pages[i], true);
        }
    }

    ui.current_page = id; /* 更新当前页面 */

    /* 更新标题栏右侧更多按钮是否显示 */
    if (id == PAGE_LIST)
    {
        lv_obj_set_hidden(ui.header_right, true);
    }
    else
    {
        lv_obj_set_hidden(ui.header_right, false);
    }

    static const char* titles[PAGE_COUNT] = {"List", "Home", "INA226", "PWM"};
    lv_label_set_text(ui.title_label, titles[id]);

    /* 切换输入设备的组绑定，焦点自动落到目标页 */
    if (ui.indev)
    {
        lv_indev_set_group(ui.indev, ui.groups[id]);

        /* 关键：组绑定后，把焦点设到组内第一个对象 */
        lv_obj_t* first = lv_obj_get_child(ui.pages[id], 0);
        if (first)
        {
            lv_group_focus_obj(first);
        }
    }
}

void ui_show_list_page(void)
{
    ui_show_page(PAGE_LIST);
}