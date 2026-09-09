\
/**
 * @file slope_turn_cmsis.c
 * @brief Implementation of slope turn angle solver using CMSIS-DSP matrices.
 */

#include "slope_turn_cmsis.h"

/* =========================
 *  Helpers
 * ========================= */

float slope_turn_wrap_pi1(float a)
{
    while (a >  SLOPE_TURN_PI) a -= 2.0f * SLOPE_TURN_PI;
    while (a < -SLOPE_TURN_PI) a += 2.0f * SLOPE_TURN_PI;
    return a;
}

/* Row-major index macro */
#define M00(m) (m[0])
#define M01(m) (m[1])
#define M02(m) (m[2])
#define M10(m) (m[3])
#define M11(m) (m[4])
#define M12(m) (m[5])
#define M20(m) (m[6])
#define M21(m) (m[7])
#define M22(m) (m[8])

void slope_turn_build_yaw(float yaw_rad, float out_3x3[9])
{
    const float c = cosf(yaw_rad);
    const float s = sinf(yaw_rad);

    /* Rz(yaw) */
    M00(out_3x3) =  c;   M01(out_3x3) = -s;   M02(out_3x3) = 0.0f;
    M10(out_3x3) =  s;   M11(out_3x3) =  c;   M12(out_3x3) = 0.0f;
    M20(out_3x3) = 0.0f; M21(out_3x3) = 0.0f; M22(out_3x3) = 1.0f;
}

void slope_turn_build_pitch(float pitch_rad, float out_3x3[9])
{
    const float c = cosf(pitch_rad);
    const float s = sinf(pitch_rad);

    /* Ry(pitch) */
    M00(out_3x3) =  c;    M01(out_3x3) = 0.0f; M02(out_3x3) =  s;
    M10(out_3x3) = 0.0f; M11(out_3x3) = 1.0f; M12(out_3x3) = 0.0f;
    M20(out_3x3) = -s;   M21(out_3x3) = 0.0f; M22(out_3x3) =  c;
}

void slope_turn_build_roll(float roll_rad, float out_3x3[9])
{
    const float c = cosf(roll_rad);
    const float s = sinf(roll_rad);

    /* Rx(roll) */
    M00(out_3x3) = 1.0f; M01(out_3x3) = 0.0f; M02(out_3x3) = 0.0f;
    M10(out_3x3) = 0.0f; M11(out_3x3) =  c;   M12(out_3x3) = -s;
    M20(out_3x3) = 0.0f; M21(out_3x3) =  s;   M22(out_3x3) =  c;
}

void slope_turn_build_chassis_C(float yaw_rad, float pitch_rad, float roll_rad, float out_3x3[9])
{
    float Y[9], P[9], R[9];
    float YP[9];

    slope_turn_build_yaw(yaw_rad, Y);
    slope_turn_build_pitch(pitch_rad, P);
    slope_turn_build_roll(roll_rad, R);

    arm_matrix_instance_f32 mY, mP, mR, mYP, mC;

    arm_mat_init_f32(&mY,  3, 3, Y);
    arm_mat_init_f32(&mP,  3, 3, P);
    arm_mat_init_f32(&mR,  3, 3, R);
    arm_mat_init_f32(&mYP, 3, 3, YP);
    arm_mat_init_f32(&mC,  3, 3, out_3x3);

    /* YP = Y * P */
    (void)arm_mat_mult_f32(&mY, &mP, &mYP);

    /* C = (Y * P) * R */
    (void)arm_mat_mult_f32(&mYP, &mR, &mC);
}

slope_turn_result_t slope_turn_solve_from_euler(float yaw_c_rad,
                                                float pitch_c_rad,
                                                float roll_c_rad,
                                                float flat_eps)
{
    slope_turn_result_t res;
    res.alpha_s = 0.0f;
    res.beta_s  = 0.0f;
    res.delta   = 0.0f;
    res.valid   = 0;

    float C[9];
    slope_turn_build_chassis_C(yaw_c_rad, pitch_c_rad, roll_c_rad, C);

    /* c_z is 3rd column of C */
    const float czx = M02(C);
    const float czy = M12(C);
    const float czz = M22(C);

    const float h = sqrtf(czx*czx + czy*czy);

    /* beta_S = atan( -sqrt(c_zx^2+c_zy^2) / c_zz ) */
    res.beta_s = atan2f(-h, czz);

    /* alpha_S = atan2(c_zy, c_zx) is meaningful only when not near-flat */
    if (h > flat_eps)
    {
        res.alpha_s = atan2f(czy, czx);
        res.valid = 1;
    }
    else
    {
        res.alpha_s = 0.0f;
        res.valid = 0;
    }

    res.delta = slope_turn_wrap_pi1(yaw_c_rad - res.alpha_s);

    return res;
}

float slope_turn_delta_from_euler(float yaw_c_rad,
                                  float pitch_c_rad,
                                  float roll_c_rad,
                                  float flat_eps)
{
    slope_turn_result_t r = slope_turn_solve_from_euler(yaw_c_rad, pitch_c_rad, roll_c_rad, flat_eps);
    return r.delta;
}
