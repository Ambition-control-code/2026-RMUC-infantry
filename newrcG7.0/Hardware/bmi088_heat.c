#include "bmi088_heat.h"
#include "BMI088driver.h"
#include "INS_task.h"
#include "tim.h"

extern bmi088_real_data_t bmi088_real_data;

#define HEAT_CONTROL_INTERVAL_MS  10U

static PID_T g_heat_pid;
static uint32_t s_heat_tick;

static const PID_Config_t g_heat_pid_config = {
    .kp = 17.0f,
    .ki = 0.09f,
    .kd = 0.0f,
    .kf = 0.0f,
    .d_first_enable = 0,
    .dead = 0.1f,
    .I_Separate_Threshold = 0.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 0.01f,
    .poutmax = 100.0f,
    .ioutmax = 80.0f,
    .doutmax = 0.0f,
    .foutmax = 0.0f,
    .outmax = 100.0f,
    .lpf_alpha = 1.0f,
};

void BMI088_Heat_Init(void)
{
    PID_InitFromConfig(&g_heat_pid, &g_heat_pid_config);
    s_heat_tick = 0U;
    HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 0);
}

void BMI088_Heat_Control(void)
{
    s_heat_tick++;
    if (s_heat_tick < HEAT_CONTROL_INTERVAL_MS)
        return;
    s_heat_tick = 0U;

    float temp = bmi088_real_data.temp;
    float duty = PID_Cal(&g_heat_pid, temp, BMI088_HEAT_TARGET);

    if (duty < 0.0f)
        duty = 0.0f;
    if (duty > 100.0f)
        duty = 100.0f;

    __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, (uint32_t)duty);
}
