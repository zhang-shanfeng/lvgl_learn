# LVGL 页面循环切换

循环切换的核心是把“页面 + 对应 Group”组织成一个数组，8 号键每次按下时让索引 (idx + 1) % page_count 递增即可。这样以后加新页面只需往数组里加一项，切换逻辑无需改动。

## 步骤1：定义页面注册结构

用结构体把每个页面和它的 Group 绑定在一起，避免 Group 与屏幕错位。

```c
// file: lvgl_app.h
typedef struct {
    lv_obj_t   *scr;     // 页面根对象
    lv_group_t *group;   // 该页面专属的按键导航 Group
    const char *name;    // 调试用
} page_entry_t;
```

```c
// file: lvgl_app.c
#define PAGE_COUNT  2    // 目前是 2，以后加页面就改这里 + 补数组

static page_entry_t pages[PAGE_COUNT];
static uint8_t      current_page_idx = 0;
static lv_indev_t  *keypad_indev     = NULL;
```

## 步骤2：初始化时注册所有页面

在 UI 初始化阶段（创建好控件、Keypad 输入设备之后）统一完成绑定：

```c
// file: lvgl_app.c
static void pages_register(void)
{
    // ---------- 页面 0：Home ----------
    pages[0].scr   = scr_home;
    pages[0].group = lv_group_create();
    pages[0].name  = "Home";
    lv_group_add_obj(pages[0].group, home_btn1);
    lv_group_add_obj(pages[0].group, home_btn2);
    // ...

    // ---------- 页面 1：Settings ----------
    pages[1].scr   = scr_settings;
    pages[1].group = lv_group_create();
    pages[1].name  = "Settings";
    lv_group_add_obj(pages[1].group, settings_spinbox);
    lv_group_add_obj(pages[1].group, settings_slider);
    // ...

    // 默认显示 Home，并把 Keypad 绑到 Home 的 Group
    current_page_idx = 0;
    lv_screen_load(pages[0].scr);
    lv_indev_set_group(keypad_indev, pages[0].group);
}
```

> [!tip] 注意
> - 各个页面的代码写在独立的文件中，如：`pages/page_ina226.h/.c`
> - `lvgl_app.c` 作为主文件，lvgl 初始化、页面注册、全局变量定义在这个文件中
> - 