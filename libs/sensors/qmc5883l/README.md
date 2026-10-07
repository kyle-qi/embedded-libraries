# QMC5883L

## Overview

Driver for the QMC5883L 3-axis magnetometer. Provides register-level
configuration, soft-iron calibration via min/max collection, and raw /
normalized / Gauss readings.

## Hardware

- **Device address:** `0x2C` (default, `QMC5883L_I2C_ADDR_PIN_LOW`)
- **Communication protocol:** I2C
- **Required pins:** SDA, SCL

## Dependencies

- [`i2c_handler`](../../hal/i2c) — provides `ArduinoI2C`.
- [`clock`](../../hal/clock) — provides `ArduinoClock`.

## Usage Example

```cpp
#include <Arduino.h>
#include "i2c.h"
#include "clock.h"
#include "qmc5883l.h"

ArduinoI2C   bus;
ArduinoClock clk;
QMC5883L     mag(bus, clk);

void setup() {
    Serial.begin(115200);
    bus.begin(21, 22, 100000);

    if (!mag.configureDefaults()) {
        Serial.println("QMC5883L failed to initialize!");
    }
}

void loop() {
    Result<bool, Status> dr = mag.isDRDY();
    if (dr && dr.value) {
        if (mag.read()) {
            Serial.print("X: "); Serial.print(mag.getX());
            Serial.print(" Y: "); Serial.print(mag.getY());
            Serial.print(" Z: "); Serial.println(mag.getZ());
        }
    }
    delay(200);
}
```

## Supported Platforms

ESP32, STM32, Arduino AVR, and any other Arduino-framework target with `Wire`.
