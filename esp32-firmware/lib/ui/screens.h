#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *scope;
    lv_obj_t *settings;
    lv_obj_t *info;
    lv_obj_t *zdroj_panel;
    lv_obj_t *proud;
    lv_obj_t *i_set;
    lv_obj_t *obj0;
    lv_obj_t *i_readout_source;
    lv_obj_t *current_button;
    lv_obj_t *napeti;
    lv_obj_t *obj1;
    lv_obj_t *v_readout_source;
    lv_obj_t *v_set;
    lv_obj_t *voltage_button;
    lv_obj_t *overpower_alert;
    lv_obj_t *obj2;
    lv_obj_t *zdroj_header;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *output_enable;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *charge_indicator;
    lv_obj_t *input_v_readout;
    lv_obj_t *kanal2;
    lv_obj_t *obj7;
    lv_obj_t *v_readout_2;
    lv_obj_t *i_readout_2;
    lv_obj_t *kanal1;
    lv_obj_t *v_readout_1;
    lv_obj_t *obj8;
    lv_obj_t *i_readout_1;
    lv_obj_t *settings_button;
    lv_obj_t *scope_button;
    lv_obj_t *info_button;
    lv_obj_t *cal_marker;
    lv_obj_t *scope_chart;
    lv_obj_t *return_button;
    lv_obj_t *return_button_1;
    lv_obj_t *return_button_2;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SCOPE = 2,
    SCREEN_ID_SETTINGS = 3,
    SCREEN_ID_INFO = 4,
};

void create_screen_main();
void tick_screen_main();

void create_screen_scope();
void tick_screen_scope();

void create_screen_settings();
void tick_screen_settings();

void create_screen_info();
void tick_screen_info();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/