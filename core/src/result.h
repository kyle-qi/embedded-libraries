#pragma once

#include <stdint.h>

/**
 * @file result.h
 * @brief Lightweight value-or-status wrapper for error handling.
 *
 * Result pairs a returned @p value with a @p status that reports whether the
 * operation succeeded. It is a zero-overhead aggregate: no heap, no
 * exceptions, no RTTI, and C++11 compatible — suitable for constrained
 * embedded targets.
 *
 * The second template parameter is the status type. For now it is expected to
 * be `bool` (true = success). It is left generic so it can later be replaced by
 * a scoped enum for richer error reporting without changing call-site shape.
 *
 * Usage:
 * @code
 * Result<uint8_t, bool> r = bus.read(addr, reg);
 * if (!r) {
 *     // handle failure
 *     return false;
 * }
 * uint8_t value = r.value;
 * @endcode
 *
 * Functions that return data which may fail should return a Result. Functions
 * that only perform an action (setters, configuration) should return a plain
 * status (bool). Predicate functions whose answer is itself a bool should
 * return bool directly to avoid ambiguity.
 */
template <typename T, typename E>
struct Result {
    /**
     * @brief The returned value. Only meaningful when the operation succeeded.
     */
    T value;

    /**
     * @brief The operation status. For E = bool, true means success.
     */
    E status;

    /**
     * @brief Contextual conversion to bool for `if (result)` checks.
     *
     * @return true if the operation succeeded.
     */
    explicit operator bool() const { return static_cast<bool>(status); }

    /**
     * @brief Explicit success query.
     *
     * @return true if the operation succeeded.
     */
    bool ok() const { return static_cast<bool>(status); }
};
