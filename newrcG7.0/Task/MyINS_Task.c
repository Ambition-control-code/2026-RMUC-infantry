#include "main.h"
#include "tim.h"
#include "stm32f4xx_hal_tim.h"
#include "bmi088_heat.h"

extern uint8_t YiNianTongTian;

void Start_INS_Task(void const * argument)
{
	uint32_t previousWakeTime;

	BMI088_Heat_Init();
	previousWakeTime = osKernelSysTick();

	for(;;)
	{
		INS_task();
		if(offset_OK_flag==0)offset_OK_flag = cali_gyro_hook();
		BMI088_Heat_Control();
		osDelayUntil(&previousWakeTime, 1);
	}
}
