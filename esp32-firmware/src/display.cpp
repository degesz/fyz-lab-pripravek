#include "display.h"
#include "actions.h"
#include "./scope.h"

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

XPT2046_Touchscreen ts(2);

ESP32Encoder encoder;
int lastCount = 0;
uint32_t last_alert_show_time = 0;
// === INTERNAL STATE ===
struct ButtonState
{
  volatile bool pressed;             // true while physically held
  volatile unsigned long pressTime;  // when pressed down
  volatile unsigned long lastChange; // last edge time for debounce
};

static ButtonState btn1 = {false, 0, 0};
static ButtonState btn2 = {false, 0, 0};
static ButtonState btn3 = {false, 0, 0};
static ButtonState btn4 = {false, 0, 0};
static ButtonState enc = {false, 0, 0};

// === OUTPUT FLAGS ===
volatile bool btn1ShortPressed = false;
volatile bool btn1LongPressed = false;
volatile bool btn2ShortPressed = false;
volatile bool btn2LongPressed = false;
volatile bool btn3ShortPressed = false;
volatile bool btn3LongPressed = false;
volatile bool btn4ShortPressed = false;
volatile bool btn4LongPressed = false;
volatile bool encoderShortPressed = false;
volatile bool encoderLongPressed = false;

// === HELPER FUNCTION ===
static void IRAM_ATTR handleButtonEdge(ButtonState &bState, volatile bool &shortFlag, volatile bool &longFlag, int pin)
{
  unsigned long now = millis();

  // Debounce
  if (now - bState.lastChange < DEBOUNCE_MS)
    return;
  bState.lastChange = now;

  int level = digitalRead(pin);
  if (level == LOW)
  {
    // Button pressed
    bState.pressed = true;
    bState.pressTime = now;
  }
  else
  {
    // Button released
    if (bState.pressed)
    {
      unsigned long held = now - bState.pressTime;
      if (held >= LONG_PRESS_MS)
      {
        longFlag = true;
      }
      else
      {
        shortFlag = true;
      }
    }
    bState.pressed = false;
  }
}

// === ISR WRAPPERS ===
void IRAM_ATTR ISR_btn1() { handleButtonEdge(btn1, btn1ShortPressed, btn1LongPressed, BTN_1); }
void IRAM_ATTR ISR_btn2() { handleButtonEdge(btn2, btn2ShortPressed, btn2LongPressed, BTN_2); }
void IRAM_ATTR ISR_btn3() { handleButtonEdge(btn3, btn3ShortPressed, btn3LongPressed, BTN_3); }
void IRAM_ATTR ISR_btn4() { handleButtonEdge(btn4, btn4ShortPressed, btn4LongPressed, BTN_4); }
void IRAM_ATTR ISR_encoderBtn() { handleButtonEdge(enc, encoderShortPressed, encoderLongPressed, ENCODER_BTN); }

uint32_t draw_buf[DRAW_BUF_SIZE / 4];
// Global variable
TouchCalibration calData;

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}
#endif

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  /*Copy `px map` to the `area`*/

  /*For example ("my_..." functions needs to be implemented by you)
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

  my_set_window(area->x1, area->y1, w, h);
  my_draw_bitmaps(px_map, w * h);
   */

  /*Call it to tell LVGL you are ready*/
  lv_display_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  static int16_t last_x = 0, last_y = 0;

  if (ts.touched())
  {
    TS_Point p = ts.getPoint();

    // Map raw touch to logical pixels (480×320, X horizontal Y vertical)
    int16_t logical_x = map(p.x, calData.minX, calData.maxX, 28, TFT_VER_RES - 30);
    int16_t logical_y = map(p.y, calData.minY, calData.maxY, 28, TFT_HOR_RES - 30);
    logical_x = constrain(logical_x, 0, TFT_VER_RES - 1);
    logical_y = constrain(logical_y, 0, TFT_HOR_RES - 1);

    last_x = logical_x;
    last_y = logical_y;

    /* LVGL expects NATIVE (unrotated) coords and applies display rotation for hit-test.
     * Native is 320×480; for ROTATION_90 the inverse is: native_x = logical_y, native_y = 479 - logical_x */
    data->point.x = (int16_t)logical_y;
    data->point.y = (int16_t)(TFT_VER_RES - 1 - logical_x);
    data->state = LV_INDEV_STATE_PR;

    /* Move cal_marker in logical coords so it follows the finger */
    if (objects.cal_marker != nullptr) {
      lv_coord_t w = lv_obj_get_width(objects.cal_marker);
      lv_coord_t h = lv_obj_get_height(objects.cal_marker);
      lv_obj_set_pos(objects.cal_marker, logical_x - w / 2, logical_y - h / 2);
    }

#if DEBUG_VERBOSE_LOGS
    Serial.printf("touching logical X:%d Y:%d  native x:%d y:%d  raw:%d,%d\n", logical_x, logical_y, (int)data->point.x, (int)data->point.y, p.x, p.y);
#endif
  }
  else
  {
    /* Same native transform as pressed: native_x = logical_y, native_y = 479 - logical_x */
    data->point.x = (int16_t)last_y;
    data->point.y = (int16_t)(TFT_VER_RES - 1 - last_x);
    data->state = LV_INDEV_STATE_REL;
  }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
  if (encoder.getCount() != lastCount)
  {
    lastCount = encoder.getCount();
#if DEBUG_VERBOSE_LOGS
    Serial.println(lastCount);
#endif
  }
// if (btn1ShortPressed) {
// btn1ShortPressed = false;
// Serial.println("BTN1 short press");
// }
// if (btn1LongPressed) {
// btn1LongPressed = false;
// Serial.println("BTN1 long press");
// }
//
// if (btn2ShortPressed) {
// btn2ShortPressed = false;
// Serial.println("BTN2 short press");
// }
// if (btn2LongPressed) {
// btn2LongPressed = false;
// Serial.println("BTN2 long press");
// }
//
// if (btn3ShortPressed) {
// btn3ShortPressed = false;
// Serial.println("BTN3 short press");
// }
// if (btn3LongPressed) {
// btn3LongPressed = false;
// Serial.println("BTN3 long press");
// }
//
// if (btn4ShortPressed) {
// btn4ShortPressed = false;
// Serial.println("BTN4 short press");
// }
// if (btn4LongPressed) {
// btn4LongPressed = false;
// Serial.println("BTN4 long press");
// }
//
// if (encoderShortPressed) {
// encoderShortPressed = false;
// Serial.println("ENCODER short press");
// }
// if (encoderLongPressed) {
// if (encoderLongPressed) {
//   encoderLongPressed = false;
//   Serial.println("ENCODER long press");
// }

return millis();
}

void setup_display()
{

  encoder.attachHalfQuad(4, 3);
  encoder.setCount(0);

  pinMode(ENCODER_BTN, INPUT);
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
  pinMode(BTN_4, INPUT);

  attachInterrupt(digitalPinToInterrupt(BTN_1), ISR_btn1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN_2), ISR_btn2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN_3), ISR_btn3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN_4), ISR_btn4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), ISR_encoderBtn, CHANGE);

  ts.begin();
  ts.setRotation(1);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  cal_display();

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println(LVGL_Arduino);

  lv_init();

  /*Set a tick source so that LVGL will know how much time elapsed. */
  lv_tick_set_cb(my_tick);

  /* register print function for debugging */
#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print);
#endif

  lv_display_t *disp;
#if LV_USE_TFT_ESPI
  /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, TFT_ROTATION);

#else
  /*Else create a display yourself*/
  disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

  /*Initialize the (dummy) input device driver*/
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);
  lv_indev_set_display(indev, disp); /* required: bind touch to this display so clicks reach widgets */
}


float VoltageSet = 0;
float CurrentSet = 0;
SettingMode currentMode = MODE_IDLE;

static void setConverterOutputEnabled(bool enabled)
{
  if (enabled == converter.enabled)
  {
    return;
  }

  if (enabled)
  {
    converter.enable();
    converter.update();
    return;
  }

  converter.disable();
}

static void handleOutputEnableChanged(lv_event_t *e)
{
  lv_obj_t *target = lv_event_get_target_obj(e);
  setConverterOutputEnabled(lv_obj_has_state(target, LV_STATE_CHECKED));
}

static void enterVoltageSettingMode()
{
  if (currentMode != MODE_IDLE)
  {
    return;
  }

  currentMode = MODE_VOLTAGE_SETTING;
  encoder.clearCount();
}

static void enterCurrentSettingMode()
{
  if (currentMode != MODE_IDLE)
  {
    return;
  }

  currentMode = MODE_CURRENT_SETTING;
  encoder.clearCount();
}

static void handleVoltageButtonPressed(lv_event_t *e)
{
  LV_UNUSED(e);
  enterVoltageSettingMode();
}

static void handleCurrentButtonPressed(lv_event_t *e)
{
  LV_UNUSED(e);
  enterCurrentSettingMode();
}

static void handleScopeButtonPressed(lv_event_t *e)
{
  LV_UNUSED(e);
  loadScreen(SCREEN_ID_SCOPE);
}

static void handleSettingsButtonPressed(lv_event_t *e)
{
  LV_UNUSED(e);
  loadScreen(SCREEN_ID_SETTINGS);
}

static void handleInfoButtonPressed(lv_event_t *e)
{
  LV_UNUSED(e);
  loadScreen(SCREEN_ID_INFO);
}

static void handleReturnButtonPressed(lv_event_t *e)
{
  LV_UNUSED(e);
  loadScreen(SCREEN_ID_MAIN);
}

void setup_ui()
{
  ui_init();
  lv_obj_add_flag(objects.settings_button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(objects.scope_button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(objects.info_button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(objects.output_enable, handleOutputEnableChanged, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.voltage_button, handleVoltageButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.current_button, handleCurrentButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.settings_button, handleSettingsButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.scope_button, handleScopeButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.info_button, handleInfoButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.return_button, handleReturnButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.return_button_1, handleReturnButtonPressed, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.return_button_2, handleReturnButtonPressed, LV_EVENT_CLICKED, NULL);
  setupScopeChart();
  char buf[FORMAT_BUF_SIZE];
  formatVoltageLabel(buf, sizeof(buf), converter.voltage);
  lv_label_set_text(objects.v_set, buf);
  formatCurrentLabel(buf, sizeof(buf), converter.current);
  lv_label_set_text(objects.i_set, buf);
  VoltageSet = converter.voltage;
  CurrentSet = converter.current;
  lv_obj_set_pos(objects.overpower_alert, OVERPOWER_ALERT_HIDDEN_X, OVERPOWER_ALERT_HIDDEN_Y);
}

void handleUserInput()
{

  switch (currentMode)
  {
  case MODE_IDLE:
  {
    if (btn3ShortPressed)
    {
      btn3ShortPressed = false;
      enterVoltageSettingMode();
    }
    if (btn3LongPressed)
    {
      btn3LongPressed = false;
      enterCurrentSettingMode();
    }

    if (encoderLongPressed)
    {
      encoderLongPressed = false;
      setConverterOutputEnabled(!converter.enabled);
    }
    if (encoderShortPressed)
    {
      encoderShortPressed = false;
    }
    encoder.setCount(0);

    break;
  }
  case MODE_VOLTAGE_SETTING:
  {
    if (encoder.getCount() != 0)
    {
      VoltageSet += encoder.getCount() * 0.1;
      if (VoltageSet <= 0)
      {
        VoltageSet = 0;
      }
      if (VoltageSet >= 20)
      {
        VoltageSet = 20;
      }
      encoder.clearCount();
    }

    char buf[FORMAT_BUF_SIZE];
    formatVoltageLabelEditing(buf, sizeof(buf), VoltageSet);
    lv_label_set_text(objects.v_set, buf);

    if (encoderShortPressed)
    {
      encoderShortPressed = false;
      formatVoltageLabel(buf, sizeof(buf), VoltageSet);
      lv_label_set_text(objects.v_set, buf);
      converter.voltage = VoltageSet;
      converter.update();

      currentMode = MODE_IDLE;
    }
    if (btn3ShortPressed)
    {
      btn3ShortPressed = false;
    }

    break;
  }

  case MODE_CURRENT_SETTING:{

    const DeviceState* dev = get_device_state();
    int current_ceiling = (dev->power_limit_w / converter.voltage) * 1000;

    if (encoder.getCount() != 0)
    {
      CurrentSet += encoder.getCount() * 50;
      if (CurrentSet <= 0)
      {
        CurrentSet = 0;
      }
      if (CurrentSet > current_ceiling)
      {
        CurrentSet = current_ceiling;
        lv_obj_set_pos(objects.overpower_alert, OVERPOWER_ALERT_VISIBLE_X, OVERPOWER_ALERT_VISIBLE_Y);
        last_alert_show_time = millis();
      }
      encoder.clearCount();
    }

    char buf[FORMAT_BUF_SIZE];
    formatCurrentLabelEditing(buf, sizeof(buf), CurrentSet);
    lv_label_set_text(objects.i_set, buf);

    if (encoderShortPressed)
    {
      formatCurrentLabel(buf, sizeof(buf), CurrentSet);
      lv_label_set_text(objects.i_set, buf);
      converter.current = CurrentSet;
      converter.update();

      currentMode = MODE_IDLE;
    }
    if (btn3ShortPressed)
    {
      btn3ShortPressed = false;
    }
    if (btn3LongPressed)
    {
      btn3LongPressed = false;
    }
    break;}

  default:
    break;
  }
}

void update_display()
{
  char buf[FORMAT_BUF_SIZE];

  const DeviceState* dev = get_device_state();
  if (converter.voltage * (converter.current / 1000) >= dev->power_limit_w)
  {
    CurrentSet = dev->power_limit_w / converter.voltage;
    formatCurrentLabel(buf, sizeof(buf), CurrentSet);
    lv_label_set_text(objects.i_set, buf);
    converter.current = CurrentSet;
    lv_obj_set_pos(objects.overpower_alert, OVERPOWER_ALERT_VISIBLE_X, OVERPOWER_ALERT_VISIBLE_Y);
    last_alert_show_time = millis();
    converter.update();
  }

  if (millis() > last_alert_show_time + OVERPOWER_ALERT_DURATION_MS)
  {
    lv_obj_set_pos(objects.overpower_alert, OVERPOWER_ALERT_HIDDEN_X, OVERPOWER_ALERT_HIDDEN_Y);
  }

  formatVoltageReadout(buf, sizeof(buf), dev->vbus_mv / 1000.0f);
  lv_label_set_text(objects.input_v_readout, buf);

  snprintf(buf, sizeof(buf), "%d %%", voltageToPercentage(dev->vbat_mv / 2));
  lv_label_set_text(objects.charge_indicator, buf);

  float voltages[3];
  float currents[3];
  getMeasurements(voltages, currents);

  lv_obj_t *v_labels[3] = { objects.v_readout_source, objects.v_readout_1, objects.v_readout_2 };
  lv_obj_t *i_labels[3] = { objects.i_readout_source, objects.i_readout_1, objects.i_readout_2 };
  for (int ch = 0; ch < 3; ch++)
  {
    formatVoltageReadout(buf, sizeof(buf), voltages[ch]);
    lv_label_set_text(v_labels[ch], buf);
    formatCurrentReadout(buf, sizeof(buf), currents[ch]);
    lv_label_set_text(i_labels[ch], buf);
  }
}

Preferences prefs;                       // NVS storage
#define CALIBRATION_NAMESPACE "touchcal" // Namespace in NVS

void cal_display()
{
  prefs.begin(CALIBRATION_NAMESPACE, false);

  // Check if already calibrated
  bool touchCalibrated = prefs.getBool("calibrated", false);

  if (touchCalibrated && !RECALIBRATE)
  {
    // Load saved calibration
    calData.minX = prefs.getUShort("minX", 200);
    calData.maxX = prefs.getUShort("maxX", 3800);
    calData.minY = prefs.getUShort("minY", 200);
    calData.maxY = prefs.getUShort("maxY", 3800);
    calData.calibrated = true;

    Serial.println("Touch already calibrated, loaded from NVS.");
    prefs.end();
    return;
  }

  

  // Not calibrated — perform calibration
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Touch the points to calibrate");

  uint16_t rawX[4], rawY[4];
  uint16_t screenX[4] = {20, tft.width() - 20, tft.width() - 20, 20};
  uint16_t screenY[4] = {20, 20, tft.height() - 20, tft.height() - 20};

  for (uint8_t i = 0; i < 4; i++)
  {
    drawCalibrationPoint(screenX[i], screenY[i]);

    bool touched = false;
    while (!touched)
    {
      if (ts.touched())
      {
        TS_Point p = ts.getPoint();
        rawX[i] = p.x;
        rawY[i] = p.y;
        touched = true;
        Serial.printf("CAL: X %d    Y %d      ", p.x, p.y);
        delay(300); // debounce
      }
    }
    tft.fillCircle(screenX[i], screenY[i], 10, TFT_BLACK); // erase marker
    delay(400);
  }

  // Compute calibration min/max
  calData.maxX = (rawX[2] + rawX[3]) / 2;
  calData.minX = (rawX[0] + rawX[1]) / 2;
  calData.maxY = (rawY[0] + rawY[3]) / 2;
  calData.minY = (rawY[2] + rawY[1]) / 2;
  calData.calibrated = true;

  // Save calibration
  prefs.putUShort("minX", calData.minX);
  prefs.putUShort("maxX", calData.maxX);
  prefs.putUShort("minY", calData.minY);
  prefs.putUShort("maxY", calData.maxY);
  prefs.putBool("calibrated", true);
  prefs.end();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Calibration complete!");
  delay(1000);
}

void drawCalibrationPoint(int16_t x, int16_t y)
{
  tft.fillScreen(TFT_BLACK);
  tft.drawCircle(x, y, 10, TFT_WHITE);
  tft.fillCircle(x, y, 4, TFT_RED);
}



int voltageToPercentage(int millivolts)
{
  float voltage = millivolts / 1000.0; // convert mV to V

  if (voltage >= 4.2)
    return 100;
  if (voltage <= 3.3)
    return 0;

  // Piecewise approximation
  if (voltage > 4.0)
    return map(millivolts, 4000, 4200, 90, 100);
  if (voltage > 3.9)
    return map(millivolts, 3900, 4000, 80, 90);
  if (voltage > 3.8)
    return map(millivolts, 3800, 3900, 60, 80);
  if (voltage > 3.7)
    return map(millivolts, 3700, 3800, 40, 60);
  if (voltage > 3.5)
    return map(millivolts, 3500, 3700, 0, 40);

  return 0; // fallback
}

void formatVoltageLabel(char *buf, size_t size, float v)
{
  snprintf(buf, size, "%.1f V", (double)v);
}

void formatVoltageLabelEditing(char *buf, size_t size, float v)
{
  snprintf(buf, size, ">%.1f V<", (double)v);
}

void formatCurrentLabel(char *buf, size_t size, float mA)
{
  if (mA < 1000)
    snprintf(buf, size, "%.0f mA", (double)mA);
  else
    snprintf(buf, size, "%.2f A", (double)(mA / 1000.0));
}

void formatCurrentLabelEditing(char *buf, size_t size, float mA)
{
  if (mA < 1000)
    snprintf(buf, size, ">%.0f mA<", (double)mA);
  else
    snprintf(buf, size, ">%.2f A<", (double)(mA / 1000.0));
}

void formatVoltageReadout(char *buf, size_t size, float v)
{
  if (v < 10)
    snprintf(buf, size, "%.2f V", (double)v);
  else
    snprintf(buf, size, "%.1f V", (double)v);
}

void formatCurrentReadout(char *buf, size_t size, float mA)
{
  if (mA < 1000)
    snprintf(buf, size, "%.0f mA", (double)mA);
  else
    snprintf(buf, size, "%.3f A", (double)(mA / 1000.0));
}

void setSourceVoltage(float v)
{
  VoltageSet = v;
  if (VoltageSet <= 0)
    VoltageSet = 0;
  if (VoltageSet >= 20)
    VoltageSet = 20;

  char buf[FORMAT_BUF_SIZE];
  formatVoltageLabel(buf, sizeof(buf), VoltageSet);
  lv_label_set_text(objects.v_set, buf);

  converter.voltage = VoltageSet;
  converter.update();
}

void setSourceCurrent(float mA)
{
  const DeviceState *dev = get_device_state();
  int current_ceiling = (dev->power_limit_w / converter.voltage) * 1000;

  CurrentSet = mA;
  if (CurrentSet <= 0)
    CurrentSet = 0;
  if (CurrentSet > current_ceiling)
    CurrentSet = current_ceiling;

  char buf[FORMAT_BUF_SIZE];
  formatCurrentLabel(buf, sizeof(buf), CurrentSet);
  lv_label_set_text(objects.i_set, buf);

  converter.current = CurrentSet;
  converter.update();
}

