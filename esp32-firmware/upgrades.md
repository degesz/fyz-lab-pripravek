# Upgrades – Simplifications, Refactors, and Maintainability

Actionable list of bug fixes, simplifications, refactors, and maintainability improvements. Use as a checklist; tick off items as they are implemented.

---

## 1. Bug fixes

- [ ] **power_management – stop re-initializing charger**  
  Remove the call to `setup_charger()` from the end of `power_management_update()` in `src/power_management.cpp`. The charger should be initialized only once from `main.cpp` in `setup()`. Calling `setup_charger()` every second can reset charger state and cause unstable behavior.

- [ ] **power_management – reduce Serial spam (optional)**  
  `power_management_update()` prints a large block to Serial every second. Consider logging only on state change (e.g. charging ↔ not charging, low_bat, critical_bat) or at a lower rate (e.g. every 10 s) to reduce noise and CPU.

- [ ] **USB-PD – initialize before use**  
  In `main.cpp`, `setup_usb()` is commented out, so the `pd` CLI command uses an uninitialized `usb` object. Either:
  - Uncomment and call `setup_usb()` in `setup()` when USB-PD hardware is present, or
  - Document that the `pd` command is non-functional without USB-PD hardware and optionally guard the `pd` branch in `handle_cli()` with a flag (e.g. `usb_initialized`) set by `setup_usb()`.

---

## 2. Simplifications

- [ ] **display.cpp – formatting helpers**  
  Extract “format voltage for label” and “format current for label” into small helpers (e.g. `formatVoltage(float v)` and `formatCurrent(float mA)` returning a string or writing into a buffer). Reuse them in `update_display()` and in the voltage/current setting branches in `handleUserInput()` to remove duplicated `sprintf` logic and to fix the bug where channel 2 readouts use `currents[0]` (or `voltages[0]`) for the formatting choice instead of the channel index (use per-channel values for formatting).

- [ ] **update_display() – loop over channels**  
  The three INA3221 channel readouts (source, channel 1, channel 2) are three nearly identical blocks. Replace with a single loop over channel index 0..2 and an array of the corresponding LVGL label pointers (e.g. `v_readout_source` / `v_readout_1` / `v_readout_2` and same for current) to avoid copy-paste and index mistakes.

- [ ] **Overpower alert – centralize logic**  
  “Show” and “hide” overpower alert are implemented by moving `objects.overpower_alert` to (140, 66) vs (500, 66) and using a 500 ms timeout. Centralize in one place: e.g. a small function like `setOverpowerAlertVisible(bool)` or named constants (e.g. `OVERPOWER_ALERT_VISIBLE_X`, `OVERPOWER_ALERT_HIDDEN_X`, `OVERPOWER_ALERT_DURATION_MS`) and a single place that applies the timing and position.

---

## 3. Refactors

- [ ] **Converter ↔ UI decoupling**  
  Remove `#include <ui.h>` and all LVGL calls from `src/converter.cpp`. In `enable()` and `disable()`, only set the `enabled` flag and GPIO 13 (and MODE register). In `display.cpp`, inside `update_display()` (or a dedicated UI refresh path), read `converter.enabled` and update `objects.output_enable` (label text ON/OFF and LVGL checked state). Alternatively introduce a callback or event that the display layer subscribes to when converter state changes.

- [ ] **Globals – optional AppState**  
  Consider a single struct or namespace (e.g. `AppState` or `PowerState`) for `power_limit`, `charging`, `low_bat`, `critical_bat` (and optionally `VoltageSet`, `CurrentSet`, `currentMode` if moved out of display). Reduces scattered `extern` declarations and makes dependencies clearer.

- [ ] **CLI naming**  
  In `src/cli.cpp`, the SimpleCLI instance is named `cli`, which shadows the module name. Rename to e.g. `serialCli`. Optionally rename command variables (e.g. `out` → `cmdOut`, `source` → `cmdSource`) for clarity.

- [ ] **display.cpp split (optional, phased)**  
  `display.cpp` is large and mixes display init, touch calibration, button/encoder handling, LVGL glue, and source-panel update logic. Optionally split into:
  - Touch and calibration (e.g. `touch.cpp` / `touch.h`),
  - Button and encoder handling (e.g. `input.cpp` / `input.h`),
  - Source-panel update logic (e.g. a function in display or a small `source_panel.cpp`),
  keeping `display.cpp` for display/LVGL init and high-level glue. Can be done incrementally.

---

## 4. Maintainability

- [ ] **Named constants**  
  Move magic numbers to named constants in one place (e.g. `config.h` or per-module headers): e.g. 7000 and 6800 mV in power_management (e.g. `BATTERY_LOW_MV`, `BATTERY_CRITICAL_MV`), 500 ms overpower alert duration, and ensure DEBOUNCE_MS / LONG_PRESS_MS are in a single header (already in display.h; consider a shared config if used elsewhere).

- [ ] **Module headers – one-line purpose**  
  Add a short comment at the top of each module (e.g. “BQ25703A charger control over I2C”) so new readers see the purpose immediately.

- [ ] **objects_t documentation**  
  Document the meaning of `objects_t` fields (which label is V set, I set, which readout is which channel) in `lib/ui/screens.h` or in explanation.md under a “UI object map” subsection.

- [ ] **Serial – gate verbose logs**  
  Touch coordinates in `my_touchpad_read` and encoder count in `my_tick` are printed every change and can be noisy. Gate them behind `#ifdef DEBUG_TOUCH` / `DEBUG_ENCODER` or a runtime debug level so normal builds are quiet and CPU use is lower.

- [ ] **Recalibration without recompile**  
  `RECALIBRATE` in `include/display.h` (0/1) forces touch recalibration. Consider a compile-time flag (e.g. `-DRECALIBRATE_TOUCH`) or a CLI command (e.g. `cal` or `recal`) that clears the NVS calibration flag and restarts or re-runs calibration so recalibration does not require editing the header and recompiling.

---

## 5. Optional / future

- [ ] **Unit tests**  
  Add PlatformIO native or embedded tests for pure functions where possible: e.g. `voltageToPercentage(int millivolts)` and any new formatting helpers (`formatVoltage`, `formatCurrent`), so refactors don’t break behavior.

- [ ] **Measurement abstraction**  
  Optional thin abstraction over “read all measurements” (charger + INA3221) to a single structure or API. Helps with logging, mocking, or future data logging features.

- [ ] **Hardware doc**  
  Document I2C addresses and pinout in one place (e.g. explanation.md “Hardware and dependencies” or a dedicated `hardware.md`) and keep it in sync with code so new contributors have a single reference.
