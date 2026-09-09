cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME MAN-WCIS)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/bsp_flash.c
    Bmi088/BMI088driver.c
    Bmi088/BMI088Middleware.c
    Bmi088/AHRS_middleware.c
    Bmi088/AHRS_local.c
    Bmi088/INS_task.c
    Task/MyINS_Task.c
    Task/Chassis_task.c
    Hardware/StateMc.c
    Hardware/rc.c
    Hardware/motor.c
    Hardware/UmpireData.c
    Hardware/CRC8_16.c
    Hardware/USE_DRAW.c
    Hardware/UmpireDraw.c
    UI/ui_g.c
    UI/ui_interface.c
    Hardware/slope_turn_cmsis.c
    Hardware/kalman.c
    Hardware/can_queue.c
    Hardware/dm4340_ctrl.c
    Hardware/dm4340_drv.c
    Hardware/vofa.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Bmi088
    Core/Inc
    DSP
    DSP/Include
    Hardware
    Task
    UI
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USE_STDPERIPH_DRIVER
    __FPU_USED=1U
    __FPU_PRESENT=1U
    ARM_MATH_CM4
    ARM_MATH_MATRIX_CHECK
    ARM_MATH_ROUNDING
)

set(CMSIS_DSP_ROOT "$ENV{USERPROFILE}/STM32Cube/Repository/Packs/STMicroelectronics/X-CUBE-ALGOBUILD/1.4.0/Middlewares/Third_Party/ARM/DSP")
find_library(CMSIS_DSP_LIB
    NAMES libarm_cortexM4lf_math.a
    PATHS "${CMSIS_DSP_ROOT}/Lib"
    NO_DEFAULT_PATH
)
if(NOT CMSIS_DSP_LIB)
    message(FATAL_ERROR "GCC CMSIS-DSP library not found. Install X-CUBE-ALGOBUILD or set CMSIS_DSP_LIB to libarm_cortexM4lf_math.a.")
endif()

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    ${CMSIS_DSP_LIB}
)
