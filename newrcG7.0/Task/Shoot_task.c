#include "main.h"
#include "Gimbal_task.h"

#define SHOOT_TASK_S    (6300.0f)
#define SHOOT_SPEED_MIN (5600.0f)
#define SHOOT_SPEED_MAX (7400.0f)
#define SHOOT_SPEED_STEP (0.2f)

void Shoot_Control(void const * argument)
{
    static float g_shoot_target_speed = SHOOT_TASK_S;

    for (;;)
    {
        if (rc.Z||VT13_USE.use_data.Z) {
            g_shoot_target_speed -= SHOOT_SPEED_STEP;
            if (g_shoot_target_speed < SHOOT_SPEED_MIN)
                g_shoot_target_speed = SHOOT_SPEED_MIN;
        } else if (rc.X||VT13_USE.use_data.X) {
            g_shoot_target_speed += SHOOT_SPEED_STEP;
            if (g_shoot_target_speed > SHOOT_SPEED_MAX)
                g_shoot_target_speed = SHOOT_SPEED_MAX;
        } else if (rc.C||VT13_USE.use_data.C) {
            g_shoot_target_speed = SHOOT_TASK_S;
        }

        
        All_control.shoot.shoot_speed = ((rc.R == 0)) ? g_shoot_target_speed: 0.0f;
        //All_control.shoot.shoot_speed=0;
        osDelay(1);
    }
}
