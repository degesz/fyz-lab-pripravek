#include <SPI.h>
#include <ui.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <lvgl.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include <ESP32Encoder.h>
#include "charger.h"
#include "converter.h"
#include "measurement.h"

extern TPS55288 converter;

#define ENCODER_BTN 5
#define BTN_1 1
#define BTN_2 2
#define BTN_3 0

#define DEBOUNCE_MS 80
#define LONG_PRESS_MS   400

// =========================
// === STATE VARIABLES ====
// =========================
extern volatile bool btn1ShortPressed;
extern volatile bool btn1LongPressed;

extern volatile bool btn2ShortPressed;
extern volatile bool btn2LongPressed;

extern volatile bool btn3ShortPressed;
extern volatile bool btn3LongPressed;

extern volatile bool encoderShortPressed;
extern volatile bool encoderLongPressed;

// ==========================
// === FUNCTION PROTOTYPES - BUTTONS===
// ==========================
void initButtons();
void IRAM_ATTR ISR_btn1();
void IRAM_ATTR ISR_btn2();
void IRAM_ATTR ISR_btn3();
void IRAM_ATTR ISR_encoderBtn();


#define TFT_HOR_RES   320
#define TFT_VER_RES   480
#define TFT_ROTATION  LV_DISPLAY_ROTATION_90

#define RECALIBRATE 1

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

void show_splashscreen();

void update_display();
void handleUserInput();

int voltageToPercentage(int millivolts);

