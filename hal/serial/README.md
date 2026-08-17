# serial

## Overview

Arduino `HardwareSerial`-backed serial helper. Wraps any `HardwareSerial` instance (`Serial`, `Serial1`, `Serial2`, …), whether the underlying transport is a UART peripheral or a USB-CDC virtual COM port.

Instantiate `ArduinoSerial` in the application sketch and pass it into libraries via their constructors.

## API Summary

| Method | Description |
|---|---|
| `begin(baud)` | Initialize the wrapped `HardwareSerial` port at the given baud rate. |
| `write(byte)` | Transmit a single byte. Returns true on success. |
| `writeBytes(buf, len)` | Transmit `len` bytes from `buf`. Returns bytes written. |
| `writeString(str)` | Transmit a null-terminated string (without the null). |
| `available()` | Number of bytes waiting in the receive buffer. |
| `read()` | Read and consume the next byte. Returns `Result<uint8_t, Status>`. |
| `readBytes(buf, len)` | Read up to `len` bytes into `buf` without blocking. |
| `peek()` | Peek the next byte without consuming it. Returns `Result<uint8_t, Status>`. |
| `flush()` | Block until all pending transmit bytes have been sent. |

`read()` and `peek()` return a `Result<uint8_t, Status>` (see the `core` library): `Status::Error` means no byte was available.

## Usage Example

### Echo sketch

```cpp
#include <Arduino.h>
#include "serial.h"

ArduinoSerial serial(Serial);

void setup() {
    serial.begin(115200);
    serial.writeString("Serial ready\n");
}

void loop() {
    while (serial.available() > 0) {
        Result<uint8_t, Status> r = serial.read();
        if (r) {
            serial.write(r.value);
        }
    }
}
```

### Using a secondary port

```cpp
ArduinoSerial gpsPort(Serial2);

void setup() {
    gpsPort.begin(9600);
}
```

### Injecting into a library

```cpp
#include "serial.h"
#include "neo6m.h"

ArduinoSerial gpsSerial(Serial2);
Neo6M gps(gpsSerial);

void setup() {
    gpsSerial.begin(9600);
}
```

## Supported Platforms

ESP32, STM32, Arduino AVR, and any other Arduino-framework target providing `HardwareSerial`.
