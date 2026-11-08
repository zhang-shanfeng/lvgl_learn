#pragma once

// 1. 声明你的自定义字体（使它在全局可用）

LV_FONT_DECLARE(ibm_plex_mono_medium_16_2bpp);
LV_FONT_DECLARE(ibm_plex_mono_medium_18_2bpp);
LV_FONT_DECLARE(ibm_plex_mono_medium_20_2bpp);
LV_FONT_DECLARE(ibm_plex_mono_medium_22_2bpp);
LV_FONT_DECLARE(ibm_plex_mono_medium_24_2bpp);
LV_FONT_DECLARE(ibm_plex_mono_medium_26_2bpp);
LV_FONT_DECLARE(alibaba_puhuiti_regular_22_4bpp);
LV_FONT_DECLARE(alibaba_puhuiti_regular_24_4bpp);
LV_FONT_DECLARE(smiley_sans_oblique_24_4bpp);

// 2. (可选) 将此字体设置为 LVGL 的默认字体
// 如果你在 menuconfig 中勾选了 "Custom default font"，这一行是必须的
// #define LV_FONT_DEFAULT (&smiley_sans_oblique_24_4bpp)