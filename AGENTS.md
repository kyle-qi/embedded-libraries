# AGENTS.md

## Purpose

This repository contains reusable embedded libraries for:

* Arduino HAL helpers (I2C, UART/serial, timing)
* Sensor drivers
* Integrated circuit (IC) drivers
* Utility libraries

The goal is to create modular, composable libraries that can be reused across Arduino-framework PlatformIO projects.

---

# Core Principles

## Greenfield Development

This codebase is greenfield.

Do **not** introduce complexity solely for backward compatibility or legacy support. Favor clean APIs and sound architecture over preserving old interfaces.

If an interface should change, change it rather than layering compatibility shims on top.

---

## Separation of Responsibilities

Each library should have one well-defined responsibility.

Examples:

* Communication libraries know how to communicate.
* Device drivers know how to operate a device.
* Platform layers know how to access hardware.
* Application logic belongs outside of reusable libraries.

Avoid mixing responsibilities across layers.

---

## Layer Independence

Higher-level libraries should depend only on lower-level abstractions.

For example:

```
Application
      ↓
Sensor Driver
      ↓
Arduino HAL (I2C / Serial / Clock)
      ↓
Arduino framework
```

A communication library should never know which sensor is using it.

A sensor library should talk to HAL helpers (`ArduinoI2C`, `ArduinoSerial`, `ArduinoClock`) rather than MCU peripherals or Arduino globals directly.

---

## Dependency Injection

Dependencies should be supplied from the outside whenever practical.

For example, a sensor driver should receive an `ArduinoI2C` instance rather than instantiate one itself.

Avoid global objects and hidden dependencies.

---

## Hardware Access

Device drivers should not directly access MCU peripherals.

Instead, communicate through the Arduino HAL helpers in `hal/` (`ArduinoI2C`, `ArduinoSerial`, `ArduinoClock`).

HAL libraries target the Arduino framework (ESP32, STM32, AVR, and other Arduino-framework boards). Do not introduce platform-agnostic virtual interfaces unless there is a concrete need.

---

## Composition over Inheritance

Prefer composing small reusable components over creating deep inheritance hierarchies.

Interfaces should remain small and focused.

---

## Keep Public APIs Small

Every public API becomes long-term maintenance.

Expose only functionality that users actually need.

Prefer fewer, clearer functions over large configurable interfaces.

---

## Error Handling

Libraries should never silently ignore failures.

Functions should communicate errors explicitly using return values or well-defined error types.

Avoid hidden side effects.

---

## Minimize Dynamic Allocation

Embedded systems often have constrained resources.

Avoid heap allocation unless it is clearly justified.

Prefer stack allocation and deterministic memory usage.

---

## Deterministic Behavior

Library behavior should be predictable.

Avoid:

* hidden background tasks
* implicit timing assumptions
* unexpected blocking operations

If a function blocks or performs lengthy work, document it explicitly.

---

## Documentation

Public APIs should explain:

* what they do
* expected inputs
* ownership rules
* error conditions
* timing or performance considerations where relevant

Code should primarily explain *why*, not *what*.

---

## Testing

Where practical:

* isolate hardware-independent logic for unit testing
* separate protocol parsing from hardware communication

Design for testability from the beginning.

---

## Code Style

Prefer:

* readable code over clever code
* explicit behavior over implicit behavior
* descriptive names over abbreviations
* small focused functions
* const-correctness where applicable

Avoid unnecessary abstraction.

Do not optimize prematurely.

---

## Adding New Libraries

When creating a new library:

1. Keep its responsibility narrow.
2. Minimize dependencies.
3. Reuse existing abstractions instead of duplicating functionality.
4. Design the public API before implementing internals.
5. Target the Arduino framework unless there is a reason not to.

---

## When Unsure

When faced with multiple valid designs, prefer the one that:

1. reduces coupling,
2. keeps APIs smaller,
3. is easier to test,
4. is easier to understand six months from now.
