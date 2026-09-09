#include "dm_imu.h"

#define DM_IMU_ACCEL_CAN_MAX (235.2f)
#define DM_IMU_ACCEL_CAN_MIN (-235.2f)
#define DM_IMU_GYRO_CAN_MAX  (34.88f)
#define DM_IMU_GYRO_CAN_MIN  (-34.88f)
#define DM_IMU_PITCH_CAN_MAX (90.0f)
#define DM_IMU_PITCH_CAN_MIN (-90.0f)
#define DM_IMU_ROLL_CAN_MAX  (180.0f)
#define DM_IMU_ROLL_CAN_MIN  (-180.0f)
#define DM_IMU_YAW_CAN_MAX   (180.0f)
#define DM_IMU_YAW_CAN_MIN   (-180.0f)
#define DM_IMU_QUAT_CAN_MAX  (1.0f)
#define DM_IMU_QUAT_CAN_MIN  (-1.0f)

dm_imu_t bmi088_dm_imu = {0};
uint16_t dm_counts = 0;
uint8_t dm_live = 0;

static float dm_imu_uint_to_float(uint32_t x_int, float x_min, float x_max, uint8_t bits)
{
    float span = x_max - x_min;
    float offset = x_min;

    return ((float)x_int) * span / ((float)((1UL << bits) - 1UL)) + offset;
}

static uint16_t dm_imu_u16_le(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

dm_imu_frame_type_t dm_imu_process(dm_imu_t *imu, const uint8_t *data)
{
    if ((imu == 0) || (data == 0))
    {
        return DM_IMU_FRAME_NONE;
    }

    switch ((dm_imu_frame_type_t)data[0])
    {
        case DM_IMU_FRAME_ACCEL:
            imu->temperature = data[1];
            imu->accel.x = dm_imu_uint_to_float(dm_imu_u16_le(&data[2]), DM_IMU_ACCEL_CAN_MIN, DM_IMU_ACCEL_CAN_MAX, 16);
            imu->accel.y = dm_imu_uint_to_float(dm_imu_u16_le(&data[4]), DM_IMU_ACCEL_CAN_MIN, DM_IMU_ACCEL_CAN_MAX, 16);
            imu->accel.z = dm_imu_uint_to_float(dm_imu_u16_le(&data[6]), DM_IMU_ACCEL_CAN_MIN, DM_IMU_ACCEL_CAN_MAX, 16);
            break;

        case DM_IMU_FRAME_GYRO:
            imu->gyro.x = dm_imu_uint_to_float(dm_imu_u16_le(&data[2]), DM_IMU_GYRO_CAN_MIN, DM_IMU_GYRO_CAN_MAX, 16);
            imu->gyro.y = dm_imu_uint_to_float(dm_imu_u16_le(&data[4]), DM_IMU_GYRO_CAN_MIN, DM_IMU_GYRO_CAN_MAX, 16);
            imu->gyro.z = dm_imu_uint_to_float(dm_imu_u16_le(&data[6]), DM_IMU_GYRO_CAN_MIN, DM_IMU_GYRO_CAN_MAX, 16);
            break;

        case DM_IMU_FRAME_EULER:
            imu->euler.pitch = dm_imu_uint_to_float(dm_imu_u16_le(&data[2]), DM_IMU_PITCH_CAN_MIN, DM_IMU_PITCH_CAN_MAX, 16);
            imu->euler.yaw = dm_imu_uint_to_float(dm_imu_u16_le(&data[4]), DM_IMU_YAW_CAN_MIN, DM_IMU_YAW_CAN_MAX, 16);
            imu->euler.roll = dm_imu_uint_to_float(dm_imu_u16_le(&data[6]), DM_IMU_ROLL_CAN_MIN, DM_IMU_ROLL_CAN_MAX, 16);
            break;

        case DM_IMU_FRAME_QUAT:
            imu->quat.w = dm_imu_uint_to_float(((uint32_t)data[1] << 6) | ((uint32_t)(data[2] & 0xF8U) >> 2), DM_IMU_QUAT_CAN_MIN, DM_IMU_QUAT_CAN_MAX, 14);
            imu->quat.x = dm_imu_uint_to_float(((uint32_t)(data[2] & 0x03U) << 12) | ((uint32_t)data[3] << 4) | ((uint32_t)(data[4] & 0xF0U) >> 4), DM_IMU_QUAT_CAN_MIN, DM_IMU_QUAT_CAN_MAX, 14);
            imu->quat.y = dm_imu_uint_to_float(((uint32_t)(data[4] & 0x0FU) << 10) | ((uint32_t)data[5] << 2) | ((uint32_t)(data[6] & 0xC0U) >> 6), DM_IMU_QUAT_CAN_MIN, DM_IMU_QUAT_CAN_MAX, 14);
            imu->quat.z = dm_imu_uint_to_float(((uint32_t)(data[6] & 0x3FU) << 8) | (uint32_t)data[7], DM_IMU_QUAT_CAN_MIN, DM_IMU_QUAT_CAN_MAX, 14);
            break;

        default:
            return DM_IMU_FRAME_NONE;
    }

    dm_live = 1;
    dm_counts=0;

    return (dm_imu_frame_type_t)data[0];
}
