# AGENTS.md

## Project Overview

This repository contains the chassis firmware for a RoboMaster robot. It targets an STM32F407 and uses FreeRTOS, STM32 HAL, CMake, Ninja, ARM GCC, and CMSIS-DSP. The chassis and gimbal use separate controller boards and communicate over CAN.

## Repository Layout

- `Core/Src/` and `Core/Inc/`: CubeMX-generated initialization, interrupts, FreeRTOS setup, and board-level glue.
- `Hardware/`: hardware drivers and reusable control modules, including motors, CAN, remote control, referee data, filters, and VOFA output.
- `Task/`: application and FreeRTOS task logic.
- `Bmi088/`: BMI088 driver and attitude-estimation code.
- `UI/`: referee-system UI drawing code.
- `Drivers/` and `Middlewares/`: STM32 HAL, CMSIS, FreeRTOS, and USB dependencies.
- `cmake/`: ARM toolchains and CubeMX-generated CMake integration.
- `DSP/`: CMSIS-DSP headers and libraries.

## Build

- Configure Debug: `cmake --preset Debug`
- Build Debug: `cmake --build --preset Debug`
- Configure/build Release by replacing `Debug` with `Release`.
- `build_flash.ps1` builds and flashes the target; do not run it unless the developer explicitly requests flashing.
- The CMSIS-DSP library is expected under the STM32Cube X-CUBE-ALGOBUILD installation configured in `CMakeLists.txt`.

## Code Conventions

- Preserve the encoding of every file. Existing Chinese comments may use GBK/GB2312; do not silently convert files to UTF-8.
- Write comments in Chinese and keep them concise.
- Match existing C style and keep each module as an `xxx.c`/`xxx.h` pair.
- Use `xxx_T` or `xxx_t` for types, `g_` for globals, `s_` for file-local state where consistent, and `UPPER_SNAKE_CASE` for macros.
- Add `U` to unsigned integer constants and `f` to floating-point constants where applicable.
- Prefer short, single-purpose functions. Use enum-backed `switch` state machines instead of deeply nested conditionals for control logic.
- Reuse existing PID, motor, filter, angle-estimation, and offline-detection modules before adding new implementations.
- Put reusable hardware-facing code in `Hardware/` and application/task behavior in `Task/`.
- Avoid adding third-party dependencies beyond the existing HAL/CMSIS stack.
- For hot floating-point paths, prefer existing CMSIS-DSP operations and avoid unnecessary division.

## FreeRTOS And Safety

- Follow the existing initialization order: HAL and peripherals, driver initialization, task creation, then scheduler startup.
- Keep task timing explicit; the main chassis, INS, and control loops commonly run at 1 ms intervals.
- Never introduce a motor output without a clear enable condition, offline/failsafe behavior, and applicable mechanical limits.
- Preserve remote-control timeout handling, joystick dead zones, and safe output behavior when communications are lost.
- Treat current, speed, angle, and timing units explicitly in comments and interfaces (`mA`, `rpm`, degrees, degrees/s, `ms`).

## Generated And Sensitive Areas

- Make minimal edits to CubeMX-generated files under `Core/`, `Drivers/`, `Middlewares/`, and `cmake/stm32cubemx/`; prefer user-code sections or project-owned modules where possible.
- Do not edit generated build output under `build/`.
- Do not modify linker scripts, startup assembly, `.ioc` configuration, or precompiled libraries unless the task explicitly requires it.

## Working Rules

- Reply to the developer in Chinese; keep identifiers and API names unchanged.
- Before changing code across multiple files, describe the intended files, behavior, and reason, and obtain developer approval.
- Keep changes scoped to the request and do not perform unrelated refactors.
- Do not attempt to debug this project. Do not start a debugger, connect to hardware, flash firmware, or perform speculative runtime troubleshooting. When runtime diagnosis is needed, summarize the relevant code paths, observations, hypotheses, and suggested inspection points, then hand the debugging work to the developer.
- Static inspection and an explicitly requested compile check are allowed, but compilation success must not be presented as hardware validation.
