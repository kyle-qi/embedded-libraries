# serial

## Overview

Provides a platform-agnostic serial abstraction for HAL and library code. The library has two parts:

- **`ISerial`** (`serial.h`) — pure abstract interface that libraries depend on. Transport-agnostic: can be backed by a hardware UART, a USB-CDC virtual COM port, or any other byte stream. No platform headers. Suitable for mocking in host-side tests.
- **`ArduinoSerial`** (`serial_arduino.h`) — concrete `HardwareSerial`-backed implementation for Arduino-framework targets. Wraps any `HardwareSerial` instance (`Serial`, `Serial1`, `Serial2`, …). Instantiated in the application sketch and injected into libraries via their constructors.

## API Summary

### `ISerial` (serial.h)

| Method | Description |
|---|---|
| `write(byte)` | Transmit a single byte. Returns true on success. |
| `writeBytes(buf, len)` | Transmit `len` bytes from `buf`. Returns bytes written. |
| `writeString(str)` | Transmit a null-terminated string (without the null). |
| `available()` | Number of bytes waiting in the receive buffer. |
| `read()` | Read and consume the next byte from the receive buffer. |
| `readBytes(buf, len)` | Read up to `len` bytes into `buf` without blocking. |
| `peek()` | Read the next byte without consuming it. Returns -1 if empty. |
| `flush()` | Block until all pending transmit bytes have been sent. |

### `ArduinoSerial` (serial_arduino.h)

Inherits the full `ISerial` interface above, plus:

| Method | Description |
|---|---|
| `begin(baud)` | Initialize the wrapped `HardwareSerial` port at the given baud rate. |

## Usage Example

### Echo sketch

```cpp
#include <Arduino.h>
#include "serial_arduino.h"

ArduinoSerial serial(Serial);

void setup() {
    serial.begin(115200);
    serial.writeString("Serial ready\n");
}

void loop() {
    while (serial.available() > 0) {
        serial.write(serial.read());
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
#include "serial_arduino.h"
#include "neo6m.h"

ArduinoSerial gpsSerial(Serial2);
NEO6M gps(gpsSerial);

void setup() {
    gpsSerial.begin(9600);
    gps.begin();
}
```

### Implementing a mock for host-side testing

```cpp
#include "serial.h"

class MockSerial : public ISerial {
public:
    uint8_t txBuf[256] = {};
    uint8_t txLen      = 0;
    uint8_t rxBuf[256] = {};
    uint8_t rxLen      = 0;
    uint8_t rxPos      = 0;

    bool write(uint8_t byte) override {
        txBuf[txLen++] = byte;
        return true;
    }

    uint8_t writeBytes(const uint8_t* buf, uint8_t len) override {
        for (uint8_t i = 0; i < len; ++i) txBuf[txLen++] = buf[i];
        return len;
    }

    bool writeString(const char* str) override {
        while (*str) txBuf[txLen++] = static_cast<uint8_t>(*str++);
        return true;
    }

    uint8_t available() override { return rxLen - rxPos; }

    uint8_t read() override {
        return (rxPos < rxLen) ? rxBuf[rxPos++] : 0xFF;
    }

    uint8_t readBytes(uint8_t* buf, uint8_t len) override {
        uint8_t n = 0;
        while (n < len && rxPos < rxLen) buf[n++] = rxBuf[rxPos++];
        return n;
    }

    int16_t peek() override {
        return (rxPos < rxLen) ? rxBuf[rxPos] : -1;
    }

    void flush() override {}
};
```

## Supported Platforms

Any platform with an `ISerial`-compatible implementation. `ArduinoSerial` supports ESP32, STM32, Arduino AVR, and any other Arduino-framework target providing `HardwareSerial` — whether the underlying transport is a UART peripheral or a USB-CDC virtual COM port.
