#include "page_list.h"

#include "ui.h"

static void list_item_cb(lv_event_t *e)
{
    lv_obj_set_hidden(ui.header_right, true);

    enum page_id_t id = (enum page_id_t)(lv_uintptr_t)lv_event_get_user_data(e);

    ui_show_page(id);
}

void build_page_list(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(page, 8, 0);

    static const char *names[PAGE_COUNT] = {"List", "Home", "INA226", "PWM"};
    for (int i = 0; i < PAGE_COUNT; i++)
    {
        if (i == PAGE_LIST)
            continue;

        lv_obj_t *item = lv_button_create(page);
        lv_obj_set_size(item, LV_PCT(100), 44);
        lv_obj_add_event_cb(item, list_item_cb, LV_EVENT_CLICKED, (void *)(lv_uintptr_t)i);

        lv_group_add_obj(group, item);

        lv_obj_t *lbl = lv_label_create(item);
        lv_label_set_text(lbl, names[i]);
        lv_obj_center(lbl);
    }
}