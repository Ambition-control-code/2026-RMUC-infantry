#include "main.h"
#include "string.h"
#include "motor.h"
#include "dm4340_drv.h"

extern void Set_FEED_Motor_Current(int16_t i1);

#ifndef PI
#define PI 3.1416
#endif

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
	// 初始化Yaw/Pitch2电机结构
	memset(&motor[MotorYaw], 0, sizeof(motor[MotorYaw])); //分配内存空间
	memset(&motor[MotorPitch2], 0, sizeof(motor[MotorPitch2]));

	// 设置Yaw的电机信息
	motor[MotorYaw].id = 3;
	motor[MotorYaw].ctrl.mode = 0;		// 0: MIT模式   1: 位置速度模式   2: 速度模式
	motor[MotorYaw].cmd.mode = 0;
	
	motor[MotorYaw].ctrl.kd_set=0.0f;//0.4
	motor[MotorYaw].ctrl.kp_set=0.0f;//60
	motor[MotorYaw].ctrl.pos_set=0.0f;
	motor[MotorYaw].ctrl.tor_set=0.0f;
	motor[MotorYaw].ctrl.vel_set=0.0f;//1

	
	
}



