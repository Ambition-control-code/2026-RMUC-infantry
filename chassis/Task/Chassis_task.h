#ifndef CHASSIS_TASK_H
#define CHASSIS_TASK_H
#include "main.h"
#include "usart.h"
#include "slope_turn_cmsis.h"
#include "../Hardware/kalman.h"
#define F_R 3
#define F_L 2
#define B_L 1
#define B_R 0
#define DISTANCE 0.358f
#define spin_change 0.037f
#define GYRO_Z_FILTER_ALPHA 0.1f
typedef enum
{
    CHASSIS_STOP=0,	
	CHASSIS_FOLLOW,
	CHASSIS_SPIN,
}Chassis_mode_e;
typedef enum
{
    MODE_1_360=0,
	MODE_3_180,
}Head_mode_e;
typedef struct
{
 	 float CAP_J_TX_DATA;
	 float CAP_U_TX_DATA;
}capacity;
typedef struct
{
	float vx_set;  
	float vy_set;
	capacity D_R;
	int circle_count;
	int remainder;
	float angle_L;
	float ecd_angle_difference;
	float follow_angle_set;
	float wz_set;
	float chassis_motor_set[4];
	float value_3508_ID[4];
	Chassis_mode_e chassis_mode_e;
	Head_mode_e head_mode;
	float OUTI[4];
	first_order_filter_t gyro_z_filter;
	float power_capcity;
}CHASSIS_Task;
#define YAW_MECHANICAL_CENTRE  -70.7f
extern CHASSIS_Task chassis_task;
void macnum_run(CHASSIS_Task *chassis_Task);
void Chassis_control(void const * argument);
void chassis_mode(void);
void power_limit(float pow_max); 
void chassis_gravity_comp_xy(CHASSIS_Task *chassis_task);
void Chassis_Stop_AllTargetCurrent(CHASSIS_Task *chassis_task);
#endif
