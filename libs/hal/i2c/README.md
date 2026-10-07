# i2c_handler

## Overview

Arduino `Wire`-backed I2C helper for register-based device drivers.

- **`ArduinoI2C`** (`i2c.h`) — write and read device registers over I2C.
- **`i2c::writeMasked` / `i2c::writeBit`** (`i2c_utils.h`) — read-modify-write helpers.

Instantiate `ArduinoI2C` in the application sketch and pass it into drivers via their constructors.

## Hardware

- **Communication protocol:** I2C (via the Arduino `Wire` library)
- **Bus roles:** MCU acts as the I2C controller
- **Required pins:** SDA and SCL (platform-dependent; e.g. GPIO21/GPIO22 on ESP32)

## API summary

| Method | Description |
|---|---|
| `begin(sdaPin, sclPin, frequency)` | Initialize the `Wire` bus. |
| `write(addr, reg, data)` | Write a full register byte. |
| `writeBytes(addr, reg, buf, len)` | Write `len` consecutive bytes starting at `reg`. |
| `read(addr, reg)` | Read a single register byte. Returns `Result<uint8_t, Status>`. |
| `readBytes(addr, reg, buf, len)` | Read `len` consecutive bytes into `buf`. |

`read()` returns a `Result<uint8_t, Status>` (see the `core` library): `Status::Error` means the transaction failed (NACK, timeout, or no data).

Masked/bit helpers in `i2c_utils.h`:

| Function | Description |
|---|---|
| `i2c::writeMasked(bus, addr, reg, data, mask)` | Read-modify-write masked bits. |
| `i2c::writeBit(bus, addr, reg, bit, bitPos)` | Set or clear a single bit. |

## Usage Example

```cpp
#include <Arduino.h>
#include "i2c.h"
#include "mpu6500.h"

ArduinoI2C bus;
MPU6500 imu(bus);

void setup() {
    Serial.begin(115200);
    bus.begin(21, 22, 100000);
    imu.configureDefaults();
}
```

## Supported Platforms

Any Arduino-framework platform providing the `Wire` library (ESP32, STM32, Arduino AVR, …).
