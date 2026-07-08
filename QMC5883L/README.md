# QMC5883L PlatformIO Driver

## Overview

Driver for the QMC5883L 3-axis magnetometer. Provides register-level
configuration (mode, output data rate, over/down sampling, range, set/reset
mode), soft-iron calibration via min/max collection, raw / normalized / Gauss
readings, and a compass azimuth helper.

## Hardware

- **Device address:** `0x2C` by default (`QMC5883L_I2C_ADDR_PIN_LOW`)
- **Communication protocol:** I2C
- **Required pins:** SDA, SCL

## Dependencies

- [`i2c_handler`](../../common/i2c_handler) — I2C register access helpers.

## Usage Example

```cpp
#include <Arduino.h>
#include "i2c_handler.h"
#include "qmc5883l.h"

QMC5883L qmc5883l;

void setup() {
    Serial.begin(115200);
    i2cInit(21, 22, 100000);
    qmc5883l.configureDefaults();
}

void loop() {
    if (qmc5883l.isDRDY()) {
        qmc5883l.read();
        Serial.println(qmc5883l.getX());
    }
    delay(200);
}
```

## Supported Platforms

- ESP32
- STM32
- Arduino AVR
- Any Arduino-framework platform with I2C support
