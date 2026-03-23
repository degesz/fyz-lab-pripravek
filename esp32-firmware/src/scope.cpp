#include "./scope.h"

#include <cstdint>

#include <lvgl.h>
#include <screens.h>

void setupScopeChart()
{
  static const int32_t sampleVoltageMv[] = {5000, 5100, 4950, 5200, 5350, 5250, 5400, 5500, 5450, 5600, 5550, 5700};
  static const int32_t sampleCurrentMa[] = {120, 180, 160, 240, 320, 280, 360, 410, 390, 450, 430, 470};
  static const char *voltageLabels[] = {"0 V", "1 V", "2 V", "3 V", "4 V", "5 V", "6 V", NULL};
  static const char *currentLabels[] = {"0.0 A", "0.1 A", "0.2 A", "0.3 A", "0.4 A", "0.5 A", NULL};

  lv_obj_t *chart = objects.scope_chart;
  lv_obj_t *voltageScale = lv_scale_create(objects.scope);
  lv_obj_t *currentScale = lv_scale_create(objects.scope);

  lv_obj_set_pos(voltageScale, 2, 58);
  lv_obj_set_size(voltageScale, 46, 234);
  lv_scale_set_mode(voltageScale, LV_SCALE_MODE_VERTICAL_LEFT);
  lv_scale_set_label_show(voltageScale, true);
  lv_scale_set_total_tick_count(voltageScale, 31);
  lv_scale_set_major_tick_every(voltageScale, 5);
  lv_scale_set_range(voltageScale, 0, 6);
  lv_scale_set_text_src(voltageScale, voltageLabels);
  lv_obj_set_style_length(voltageScale, 8, LV_PART_INDICATOR);
  lv_obj_set_style_length(voltageScale, 4, LV_PART_ITEMS);
  lv_obj_set_style_text_color(voltageScale, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);

  lv_obj_set_pos(currentScale, 432, 58);
  lv_obj_set_size(currentScale, 46, 234);
  lv_scale_set_mode(currentScale, LV_SCALE_MODE_VERTICAL_RIGHT);
  lv_scale_set_label_show(currentScale, true);
  lv_scale_set_total_tick_count(currentScale, 26);
  lv_scale_set_major_tick_every(currentScale, 5);
  lv_scale_set_range(currentScale, 0, 5);
  lv_scale_set_text_src(currentScale, currentLabels);
  lv_obj_set_style_length(currentScale, 8, LV_PART_INDICATOR);
  lv_obj_set_style_length(currentScale, 4, LV_PART_ITEMS);
  lv_obj_set_style_text_color(currentScale, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);

  lv_obj_set_pos(chart, 52, 58);
  lv_obj_set_size(chart, 376, 234);

  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(chart, sizeof(sampleVoltageMv) / sizeof(sampleVoltageMv[0]));
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 6000);
  lv_chart_set_axis_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 500);
  lv_chart_set_div_line_count(chart, 6, 12);

  lv_chart_series_t *voltageSeries = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_series_t *currentSeries = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_SECONDARY_Y);

  for (uint32_t i = 0; i < sizeof(sampleVoltageMv) / sizeof(sampleVoltageMv[0]); i++)
  {
    lv_chart_set_series_value_by_id(chart, voltageSeries, i, sampleVoltageMv[i]);
    lv_chart_set_series_value_by_id(chart, currentSeries, i, sampleCurrentMa[i]);
  }

  lv_chart_refresh(chart);
}
