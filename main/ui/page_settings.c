#include "page_settings.h"

#include "buzzer.h"
#include "string.h"
#include "ui.h"
#include "ui_widgets.h"

struct page_settings_t page_settings;

static void switch_event_cb(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target_obj(e);
    const char *tag = lv_event_get_user_data(e);
    bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);

    if (tag == NULL)
        return;

    if (strcmp(tag, "buzzer") == 0)
    {
        buzzer_set_enabled(on);
        buzzer_nvs_save(on);
    }
}

void build_page_setting(lv_obj_t *page, lv_group_t *group)
{
    lv_obj_t *cont = lv_obj_create(page);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    lv_obj_set_flex_flow(cont, LV_FLEX_COLUMN);

    struct ui_kv_row_switch_cfg_t cfg = make_row_switch_kv_cfg("Buzzer");
    page_settings.switch_btn = ui_create_kv_row_switch(cont, &cfg);

    lv_obj_add_event_cb(page_settings.switch_btn, switch_event_cb, LV_EVENT_VALUE_CHANGED, "buzzer");

    lv_group_add_obj(group, page_settings.switch_btn);
}