# i2c_handler

## Overview

`i2c_handler` is a small set of Arduino `Wire`-based helper functions plus
bit-field utilities used by register-driven I2C device drivers. It centralizes
common operations such as initializing the bus, scanning for devices, and
reading/writing registers (including masked and single-bit writes) so that
individual sensor drivers don't have to reimplement them.

## Hardware

- **Communication protocol:** I2C (via the Arduino `Wire` library)
- **Bus roles:** MCU acts as the I2C controller
- **Required pins:** SDA and SCL (platform-dependent; e.g. GPIO21/GPIO22 on ESP32)

## API summary

- `i2cInit(sdaPin, sclPin, frequency)` — start the I2C bus.
- `i2cScan()` — probe addresses `0x01`–`0x7E` and report responders.
- `i2cWrite(addr, reg, data)` — write a full register byte.
- `i2cWrite(addr, reg, data, mask)` — read-modify-write masked bits.
- `i2cWriteBit(addr, reg, bit, bitPos)` — set/clear a single bit.
- `i2cRead(addr, reg)` — read a single register byte.
- `i2cReadTwo(addr, lsbReg, msbReg)` — read two bytes into a 16-bit value.
- `i2c_field::merge_masked(...)` / `i2c_field::read_shifted(...)` — bit-field helpers.

## Usage Example

```cpp
#include <Arduino.h>
#include "i2c_handler.h"

void setup() {
    Serial.begin(115200);
    i2cInit(21, 22, 100000);
    i2cScan();
}

void loop() {
    uint8_t value = i2cRead(0x68, 0x00);
    Serial.println(value, HEX);
    delay(1000);
}
```

> `i2c_handler.h` relies on Arduino integer types being available. Include
> `<Arduino.h>` (directly or transitively) before this header, as the sensor
> drivers in this repo do.

## Supported Platforms

- ESP32
- STM32
- Arduino AVR
- Any Arduino-framework platform providing the `Wire` library
