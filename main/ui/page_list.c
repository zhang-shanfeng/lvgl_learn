#include "page_list.h"

#include "color.h"
#include "flex_list.h"
#include "ui.h"

static void list_item_cb(lv_event_t *e)
{
    lv_obj_set_hidden(ui.header_right, true);

    enum page_id_t id = (enum page_id_t)(lv_uintptr_t)lv_event_get_user_data(e);

    ui_show_page(id);
}

void build_page_list(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_set_style_bg_color(page, lv_color_hex(BODY_BG_COLOR), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(page, 4, 0);
    lv_obj_set_style_pad_row(page, 8, 0);

    static const char *names[PAGE_COUNT] = {"List", "Home", "INA226", "PWM", "Settings"};

    flex_list_styles_init();
    lv_obj_t *list = flex_list_create(page);
    lv_obj_set_size(list, lv_pct(80), lv_pct(100));
    lv_obj_center(list);

    flex_list_add_header(list, "Function");
    for (int i = 0; i < PAGE_COUNT; i++)
    {
        if (i == PAGE_LIST)
            continue;
        lv_obj_t *li = flex_list_add_button(list, NULL, names[i], list_item_cb, (void *)(lv_uintptr_t)i);
        lv_group_add_obj(group, li);
    }
}