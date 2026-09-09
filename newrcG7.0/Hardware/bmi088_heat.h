#ifndef BMI088_HEAT__H
#define BMI088_HEAT__H

#include "main.h"
#include "motor.h"

#define BMI088_HEAT_TARGET  45.0f

void BMI088_Heat_Init(void);
void BMI088_Heat_Control(void);

#endif
