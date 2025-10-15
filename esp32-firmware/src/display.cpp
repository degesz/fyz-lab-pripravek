#include "display.h"

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

XPT2046_Touchscreen ts(2);

ESP32Encoder encoder;
int lastCount = 0;

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
static ButtonState enc = {false, 0, 0};

// === OUTPUT FLAGS ===
volatile bool btn1ShortPressed = false;
volatile bool btn1LongPressed = false;
volatile bool btn2ShortPressed = false;
volatile bool btn2LongPressed = false;
volatile bool btn3ShortPressed = false;
volatile bool btn3LongPressed = false;
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

    // Map raw touch to screen pixels
    int16_t x = map(p.x, calData.minX, calData.maxX, 0, tft.width());
    int16_t y = map(p.y, calData.minY, calData.maxY, 0, tft.height());

    // Clamp to display boundaries
    x = constrain(x, 0, tft.width() - 1);
    y = constrain(y, 0, tft.height() - 1);

    last_x = x;
    last_y = y;

    data->point.y = x;
    data->point.x = y;
    data->state = LV_INDEV_STATE_PR;
    Serial.printf("touching X:%d   Y:%d   rawX:%d    rawY:%d   minX:%d maxX:%d minY:%d  maxY:%d   width:%d, height:%d\n" , x, y, p.x, p.y, calData.minX, calData.maxX,calData.minY,calData.maxY,tft.width(),tft.height() );
  }
  else
  {
    data->point.y = last_x;
    data->point.x = last_y;
    data->state = LV_INDEV_STATE_REL;
  }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
  if (encoder.getCount() != lastCount)
  {
    lastCount = encoder.getCount();
    Serial.println(lastCount);
  }

  //    if (btn1ShortPressed) {
  //    btn1ShortPressed = false;
  //    Serial.println("BTN1 short press");
  //  }
  //  if (btn1LongPressed) {
  //    btn1LongPressed = false;
  //    Serial.println("BTN1 long press");
  //  }
  //
  //  if (btn2ShortPressed) {
  //    btn2ShortPressed = false;
  //    Serial.println("BTN2 short press");
  //  }
  //  if (btn2LongPressed) {
  //    btn2LongPressed = false;
  //    Serial.println("BTN2 long press");
  //  }
  //
  //  if (btn3ShortPressed) {
  //    btn3ShortPressed = false;
  //    Serial.println("BTN3 short press");
  //  }
  //  if (btn3LongPressed) {
  //    btn3LongPressed = false;
  //    Serial.println("BTN3 long press");
  //  }
  //
  //  if (encoderShortPressed) {
  //    encoderShortPressed = false;
  //    Serial.println("ENCODER short press");
  //  }
  //  if (encoderLongPressed) {
  //    encoderLongPressed = false;
  //    Serial.println("ENCODER long press");
  //  }

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

  // attachInterrupt(digitalPinToInterrupt(BTN_1), ISR_btn1, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(BTN_2), ISR_btn2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN_3), ISR_btn3, CHANGE);
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
}

SettingMode currentMode = MODE_IDLE;
float VoltageSet = 0;
float CurrentSet = 0;

void handleUserInput()
{

  switch (currentMode)
  {
  case MODE_IDLE:
    if (btn3ShortPressed)
    {
      btn3ShortPressed = false;
      currentMode = MODE_VOLTAGE_SETTING;
    }

    break;
  case MODE_VOLTAGE_SETTING:
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

    char str_v_set[10];
    sprintf(str_v_set, ">%.1f V<", VoltageSet);
    lv_label_set_text(objects.v_set, str_v_set);

    if (encoderShortPressed)
    {
      encoderShortPressed = false;
      char str_v_set[10];
      sprintf(str_v_set, "%.1f V", VoltageSet);
      lv_label_set_text(objects.v_set, str_v_set);
      converter.setVoltage(VoltageSet);

      currentMode = MODE_IDLE;
    }
    if (btn3ShortPressed)
    {
      btn3ShortPressed = false;
    }

    break;

  default:
    break;
  }
}

void update_display()
{

  uint16_t vbus, vbat, vsys, ichg;
  read_charger(&vbus, &vbat, &vsys, &ichg);

  char str_input_v_readout[8];
  sprintf(str_input_v_readout, "%.1f V", vbus / 1000.0);
  lv_label_set_text(objects.input_v_readout, str_input_v_readout);

  // int percent_charged = ;
  char str_percent_charged[8];
  sprintf(str_percent_charged, "%d \%", voltageToPercentage(vbat / 2));
  lv_label_set_text(objects.charge_indicator, str_percent_charged);

  float voltages[3];
  float currents[3];
  getMeasurements(voltages, currents);

  char str_v_readout_source[8];
  if (voltages[0] < 10)
  {
    sprintf(str_v_readout_source, "%.2f V", voltages[0]);
  }
  else
  {
    sprintf(str_v_readout_source, "%.1f V", voltages[0]);
  }
  lv_label_set_text(objects.v_readout_source, str_v_readout_source);

  char str_i_readout_source[8];
  if (currents[0] < 1000)
  {
    sprintf(str_i_readout_source, "%.0f mA", currents[0]);
  }
  else
  {
    sprintf(str_i_readout_source, "%.3f A", currents[0] / 1000.0);
  }
  lv_label_set_text(objects.i_readout_source, str_i_readout_source);

  char str_v_readout_1[8];
  if (voltages[0] < 10)
  {
    sprintf(str_v_readout_1, "%.2f V", voltages[1]);
  }
  else
  {
    sprintf(str_v_readout_1, "%.1f V", voltages[1]);
  }
  lv_label_set_text(objects.v_readout_1, str_v_readout_1);

  char str_i_readout_1[8];
  if (currents[0] < 1000)
  {
    sprintf(str_i_readout_1, "%.0f mA", currents[1]);
  }
  else
  {
    sprintf(str_i_readout_1, "%.3f A", currents[1] / 1000.0);
  }
  lv_label_set_text(objects.i_readout_1, str_i_readout_1);

  char str_v_readout_2[8];
  if (voltages[0] < 10)
  {
    sprintf(str_v_readout_2, "%.2f V", voltages[2]);
  }
  else
  {
    sprintf(str_v_readout_2, "%.1f V", voltages[2]);
  }
  lv_label_set_text(objects.v_readout_2, str_v_readout_2);

  char str_i_readout_2[8];
  if (currents[0] < 1000)
  {
    sprintf(str_i_readout_2, "%.0f mA", currents[2]);
  }
  else
  {
    sprintf(str_i_readout_2, "%.3f A", currents[2] / 1000.0);
  }
  lv_label_set_text(objects.i_readout_2, str_i_readout_2);
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

// Example of how to use calibration data later:
int16_t touchToPixelX(uint16_t rawX)
{
  return map(rawX, calData.minX - 20, calData.maxX + 5, 0, tft.width());
}

int16_t touchToPixelY(uint16_t rawY)
{
  return map(rawY, calData.minY - 20, calData.maxY + 20, 0, tft.height());
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

/*


void show_splashscreen(){
    tft.fillScreen(TFT_BLACK);                   // Clear screen

    tft.setTextColor(TFT_ORANGE, TFT_BLACK);     // Orange text

    // Draw "OMG" in bold
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setCursor(180, 170);
    tft.print("OMG");

    // Draw "ROBOTICS" in normal font
    tft.setFreeFont(&FreeSans12pt7b);                        // Default font
    tft.setCursor(240, 170);                      // Adjust X to align after OMG
    tft.print("ROBOTICS");
    for (int i = 0; i <= 200; i += 10) {         // Animate loading bar
        tft.fillRect(140, 250, i, 20, TFT_GREEN);
        delay(100);
    }
}

*/
