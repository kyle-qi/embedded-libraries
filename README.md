# embedded-libraries

A monorepo for my embedded projects: reusable hardware drivers, Arduino HAL helpers, and utilities under `libs/`, and the PlatformIO applications that use them under `apps/`.

Every library follows the PlatformIO library layout. Apps link libraries in place, so a change under `libs/` is picked up by every app on its next build, and a library change and the app changes it requires can land in one commit.

## Design principles

- **Dependency injection** — Arduino HAL objects (`ArduinoI2C`, `ArduinoClock`, `ArduinoSerial`) are injected at construction time. Drivers do not create their own bus or port instances.
- **Arduino HAL** — `libs/hal/` wraps Arduino `Wire`, `HardwareSerial`, and timing APIs. Sensor drivers talk to these helpers rather than MCU peripherals directly.
- **Explicit error handling** — functions that return data use `Result<T, Status>` (see `core`). Functions that only perform an action return `bool`. Failures are never silently swallowed.
- **No dynamic allocation** — all state is stack or member allocated. No `new`, no `malloc`.
- **Libraries never depend on apps** — dependencies only point from `apps/` into `libs/`, and from higher library layers into lower ones.

## Repository structure

```
embedded-libraries/
├── libs/                       Reusable libraries
│   ├── core/                   Result<T, E> — value-or-status wrapper for error handling
│   ├── hal/                    Arduino HAL helpers
│   │   ├── clock/              ArduinoClock — timing and delay
│   │   ├── i2c/                ArduinoI2C — I2C register access + i2c_utils helpers
│   │   └── serial/             ArduinoSerial — serial byte-stream
│   ├── sensors/                IC drivers
│   │   ├── mpu6500/            MPU6500 — 6-axis IMU (accelerometer + gyroscope)
│   │   ├── neo6m/              NEO-6M — GPS/GNSS module (NMEA parsing)
│   │   └── qmc5883l/           QMC5883L — 3-axis magnetometer
│   └── fusion/                 Sensor fusion
│       └── nine_dof/           9-DOF Kalman filter (work in progress, not yet buildable)
│
├── apps/                       PlatformIO applications, one project per folder
│   └── beer-compass/           Compass that points to the nearest liquor store
│
└── free/                       Miscellaneous scripts
```

## Layer dependencies

```
Application           (apps/)              depends on → any library below
     ↓
Sensor drivers        (libs/sensors/)      depend on  → libs/hal/*, libs/core
     ↓
Arduino HAL           (libs/hal/)          depend on  → libs/core, Arduino framework
```

`ArduinoI2C`, `ArduinoClock`, and `ArduinoSerial` are instantiated in the application and injected into drivers via their constructors.

## Apps

| App | Target | Description |
|---|---|---|
| [`beer-compass`](apps/beer-compass) | ESP32 DevKit V1 | Compass that always points to the nearest liquor store |

Each app is a standalone PlatformIO project. Build or upload one from the repo root with `-d`, or open its folder in VS Code with the PlatformIO extension:

```bash
pio run -d apps/beer-compass               # build
pio run -d apps/beer-compass -t upload     # flash
pio device monitor -d apps/beer-compass    # serial monitor
```

### Adding an app

1. Create `apps/<app-name>/` with the usual PlatformIO layout (`platformio.ini`, `src/`, `include/`).
2. Link the libraries it uses in `lib_deps` with `symlink://` paths relative to the app folder (see below).
3. Add a row to the table above.

## Using libraries

Apps link libraries with `symlink://` entries in `lib_deps`. The paths are relative to the app's `platformio.ini`:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
lib_deps =
    symlink://../../libs/core
    symlink://../../libs/hal/clock
    symlink://../../libs/hal/i2c
    symlink://../../libs/sensors/qmc5883l
```

List every library the app needs, including indirect ones. For example, `qmc5883l` includes `i2c.h`, `clock.h`, and `result.h`, so `hal/i2c`, `hal/clock`, and `core` must be listed too. A missing entry fails the build with a missing-header error.

Library manifests intentionally do not declare internal `dependencies`. PlatformIO resolves name-only dependencies against the PlatformIO Registry when they are not already installed. A generic name like `clock` silently pulls in an unrelated registry package.

Projects outside this repo can use the same `symlink://` entries with a path to a local clone. `lib_extra_dirs` is deprecated in PlatformIO 6 and is not used here.

A minimal setup:

```cpp
#include <Arduino.h>
#include "i2c.h"
#include "clock.h"
#include "qmc5883l.h"

ArduinoI2C   bus;
ArduinoClock clk;   // not `clock`, which collides with clock() from <time.h>
QMC5883L     mag(bus, clk);

void setup() {
    Serial.begin(115200);
    if (!bus.begin(21, 22, 400000) || !mag.configureDefaults()) {
        Serial.println("Magnetometer setup failed");
    }
}

void loop() {
    Result<bool, Status> dr = mag.isDRDY();
    if (dr && dr.value && mag.read()) {
        Serial.println(mag.getXGauss());
    }
}
```

### Building a library example

Library examples are not part of any app. Build one with `pio ci`, passing the library and its dependencies with `--lib`:

```bash
pio ci libs/sensors/qmc5883l/examples \
    --lib libs/core --lib libs/hal/clock --lib libs/hal/i2c --lib libs/sensors/qmc5883l \
    --board esp32doit-devkit-v1
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

`Status` currently has two values, `Status::Ok` and `Status::Error`. It is a scoped enum so it can be extended with richer codes later without changing any call-site shape.

## Library summaries

| Path | Library name | Header | Description |
|---|---|---|---|
| `libs/core` | `core` | `result.h` | `Result<T, E>` — zero-overhead value-or-status wrapper |
| `libs/hal/clock` | `clock` | `clock.h` | Arduino timing and delay (`ArduinoClock`) |
| `libs/hal/i2c` | `i2c_handler` | `i2c.h` | Arduino I2C register access + bit-field utilities (`ArduinoI2C`) |
| `libs/hal/serial` | `serial` | `serial.h` | Arduino serial byte-stream (`ArduinoSerial`, UART or USB-CDC) |
| `libs/sensors/mpu6500` | `MPU6500` | `mpu6500.h` | MPU6500 6-axis IMU driver — accel, gyro, temperature |
| `libs/sensors/neo6m` | `Neo6M` | `neo6m.h` | NEO-6M GPS driver — GPGGA NMEA sentence parsing |
| `libs/sensors/qmc5883l` | `QMC5883L` | `qmc5883l.h` | QMC5883L magnetometer driver — configuration, Gauss readings |
| `libs/fusion/nine_dof` | — | `nine_dof.h` | 9-DOF Kalman filter (work in progress) |

Each library in `core`, `hal`, and `sensors` has its own `README.md` with full API documentation and usage examples.
