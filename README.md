# embedded-libraries

A centralized monorepo of reusable embedded hardware drivers, hardware
abstractions, and utilities for PlatformIO projects.

Each library is self-contained and follows the PlatformIO library layout, so it
can be imported independently by any project without copying source code.

## Structure

```
embedded-libraries/
├── common/          Shared embedded utilities (I2C helpers, etc.)
│   └── i2c_handler/
├── sensors/         IC-specific sensor drivers
│   ├── QMC5883L/    Magnetometer driver
│   └── MPU6500/     Accelerometer / gyroscope driver
├── gps/             GNSS receiver drivers
│   └── Neo6M/
└── displays/        Display drivers (reserved for future libraries)
```

## Using a library in a PlatformIO project

Point your project's `platformio.ini` at the relevant category folders using
`lib_extra_dirs` for local development:

```ini
lib_extra_dirs =
    embedded-libraries/common
    embedded-libraries/sensors
    embedded-libraries/gps
```

Then include the driver headers directly in your source:

```cpp
#include "qmc5883l.h"
#include "mpu6500.h"
#include "neo_6m.h"
```

PlatformIO's Library Dependency Finder resolves cross-library includes (for
example, the sensor drivers depend on `i2c_handler` from `common`).

## Library responsibilities

Libraries in this repo contain reusable hardware abstraction only:

- Register definitions
- Device initialization
- Communication protocols
- Sensor calibration
- Hardware abstraction
- Raw data conversion

Application-level concerns (state machines, UIs, control algorithms, sensor
fusion, and business logic) belong in the individual project repositories.
