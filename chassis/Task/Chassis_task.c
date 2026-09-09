#include "Chassis_task.h"
#include <math.h>
extern Data Data_C;
extern Robot_Para robot;
extern IMU_angle imu_angle;
extern volatile uint16_t g_umpire_watchdog_cnt;
CHASSIS_Task chassis_task = {0};

#define SPIN_SPEED_POWER_MIN   45.0f
#define SPIN_SPEED_POWER_MAX  190.0f
#define SPIN_SPEED_RPM_MIN    1200.0f
#define SPIN_SPEED_RPM_MAX   2200.0f

#define CAP_FULL_VOLTAGE  2300.0f
#define CAP_MIN_VOLTAGE   1100.0f
#define CAP_MAX_POWER     90.0f

// 热量管理——全程线性降速 + 停射保护
#define HEAT_STOP_MARGIN      20.0f   // 剩余热量≤10（一发弹）时停射，拒绝发弹请求
#define FEED_RPM_PER_SHOT     617.0f  // 每发/秒对应的拨弹盘转速


static float spin_speed_from_power(float power)
{
    if (power <= SPIN_SPEED_POWER_MIN) return SPIN_SPEED_RPM_MIN;
    if (power >= SPIN_SPEED_POWER_MAX) return SPIN_SPEED_RPM_MAX;
    return SPIN_SPEED_RPM_MIN + (power - SPIN_SPEED_POWER_MIN)
            * (SPIN_SPEED_RPM_MAX - SPIN_SPEED_RPM_MIN)
            / (SPIN_SPEED_POWER_MAX - SPIN_SPEED_POWER_MIN);
}

static float cap_power_from_voltage(float cap_voltage)
{
    if (cap_voltage >= CAP_FULL_VOLTAGE) return CAP_MAX_POWER;
    if (cap_voltage <= CAP_MIN_VOLTAGE)  return 0.0f;
    return CAP_MAX_POWER * (cap_voltage - CAP_MIN_VOLTAGE)
           / (CAP_FULL_VOLTAGE - CAP_MIN_VOLTAGE);
}

static void power_calculate(void)
{
    float cap_power = cap_power_from_voltage(chassis_task.D_R.CAP_U_TX_DATA);
    chassis_task.power_capcity = (float)robot.chassis_power_limit + cap_power;
}

void Chassis_control(void const * argument)
{
    PID_Init_All();
    Chassis_DeviceWatchdog_Init();
    Data_C.stop=1;
    uint8_t send_to_capacity_cnt = 0;
    uint8_t send_speed_cnt = 0;
    for (;;)
    {
        chassis_task.circle_count = (int)(motor[MotorYaw].para.sum_angle - YAW_MECHANICAL_CENTRE) / 360;
        chassis_task.remainder = (int)(motor[MotorYaw].para.sum_angle - YAW_MECHANICAL_CENTRE) % 360;
        chassis_task.angle_L = roundf(chassis_task.remainder / 180.0f) * 180.0f;
        chassis_task.head_mode = (fmodf(chassis_task.angle_L, 360.0f) == 0.0f) ? MODE_1_360 : MODE_3_180;
        chassis_task.follow_angle_set = YAW_MECHANICAL_CENTRE +
                                                (chassis_task.circle_count) * 360.0f +
                                                chassis_task.angle_L;
        if (Data_C.tunnel && chassis_task.head_mode == MODE_3_180)
        {
            chassis_task.angle_L = 0.0f;
            chassis_task.head_mode = MODE_1_360;
            chassis_task.follow_angle_set = YAW_MECHANICAL_CENTRE +
                                            (chassis_task.circle_count) * 360.0f;
        }                                       
        Chassis_DeviceWatchdog_Tick();
        power_calculate();
        chassis_mode();
        switch (chassis_task.chassis_mode_e)
        {
            case CHASSIS_STOP:
                break;
            case CHASSIS_FOLLOW:
                PID_Cal(&follow_Yaw_angle_loop,
                        motor[MotorYaw].para.sum_angle,
                        chassis_task.follow_angle_set);
                PID_Cal(&follow_Yaw_speed_loop,
                        imu_angle.gyro_z,
                        -follow_Yaw_angle_loop.out);
                break;
            case CHASSIS_SPIN:
                PID_Cal(&spin_spe,
                        imu_angle.gyro_z,
                        spin_speed_from_power(chassis_task.power_capcity));
                break;
        }
        macnum_run(&chassis_task);
        if (++send_to_capacity_cnt >= 10)
        {
            send_to_capacity_cnt = 0;
            Send_to_capacity(robot.classis_power_buffer, robot.chassis_power_limit);
        }
        if (++send_speed_cnt >= 100)
        {
            send_speed_cnt = 0;
            send_speed();
        }
        g_umpire_watchdog_cnt++;
        vTaskDelay(1);
    }
}
#define rpm_to_rad (2.0f * PI / 60.0f) //宏定义�?�用�?号定�?
#define i_to_real (16384.0f / 20.0f)
float power_limit_k;
void power_limit(float pow_max)
{
	  if(pow_max < 35.0f)
        {pow_max = 35.0f;}
    float k0 = 0.66419934f, k1 = 0.00644428f, k2 = 0.00014239f,
          k3 = 0.01764443f, k4 = 0.16501439f, k5 = 3.09672e-05f;
    //pow_max+=-PID_Cal(&Buffer_power,robot.classis_power_buffer,20);
    float velo1 = BR.re_speed * rpm_to_rad;
    float velo2 = BL.re_speed * rpm_to_rad;
    float velo3 = FL.re_speed * rpm_to_rad;
    float velo4 = FR.re_speed * rpm_to_rad;

    float cmd1 = chassis_task.OUTI[0] / i_to_real;
    float cmd2 = chassis_task.OUTI[1] / i_to_real;
    float cmd3 = chassis_task.OUTI[2] / i_to_real;
    float cmd4 = chassis_task.OUTI[3] / i_to_real;

    float sum_cmd = cmd1 + cmd2 + cmd3 + cmd4;
    float sum_velo = velo1 + velo2 + velo3 + velo4;
    float sum_cmd2 = cmd1 * cmd1 + cmd2 * cmd2 + cmd3 * cmd3 + cmd4 * cmd4;
    float sum_velo2 = velo1 * velo1 + velo2 * velo2 + velo3 * velo3 + velo4 * velo4;
    float sum_velo_cmd = velo1 * cmd1 + velo2 * cmd2
                         + velo3 * cmd3 + velo4 * cmd4;

    float a = k4 * sum_cmd2;
    float b = k1 * sum_cmd + k3 * sum_velo_cmd;
    float c = k0 + k2 * sum_velo + k5 * sum_velo2 - pow_max;

    float mid_temp = b * b - 4.0f * a * c;
    if (mid_temp < 0.0f)
        mid_temp = 0.0f;

    float k = 1.0f;
    if (fabsf(a) > 1e-6f)
    {
        k = (-b + sqrtf(mid_temp)) / (2.0f * a);
    }

    if (k > 1.0f)
        k = 1.0f;
    if (k < 0.0f)
        k = 0.0f;
    power_limit_k = k;
    for(int i=0;i<4;i++)
	{chassis_task.OUTI[i] = power_limit_k * chassis_task.OUTI[i];}
}
void chassis_mode(void)
{
    chassis_task.chassis_mode_e = (Data_C.stop == 1) ? CHASSIS_STOP :
                                  (Data_C.spin == 1) ? CHASSIS_SPIN :
                                                       CHASSIS_FOLLOW;

    if (Chassis_DeviceWatchdog_IsLost())
    {
        chassis_task.chassis_mode_e = CHASSIS_STOP;
    }
}
void Chassis_Stop_AllTargetCurrent(CHASSIS_Task *chassis_task)
{
    if (chassis_task)
    {
        chassis_task->vx_set = 0.0f;
        chassis_task->vy_set = 0.0f;
        chassis_task->wz_set = 0.0f;
        for (int i = 0; i < 4; i++)
        {
            chassis_task->OUTI[i] = 0.0f;
            chassis_task->value_3508_ID[i] = 0.0f;
            chassis_task->chassis_motor_set[i] = 0.0f;
        }
    }
    Set_CHASSIS_Current(0, 0, 0, 0);
    Shoot_2006_StopCurrent();
}
static float shooter_heat_speed_limit(void)
{
    uint8_t level = robot.robot_level;
    if (level < 1) level = 1;

    // 等级最大拨弹盘转速（保留原有逻辑）
    float max_rpm = 7000.0f + (float)(level - 1) * 600.0f;
    if (max_rpm > 8200.0f) max_rpm = 8200.0f;

    // 裁判系统离线（1000ms未收到帧），取消热量控制，按满速打
    if (g_umpire_watchdog_cnt >= 1000U)
        return max_rpm;

    float heat = (float)robot.shooter_heat0;
    float limit = (float)robot.shooter_id1_17mm_cooling_limit;
    float cooling_rate = (float)robot.shooter_id2_17mm_cooling_rate;

    // 无热量数据，按满速打
    if (limit <= 0.0f) return max_rpm;

    // 平衡转速：每秒冷却多少热量就打多少发弹
    // cooling_rate / 10 * 617 = 每秒可打发数对应的转速
    float balance_rpm = cooling_rate * (FEED_RPM_PER_SHOT / 10.0f);

    // 停射区：剩余热量不足一发，拒绝发弹
    if (heat >= limit - HEAT_STOP_MARGIN)
        return 0.0f;

    // 冷却足够强，满速也不会过热
    if (balance_rpm >= max_rpm)
        return max_rpm;

    // 全程线性：heat=0→max_rpm, heat=limit→balance_rpm
    float t = heat / limit;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return max_rpm + t * (balance_rpm - max_rpm);
}

void macnum_run(CHASSIS_Task *chassis_task)
{
    // 如果�?停�?�模式，直接�?0并清除数�?，不执�?�后�?解算
    if (chassis_task->chassis_mode_e == CHASSIS_STOP)
    {
        Chassis_Stop_AllTargetCurrent(chassis_task);
        return; 
    }

       chassis_task->wz_set = (chassis_task->chassis_mode_e == CHASSIS_FOLLOW) ?
                               follow_Yaw_speed_loop.out : spin_spe.out;

    float raw_x = -Data_C.VX * 14.0f;
    float raw_y = -Data_C.VY * 14.0f;
		
	 first_order_filter(&chassis_task->gyro_z_filter, imu_angle.gyro_z, GYRO_Z_FILTER_ALPHA);
	 chassis_task->ecd_angle_difference = chassis_task->follow_angle_set -
	                                    motor[MotorYaw].para.sum_angle + chassis_task->gyro_z_filter.out * spin_change;
    float rad = chassis_task->ecd_angle_difference / 57.29578f;
    float cos_val = cosf(rad);
    float sin_val = sinf(rad);
    chassis_task->vx_set =  raw_x * cos_val + raw_y * sin_val; //左右
    chassis_task->vy_set =  raw_y * cos_val - raw_x * sin_val; //前进
    float dir = (chassis_task->head_mode == MODE_3_180) ? -1.0f : 1.0f;    
    float vx = (chassis_task->vx_set *0.05f) * dir;
    float vy = (chassis_task->vy_set *0.05f) * dir;
    float wz = chassis_task->wz_set * DISTANCE;
    chassis_task->chassis_motor_set[0] =  vx + vy + wz;
    chassis_task->chassis_motor_set[1] =  vx - vy + wz;
    chassis_task->chassis_motor_set[2] = -vx - vy + wz;
    chassis_task->chassis_motor_set[3] =  -vx + vy + wz;
    PID_T *pids[] = {&BR_Spe, &BL_Spe, &FL_Spe, &FR_Spe};
    int16_t speeds[] = {BR.re_speed, BL.re_speed, FL.re_speed, FR.re_speed};
    for (int i = 0; i < 4; i++)
    { chassis_task->value_3508_ID[i] = PID_Cal(pids[i], speeds[i], chassis_task->chassis_motor_set[i]);
     chassis_task->OUTI[i] = chassis_task->value_3508_ID[i];}
	 //chassis_gravity_comp_xy(chassis_task);
    power_limit(chassis_task->power_capcity);
    Set_CHASSIS_Current((int16_t)chassis_task->OUTI[B_R],
	                    (int16_t)chassis_task->OUTI[B_L],
					    (int16_t)chassis_task->OUTI[F_L],
					    (int16_t)chassis_task->OUTI[F_R]);	
		 //Set_CHASSIS_Current(0, 0, 0, 0);
   	Shoot_2006_SpeedLoop_SendCurrent(Data_C.W * shooter_heat_speed_limit());
        
}
/**
 * @brief 全向重力补偿 (X�? + Y�?)
 * @note  复用麦轮逆运动�?�逻辑，将重力分解为四�?�?子的电流
 */
void chassis_gravity_comp_xy(CHASSIS_Task *chassis_task)
{
}

