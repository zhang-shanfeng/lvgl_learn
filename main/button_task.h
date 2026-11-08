/**************************************************************************************************
 * @file button_task.h
 * @author freerealmshn@gmail.com
 * @brief
 * @version V0.1.0
 * @date 2026-10-07
 *
 * @copyright Copyright (c) 2025 by zhang-shanfeng, All Rights Reserved.
 * @website https://zhang-shanfeng.github.io
 *
 *************************************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl.h"
/* C 语言声明 */
void button_task_init(void);
void keypad_read_cb(lv_indev_t *indev, lv_indev_data_t *data);

#ifdef __cplusplus
}
#endif

/* C++ 声明|类定义等 */
