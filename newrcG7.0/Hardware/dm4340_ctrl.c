#include "main.h"
#include "string.h"
#include "motor.h"
#include "dm4340_drv.h"

extern void Set_Shoot_Motor_Current(int16_t i1, int16_t i2);
extern void Set_FEED_Motor_Current(int16_t i1);

#ifndef PI
#define PI 3.1416
#endif

#define PITCH_GFF_DEG2RAD (PI / 180.0f)
#define PITCH_GFF_SCALE 1.0f
#define PITCH_GFF_A 1704.0882f
#define PITCH_GFF_B -4624.2890f
#define PITCH_GFF_C -1133.5477f

// Least-squares gravity compensation fit for pitch hold current:
// I = A*sin(theta) + B*cos(theta) + C, theta in degrees.
// Data fit from:
// (8.06,-5400), (4,-5200), (-3.1,-5700), (-10,-6500),
// (-13.3,-6000), (-20.42,-5900), (11.8,-5600),
// (15.2,-5250), (20.4,-4800)



static float pitch_gravity_comp(float pitch_deg)
{
	float pitch_rad = pitch_deg * PITCH_GFF_DEG2RAD;
	return PITCH_GFF_SCALE * (PITCH_GFF_A * sinf(pitch_rad) +
	                          PITCH_GFF_B * cosf(pitch_rad) +
	                          PITCH_GFF_C);
}

static int16_t clamp_current_to_int16(float current)
{
	if (current > 16384.0f)
	{
		return 16384;
	}
	if (current < -16384.0f)
	{
		return -16384;
	}
	return (int16_t)current;
}

motor_t motor[num];
int8_t motor_id = 1;

/**
************************************************************************
* @brief:      	dm4340_motor_init: DM4310电机初始化函数
* @param:      	void
* @retval:     	void
* @details:    	初始化DM4340型号的电机，设置默认参数和控制模式。
*               初始化Yaw/Pitch2，设置ID、控制模式和命令模式等信息。
************************************************************************
**/
void dm4340_motor_init(void)
{
	dm4340_motor_init_yaw();
	dm4340_motor_init_pitch2();
}

void dm4340_motor_init_yaw(void)
{
	memset(&motor[MotorYaw], 0, sizeof(motor[MotorYaw]));

	motor[MotorYaw].id = 3;
	motor[MotorYaw].ctrl.mode = 0;
	motor[MotorYaw].cmd.mode = 0;

	motor[MotorYaw].ctrl.kd_set = 0.0f;
	motor[MotorYaw].ctrl.kp_set = 0.0f;
	motor[MotorYaw].ctrl.pos_set = 0.0f;
	motor[MotorYaw].ctrl.tor_set = 0.0f;
	motor[MotorYaw].ctrl.vel_set = 0.0f;
}

void dm4340_motor_init_pitch2(void)
{
	memset(&motor[MotorPitch2], 0, sizeof(motor[MotorPitch2]));

	motor[MotorPitch2].id = 2;
	motor[MotorPitch2].ctrl.mode = 1;
	motor[MotorPitch2].cmd.mode = 1;

	motor[MotorPitch2].ctrl.kd_set = 1.0f;
	motor[MotorPitch2].ctrl.kp_set = 1.0f;
	motor[MotorPitch2].ctrl.pos_set = 60.0f;
	motor[MotorPitch2].ctrl.tor_set = 0.1f;
	motor[MotorPitch2].ctrl.vel_set = 1.8f;
}

#define GIMBAL_CAN_CTRL_DIV 2U  // Gimbal loop 1kHz -> CAN1 pitch and chassis 500Hz

static uint8_t pitch2_can_count = 0;
static uint8_t gimbal_can_count = 0;
void ctrl_send(void)
{
	dm4310_ctrl_send(&hcan1, &motor[MotorYaw]);

	if (++pitch2_can_count >= PITCH2_CTRL_DIV)
	{
		pitch2_can_count = 0;
		dm4310_ctrl_send(&hcan1, &motor[MotorPitch2]);
	}
		//dm4310_ctrl_send(&hcan1, &motor[MotorPitch2]);

	if (++gimbal_can_count >= GIMBAL_CAN_CTRL_DIV)
	{
		gimbal_can_count = 0;
		mecanum_Resolving();
	}
	{
		float pitch_current = -Stop * (GMPitchPid_Spe.out + GMPitchTorqueFeedforward)
		                      -Stop * pitch_gravity_comp(imu_angle.pitch);
		if (All_control.DJ_handler.PITCH_flag != 0U)
		{
			pitch_current = 0.0f;
		}
		CAN2_0x200(clamp_current_to_int16(pitch_current),
		           STRpid_Spe.out,
		           STLpid_Spe.out,
		           0,
		           8);
	}
}



