#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // zdroj-panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.zdroj_panel = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 220);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // proud
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.proud = obj;
                    lv_obj_set_pos(obj, -18, 115);
                    lv_obj_set_size(obj, 480, 82);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_main_stop(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_main_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // I_set
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.i_set = obj;
                            lv_obj_set_pos(obj, -6, 23);
                            lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffb800ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "200 mA");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj0 = obj;
                            lv_obj_set_pos(obj, -18, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_20_dos, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 74, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_stop(obj, 230, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffff00df), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_main_stop(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_main_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Proud");
                        }
                        {
                            // I_readout_source
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.i_readout_source = obj;
                            lv_obj_set_pos(obj, 162, -9);
                            lv_obj_set_size(obj, 282, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_64_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "123 mA");
                        }
                    }
                }
                {
                    // napeti
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.napeti = obj;
                    lv_obj_set_pos(obj, -18, 33);
                    lv_obj_set_size(obj, 480, 82);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_main_stop(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_main_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj1 = obj;
                            lv_obj_set_pos(obj, -18, -18);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_20_dos, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_stop(obj, 230, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff00d6ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_main_stop(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_main_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Napětí");
                        }
                        {
                            // V_readout_source
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.v_readout_source = obj;
                            lv_obj_set_pos(obj, 162, -8);
                            lv_obj_set_size(obj, 282, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_64_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "5.61 V");
                        }
                        {
                            // V_set
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.v_set = obj;
                            lv_obj_set_pos(obj, -18, 23);
                            lv_obj_set_size(obj, 152, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff009dff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffcb00), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_label_set_text(obj, "5.6 V");
                        }
                    }
                }
                {
                    // overpower_alert
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.overpower_alert = obj;
                    lv_obj_set_pos(obj, 500, 66);
                    lv_obj_set_size(obj, 303, 99);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffffbf00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd06400), LV_PART_MAIN | LV_STATE_CHECKED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj2 = obj;
                            lv_obj_set_pos(obj, -18, 1);
                            lv_obj_set_size(obj, 303, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Výkonový limit dosažen !");
                        }
                    }
                }
                {
                    // zdroj-header
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.zdroj_header = obj;
                    lv_obj_set_pos(obj, -18, -18);
                    lv_obj_set_size(obj, 480, 51);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff7b00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd06400), LV_PART_MAIN | LV_STATE_CHECKED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj3 = obj;
                            lv_obj_set_pos(obj, -6, -8);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00002d), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_label_set_text(obj, "Zdroj");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj4 = obj;
                            lv_obj_set_pos(obj, 264, -8);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000039), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_label_set_text(obj, "Výstup:");
                        }
                        {
                            // output enable
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.output_enable = obj;
                            lv_obj_set_pos(obj, 383, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE|LV_OBJ_FLAG_CLICKABLE);
                            lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffec0606), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2bff2b), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_shadow_width(obj, 100, LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_shadow_spread(obj, 10, LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff00dc27), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_label_set_text(obj, "OFF");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj5 = obj;
                            lv_obj_set_pos(obj, 102, -11);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Vstup: ");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj6 = obj;
                            lv_obj_set_pos(obj, 92, 11);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Baterie: ");
                        }
                        {
                            // charge_indicator
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.charge_indicator = obj;
                            lv_obj_set_pos(obj, 156, 11);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "100 %");
                        }
                        {
                            // input_v_readout
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.input_v_readout = obj;
                            lv_obj_set_pos(obj, 157, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "20 V");
                        }
                    }
                }
            }
        }
        {
            // kanal2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.kanal2 = obj;
            lv_obj_set_pos(obj, 303, 215);
            lv_obj_set_size(obj, 175, 105);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, -16, -16);
                    lv_obj_set_size(obj, 171, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_16_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 74, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_stop(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff00ff66), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_main_stop(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_main_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Kanál 2");
                }
                {
                    // V_readout_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.v_readout_2 = obj;
                    lv_obj_set_pos(obj, 1, 4);
                    lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff009dff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "5.6 V");
                }
                {
                    // I_readout_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.i_readout_2 = obj;
                    lv_obj_set_pos(obj, 1, 49);
                    lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffb800ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "200 mA");
                }
            }
        }
        {
            // kanal1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.kanal1 = obj;
            lv_obj_set_pos(obj, 130, 215);
            lv_obj_set_size(obj, 175, 105);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // V_readout_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.v_readout_1 = obj;
                    lv_obj_set_pos(obj, 4, 4);
                    lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff009dff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "5.6 V");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, -16, -16);
                    lv_obj_set_size(obj, 171, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_16_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 74, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_stop(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_main_stop(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_main_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Kanál 1");
                }
                {
                    // I_readout_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.i_readout_1 = obj;
                    lv_obj_set_pos(obj, 4, 49);
                    lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &ui_font_32_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffb800ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "200 mA");
                }
            }
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 14, 234);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_gear_icon);
            lv_image_set_scale(obj, 255);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 14, 281);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_oscilloscope_icon);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 66, 234);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_info_icon);
            lv_image_set_scale(obj, 255);
        }
        {
            // cal_marker
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.cal_marker = obj;
            lv_obj_set_pos(obj, 416, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_64_sharetech_mono, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "+");
        }
        {
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.obj9 = obj;
            lv_obj_set_pos(obj, 333, 99);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Button");
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}


