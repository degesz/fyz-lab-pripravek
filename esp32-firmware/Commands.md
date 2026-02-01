# Serial CLI – User Manual

The device exposes a **serial command-line interface (CLI)** for remote control and debugging. Commands are sent over USB Serial and executed on the ESP32.

---

## Connection

- **Baud rate:** 115200
- **Line ending:** newline (e.g. Enter)
- **Port:** Use the USB serial port assigned to the ESP32-S2 (e.g. in PlatformIO: **Monitor** or `pio device monitor`).

After connecting, type a command and press Enter. The device echoes your input with a `#` prefix, then runs the command (or prints an error).

---

## Commands

### `ping`

**Syntax:** `ping`

**Description:** Simple connectivity check. The device replies with `Pong!`.

**Example:**
```
# ping
Pong!
```

---

### `source`

**Syntax:** `source [v <voltage>] [i <current>]`

**Description:** Set the programmable output voltage and/or current limit. Omit an argument to leave that value unchanged.

| Argument | Meaning        | Unit | Example |
|----------|----------------|------|---------|
| `v`      | Output voltage | V    | 5.0, 12 |
| `i`      | Current limit  | mA   | 500, 1000 |

- Voltage is applied only when the output is enabled (see `out`).
- Current limit is rounded to the hardware step (about 50 mA).

**Examples:**
```
# source v 5 i 500
# source v 12
# source i 1000
```

---

### `out`

**Syntax:** `out`

**Description:** Toggle the converter output on or off. When enabled, the output delivers the voltage and current limit set by `source` (and by the on-screen controls). When disabled, the output is off.

**Response:** The device prints either `Enabling output` or `Disabling output`.

**Example:**
```
# out
Enabling output
```

---

### `halt`

**Syntax:** `halt`

**Description:** Pause or resume the main loop. When paused, only the CLI keeps running (no display updates, no power management tick, etc.). Useful for debugging or when you want to drive everything from Serial.

**Response:** The device prints either `pausing loop` or `unpausing loop`.

**Example:**
```
# halt
pausing loop
# halt
unpausing loop
```

---

### `reset`

**Syntax:** `reset`

**Description:** Restart the ESP32. The device reboots and runs `setup()` again.

**Example:**
```
# reset
```

---

### `pd`

**Syntax:** `pd v <voltage>`

**Description:** Request a USB Power Delivery voltage from the STUSB4500. The device sets the requested PDO voltage (e.g. 5, 9, 12, 15, 20 V, depending on the supply) and triggers re-negotiation.

**Note:** Requires USB-PD hardware and `setup_usb()` to be called at boot. If USB-PD is not initialized, this command has no effect (see upgrades.md).

**Example:**
```
# pd v 9
Requesting USB-PD voltage: 9
```

---

## Errors

If a command is misspelled or arguments are invalid, the CLI prints an error and may suggest the correct command name. Fix the line and try again.

---

## Summary

| Command   | Purpose                          |
|-----------|----------------------------------|
| `ping`    | Connectivity check               |
| `source`  | Set output voltage/current       |
| `out`     | Toggle output on/off             |
| `halt`    | Pause/resume main loop           |
| `reset`   | Reboot device                    |
| `pd v <V>`| Request USB-PD voltage (if used) |
