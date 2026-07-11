# clock

## Overview

Provides a platform-agnostic clock abstraction for HAL and sensor drivers. The library has two parts:

- **`IClock`** (`clock.h`) — pure abstract interface that drivers depend on. No platform headers. Suitable for mocking in host-side tests.
- **`ArduinoClock`** (`clock_arduino.h`) — concrete Arduino-backed implementation wrapping `millis()`, `micros()`, `delay()`, and `delayMicroseconds()`. Instantiated in the application sketch and injected into drivers via their constructors.

## API summary

### `IClock` (clock.h)

| Method | Description |
|---|---|
| `millis()` | Milliseconds since startup (wraps at ~49.7 days). |
| `micros()` | Microseconds since startup (wraps at ~71.6 minutes). |
| `delayMs(ms)` | Block for at least `ms` milliseconds. |
| `delayUs(us)` | Block for at least `us` microseconds. |
| `elapsed(lastMs, intervalMs)` | Returns true once `intervalMs` has passed since `lastMs`. Handles rollover. |

## Usage Example

### Blocking delay

```cpp
#include <Arduino.h>
#include "clock_arduino.h"

ArduinoClock clk;

void setup() {
    clk.delayMs(100); // wait for sensor power-up
}
```

### Non-blocking periodic execution

```cpp
ArduinoClock clk;
uint32_t lastRead = 0;

void loop() {
    if (clk.elapsed(lastRead, 50)) {   // every 50 ms
        lastRead = clk.millis();
        // read sensor...
    }
}
```

### Implementing a mock for host-side testing

```cpp
#include "clock.h"

class MockClock : public IClock {
public:
    uint32_t now = 0;

    uint32_t millis()  override { return now; }
    uint32_t micros()  override { return now * 1000; }
    void delayMs(uint32_t ms) override { now += ms; }
    void delayUs(uint32_t us) override { now += us / 1000; }
    bool elapsed(uint32_t last, uint32_t interval) override {
        return (now - last) >= interval;
    }
};
```

## Supported Platforms

Any platform with an `IClock`-compatible implementation. `ArduinoClock` supports ESP32, STM32, Arduino AVR, and any other Arduino-framework target.
