# Upgrades – Simplifications, Refactors, and Maintainability

Actionable list of bug fixes, simplifications, refactors, and maintainability improvements. Use as a checklist; tick off items as they are implemented.

---

## 1. Bug fixes

- [ ] **USB-PD – initialize before use**  
  In `main.cpp`, `setup_usb()` is commented out, so the `pd` CLI command uses an uninitialized `usb` object. Either:
  - Uncomment and call `setup_usb()` in `setup()` when USB-PD hardware is present, or
  - Document that the `pd` command is non-functional without USB-PD hardware and optionally guard the `pd` branch in `handle_cli()` with a flag (e.g. `usb_initialized`) set by `setup_usb()`.

---

## 2. Refactors

- [ ] **Converter ↔ UI decoupling**  
  Remove `#include <ui.h>` and all LVGL calls from `src/converter.cpp`. In `enable()` and `disable()`, only set the `enabled` flag and GPIO 13 (and MODE register). In `display.cpp`, inside `update_display()` (or a dedicated UI refresh path), read `converter.enabled` and update `objects.output_enable` (label text ON/OFF and LVGL checked state). Alternatively introduce a callback or event that the display layer subscribes to when converter state changes.

- [ ] **display.cpp split (optional, phased)**  
  `display.cpp` is large and mixes display init, touch calibration, button/encoder handling, LVGL glue, and source-panel update logic. Optionally split into:
  - Touch and calibration (e.g. `touch.cpp` / `touch.h`),
  - Button and encoder handling (e.g. `input.cpp` / `input.h`),
  - Source-panel update logic (e.g. a function in display or a small `source_panel.cpp`),
  keeping `display.cpp` for display/LVGL init and high-level glue. Can be done incrementally.

---

## 3. Maintainability

- [ ] **Module headers – one-line purpose**  
  Add a short comment at the top of each module (e.g. “BQ25703A charger control over I2C”) so new readers see the purpose immediately.

- [ ] **objects_t documentation**  
  Document the meaning of `objects_t` fields (which label is V set, I set, which readout is which channel) in `lib/ui/screens.h` or in explanation.md under a “UI object map” subsection.

- [ ] **Recalibration without recompile**  
  `RECALIBRATE` in `include/display.h` (0/1) forces touch recalibration. Consider a compile-time flag (e.g. `-DRECALIBRATE_TOUCH`) or a CLI command (e.g. `cal` or `recal`) that clears the NVS calibration flag and restarts or re-runs calibration so recalibration does not require editing the header and recompiling.

---

## 4. Optional / future

- [ ] **Unit tests**  
  Add PlatformIO native or embedded tests for pure functions where possible: e.g. `voltageToPercentage(int millivolts)` and any new formatting helpers (`formatVoltage`, `formatCurrent`), so refactors don’t break behavior.

- [ ] **Measurement abstraction**  
  Optional thin abstraction over “read all measurements” (charger + INA3221) to a single structure or API. Helps with logging, mocking, or future data logging features.

- [ ] **Hardware doc**  
  Document I2C addresses and pinout in one place (e.g. explanation.md “Hardware and dependencies” or a dedicated `hardware.md`) and keep it in sync with code so new contributors have a single reference.
