# i2c_handler

## Overview

Provides a platform-agnostic I2C abstraction for use by sensor and device
drivers. The library has two parts:

- **`II2C`** (`i2c_interface.h`) — pure abstract interface that drivers depend on.
  No platform headers, no `Wire.h`. Suitable for mocking in host-side tests.
- **`ArduinoI2C`** (`arduino_i2c.h`) — concrete `Wire`-backed implementation of
  `II2C` for Arduino-framework targets. Instantiated in the application sketch
  and injected into drivers via their constructors.

## Hardware

- **Communication protocol:** I2C (via the Arduino `Wire` library)
- **Bus roles:** MCU acts as the I2C controller
- **Required pins:** SDA and SCL (platform-dependent; e.g. GPIO21/GPIO22 on ESP32)

## API summary

### `II2C` (i2c_interface.h)

| Method | Description |
|---|---|
| `write(addr, reg, data)` | Write a full register byte. |
| `writeMasked(addr, reg, data, mask)` | Read-modify-write masked bits. |
| `writeBit(addr, reg, bit, bitPos)` | Set or clear a single bit. |
| `read(addr, reg)` | Read a single register byte. Returns `Result<uint8_t, bool>`. |
| `readBytes(addr, reg, buf, len)` | Read `len` consecutive bytes into `buf`. |

`read()` returns a `Result<uint8_t, bool>` (see the `core` library): a `false` status means the transaction failed (NACK, timeout, or no data).

### `ArduinoI2C` (arduino_i2c.h)

Inherits the full `II2C` interface above, plus:

| Method | Description |
|---|---|
| `begin(sdaPin, sclPin, frequency)` | Initialize the `Wire` bus. |

## Usage Example

```cpp
#include <Arduino.h>
#include "arduino_i2c.h"
#include "mpu6500.h"

ArduinoI2C bus;
MPU6500 imu(bus);

void setup() {
    Serial.begin(115200);
    bus.begin(21, 22, 100000);
    imu.configureDefaults();
}
```

### Implementing a mock for host-side testing

```cpp
#include "i2c.h"

class MockI2C : public II2C {
public:
    uint8_t registers[256] = {};

    bool write(uint8_t, uint8_t reg, uint8_t data) override {
        registers[reg] = data;
        return true;
    }

    bool writeBytes(uint8_t, uint8_t reg, const uint8_t* buf, uint8_t len) override {
        for (uint8_t i = 0; i < len; ++i) registers[reg + i] = buf[i];
        return true;
    }

    Result<uint8_t, bool> read(uint8_t, uint8_t reg) override {
        return {registers[reg], true};
    }

    bool readBytes(uint8_t, uint8_t reg, uint8_t* buf, uint8_t len) override {
        for (uint8_t i = 0; i < len; ++i) buf[i] = registers[reg + i];
        return true;
    }
};
```

The masked/bit helpers (`writeMasked`, `writeBit`) are free functions in `i2c_utils.h` built on top of this interface — they work with any `II2C` implementation, including this mock, without needing to be overridden.

## Supported Platforms

- ESP32
- STM32
- Arduino AVR
- Any Arduino-framework platform providing the `Wire` library
