set(app_srcs
    "main.c"
    "button_task.c"
    "lvgl_setup.c"
    "lvgl_app.c"
    "ui_styles.c"
    "ina226_task.c"
    "fonts/ibm_plex_mono_medium_16_2bpp.c"
    "fonts/ibm_plex_mono_medium_18_2bpp.c"
    "fonts/ibm_plex_mono_medium_20_2bpp.c"
    "fonts/ibm_plex_mono_medium_22_2bpp.c"
    "fonts/ibm_plex_mono_medium_24_2bpp.c"
    "fonts/ibm_plex_mono_medium_26_2bpp.c"
    "fonts/alibaba_puhuiti_regular_22_4bpp.c"
    "fonts/alibaba_puhuiti_regular_24_4bpp.c"
    "fonts/smiley_sans_oblique_24_4bpp.c"
    "utility.cpp"
    "ui/ui_widgets.c"
    "pages/page_ina226.c"
    "pages/page_home.c"
    "buzzer.c"
    "pages/page_settings.c"
    "pages/page_more.c"
)

set(includes 
    "."
    "fonts"
    "ui"
    "pages"
)

idf_component_register(SRCS ${app_srcs}
                       PRIV_REQUIRES spi_flash esp_driver_i2c esp_timer nvs_flash ina226
                       INCLUDE_DIRS ${includes}
                       )

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-unused-function 
    -Wno-unused-variable
    -Wno-unused-const-variable
)