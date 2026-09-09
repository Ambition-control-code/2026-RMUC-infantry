// LADRC public API. All state lives in LADRC_t; no globals.

#ifndef LADRC_H
#define LADRC_H

typedef struct
{
    // Tuning parameters
    float b0;      // nominal plant input gain
    float wc;      // Controller bandwidth
    float wo;      // Observer bandwidth
    float r;       // TD tracking speed (smoothness factor). Larger = faster tracking.
    float dt;      // Loop time
    float u_max;   // Output saturation

    // Internal gains (calculated from wc/wo)
    float kp;
    float kd;
    float beta1;
    float beta2;
    float beta3;

    // ESO States (Estimator)
    float z1;      // Estimated Position
    float z2;      // Estimated Velocity
    float z3;      // Estimated Disturbance

    // TD States (Trajectory Planner)
    float v1;      // Tracked Target Position (Smoothed ref)
    float v2;      // Tracked Target Velocity (ref derivative)

    float last_u;
} LADRC_t;

/**
 * @brief Initialize LADRC with Tracking Differentiator
 * @param r  Tracking speed parameter (similar to bandwidth). 
 * Try starting with r = wc * 5 or adjust based on physical limit.
 */
void LADRC_Init(LADRC_t *ladrc, float b0, float wc, float wo, float r, float dt, float u_max);

/**
 * @brief Reset internal states. Sets both ESO and TD to current output y0.
 */
void LADRC_Reset(LADRC_t *ladrc, float y0);

/**
 * @brief Update control loop
 * @param ref  Raw target setpoint (step input is fine, TD will smooth it)
 * @param y    Current measured feedback
 */
float LADRC_Update(LADRC_t *ladrc, float ref, float y);
float LADRC_Update_1st(LADRC_t *ladrc, float ref, float y);

// ¶þ½× TD ÂË²¨Æ÷£º½«½×Ô¾Ä¿±êÆ½»¬³ÉÔ²Èó¹ì¼£
void TD_Filter(float target_input, float* v1, float* v2, float r, float dt);
// Accessors
static inline float LADRC_GetRefPos(const LADRC_t *ladrc) { return ladrc ? ladrc->v1 : 0.0f; }
static inline float LADRC_GetRefVel(const LADRC_t *ladrc) { return ladrc ? ladrc->v2 : 0.0f; }

#endif // LADRC_H