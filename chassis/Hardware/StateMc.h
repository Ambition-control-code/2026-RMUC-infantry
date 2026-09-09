#ifndef STATEMC_H
#define STATEMC_H

#include "main.h"
#define x_speed 1000
#define y_speed 1000
#define r_speed 10

#define keybord_control 2
#define remote_control 1

#define BOOL u8
#define TRUE 1
#define FALSE 0

enum aim_status_t
{
	auto_aim=1, //自瞄
	hand_movement=2,//手动
	point_strike=3,//定点打击
};

void chassis_data_set(void);
void gimbal_data_set(void);

#endif

