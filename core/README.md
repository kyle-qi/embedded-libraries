# core

## Overview

Shared utilities used across the embedded libraries. Header-only, no dependencies, C++11 compatible.

### `Result<T, E>` (result.h)

A zero-overhead wrapper pairing a returned `value` with a `status`. No heap, no exceptions, no RTTI.

- `value` — the returned value, meaningful only when the operation succeeded.
- `status` — the operation status. For `E = bool`, `true` means success.
- `operator bool()` / `ok()` — success check.

## Conventions

- Functions that return **data that can fail** return `Result<T, bool>`.
- Functions that only **perform an action** (setters, configuration) return plain `bool`.
- Predicate functions whose answer is itself a `bool` (e.g. `isDRDY()`) return `bool` directly. A hardware read failure maps to `false`.

The `E` parameter is `bool` for now. It is kept generic so it can later be replaced by a scoped enum for richer error reporting without changing call-site shape.

## Usage Example

```cpp
#include "result.h"

Result<uint8_t, bool> r = bus.read(addr, reg);
if (!r) {
    // handle failure
    return false;
}
uint8_t value = r.value;
```

### Producing a Result

```cpp
Result<int16_t, bool> readAxis() {
    uint8_t buf[2];
    if (!bus.readBytes(addr, reg, buf, 2)) {
        return {0, false};   // failure — value is a placeholder
    }
    int16_t val = assemble(buf);
    return {val, true};      // success
}
```
