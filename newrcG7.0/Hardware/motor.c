#include "main.h"
#include "usart.h"
#include "dm4340_drv.h"
#include "kalman.h"
#include "can_queue.h"
#include "vofa.h"
#include "ladrc.h"
LADRC_t ladrc_yaw;

static first_order_filter_t imu_speed_filter = {0};
SMC_HandleTypeDef smc_dm = {0};

/**
 * @brief  ???????????????
 * @param  ff: ?????????????
 * @param  Fc: ??????????????
 * @param  tanh_coeff: tanh?????????????????????0.3~2.0??
 * @param  lpf_alpha: ?????????????0~1???��???????
 */
void CoulombFrictionFF_Init(CoulombFrictionFF_t *ff, float Fc, float tanh_coeff, float lpf_alpha)
{
    if (ff == NULL) return;
    ff->Fc = Fc;
    ff->tanh_coeff = tanh_coeff;
    ff->lpf_alpha = lpf_alpha;
    ff->vel_filter = (first_order_filter_t){0};
    ff->vel_filtered = 0.0f;
    ff->output = 0.0f;
}

/**
 * @brief  ?????????????????????????????
 * @param  ff: ?????????????
 * @param  velocity: ?????????
 * @return ???????????
 * @note   ???????????��???????????? tanh ???????????
 *         ???: output = Fc * tanh(k * vel_filtered)
 */
float CoulombFrictionFF_Calc(CoulombFrictionFF_t *ff, float velocity)
{
    if (ff == NULL) return 0.0f;
    ff->vel_filtered = first_order_filter(&ff->vel_filter, velocity, ff->lpf_alpha);
    ff->output = ff->Fc * tanhf(ff->tanh_coeff * ff->vel_filtered);
    return ff->output;
}

AxisFeedforwardOutput_t AxisFeedforward_Calc(const AxisFeedforwardConfig_t *config,
                                             uint8_t enable,
                                             float base_speed_set,
                                             float vision_velocity,
                                             float vision_acceleration,
                                             float friction_velocity)
{
    AxisFeedforwardOutput_t out = {0};

    out.speed_set = base_speed_set;
    if ((config == NULL) || (enable == 0U)) {
        return out;
    }

    out.speed_ff = config->vel_scale * vision_velocity;
    out.torque_ff = config->acc_scale * vision_acceleration;

    if ((config->friction_ff != NULL) && (config->friction_scale != 0.0f)) {
        out.friction_ff = config->friction_scale *
                          CoulombFrictionFF_Calc(config->friction_ff, friction_velocity);
    }

    out.speed_set += out.speed_ff;
    out.torque_total_ff = out.torque_ff + out.friction_ff;
    return out;
}

// ?????????
static CoulombFrictionFF_t yaw_friction_ff = {0};
static const AxisFeedforwardConfig_t g_yaw_ff_config = {
    .vel_scale = -50.0f,
    .acc_scale = 0.0f,
    .friction_scale = 0.0f,
    .friction_ff = &yaw_friction_ff,
};
static const AxisFeedforwardConfig_t g_pitch_ff_config = {
    .vel_scale = 0.3f,
    .acc_scale = 0.0f,
    .friction_scale = 0.0f,
    .friction_ff = NULL,
};
float GMPitchTorqueFeedforward = 0.0f;

static uint8_t GimbalFeedforward_IsEnabled(void)
{
    uint8_t zimiao_enable = (rc.sl == 1U) || (rc.mouse_r == 1U);
    uint8_t target_locked = (All_control.zimiao.status_zimiaoRX == MIAODAO) ||
                            (All_control.zimiao.status_zimiaoRX == MIAODAOKAIHUO);

    return (uint8_t)(zimiao_enable && target_locked);
}
uint8_t usedata[8];
float speed_17mm = 0.0f;
#define RATE_BUF_SIZE 6
#define MYABS(a)    ((a >= 0) ? (a) : -(a)) 
#define VALUE_LIMIT(v, l, h)   ((v) < (l) ? (l) : ((v) > (h) ? (h) : (v)))
#define VALUE_ABS_LIMIT(v, a)  VALUE_LIMIT(v, -a, a)
volatile dji_motor Pitch={0,0,0,0,0,0,0,0,0,0,0,0};
volatile dji_motor Yaw={0,0,0,0,0,0,0,0,0,0,0,0};
volatile dji_motor FEED,SHOOT_L,SHOOT_R;
extern VisionUnion vision_recieve_union;
extern send_struct send_vision;
extern int fire_ok;
extern uint8_t danpincounts;
PID_T GMPitchPid_Ang={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T GMPitchPid_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T GMYawPid_Ang={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T GMYawPid_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

PID_T GMYawPid_Ang_ZM={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T GMYawPid_Spe_ZM={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T STLpid_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T STRpid_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T Feedpid_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T PITCH2_Ang={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T PITCH2_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern uint32_t motor2_reinit_cnt;
float dataC;          // 4????????
void getEncoderData(volatile dji_motor *v)
{
    int i = 0;
    int32_t temp_sum = 0;
    v->last_raw_value = v->raw_value;
    v->raw_value = (usedata[0] << 8) | usedata[1];
    v->diff = v->raw_value - v->last_raw_value;
    if (v->diff < -4096)    //???��???????????????????????????????
    {
        v->round_cnt++;
        v->ecd_raw_rate = v->diff + 8192;
    }
    else if (v->diff > 4096)
    {
        v->round_cnt--;
        v->ecd_raw_rate = v->diff - 8192;
    }
    else
    {
        v->ecd_raw_rate = v->diff;
    }
    //?????????????????????
    v->ecd_value = v->raw_value + v->round_cnt * 8192;
    //??????????????��?????????
    v->ecd_angle = (float)(v->raw_value - v->ecd_bias) * 360 / 8192 + v->round_cnt * 360;
    v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
    if (v->buf_count == RATE_BUF_SIZE)
    {
        v->buf_count = 0;
    }

    //???????????
    for (i = 0; i < RATE_BUF_SIZE; i++)
    {
        temp_sum += v->rate_buf[i];
    }
    v->filter_rate = (int32_t)(temp_sum / RATE_BUF_SIZE);

    if (fabs((float)v->filter_rate_max) < fabs((float)v->filter_rate))
    {
        v->filter_rate_max = v->filter_rate;
    }
    v->current = (uint16_t)(usedata[4] << 8 | usedata[5]);
    v->tempature = usedata[6];
    v->re_speed = (uint16_t)(usedata[2] << 8 | usedata[3]);//????????

}

void CAN_RxFifoMsgHandler(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxFifoMsgHandler(hcan);
}
float jishidm;
void CAN_RxFifoMsgHandler(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, usedata) != HAL_OK)
    {
        return;
    }

    if (hcan->Instance == CAN1)
    {
        switch (RxHeader.StdId)
        {
            case 0x00:
                if (usedata[0] == 0x13)
                {
                    dm4310_fbdata(&motor[MotorYaw], usedata);
                    All_control.DJ_handler.YAW = 0;
									  jishidm++;
                }
                if (usedata[0] == 0x12)
                {
                    dm4310_fbdata(&motor[MotorPitch2], usedata);
                    All_control.DJ_handler.PITCH2 = 0;
                }
                break;
            case 0x402:
                memcpy(&speed_17mm, usedata, sizeof(float));
                break;
        }
    }
    if (hcan->Instance == CAN2)
    {
        switch (RxHeader.StdId)
        {
            case 0x201:
                getEncoderData(&Pitch);
                All_control.DJ_handler.PITCH = 0;
                All_control.DJ_handler.PITCH_flag = 0;
                break;
            case 0x202:
                getEncoderData(&SHOOT_R);
                All_control.DJ_handler.SHOOT_R = 0;
                break;
            case 0x203:
                getEncoderData(&SHOOT_L);
                All_control.DJ_handler.SHOOT_L = 0;
                break;
        }
    }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    CAN_Process_Next_Send(hcan);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
    CAN_Process_Next_Send(hcan);
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
    CAN_Process_Next_Send(hcan);
}

static void PID_PrimeState(PID_T *pid, float real_now, float set_now)
{
    float err_now;

    if (pid == NULL)
    {
        return;
    }

    err_now = set_now - real_now;

    pid->set = set_now;
    pid->preset = set_now;
    pid->real = real_now;
    pid->pre_real = real_now;
    pid->err = err_now;
    pid->err_last = err_now;
    pid->err_llast = err_now;
    pid->integral = 0.0f;
    pid->pout = 0.0f;
    pid->iout = 0.0f;
    pid->dout = 0.0f;
    pid->fout = 0.0f;
    pid->out = 0.0f;
    pid->out_last = 0.0f;
    pid->d_filter.out = 0.0f;
}

float PID_Cal(PID_T *pid, float get, double set)
{
    float abs_error;
    float speed_ratio = 1.0f;
    float dt = (pid->dt > 0.0f) ? pid->dt : 1.0f;

    pid->set = set;
    pid->real = get;

    pid->err_last = pid->err;
    pid->err = (float)(pid->set - pid->real);

    abs_error = fabsf(pid->err);

    if (abs_error < pid->dead) {
        pid->set = pid->real;
        pid->err = 0.0f;
        abs_error = 0.0f;
    }

    // variable speed integral ratio
    if ((pid->i_var_speed_a == 0.0f) && (pid->i_var_speed_b == 0.0f)) {
        speed_ratio = 1.0f;
    } else if (abs_error <= pid->i_var_speed_b) {
        speed_ratio = 1.0f;
    } else if ((abs_error > pid->i_var_speed_b) &&
               (abs_error < pid->i_var_speed_a + pid->i_var_speed_b)) {
        speed_ratio = (pid->i_var_speed_a + pid->i_var_speed_b - abs_error) /
                      pid->i_var_speed_a;
    } else {
        speed_ratio = 0.0f;
    }

    // P
    pid->pout = pid->kp * pid->err;
    pid->pout = VALUE_LIMIT(pid->pout, -pid->poutmax, pid->poutmax);

    // I (with integral clamping: Scheme 1)
    if (pid->I_Separate_Threshold == 0.0f || abs_error < pid->I_Separate_Threshold) {
        pid->integral += speed_ratio * dt * pid->err;

        // --- NEW: clamp integral accumulator to avoid windup ---
        // integral_max chosen so that |ki * integral| <= ioutmax
        if (pid->ki != 0.0f) {
            float ki_abs = fabsf(pid->ki);
            float integral_max = pid->ioutmax / ki_abs;
            pid->integral = VALUE_LIMIT(pid->integral, -integral_max, integral_max);
        }
        // -------------------------------------------------------

        pid->iout = pid->ki * pid->integral;
    } else {
        pid->integral = 0.0f;
        pid->iout = 0.0f;
    }

    pid->iout = VALUE_LIMIT(pid->iout, -pid->ioutmax, pid->ioutmax);

    // D
    float raw_dout;
    if (pid->d_first_enable) {
        raw_dout = -pid->kd * (pid->real - pid->pre_real) / dt;
    } else {
        raw_dout = pid->kd * (pid->err - pid->err_last) / dt;
    }

    float alpha = VALUE_LIMIT(pid->lpf_alpha, 0.0f, 1.0f);
    pid->dout = first_order_filter(&pid->d_filter, raw_dout, alpha);
    pid->dout = VALUE_LIMIT(pid->dout, -pid->doutmax, pid->doutmax);

    // F
    pid->fout = (float)((pid->set - pid->preset) * pid->kf);
    pid->fout = VALUE_LIMIT(pid->fout, -pid->foutmax, pid->foutmax);

    // OUT
    pid->out = pid->pout + pid->iout + pid->dout + pid->fout;
    pid->out = VALUE_LIMIT(pid->out, -pid->outmax, pid->outmax);

    // update state
    pid->preset = pid->set;
    pid->pre_real = pid->real;
    pid->out_last = pid->out;

    return pid->out;
}


void PID_InitFromConfig(PID_T *pid, const PID_Config_t *config)
{
    if (pid == NULL || config == NULL)
    {
        return;
    }

    pid->kp = config->kp;
    pid->ki = config->ki;
    pid->kd = config->kd;
    pid->kf = config->kf;
    pid->d_first_enable = config->d_first_enable;
    pid->dead = config->dead;
    pid->I_Separate_Threshold = config->I_Separate_Threshold;
    pid->i_var_speed_a = config->i_var_speed_a;
    pid->i_var_speed_b = config->i_var_speed_b;
    pid->dt = config->dt;
    pid->poutmax = config->poutmax;
    pid->ioutmax = config->ioutmax;
    pid->doutmax = config->doutmax;
    pid->foutmax = config->foutmax;
    pid->outmax = config->outmax;
    pid->lpf_alpha = config->lpf_alpha;

    pid->pout = 0.0f;
    pid->iout = 0.0f;
    pid->dout = 0.0f;
    pid->fout = 0.0f;
    pid->out = 0.0f;
    pid->out_last = 0.0f;
    pid->set = 0.0f;
    pid->preset = 0.0f;
    pid->real = 0.0f;
    pid->pre_real = 0.0f;
    pid->err = 0.0f;
    pid->err_last = 0.0f;
    pid->err_llast = 0.0f;
    pid->integral = 0.0f;
    pid->d_filter = (first_order_filter_t){0};
}
void Motor_Control_Run(float setPit, float setYaw, float L_setST, float R_setST, float DMPit)
{
    uint8_t feedforward_enable = GimbalFeedforward_IsEnabled();
    AxisFeedforwardOutput_t pitch_ff;
    AxisFeedforwardOutput_t yaw_ff;

    PID_Cal(&GMPitchPid_Ang,
            imu_angle.pitch,
            setPit);

    pitch_ff = AxisFeedforward_Calc(&g_pitch_ff_config,
                                    feedforward_enable,
                                    GMPitchPid_Ang.out,
                                    vision_recieve_union.receive_vision.v_pitch,
                                    vision_recieve_union.receive_vision.a_pitch,
                                    GMPitchPid_Ang.out);
    GMPitchTorqueFeedforward = pitch_ff.torque_total_ff;

    PID_Cal(&GMPitchPid_Spe,
            imu_angle.gyro_y,
            pitch_ff.speed_set);

    PID_Cal(&GMYawPid_Ang,
            imu_angle.yaw_real,
            setYaw);

    yaw_ff = AxisFeedforward_Calc(&g_yaw_ff_config,
                                  feedforward_enable,
                                  GMYawPid_Ang.out,
                                  vision_recieve_union.receive_vision.v_yaw,
                                  vision_recieve_union.receive_vision.a_yaw,
                                  GMYawPid_Ang.out);

    motor[MotorYaw].ctrl.tor_set =
        Stop *(PID_Cal(&GMYawPid_Spe,
                       first_order_filter(&imu_speed_filter, imu_angle.gyro_z, 0.4f),
                       yaw_ff.speed_set) +yaw_ff.torque_total_ff);

    if ((All_control.DJ_handler.YAW > GIMBAL_YAW_REINIT_CNT) ||
        (All_control.DJ_handler.PITCH2 > GIMBAL_PITCH2_REINIT_CNT))
    {
        motor[MotorYaw].ctrl.tor_set = 0.0f;
    }
//    vofa_transmit(All_control.gimbal.yaw_target,
//                  All_control.zimiao.yaw_zimiao,
//                  imu_angle.yaw,
//                  imu_angle.yaw);
	// vofa_transmit(pitch_ff.speed_set,
      //           imu_angle.gyro_z,
      //           imu_angle.pitch,
      //           setPit);
    PID_Cal(&STRpid_Spe,
            SHOOT_R.re_speed,
            -R_setST);
    PID_Cal(&STLpid_Spe,
            SHOOT_L.re_speed,
            L_setST);

    motor[MotorPitch2].ctrl.pos_set = DMPit;
}

static const PID_Config_t g_pid_config_pitch_angle = {
    .kp = 30.0f,
    .ki = 0.0f,
    .kd = 576.0f,
    .kf = 380.0f,
    .d_first_enable = 1,
    .dead = 0.2f,
    .I_Separate_Threshold = 4.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 1.0f,
    .poutmax = 22000.0f,
    .ioutmax = 200.0f,
    .doutmax = 12000.0f,
    .foutmax = 2000.0f,
    .outmax = 16000.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_yaw_angle = {
    .kp = 32.0f,
    .ki = 0.0f,
    .kd =280.0f,
    .kf = 350.0f,
    .d_first_enable = 1,
    .dead = 0.0f,
    .I_Separate_Threshold = 0.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 1.0f,
    .poutmax = 16000.0f,
    .ioutmax = 5000.0f,
    .doutmax = 10.0f,
    .foutmax = 5000.0f,
    .outmax = 16000.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_pitch_speed = {
    .kp = 56.0f,
    .ki = 0.07f,
    .kd = 0.0f,
    .kf = 0.0f,
    .d_first_enable = 0,
    .dead = 0.0f,
    .I_Separate_Threshold = 2900.0f,
    .i_var_speed_a = 900.0f,
    .i_var_speed_b = 2000.0f,
    .dt = 1.0f,
    .poutmax = 40000.0f,
    .ioutmax = 4000.0f,
    .doutmax = 10000.0f,
    .foutmax = 1000.0f,
    .outmax = 16300.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_yaw_speed = {
    .kp = 52.0f,
    .ki = 0.07f,
    .kd = 0.0f,
    .kf = 0.0f,
    .d_first_enable = 0,
    .dead = 0.0f,
    .I_Separate_Threshold = 12.0f,
    .i_var_speed_a = 4.5f,
    .i_var_speed_b = 7.5f,
    .dt = 1.0f,
    .poutmax = 15000.0f,
    .ioutmax = 2000.0f,
    .doutmax = 10000.0f,
    .foutmax = 1000.0f,
    .outmax = 15300.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_pitch2_angle = {
    .kp = 2.935f,
    .ki = 0.0f,
    .kd = 10.0f,
    .kf = 0.0f,
    .d_first_enable = 1,
    .dead = 0.0f,
    .I_Separate_Threshold = 0.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 0.0f,
    .poutmax = 16000.0f,
    .ioutmax = 5000.0f,
    .doutmax = 8000.0f,
    .foutmax = 3000.0f,
    .outmax = 16000.0f,
    .lpf_alpha = 0.07f,
};

static const PID_Config_t g_pid_config_pitch2_speed = {
    .kp = 1455.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .kf = 0.0f,
    .d_first_enable = 1,
    .dead = 0.0f,
    .I_Separate_Threshold = 0.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 0.0f,
    .poutmax = 16000.0f,
    .ioutmax = 5000.0f,
    .doutmax = 8000.0f,
    .foutmax = 3000.0f,
    .outmax = 16000.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_feed_speed = {
    .kp = 4.2f,
    .ki = 0.0f,
    .kd = 10.0f,
    .kf = 0.0f,
    .d_first_enable = 0,
    .dead = 0.0f,
    .I_Separate_Threshold = 100.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 0.0f,
    .poutmax = 16000.0f,
    .ioutmax = 2000.0f,
    .doutmax = 1000.0f,
    .foutmax = 0.0f,
    .outmax = 16000.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_stl_speed = {
    .kp = 10.5f,
    .ki = 0.02f,
    .kd = 0.0f,
    .kf = 0.0f,
    .d_first_enable = 0,
    .dead = 0.0f,
    .I_Separate_Threshold = 1000.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 0.0f,
    .poutmax = 26384.0f,
    .ioutmax = 2000.0f,
    .doutmax = 10000.0f,
    .foutmax = 0.0f,
    .outmax = 26384.0f,
    .lpf_alpha = 0.1f,
};

static const PID_Config_t g_pid_config_str_speed = {
    .kp = 10.5f,
    .ki = 0.02f,
    .kd = 0.0f,
    .kf = 0.0f,
    .d_first_enable = 0,
    .dead = 0.0f,
    .I_Separate_Threshold = 1000.0f,
    .i_var_speed_a = 0.0f,
    .i_var_speed_b = 0.0f,
    .dt = 0.0f,
    .poutmax = 26384.0f,
    .ioutmax = 2000.0f,
    .doutmax = 10000.0f,
    .foutmax = 0.0f,
    .outmax = 26384.0f,
    .lpf_alpha = 0.1f,
};

void PID_Init_All(void)
{
    PID_InitFromConfig(&GMPitchPid_Ang, &g_pid_config_pitch_angle);
    PID_InitFromConfig(&GMPitchPid_Spe, &g_pid_config_pitch_speed);
    PID_Init_GimbalYaw();
    PID_Init_GimbalPitch2();
    PID_InitFromConfig(&Feedpid_Spe, &g_pid_config_feed_speed);
    PID_InitFromConfig(&STLpid_Spe, &g_pid_config_stl_speed);
    PID_InitFromConfig(&STRpid_Spe, &g_pid_config_str_speed);

    // ????????
    //LADRC_Init(&ladrc_yaw, 20.0f, 100.0f, 300.0f, 200.0f, 0.001f, 16384.0f);
    //SMC_Init(&smc_dm, 10.0f, 0.0f, 100.0f, 0.05f, 28000.0f, 1.0f, 80.0f);
    //???ladrc??smc?????�h???????
}

void PID_Init_GimbalYaw(void)
{
    PID_InitFromConfig(&GMYawPid_Ang, &g_pid_config_yaw_angle);
    PID_InitFromConfig(&GMYawPid_Spe, &g_pid_config_yaw_speed);
    CoulombFrictionFF_Init(&yaw_friction_ff, 0.0f, 0.0f, 0.0f);
}

void PID_Init_GimbalPitch2(void)
{
    PID_InitFromConfig(&PITCH2_Ang, &g_pid_config_pitch2_angle);
    PID_InitFromConfig(&PITCH2_Spe, &g_pid_config_pitch2_speed);
}
