# Neo6M PlatformIO Driver

## Overview

Driver for the u-blox NEO-6M GPS module. Reads NMEA sentences over UART and
parses `$GPGGA` messages to expose time, latitude/longitude (decimal degrees),
number of satellites, horizontal dilution, altitude, and geoid height. The most
recent raw sentence is retained for debugging.

## Hardware

- **Communication protocol:** UART (NMEA 0183)
- **Default baud rate:** 9600
- **Required pins:** RX, TX (module TX → MCU RX and vice versa)

## Usage Example

```cpp
#include <Arduino.h>
#include <HardwareSerial.h>
#include "neo_6m.h"

Neo6M neo6m(Serial1, 9600, 16, 17);

void setup() {
    Serial.begin(115200);
    neo6m.begin();
}

void loop() {
    while (neo6m.isDRDY()) {
        neo6m.read();
    }
    Serial.println(neo6m.getLatitude());
    Serial.println(neo6m.getLongitude());
    delay(200);
}
```

## Supported Platforms

- ESP32
- STM32
- Arduino AVR (boards providing a `HardwareSerial` port)
- Any Arduino-framework platform with a `HardwareSerial` UART
