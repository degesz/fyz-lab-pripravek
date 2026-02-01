#include <SPI.h>
#include <cstddef>
#include <ui.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <lvgl.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include <ESP32Encoder.h>
#include "charger.h"
#include "converter.h"
#include "device_state.h"
#include "measurement.h"
#include "power_management.h"

extern TPS55288 converter;

#define ENCODER_BTN 5
#define BTN_1 1
#define BTN_2 2
#define BTN_3 45
#define BTN_4 18

#define DEBOUNCE_MS 80
#define LONG_PRESS_MS   400

/* Set to 1 to enable touch/encoder Serial debug; 0 to reduce spam */
#define DEBUG_VERBOSE_LOGS 1

/* Overpower alert position and timing */
#define OVERPOWER_ALERT_DURATION_MS  500
#define OVERPOWER_ALERT_VISIBLE_X    140
#define OVERPOWER_ALERT_VISIBLE_Y    66
#define OVERPOWER_ALERT_HIDDEN_X    500
#define OVERPOWER_ALERT_HIDDEN_Y    66

#define FORMAT_BUF_SIZE 16

// =========================
// === STATE VARIABLES ====
// =========================
extern volatile bool btn1ShortPressed;
extern volatile bool btn1LongPressed;

extern volatile bool btn2ShortPressed;
extern volatile bool btn2LongPressed;

extern volatile bool btn3ShortPressed;
extern volatile bool btn3LongPressed;

extern volatile bool btn4ShortPressed;
extern volatile bool btn4LongPressed;

extern volatile bool encoderShortPressed;
extern volatile bool encoderLongPressed;

// ==========================
// === FUNCTION PROTOTYPES - BUTTONS===
// ==========================
void initButtons();
void IRAM_ATTR ISR_btn1();
void IRAM_ATTR ISR_btn2();
void IRAM_ATTR ISR_btn3();
void IRAM_ATTR ISR_btn4();
void IRAM_ATTR ISR_encoderBtn();


/* Native panel size (portrait); after ROTATION_90 LVGL logical size is 480×320: X horizontal, Y vertical */
#define TFT_HOR_RES   320
#define TFT_VER_RES   480
#define TFT_ROTATION  LV_DISPLAY_ROTATION_90

#define RECALIBRATE 0

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
//uint32_t draw_buf[DRAW_BUF_SIZE / 4];

#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf );
#endif

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map);

/*Read the touchpad*/
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data );

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void);

// Structure to store calibration values
struct TouchCalibration {
  uint16_t minX, maxX, minY, maxY;
  bool calibrated;
};


enum SettingMode {
  MODE_IDLE,
  MODE_VOLTAGE_SETTING,
  MODE_CURRENT_SETTING
};


// Function prototypes
void drawCalibrationPoint(int16_t x, int16_t y);
void cal_display();

void setup_display();
void setup_ui();

void show_splashscreen();

void update_display();
void handleUserInput();

int voltageToPercentage(int millivolts);

/* Formatting helpers for labels (buffer size >= 16 recommended) */
void formatVoltageLabel(char *buf, size_t size, float v);
void formatVoltageLabelEditing(char *buf, size_t size, float v);
void formatCurrentLabel(char *buf, size_t size, float mA);
void formatCurrentLabelEditing(char *buf, size_t size, float mA);
void formatVoltageReadout(char *buf, size_t size, float v);
void formatCurrentReadout(char *buf, size_t size, float mA);

