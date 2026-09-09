
#include "motor.h"
#include "main.h"
#include <string.h>
uint8_t usedata[8];
Data Data_C    =    {0};
#define RATE_BUF_SIZE 6
#define MYABS(a)    ((a >= 0) ? (a) : -(a)) 
#define VALUE_LIMIT(v, l, h)   ((v) < (l) ? (l) : ((v) > (h) ? (h) : (v)))
#define JAM_SPEED_THR    100     // rpm
#define JAM_CURRENT_THR  6500   // torque current
#define JAM_TIME_MS      400    // ms
#define UNJAM_TIME_MS    400    // ms
#define UNJAM_SPEED      (-400) // rpm

dji_motor Yaw={0,0,0,0,0,0,0,0,0,0,0,0};
dji_motor BR={0,0,0,0,0,0,0,0,0,0,0,0};
dji_motor BL={0,0,0,0,0,0,0,0,0,0,0,0};
dji_motor FL={0,0,0,0,0,0,0,0,0,0,0,0};
dji_motor FR={0,0,0,0,0,0,0,0,0,0,0,0};
dji_motor shoot_2006={0,0,0,0,0,0,0,0,0,0,0,0};
PID_T BR_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T BL_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T FL_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T FR_Spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T follow_Yaw_speed_loop={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T follow_Yaw_angle_loop={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T spin_spe={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
PID_T Buffer_power;
PID_T pid_shoot_2006={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
ONLINE Online;
extern CHASSIS_Task chassis_task;
extern Robot_Para robot;
u8 back_volt;
#define CHASSIS_DEVICE_LOST_LIMIT 500U
#define CHASSIS_DEVICE_WATCHDOG_MAX (CHASSIS_DEVICE_LOST_LIMIT + 1U)

static uint8_t shoot_2006_unjam_active = 0U;
static uint32_t shoot_2006_jam_start_tick = 0U;
static uint32_t shoot_2006_unjam_start_tick = 0U;

static void Chassis_DeviceWatchdog_Inc(uint16_t *cnt)
{
	if (*cnt <= CHASSIS_DEVICE_LOST_LIMIT)
	{
		(*cnt)++;
	}
}

void Chassis_DeviceWatchdog_Init(void)
{
	Data_C.rx_watchdog_cnt = CHASSIS_DEVICE_WATCHDOG_MAX;
	Data_C.yaw_rx_watchdog_cnt = CHASSIS_DEVICE_WATCHDOG_MAX;
}

void Chassis_DeviceWatchdog_Tick(void)
{
	Chassis_DeviceWatchdog_Inc(&Data_C.rx_watchdog_cnt);
	Chassis_DeviceWatchdog_Inc(&Data_C.yaw_rx_watchdog_cnt);
	Online.c_board++;
	Online.capcity++;
	Online.pitch2++;
	Online.shoot_2006++;
	Online.whe1++;
	Online.whe2++;
	Online.whe3++;
	Online.whe4++;
	Online.yaw++;
    Online.Cc_board=Online.c_board>200?0:8;//0red 8white
	Online.Ccapcity=Online.capcity>200?0:8;
	Online.Cpitch2=Online.pitch2>200?0:8;
	Online.Cshoot_2006=Online.shoot_2006>200?0:8;
	Online.Cwhe1=Online.whe1>200?0:8;
	Online.Cwhe2=Online.whe2>200?0:8;
	Online.Cwhe3=Online.whe3>200?0:8;
	Online.Cwhe4=Online.whe4>200?0:8;
	Online.Cyaw=Online.yaw>200?0:8;
}

uint8_t Chassis_DeviceWatchdog_IsLost(void)
{
	return (Data_C.rx_watchdog_cnt > CHASSIS_DEVICE_LOST_LIMIT) ||
	       (Data_C.yaw_rx_watchdog_cnt > CHASSIS_DEVICE_LOST_LIMIT);
}

void Chassis_ControlData_Online(void)
{
	Data_C.rx_watchdog_cnt = 0U;
}

void Chassis_YawData_Online(void)
{
	Data_C.yaw_rx_watchdog_cnt = 0U;
}

static void Shoot_2006_ClearJamState(void)
{
	shoot_2006_unjam_active = 0U;
	shoot_2006_jam_start_tick = 0U;
	shoot_2006_unjam_start_tick = 0U;
}

void getEncoderData(volatile dji_motor *v)
{
	int i=0;
	int32_t temp_sum = 0;    
	v->last_raw_value = v->raw_value;
	v->raw_value = (usedata[0]<<8)|usedata[1];
	v->diff = v->raw_value - v->last_raw_value;
	if(v->diff < -4096)    //���α������ķ���ֵ���̫�󣬱�ʾȦ�������˸ı�
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>4096)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff - 8192;
	}		
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//����õ������ı��������ֵ
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	//����õ��Ƕ�ֵ����Χ���������
	v->ecd_angle = (float)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	
	
	//�����ٶ�ƽ��ֵ
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	v->filter_rate = (int32_t)(temp_sum/RATE_BUF_SIZE);
	
	if(fabs((float)v->filter_rate_max) < fabs((float)v->filter_rate))
	{
		v->filter_rate_max = v->filter_rate;//Ħ���ֵ���Ч
	}
	v->re_speed= (int16_t)(usedata[2] << 8 | usedata[3]);   //�ٶ������
	v->current = (int16_t)(usedata[4] << 8 | usedata[5]);
	v->tempature = usedata[6];

}

void getControlData(Data *c)
{
	chassis_ctrl_t ctrl;
	
	c->VX  = (int16_t)(usedata[0] << 8 | usedata[1]);
	c->VY  = (int16_t)(usedata[2] << 8 | usedata[3]);
	
	// ʹ�� union �����ںϺ�Ŀ����ֽ�
	ctrl.raw = usedata[4];
	c->W      = ctrl.bits.w;
	c->stop   = ctrl.bits.stop;
	c->spin   = ctrl.bits.spin_mode;
	c->Shift  = ctrl.bits.accelerate;
	c->tunnel = ctrl.bits.tunnel;
	c->tunnel_switch = (int8_t)(ctrl.bits.reserved & 0x01U);
	c->shoot_speed = usedata[5]*30;
	Chassis_ControlData_Online();
}
  CAN_RxHeaderTypeDef	RxHeader;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{   
  CAN_RxFifoMsgHandler(hcan);
}
void getCapacity(capacity *c)
{
	c->CAP_J_TX_DATA=(int16_t)(usedata[0] << 8 | usedata[1]);
	c->CAP_U_TX_DATA=(int16_t)(usedata[2] << 8 | usedata[3]);
}

void  CAN_RxFifoMsgHandler(CAN_HandleTypeDef *hcan)
{
  if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeader,usedata) != HAL_OK)
  {
    return;
  }

  if(hcan->Instance==CAN1)
	{
		switch(RxHeader.StdId)
		{ 
		    case 0x00:
		        if(usedata[0]==0x13)
				{
					dm4310_fbdata(&motor[MotorYaw], usedata);
					Chassis_YawData_Online();
					Online.yaw=0;
				}
				if(usedata[0]==0x12)
				{
					Online.pitch2=0;
				}
            break;
			case 0x301 : getControlData(&Data_C);Online.c_board=0;break;
			//case 0x205 : getEncoderData(&shoot_2006);  break;
	    }	
	}
  if(hcan->Instance==CAN2)
	{
        switch(RxHeader.StdId)                   
        {
			case 0x201 : getEncoderData(&BR);Online.whe1=0; break;  //���̵������
			case 0x202 : getEncoderData(&BL);Online.whe2=0;  break;	  
			case 0x203 : getEncoderData(&FL);Online.whe3=0;  break;
			case 0x204 : getEncoderData(&FR);Online.whe4=0;  break;  
			case 0x205 : getEncoderData(&shoot_2006);Online.shoot_2006=0;  break;  //2006�������
			case 0x211 : getCapacity(&chassis_task.D_R);Online.capcity=0; break;
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

float PID_Cal(PID_T *pid, float get, double set)
{	
	float abs_error;
	float speed_ratio = 1.0f;
	float dt = (pid->dt > 0.0f) ? pid->dt : 1.0f;

	pid->set = set;
	pid->real = get;

	pid->err_last = pid->err;
	pid->err = pid->set - pid->real;
	
	abs_error = fabs(pid->err);

	if (abs_error < pid->dead)
	{
		pid->set = pid->real;
		pid->err = 0.0f;
		abs_error = 0.0f;
	}

	if ((pid->i_var_speed_a == 0.0f) && (pid->i_var_speed_b == 0.0f))
	{
		speed_ratio = 1.0f;
	}
	else if (abs_error <= pid->i_var_speed_b)
	{
		speed_ratio = 1.0f;
	}
	else if ((abs_error > pid->i_var_speed_b) && (abs_error < pid->i_var_speed_a + pid->i_var_speed_b))
	{
		speed_ratio = (pid->i_var_speed_a + pid->i_var_speed_b - abs_error) / pid->i_var_speed_a;
	}
	else
	{
		speed_ratio = 0.0f;
	}

	pid->pout = pid->kp * pid->err;
	pid->pout = VALUE_LIMIT(pid->pout, -pid->poutmax, pid->poutmax);

	if (pid->I_Separate_Threshold == 0.0f || abs_error < pid->I_Separate_Threshold)
	{
		pid->integral += speed_ratio * dt * pid->err;
		pid->iout = pid->ki * pid->integral;
	}
	else
	{
		pid->integral = 0.0f;
		pid->iout = 0.0f;
	}
	pid->iout = VALUE_LIMIT(pid->iout, -pid->ioutmax, pid->ioutmax);

	float raw_dout;
	if (pid->d_first_enable)
	{
		raw_dout = -pid->kd * (pid->real- pid->pre_real) / dt;
	}
	else
	{
		raw_dout = pid->kd * (pid->err - pid->err_last) / dt;
	}
	float alpha = VALUE_LIMIT(pid->lpf_alpha, 0.0f, 1.0f);
	pid->dout = first_order_filter(&pid->d_filter, raw_dout, alpha);
	pid->dout = VALUE_LIMIT(pid->dout, -pid->doutmax, pid->doutmax);

	pid->fout = (pid->set - pid->preset) * pid->kf;
	pid->fout = VALUE_LIMIT(pid->fout, -pid->foutmax, pid->foutmax);

	pid->out = pid->pout + pid->iout + pid->dout + pid->fout;
	pid->out = VALUE_LIMIT(pid->out, -pid->outmax, pid->outmax);

	pid->preset = pid->set;
	pid->pre_real = pid->real;
	pid->out_last = pid->out;
	return pid->out;
}
static void PID_ClearState(PID_T *pid, float real_feedback)
{
	pid->set = real_feedback;
	pid->preset = real_feedback;
	pid->real = real_feedback;
	pid->pre_real = real_feedback;
	pid->err = 0.0f;
	pid->err_last = 0.0f;
	pid->err_llast = 0.0f;
	pid->integral = 0.0f;
	pid->pout = 0.0f;
	pid->iout = 0.0f;
	pid->dout = 0.0f;
	pid->fout = 0.0f;
	pid->out = 0.0f;
	pid->out_last = 0.0f;
	pid->d_filter.out = 0.0f;
}

void Shoot_2006_StopCurrent(void)
{
	Shoot_2006_ClearJamState();
	PID_ClearState(&pid_shoot_2006, shoot_2006.re_speed);
	CAN2_0x1FF(0, 0, 0, 0, 2);
}

void Shoot_2006_SpeedLoop_SendCurrent(float target_speed)
{
	uint8_t pid_need_clear = 0U;
	uint32_t now = HAL_GetTick();

	uint8_t jam_cond = (MYABS(shoot_2006.re_speed) <= JAM_SPEED_THR) &&
	                  (MYABS(shoot_2006.current) >= JAM_CURRENT_THR);

	if (!shoot_2006_unjam_active)
	{
		if (jam_cond)
		{
			if (shoot_2006_jam_start_tick == 0U)
			{
				shoot_2006_jam_start_tick = now;
			}
			else if ((now - shoot_2006_jam_start_tick) >= JAM_TIME_MS)
			{
				shoot_2006_unjam_active = 1U;
				shoot_2006_unjam_start_tick = now;
				pid_need_clear = 1U;
			}
		}
		else
		{
			shoot_2006_jam_start_tick = 0U;
		}
	}

	if (shoot_2006_unjam_active)
	{
		if ((now - shoot_2006_unjam_start_tick) >= UNJAM_TIME_MS)
		{
			shoot_2006_unjam_active = 0U;
			shoot_2006_jam_start_tick = 0U;
			pid_need_clear = 1U;
		}
		else
		{
			target_speed = UNJAM_SPEED;
		}
	}

	if (pid_need_clear)
	{
		PID_ClearState(&pid_shoot_2006, shoot_2006.re_speed);
	}

	float current = PID_Cal(&pid_shoot_2006, shoot_2006.re_speed, target_speed);
	CAN2_0x1FF((int16_t)current, 0, 0, 0, 2);
}


static void PID_Init(PID_T *pid, const PID_Config_t *cfg)
{
	memset(pid, 0, sizeof(PID_T));
	pid->kp = cfg->kp; pid->ki = cfg->ki; pid->kd = cfg->kd; pid->kf = cfg->kf;
	pid->poutmax = cfg->poutmax; pid->ioutmax = cfg->ioutmax; pid->doutmax = cfg->doutmax; pid->foutmax = cfg->foutmax; pid->outmax = cfg->outmax;
	pid->I_Separate_Threshold = cfg->I_Separate_Threshold; pid->dead = cfg->dead; pid->lpf_alpha = cfg->lpf_alpha;
	pid->d_first_enable = cfg->d_first_enable; pid->i_var_speed_a = cfg->i_var_speed_a; pid->i_var_speed_b = cfg->i_var_speed_b;
	pid->dt = cfg->dt;
}

void PID_Init_All(void)
{
    static const PID_Config_t yaw_angle_cfg = {
        .kp = 10.0f,
        .ki = 0.0f,
        .kd = 0.0f,
        .kf = 0.0f,
        .poutmax = 16000.0f,
        .ioutmax = 2000.0f,
        .doutmax = 10000.0f,
        .foutmax = 2000.0f,
        .outmax = 16000.0f,
        .I_Separate_Threshold = 5.0f,
        .dead = 0.0f,
        .lpf_alpha = 0.3f,
        .d_first_enable = 1,
        .i_var_speed_a = 0.0f,
        .i_var_speed_b = 0.0f,
        .dt = 1.0f,
    };

    static const PID_Config_t yaw_speed_cfg = {
        .kp = 19.0f,
        .ki = 0.001f,
        .kd = 28.0f,
        .kf = 0.0f,
        .poutmax = 16000.0f,
        .ioutmax = 2000.0f,
        .doutmax = 10000.0f,
        .foutmax = 0.0f,
        .outmax = 16000.0f,
        .I_Separate_Threshold = 50.0f,
        .dead = 0.0f,
        .lpf_alpha = 0.1f,
        .d_first_enable = 0,
        .i_var_speed_a = 0.0f,
        .i_var_speed_b = 0.0f,
        .dt = 1.0f,
    };

    static const PID_Config_t buffer_power_cfg = {
        .kp = 1.0f,
        .ki = 0.01f,
        .kd = 0.0f,
        .kf = 0.0f,
        .poutmax = 30.0f,
        .ioutmax = 5.0f,
        .doutmax = 1000.0f,
        .foutmax = 0.0f,
        .outmax = 30.0f,
        .I_Separate_Threshold = 0.0f,
        .dead = 0.0f,
        .lpf_alpha = 0.1f,
        .d_first_enable = 0,
        .i_var_speed_a = 0.0f,
        .i_var_speed_b = 0.0f,
        .dt = 1.0f,
    };

    static const PID_Config_t spin_spe_cfg = {
        .kp = 12.0f,
        .ki = 0.05f,
        .kd = 20.0f,
        .kf = 0.0f,
        .poutmax = 16000.0f,
        .ioutmax = 100.0f,
        .doutmax = 10000.0f,
        .foutmax = 0.0f,
        .outmax = 16000.0f,
        .I_Separate_Threshold = 0.0f,
        .dead = 0.0f,
        .lpf_alpha = 0.1f,
        .d_first_enable = 0,
        .i_var_speed_a = 0.0f,
        .i_var_speed_b = 0.0f,
        .dt = 1.0f,
    };

    static const PID_Config_t shoot_2006_cfg = {
        .kp = 16.35f,
        .ki = 0.0f,
        .kd = 0.0f,
        .kf = 0.0f,
        .poutmax = 15000.0f,
        .ioutmax = 1500.0f,
        .doutmax = 1000.0f,
        .foutmax = 0.0f,
        .outmax = 16000.0f,
        .I_Separate_Threshold = 400.0f,
        .dead = 0.0f,
        .lpf_alpha = 1.0f,
        .d_first_enable = 0,
        .i_var_speed_a = 300.0f,
        .i_var_speed_b = 100.0f,
        .dt = 1.0f,
    };

    static const PID_Config_t chassis_speed_cfg = {
        .kp = 4.35f,
        .ki = 0.07f,
        .kd = 1.0f,
        .kf = 20.0f,
        .poutmax = 15000.0f,
        .ioutmax = 1500.0f,
        .doutmax = 1000.0f,
        .foutmax = 0.0f,
        .outmax = 16000.0f,
        .I_Separate_Threshold = 400.0f,
        .dead = 0.0f,
        .lpf_alpha = 1.0f,
        .d_first_enable = 0,
        .i_var_speed_a = 300.0f,
        .i_var_speed_b = 100.0f,
        .dt = 1.0f,
    };
    PID_Init(&BR_Spe, &chassis_speed_cfg);
    PID_Init(&BL_Spe, &chassis_speed_cfg);
    PID_Init(&FL_Spe, &chassis_speed_cfg);
    PID_Init(&FR_Spe, &chassis_speed_cfg);
	PID_Init(&follow_Yaw_angle_loop, &yaw_angle_cfg);
	PID_Init(&follow_Yaw_speed_loop, &yaw_speed_cfg);
	PID_Init(&Buffer_power, &buffer_power_cfg);
	PID_Init(&spin_spe, &spin_spe_cfg);
    PID_Init(&pid_shoot_2006, &shoot_2006_cfg);

}
