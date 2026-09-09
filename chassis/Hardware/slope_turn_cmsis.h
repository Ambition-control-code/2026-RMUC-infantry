\
/**
 * @file slope_turn_cmsis.h
 * @brief Compute chassis turn angle relative to slope steepest direction
 *        following derivation_process.md Exercise 10 logic.
 *
 * This version explicitly builds and multiplies rotation matrices using
 * CMSIS-DSP (arm_math.h) matrix functions.
 *
 * Core idea (from the document):
 *  1) Build chassis rotation matrix C from intrinsic Yaw-Pitch-Roll:
 *         C = Yaw(alpha_C) * Pitch(beta_C) * Roll(theta_C)
 *  2) Chassis and slope are co-planar on the ramp:
 *         c_z == s_z
 *     where c_z is the 3rd column of C.
 *  3) Define slope Euler angles as:
 *         S = Yaw(alpha_S) * Pitch(beta_S), Roll = 0
 *  4) From c_z, solve:
 *         beta_S  = atan2( -sqrt(c_zx^2 + c_zy^2), c_zz )
 *         alpha_S = atan2( c_zy, c_zx )
 *  5) Turn angle (heading difference):
 *         delta = wrap_pi( alpha_C - alpha_S )
 *
 * Coordinate convention note:
 *  - This library assumes your yaw/pitch/roll are already defined consistently
 *    with a right-handed body frame whose axes are:
 *        +Y forward, +X right, +Z up
 *    (the "front-Y, right-X" naming you described).
 *
 * CMSIS-DSP note:
 *  - There is no single "arm_math.c". You must add the needed DSP source files
 *    (e.g., MatrixFunctions/ *.c) or link the prebuilt CMSIS-DSP library.
 */

#ifndef SLOPE_TURN_CMSIS_H
#define SLOPE_TURN_CMSIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>
#include "arm_math.h"

#ifndef SLOPE_TURN_PI
#define SLOPE_TURN_PI 3.14159265358979323846f
#endif

typedef struct
{
    float alpha_s;   /**< inferred slope yaw (rad) */
    float beta_s;    /**< inferred slope pitch/grade (rad) */
    float delta;     /**< chassis relative turn angle (rad) */
    uint8_t valid;   /**< 1 if inference is reliable, 0 if near-flat singular */
} slope_turn_result_t;

/**
 * @brief Wrap an angle to [-pi, pi].
 */
float slope_turn_wrap_pi1(float a);

/**
 * @brief Build a 3x3 Yaw rotation matrix about +Z.
 * @param yaw_rad angle in radians
 * @param out_3x3 array length 9, row-major
 */
void slope_turn_build_yaw(float yaw_rad, float out_3x3[9]);

/**
 * @brief Build a 3x3 Pitch rotation matrix about +Y.
 */
void slope_turn_build_pitch(float pitch_rad, float out_3x3[9]);

/**
 * @brief Build a 3x3 Roll rotation matrix about +X.
 */
void slope_turn_build_roll(float roll_rad, float out_3x3[9]);

/**
 * @brief Build chassis rotation matrix C using intrinsic Yaw-Pitch-Roll:
 *        C = Yaw * Pitch * Roll
 *
 * Uses arm_mat_mult_f32 internally.
 *
 * @param yaw_rad   alpha_C
 * @param pitch_rad beta_C
 * @param roll_rad  theta_C
 * @param out_3x3   result matrix (row-major, length 9)
 */
void slope_turn_build_chassis_C(float yaw_rad, float pitch_rad, float roll_rad, float out_3x3[9]);

/**
 * @brief Infer slope yaw/pitch and chassis turn angle relative to slope.
 *
 * @param yaw_c_rad   chassis yaw alpha_C (rad)
 * @param pitch_c_rad chassis pitch beta_C (rad)
 * @param roll_c_rad  chassis roll theta_C (rad)
 * @param flat_eps    threshold on sqrt(c_zx^2 + c_zy^2). Typical 0.05~0.1
 * @return slope_turn_result_t
 */
slope_turn_result_t slope_turn_solve_from_euler(float yaw_c_rad,
                                                float pitch_c_rad,
                                                float roll_c_rad,
                                                float flat_eps);

/**
 * @brief Convenience: only return delta.
 */
float slope_turn_delta_from_euler(float yaw_c_rad,
                                  float pitch_c_rad,
                                  float roll_c_rad,
                                  float flat_eps);

#ifdef __cplusplus
}
#endif

#endif /* SLOPE_TURN_CMSIS_H */
