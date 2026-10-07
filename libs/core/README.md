# core

## Overview

Shared utilities used across the embedded libraries. Header-only, no dependencies, C++11 compatible.

### `Status` enum (result.h)

The error type used as the `E` parameter in all `Result` instantiations across the repo.

```cpp
enum class Status : uint8_t {
    Ok    = 0,
    Error = 1,
};
```

Currently thin by design. Extend with richer values (e.g. `BusNack`, `Timeout`, `InvalidArg`) when finer-grained error reporting is needed — no call-site shape changes required.

### `Result<T, E>` (result.h)

A zero-overhead struct pairing a returned `value` with a `status`. No heap, no exceptions, no RTTI.

- `value` — the returned value, meaningful only when `status == Status::Ok`.
- `status` — the operation status (`Status::Ok` or `Status::Error`).
- `operator bool()` / `ok()` — true when `status == Status::Ok`.

## Conventions

- Functions returning **data that may fail** → `Result<T, Status>`
- Functions returning a **flag that may fail** (predicates like `isDRDY`) → `Result<bool, Status>`:
  - `!r` → bus/transport error (distinct from "flag is false")
  - `r.value` → the actual flag (only valid when `r.ok()`)
- Functions that only **perform an action** (setters, config) → plain `bool`

## Usage Example

```cpp
#include "result.h"

// Reading data:
Result<uint8_t, Status> r = bus.read(addr, reg);
if (!r) {
    // Status::Error — handle bus failure
    return;
}
uint8_t val = r.value; // safe to use
```

### Reading a predicate flag

```cpp
Result<bool, Status> dr = sensor.isDRDY();
if (!dr) {
    // bus error — explicitly distinct from "not ready"
    return;
}
if (dr.value) {
    // data is actually ready
}
```

### Producing a Result

```cpp
Result<int16_t, Status> readAxis() {
    uint8_t buf[2];
    if (!bus.readBytes(addr, reg, buf, 2)) {
        return {0, Status::Error};
    }
    return {assemble(buf), Status::Ok};
}
```
