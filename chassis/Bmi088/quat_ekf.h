#ifndef QUAT_EKF_H
#define QUAT_EKF_H

#include <stdbool.h>
#include <stdint.h>
#include "quat_ekf_kalman.h"

#ifndef GravityAccel
#define GravityAccel 9.718f
#endif

#ifndef VAL_LIMIT
#define VAL_LIMIT(value, min, max) \
    do                             \
    {                              \
        if ((value) < (min))       \
        {                          \
            (value) = (min);       \
        }                          \
        else if ((value) > (max))  \
        {                          \
            (value) = (max);       \
        }                          \
    } while (0)
#endif

typedef struct
{
    bool Init;

    float quat[4];
    float Deviate[3];

    float Q1, Q2, R;

    float *QuaternionEKF_A_Data;
    float *QuaternionEKF_P_Data;
    KalmanFilter_Info_TypeDef QuaternionEKF;

    float Accel[3];
    float Gyro[3];
    float AccelInvNorm;
    float GyroInvNorm;
    float Half_Gyro_dt[3];
    float EulerAngle[3];

} Quaternion_Info_Typedef;

extern Quaternion_Info_Typedef Quaternion_Info;

void QuaternionEKF_Init(Quaternion_Info_Typedef *Quat, float Process_Noise1, float Process_Noise2, float Measure_Noise, float *QuaternionEKF_A_Data, float *QuaternionEKF_P_Data);
void QuaternionEKF_Update(Quaternion_Info_Typedef *Quat, float Gyro[3], float Accel[3], float dt);

#endif
