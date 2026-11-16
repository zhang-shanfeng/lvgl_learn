#pragma once

#include "lvgl.h"

/**
 * 初始化列表所需的全部样式。进程内只需调用一次（内部有幂等保护）。
 * 必须在创建任何列表之前调用。
 */
void flex_list_styles_init(void);

/**
 * 创建一个空列表容器（列方向 flex）。
 * @param parent 父对象
 * @return 列表对象
 */
lv_obj_t* flex_list_create(lv_obj_t* parent);

/**
 * 追加一个分组标题（全宽灰色条）。
 */
lv_obj_t* flex_list_add_header(lv_obj_t* list, const char* txt);

/**
 * 追加一个列表按钮。
 * @param list   列表对象
 * @param icon   图标源，传 NULL 表示不要图标（不依赖 LV_USE_IMAGE）
 * @param txt    按钮文字，传 NULL 表示纯图标按钮
 * @param cb     点击回调，可为 NULL
 * @param user_data 回调用户数据
 * @return 按钮对象
 */
lv_obj_t* flex_list_add_button(lv_obj_t* list, const void* icon, const char* txt, lv_event_cb_t cb, void* user_data);