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
set(CMAKE_PROJECT_NAME Man-WCIS)

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

set(MDK_ARM_USER_SOURCES
    Core/Src/bsp_flash.c
    Bmi088/BMI088driver.c
    Bmi088/BMI088Middleware.c
    Bmi088/AHRS_middleware.c
    Bmi088/INS_task.c
    Task/Gimbal_task.c
    Task/Shoot_task.c
    Task/MyINS_Task.c
    Task/Vision_Task.c
    Hardware/rc.c
    Hardware/motor.c
    Hardware/crc.c
    Hardware/kalman.c
    Hardware/kalman_filter.c
    Hardware/new_rc.c
    Hardware/ladrc.c
    Hardware/can_queue.c
    Hardware/vofa.c
    Hardware/smc.c
    Hardware/dm_imu.c
    Hardware/dm4340_ctrl.c
    Hardware/dm4340_drv.c
    Hardware/can_bsp.c
    Hardware/can_driver.c
    Hardware/bmi088_heat.c
)

set(CMSIS_DSP_SOURCES
    Drivers/CMSIS/DSP/Source/FastMathFunctions/arm_sin_f32.c
    Drivers/CMSIS/DSP/Source/FastMathFunctions/arm_cos_f32.c
    Drivers/CMSIS/DSP/Source/CommonTables/arm_common_tables.c
    Drivers/CMSIS/DSP/Source/MatrixFunctions/arm_mat_add_f32.c
    Drivers/CMSIS/DSP/Source/MatrixFunctions/arm_mat_init_f32.c
    Drivers/CMSIS/DSP/Source/MatrixFunctions/arm_mat_inverse_f32.c
    Drivers/CMSIS/DSP/Source/MatrixFunctions/arm_mat_mult_f32.c
    Drivers/CMSIS/DSP/Source/MatrixFunctions/arm_mat_sub_f32.c
    Drivers/CMSIS/DSP/Source/MatrixFunctions/arm_mat_trans_f32.c
)

set(MDK_ARM_USER_HEADERS
    Core/Inc/bsp_flash.h
    Bmi088/BMI088driver.h
    Bmi088/BMI088Middleware.h
    Bmi088/BMI088reg.h
    Bmi088/struct_typedef.h
    Bmi088/AHRS.h
    Bmi088/AHRS_middleware.h
    Bmi088/INS_task.h
    DSP/Include/arm_common_tables.h
    DSP/Include/arm_const_structs.h
    DSP/Include/arm_math.h
    DSP/Include/core_sc000.h
    DSP/Include/core_sc300.h
    Task/Gimbal_task.h
    Task/Shoot_task.h
    Task/MyINS_Task.h
    Task/Vision_Task.h
    Hardware/rc.h
    Hardware/motor.h
    Hardware/crc.h
    Hardware/kalman.h
    Hardware/kalman_filter.h
    Hardware/new_rc.h
    Hardware/ladrc.h
    Hardware/can_queue.h
    Hardware/vofa.h
    Hardware/smc.h
    Hardware/dm_imu.h
    Hardware/dm4340_ctrl.h
    Hardware/dm4340_drv.h
    Hardware/can_bsp.h
    Hardware/can_driver.h
    Hardware/bmi088_heat.h
)

set(MDK_ARM_INCLUDE_DIRS
    Bmi088
    Drivers/CMSIS/DSP/Include
    DSP
    DSP/Include
    Hardware
    Task
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RC_chen")
    list(APPEND MDK_ARM_INCLUDE_DIRS RC_chen)
endif()

set(MDK_ARM_USER_LIBS
    ${CMAKE_CURRENT_SOURCE_DIR}/Bmi088/AHRS.lib
)

set(MDK_ARM_DEFINES
    USE_STDPERIPH_DRIVER
    __FPU_PRESENT=1U
    ARM_MATH_CM4
    ARM_MATH_MATRIX_CHECK
    ARM_MATH_ROUNDING=1U
)

target_include_directories(stm32cubemx INTERFACE
    ${MDK_ARM_INCLUDE_DIRS}
)

target_compile_definitions(stm32cubemx INTERFACE
    ${MDK_ARM_DEFINES}
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${MDK_ARM_USER_SOURCES}
    ${CMSIS_DSP_SOURCES}
    ${MDK_ARM_USER_HEADERS}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${MDK_ARM_INCLUDE_DIRS}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${MDK_ARM_DEFINES}
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    ${MDK_ARM_USER_LIBS}
    ${TOOLCHAIN_LINK_LIBRARIES}

    # Add user defined libraries
)
