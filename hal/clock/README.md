# clock

## Overview

Arduino-backed timing and delay helper wrapping `millis()`, `micros()`, `delay()`, and `delayMicroseconds()`. Instantiate once and inject into any driver that needs timing.

## API summary

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
#include "clock.h"

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

## Supported Platforms

Any Arduino-framework platform (ESP32, STM32, Arduino AVR, …).
