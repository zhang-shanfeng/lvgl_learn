#include "ui.h"

#include "page_home.h"
#include "page_ina226.h"
#include "page_list.h"
#include "page_pwm.h"
#include "page_settings.h"

bool is_edited_mode = false;

struct ui_t ui;

const page_builder_t page_builders[PAGE_COUNT] = {[PAGE_LIST] = build_page_list,
                                                  [PAGE_HOME] = build_page_home,
                                                  [PAGE_INA226] = build_page_ina226,
                                                  [PAGE_PWM] = build_page_pwm,
                                                  [PAGE_SETTING] = build_page_setting};

/**************************************************************************************************
 * @brief 根据页面 ID 切换页面，只是操控控件的 hidden 属性来实现
 *
 * @note 备注
 * @param id
 * @date 2026-11-08
 *************************************************************************************************/
void ui_show_page(enum page_id_t id)
{
    if (id >= PAGE_COUNT)
        return;

    /* 保存当前页面的聚焦控件 */
    if (ui.indev)
    {
        lv_obj_t* focused = lv_group_get_focused(ui.groups[ui.current_page]);
        if (focused)
            ui.last_focused[ui.current_page] = focused;
    }

    for (int i = 0; i < PAGE_COUNT; i++) lv_obj_set_hidden(ui.pages[i], i != id);

    /* 更新标题栏右侧更多按钮是否显示 */
    lv_obj_set_hidden(ui.header_right, id == PAGE_LIST);

    static const char* titles[PAGE_COUNT] = {"List", "Home", "INA226", "PWM", "Settings"};
    lv_label_set_text(ui.title_label, titles[id]);

    /* 切换输入设备的组绑定，焦点自动落到目标页 */
    if (ui.indev)
    {
        lv_indev_set_group(ui.indev, ui.groups[id]);

        lv_obj_t* obj = NULL;

        if (id == PAGE_LIST)
        {
            /* 回到来自的功能页对应的列表项。注意：
             * - page 的唯一子对象是 flex list 容器
             * - list 的第 0 个子对象是 header，不可聚焦，需跳过
             * - current_page 是 1..PAGE_COUNT-1，对应功能页
             */
            lv_obj_t* list = lv_obj_get_child(ui.pages[id], 0);
            if (list)
            {
                /* header 占第 0 个，功能页 i 对应 list 子对象索引 i */
                obj = lv_obj_get_child(list, ui.current_page);
            }
        }
        else
        {
            if (ui.last_focused[id] && lv_obj_is_valid(ui.last_focused[id]))
                obj = ui.last_focused[id];
        }

        if (!obj)
            obj = lv_group_get_obj_by_index(ui.groups[id], 0);

        if (obj)
            lv_group_focus_obj(obj);
    }

    ui.current_page = id; /* 更新当前页面 */
}

void ui_show_list_page(void)
{
    ui_show_page(PAGE_LIST);
}