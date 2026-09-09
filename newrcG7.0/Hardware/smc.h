#ifndef __SMC_H
#define __SMC_H

#include "stdint.h"
#include "string.h"
#include "math.h"

typedef struct{
	float C,C_IN;
	float K,K_IN;
	float ref; //初始目标值
	float error_eps;//误差下限
	float u_max;//输出最大值
	float J;//估计惯量
	float angle; //角度反馈，°
	float ang_vel;//角速度反馈，°/s
	float epsilon;

	float u;

	float error;
	float error_last;
	float dref;//目标值一阶导
	float ddref;//目标值二阶导
	float last_ref;//上一次的目标值

	float s;//滑模面
	
	float c_fuzzy[2],k_fuzzy[2];
}SMC_control;

void SMC_init(SMC_control *SMC,float C,float K,float ref,float error_eps,float u_max,float J,float epsilon);
float SMC_Tick(SMC_control *SMC,float ref,float angle_now,float angle_vel);

#endif
