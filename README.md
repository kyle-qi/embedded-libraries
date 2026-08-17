# embedded-libraries

A monorepo of reusable embedded hardware drivers, Arduino HAL helpers, and utilities for PlatformIO projects.

Each library is self-contained and follows the PlatformIO library layout, so any project can import individual libraries without copying source files.

## Design principles

- **Dependency injection** — Arduino HAL objects (`ArduinoI2C`, `ArduinoClock`, `ArduinoSerial`) are injected at construction time. Drivers do not create their own bus or port instances.
- **Arduino HAL** — `hal/` wraps Arduino `Wire`, `HardwareSerial`, and timing APIs. Sensor drivers talk to these helpers rather than MCU peripherals directly.
- **Explicit error handling** — functions that return data use `Result<T, Status>` (see `core`). Functions that only perform an action return `bool`. Failures are never silently swallowed.
- **No dynamic allocation** — all state is stack or member allocated. No `new`, no `malloc`.

## Repository structure

```
embedded-libraries/
├── core/                   Shared utilities
│   └── src/result.h        Result<T, E> — value-or-status wrapper for error handling
│
├── hal/                    Arduino HAL helpers
│   ├── clock/              ArduinoClock — timing and delay
│   ├── i2c/                ArduinoI2C — I2C register access + i2c_utils helpers
│   └── serial/             ArduinoSerial — serial byte-stream
│
└── sensors/                IC drivers
    ├── mpu6500/            MPU6500 — 6-axis IMU (accelerometer + gyroscope)
    ├── neo6m/              NEO-6M — GPS/GNSS module (NMEA parsing)
    └── qmc5883l/           QMC5883L — 3-axis magnetometer
```

## Layer dependencies

```
Application
     ↓
Sensor drivers        (sensors/)     depend on → hal/i2c, hal/clock, hal/serial, core
     ↓
Arduino HAL           (hal/)         depend on → core, Arduino framework
```

`ArduinoI2C`, `ArduinoClock`, and `ArduinoSerial` are instantiated in the application sketch and injected into drivers via their constructors.

## Using libraries in a PlatformIO project

Point `platformio.ini` at the repo categories with `lib_extra_dirs`:

```ini
lib_extra_dirs =
    path/to/embedded-libraries/core
    path/to/embedded-libraries/hal
    path/to/embedded-libraries/sensors
```

PlatformIO's Library Dependency Finder resolves cross-library includes automatically. Then include driver headers in your source:

```cpp
#include "i2c.h"
#include "clock.h"
#include "serial.h"
#include "qmc5883l.h"
#include "mpu6500.h"
#include "neo6m.h"
```

A minimal setup example:

```cpp
#include <Arduino.h>
#include "i2c.h"
#include "clock.h"
#include "qmc5883l.h"

ArduinoI2C  bus;
ArduinoClock clock;
QMC5883L    mag(bus, clock);

void setup() {
    bus.begin(21, 22, 400000);
    mag.configureDefaults();
}

void loop() {
    Result<bool, Status> dr = mag.isDRDY();
    if (dr && dr.value) {
        if (mag.read()) {
            float heading = mag.azimuth(mag.getX(), mag.getY());
        }
    }
}
```

## Error handling

Functions that return data use `Result<T, Status>`:

```cpp
Result<int16_t, Status> r = imu.readGyroX();
if (!r) {
    // Status::Error — bus failure
}
float gyroX = imu.getGyroX(); // use the scaled getter
```

Predicate functions like `isDRDY` use `Result<bool, Status>` so a bus failure is unambiguous:

```cpp
Result<bool, Status> dr = mag.isDRDY();
if (!dr) {
    // bus error — distinct from "not ready"
}
if (dr.value) {
    mag.read();
}
```

Functions that only perform an action return plain `bool`:

```cpp
if (!mag.configureDefaults()) {
    // configuration failed
}
```

`Status` has two values now — `Status::Ok` and `Status::Error`. It is a scoped enum so it can be extended with richer codes later without changing any call-site shape.

## Library summaries

| Library | Header | Description |
|---|---|---|
| `core` | `result.h` | `Result<T, E>` — zero-overhead value-or-status wrapper |
| `clock` | `clock.h` | Arduino timing and delay (`ArduinoClock`) |
| `i2c` | `i2c.h` | Arduino I2C register access + bit-field utilities (`ArduinoI2C`) |
| `serial` | `serial.h` | Arduino serial byte-stream (`ArduinoSerial`, UART or USB-CDC) |
| `mpu6500` | `mpu6500.h` | MPU6500 6-axis IMU driver — accel, gyro, temperature |
| `neo6m` | `neo6m.h` | NEO-6M GPS driver — GPGGA NMEA sentence parsing |
| `qmc5883l` | `qmc5883l.h` | QMC5883L magnetometer driver — Gauss readings, azimuth |

Each library has its own `README.md` with full API documentation and usage examples.
