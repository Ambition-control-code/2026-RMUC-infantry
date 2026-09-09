#ifndef DM_IMU_H
#define DM_IMU_H

#include <stdint.h>

typedef struct
{
    float x;
    float y;
    float z;
} dm_imu_accel_t;

typedef struct
{
    float x;
    float y;
    float z;
} dm_imu_gyro_t;

typedef struct
{
    float w;
    float x;
    float y;
    float z;
} dm_imu_quat_t;

typedef struct
{
    float yaw;
    float pitch;
    float roll;
} dm_imu_euler_t;

typedef struct
{
    dm_imu_accel_t accel;
    dm_imu_gyro_t gyro;
    dm_imu_quat_t quat;
    dm_imu_euler_t euler;
    uint8_t temperature;
} dm_imu_t;

typedef enum
{
    DM_IMU_FRAME_NONE  = 0,
    DM_IMU_FRAME_ACCEL = 1,
    DM_IMU_FRAME_GYRO  = 2,
    DM_IMU_FRAME_EULER = 3,
    DM_IMU_FRAME_QUAT  = 4,
} dm_imu_frame_type_t;

extern dm_imu_t bmi088_dm_imu;
extern uint16_t dm_counts;
extern uint8_t dm_live;

dm_imu_frame_type_t dm_imu_process(dm_imu_t *imu, const uint8_t *data);

#endif
