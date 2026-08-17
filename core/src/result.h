#pragma once

#include <stdint.h>

/**
 * @file result.h
 * @brief Lightweight value-or-status wrapper for error handling.
 *
 * Provides two types:
 *
 * - `Status` - a scoped enum used as the error type in all `Result`
 *   instantiations across this repo. Currently has two values (`Ok` and
 *   `Error`) for a thin but unambiguous implementation. Replace the enum
 *   values with richer codes (e.g. `BusNack`, `Timeout`) when needed
 *   without changing any call-site shape.
 *
 * - `Result<T, E>` - pairs a returned `value` with a `status`. Zero
 *   overhead: no heap, no exceptions, no RTTI, C++11 compatible.
 *
 * ## Conventions
 *
 * - Functions returning **data that may fail** -> `Result<T, Status>`
 * - Functions returning a **flag that may fail** (predicates like `isDRDY`)
 *   -> `Result<bool, Status>`:
 *     - `!r` or `r.status != Status::Ok` -> transport/hardware error
 *     - `r.value` -> the actual flag value (only meaningful when `r.ok()`)
 * - Functions that only **perform an action** (setters, config) -> plain `bool`
 *
 * ## Usage
 *
 * @code
 * // Reading data:
 * Result<uint8_t, Status> r = bus.read(addr, reg);
 * if (!r) { return; }          // bus error
 * uint8_t val = r.value;       // safe to use
 *
 * // Reading a flag:
 * Result<bool, Status> dr = sensor.isDRDY();
 * if (!dr) { return; }         // bus error, not "not ready"
 * if (dr.value) { ... }        // data is ready
 * @endcode
 */

/**
 * @brief Operation status. Used as the `E` parameter in all `Result` types.
 *
 * Extend with additional values (e.g. `BusNack`, `Timeout`, `InvalidArg`)
 * when finer-grained error reporting is needed - no call-site shape changes
 * required.
 */
enum class Status : uint8_t {
    Ok    = 0,  ///< Operation completed successfully.
    Error = 1,  ///< Operation failed. Details TBD.
};

/**
 * @brief Lightweight value-or-status aggregate.
 *
 * @tparam T  The value type. Only meaningful when `status == Status::Ok`.
 * @tparam E  The status type (use `Status`).
 */
template <typename T, typename E>
struct Result {
    /**
     * @brief The returned value. Only meaningful when the operation succeeded.
     */
    T value;

    /**
     * @brief The operation status.
     */
    E status;

    /**
     * @brief Contextual conversion to bool - true if the operation succeeded.
     *
     * Allows `if (result)` and `if (!result)` idioms.
     */
    explicit operator bool() const { return status == static_cast<E>(0); }

    /**
     * @brief Explicit success query.
     *
     * @return true if the operation succeeded.
     */
    bool ok() const { return static_cast<bool>(*this); }
};
