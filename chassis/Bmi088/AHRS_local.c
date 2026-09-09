#include <string.h>

#include "AHRS.h"

static fp32 local_hight = 0.0f;
static fp32 latitude = 0.0f;
static fp32 local_gravity = 9.8f;
static fp32 carrier_gravity = 9.8f;

static fp32 ahrs_fabsf_local(fp32 x)
{
    return (x >= 0.0f) ? x : -x;
}

static void angle_to_quat(fp32 quat[4], fp32 yaw, fp32 pitch, fp32 roll)
{
    fp32 sin_yaw_2;
    fp32 cos_yaw_2;
    fp32 sin_pitch_2;
    fp32 cos_pitch_2;
    fp32 sin_roll_2;
    fp32 cos_roll_2;

    if (quat == NULL)
    {
        return;
    }

    cos_yaw_2 = AHRS_cosf(yaw * 0.5f);
    sin_yaw_2 = AHRS_sinf(yaw * 0.5f);
    sin_pitch_2 = AHRS_sinf(pitch * 0.5f);
    cos_pitch_2 = AHRS_cosf(pitch * 0.5f);
    sin_roll_2 = AHRS_sinf(roll * 0.5f);
    cos_roll_2 = AHRS_cosf(roll * 0.5f);

    quat[0] = cos_yaw_2 * cos_pitch_2 * cos_roll_2 + sin_yaw_2 * sin_pitch_2 * sin_roll_2;
    quat[1] = cos_yaw_2 * cos_pitch_2 * sin_roll_2 - sin_yaw_2 * sin_pitch_2 * cos_roll_2;
    quat[2] = cos_yaw_2 * sin_pitch_2 * cos_roll_2 + sin_yaw_2 * cos_pitch_2 * sin_roll_2;
    quat[3] = sin_yaw_2 * cos_pitch_2 * cos_roll_2 - cos_yaw_2 * sin_pitch_2 * sin_roll_2;
}

static void quat_normalization(fp32 quat[4])
{
    fp32 norm_inv = AHRS_invSqrt(quat[0] * quat[0] +
                                 quat[1] * quat[1] +
                                 quat[2] * quat[2] +
                                 quat[3] * quat[3]);

    quat[0] *= norm_inv;
    quat[1] *= norm_inv;
    quat[2] *= norm_inv;
    quat[3] *= norm_inv;
}

static void update_w(fp32 w_mat[16], const fp32 gyro[3])
{
    w_mat[1] = -gyro[0];
    w_mat[2] = -gyro[1];
    w_mat[3] = -gyro[2];

    w_mat[4] = gyro[0];
    w_mat[6] = gyro[2];
    w_mat[7] = -gyro[1];

    w_mat[8] = gyro[1];
    w_mat[9] = -gyro[2];
    w_mat[11] = gyro[0];

    w_mat[12] = gyro[2];
    w_mat[13] = gyro[1];
    w_mat[14] = -gyro[0];
}

static void accel_update_kp_ki(const fp32 gyro[3], const fp32 accel[3], fp32 kp_ki[3])
{
    fp32 accel_norm = 1.0f / AHRS_invSqrt(accel[0] * accel[0] +
                                          accel[1] * accel[1] +
                                          accel[2] * accel[2]);
    fp32 gyro_sum = ahrs_fabsf_local(gyro[0]) +
                    ahrs_fabsf_local(gyro[1]) +
                    ahrs_fabsf_local(gyro[2]);
    fp32 accel_dev = ahrs_fabsf_local(accel_norm - get_carrier_gravity());

    if (accel_dev > 0.1f || gyro_sum > 0.12f)
    {
        fp32 scale = 1.0f;
        fp32 acc_scale = (accel_dev > 0.0f) ? (0.1f / accel_dev) : 0.0f;
        fp32 gyro_scale = (gyro_sum > 0.0f) ? (0.12f / gyro_sum) : 0.0f;

        if (acc_scale < 0.3f)
        {
            acc_scale = 0.0f;
        }

        if (acc_scale < 1.0f)
        {
            scale *= acc_scale;
        }

        if (gyro_scale < 1.0f)
        {
            scale *= gyro_scale;
        }

        kp_ki[0] = 0.2f * scale;
        kp_ki[1] = 0.2f * scale;
        kp_ki[2] = 0.2f * scale;
    }
}

static void accel_comple_filter(const fp32 quat[4], const fp32 accel[3], fp32 gyro[3])
{
    fp32 kp_ki[3] = {0.2f, 0.2f, 0.2f};
    fp32 ax;
    fp32 ay;
    fp32 az;
    fp32 acc_norm_inv;
    fp32 vx;
    fp32 vy;
    fp32 vz;
    fp32 ex;
    fp32 ey;
    fp32 ez;

    if (quat == NULL || accel == NULL || gyro == NULL)
    {
        return;
    }

    if (accel[0] == 0.0f && accel[1] == 0.0f && accel[2] == 0.0f)
    {
        return;
    }

    accel_update_kp_ki(gyro, accel, kp_ki);

    vx = 2.0f * (quat[1] * quat[3] - quat[0] * quat[2]);
    vy = 2.0f * (quat[0] * quat[1] + quat[2] * quat[3]);
    vz = 1.0f - 2.0f * (quat[1] * quat[1] + quat[2] * quat[2]);

    acc_norm_inv = AHRS_invSqrt(accel[0] * accel[0] +
                                accel[1] * accel[1] +
                                accel[2] * accel[2]);
    ax = accel[0] * acc_norm_inv;
    ay = accel[1] * acc_norm_inv;
    az = accel[2] * acc_norm_inv;

    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;

    gyro[0] += kp_ki[0] * ex;
    gyro[1] += kp_ki[1] * ey;
    gyro[2] += kp_ki[2] * ez;
}

void AHRS_init(fp32 quat[4], const fp32 accel[3], const fp32 mag[3])
{
    fp32 accel_norm_inv;
    fp32 ax;
    fp32 ay;
    fp32 az;
    fp32 pitch = 0.0f;
    fp32 roll = 0.0f;
    fp32 yaw = 0.0f;
    fp32 sin_lat;
    fp32 sin_2lat;

    if (quat == NULL || accel == NULL || mag == NULL)
    {
        return;
    }

    AHRS_get_height(&local_hight);
    AHRS_get_latitude(&latitude);

    sin_lat = AHRS_sinf(latitude * ANGLE_TO_RAD);
    sin_2lat = AHRS_sinf(2.0f * latitude * ANGLE_TO_RAD);

    local_gravity = (1.0f + 0.0053024f * sin_lat * sin_lat - 5.9e-06f * sin_2lat * sin_2lat) * 9.780318f;
    carrier_gravity = local_gravity / ((1.0f + local_hight / 6370856.0f) * (1.0f + local_hight / 6370856.0f));

    if (accel[0] != 0.0f || accel[1] != 0.0f || accel[2] != 0.0f)
    {
        accel_norm_inv = AHRS_invSqrt(accel[0] * accel[0] +
                                      accel[1] * accel[1] +
                                      accel[2] * accel[2]);
        ax = accel[0] * accel_norm_inv;
        ay = accel[1] * accel_norm_inv;
        az = accel[2] * accel_norm_inv;

        pitch = AHRS_asinf(-ax);
        roll = AHRS_atan2f(ay, az);
    }

    if (mag[0] != 0.0f || mag[1] != 0.0f || mag[2] != 0.0f)
    {
        fp32 sin_pitch = AHRS_sinf(pitch);
        fp32 cos_pitch = AHRS_cosf(pitch);
        fp32 sin_roll = AHRS_sinf(roll);
        fp32 cos_roll = AHRS_cosf(roll);
        fp32 yaw_x = sin_pitch * sin_roll * mag[1] + cos_pitch * mag[0] + sin_pitch * cos_roll * mag[2];
        fp32 yaw_y = mag[1] * cos_roll - sin_roll * mag[2];

        yaw = AHRS_atan2f(yaw_x, yaw_y);
    }

    angle_to_quat(quat, yaw, pitch, roll);
}

bool_t AHRS_update(fp32 quat[4], const fp32 timing_time, const fp32 gyro[3], const fp32 accel[3], const fp32 mag[3])
{
    fp32 gyro_buf[3];
    fp32 accel_buf[3];
    fp32 w_mat[16];
    fp32 dq[4];
    uint8_t i;

    if (quat == NULL || gyro == NULL || accel == NULL || mag == NULL || timing_time == 0.0f)
    {
        return 0;
    }

    (void)mag;

    for (i = 0; i < 3; i++)
    {
        gyro_buf[i] = gyro[i];
        accel_buf[i] = accel[i];
    }

    accel_comple_filter(quat, accel_buf, gyro_buf);

    memset(w_mat, 0, sizeof(w_mat));
    update_w(w_mat, gyro_buf);

    for (i = 0; i < 4; i++)
    {
        fp32 dot = 0.0f;
        uint8_t j;

        for (j = 0; j < 4; j++)
        {
            dot += w_mat[i * 4 + j] * quat[j];
        }

        dq[i] = 0.5f * timing_time * dot;
    }

    for (i = 0; i < 4; i++)
    {
        quat[i] += dq[i];
    }

    quat_normalization(quat);

    return 1;
}

fp32 get_yaw(const fp32 quat[4])
{
    return AHRS_atan2f(2.0f * (quat[0] * quat[3] + quat[1] * quat[2]),
                       1.0f - 2.0f * (quat[2] * quat[2] + quat[3] * quat[3]));
}

fp32 get_pitch(const fp32 quat[4])
{
    return AHRS_asinf(-2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
}

fp32 get_roll(const fp32 quat[4])
{
    return AHRS_atan2f(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]),
                       1.0f - 2.0f * (quat[1] * quat[1] + quat[2] * quat[2]));
}

void get_angle(const fp32 quat[4], fp32 *yaw, fp32 *pitch, fp32 *roll)
{
    if (quat == NULL || yaw == NULL || pitch == NULL || roll == NULL)
    {
        return;
    }

    *pitch = AHRS_asinf(-2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
    *yaw = AHRS_atan2f(2.0f * (quat[0] * quat[3] + quat[1] * quat[2]),
                       1.0f - 2.0f * (quat[2] * quat[2] + quat[3] * quat[3]));
    *roll = AHRS_atan2f(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]),
                        1.0f - 2.0f * (quat[1] * quat[1] + quat[2] * quat[2]));
}

fp32 get_carrier_gravity(void)
{
    return carrier_gravity;
}

bool_t AHRS_update_local(fp32 quat[4], const fp32 timing_time, const fp32 gyro[3], const fp32 accel[3], const fp32 mag[3])
{
    return AHRS_update(quat, timing_time, gyro, accel, mag);
}

void AHRS_get_angle_local(const fp32 quat[4], fp32 *yaw, fp32 *pitch, fp32 *roll)
{
    get_angle(quat, yaw, pitch, roll);
}
