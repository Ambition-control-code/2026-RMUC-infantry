#include "rc.h"
#include "main.h"
#include "can.h"
#define RC_STICK_DEADZONE 11
#define RX_BUF_SIZE 18		//DMA���ջ�������С
u8 RXBuff[RX_BUF_SIZE];		//DMA���ջ�����
u8 sendchassic[5];
extern float L_setST,R_setST;

RC rc;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern u8 dbus_recieve_1[36],dbus_recieve_2[36];
u8 if_change(u8 flag,u8 last_flag)
{
if(flag!=last_flag) {return 1;}
else {return 0;}
}

u8 if_jump_up(u8 flag,u8 last_flag)
{
if(flag-last_flag==-1) {return 1;}
else {return 0;}
}

uint16_t rrr;
float debus_jishi;
float keyboard_jump_flag_shift,keyboard_jump_flag_e,key_shift_last,key_e_last;

static int16_t rc_apply_deadzone(int16_t value, int16_t deadzone)
{
	if (deadzone < 0)
	{
		deadzone = 0;
	}

	if (value > deadzone)
	{
		return value - deadzone;
	}
	if (value < -deadzone)
	{
		return value + deadzone;
	}
	return 0;
}

static void rc_apply_stick_deadzone(RC *rc_data, int16_t deadzone)
{
	rc_data->R_x = rc_apply_deadzone(rc_data->R_x, deadzone);
	rc_data->R_y = rc_apply_deadzone(rc_data->R_y, deadzone);
	rc_data->L_x = rc_apply_deadzone(rc_data->L_x, deadzone);
	rc_data->L_y = rc_apply_deadzone(rc_data->L_y, deadzone);
}

void deal_dbus(u8 *RXBuff)
{
    debus_jishi=0;
		if(((RXBuff[0]| (RXBuff[1] << 8)) & 0x07ff)){
		rc.R_x = ((RXBuff[0]| (RXBuff[1] << 8)) & 0x07ff)-1024; 
		sendchassic[0]=RXBuff[0];
		sendchassic[1]=RXBuff[1];}
		if((((RXBuff[1] >> 3) | (RXBuff[2] << 5)) & 0x07ff)){
		rc.R_y = (((RXBuff[1] >> 3) | (RXBuff[2] << 5)) & 0x07ff)-1024;
		sendchassic[2]=RXBuff[2];} //!< Channel 1
		if((((RXBuff[2] >> 6) | (RXBuff[3] << 2) | (RXBuff[4] << 10))& 0x07ff)){
		rc.L_x = (((RXBuff[2] >> 6) | (RXBuff[3] << 2) | (RXBuff[4] << 10))& 0x07ff)-1024;}//!< Channel 2
		if((((RXBuff[4] >> 1) | (RXBuff[5] << 7)) & 0x07ff)){
		rc.L_y = (((RXBuff[4] >> 1) | (RXBuff[5] << 7)) & 0x07ff)-1024;} //!< Channel 3
		rc.sl = (((RXBuff[5] >> 4)& 0x000C) >> 2); //!< Switch left
		rc.sr = (((RXBuff[5] >> 4)& 0x0003)); //!< Switch left
		int16_t temp;
		temp = 1024 - (RXBuff[16]|(RXBuff[17] << 8));//ң��������Ĳ���???
		if(temp>3000) {rc.trigger=65535-temp;}
		else {rc.trigger=temp;}

		rc.mouse_x = RXBuff[6] | (RXBuff[7] << 8); //!< Mouse X axis
		rc.mouse_y = RXBuff[8] | (RXBuff[9] << 8); //!< Mouse Y axis
		rc.mouse_z = RXBuff[10] | (RXBuff[11] << 8); //!< Mouse Z axis
		rc.mouse_l = RXBuff[12]; //!< Mouse Left Is Press ?
		rc.mouse_r = RXBuff[13]; //!< Mouse Right Is Press ?
		rc.key = RXBuff[14] | (RXBuff[15] << 8); //!< KeyBoard value
		rc.W = (rc.key&0x01)>>0;
		rc.S = (rc.key&0x02)>>1;
		rc.A = (rc.key&0x04)>>2;
		rc.D = (rc.key&0x08)>>3;
		rc.Shift = (rc.key&0x10)>>4;
		rc.Ctrl = (rc.key&0x20)>>5;
		rc.Q = (rc.key&0x40)>>6;
		rc.E = (rc.key&0x80)>>7;
		rc.R = (rc.key&0x100)>>8;
		rc.F = (rc.key&0x200)>>9;
		rc.G = (rc.key&0x400)>>10;
		rc.Z = (rc.key&0x800)>>11;
		rc.X = (rc.key&0x1000)>>12;
		rc.C = (rc.key&0x2000)>>13;
		rc.V = (rc.key&0x4000)>>14;
		rc.B = (rc.key&0x8000)>>15;
		//ң����ֵ�׳��쳣
		rc.R_x = rc.R_x > 660 ? 0 : rc.R_x;
		rc.R_x = rc.R_x < -660 ? 0 : rc.R_x;
		
		rc.R_y = rc.R_y > 660 ? 0 : rc.R_y;
		rc.R_y = rc.R_y < -660 ? 0 : rc.R_y;
		
		rc.L_x = rc.L_x > 660 ? 0 : rc.L_x;
		rc.L_x = rc.L_x < -660 ? 0 : rc.L_x;
		
		rc.L_y = rc.L_y > 660 ? 0 : rc.L_y;
		rc.L_y = rc.L_y < -660 ? 0 : rc.L_y;

		rc_apply_stick_deadzone(&rc, RC_STICK_DEADZONE);
		
		rc.mouse_x = rc.mouse_x > 1024 ? 0 : rc.mouse_x;
		rc.mouse_x = rc.mouse_x < -1024 ? 0 : rc.mouse_x;
		rc.mouse_y = rc.mouse_y > 1024 ? 0 : rc.mouse_y;
		rc.mouse_y = rc.mouse_y < -1024 ? 0 : rc.mouse_y;
		
		rc.sl_change_flag=if_change(rc.sl,rc.last_sl);
		rc.sr_change_flag=if_change(rc.sr,rc.last_sr);	
        rc.mr_change_flag=if_change(rc.mouse_r,rc.last_mouse_r);		
        rc.ml_change_flag=if_change(rc.mouse_l,rc.last_mouse_l);		
		
		rc.last_sl=rc.sl;
		rc.last_sr=rc.sr;
		rc.last_mouse_r=rc.mouse_r;
		rc.last_mouse_l=rc.mouse_l;
}


static float s_kb_x = 0.0f, s_kb_y = 0.0f;

void rc_reset_keyboard_ramp(void)
{
	s_kb_x = 0.0f;
	s_kb_y = 0.0f;
}

/**
 * @brief 键盘斜坡规划器 —— 让信号按设定速率平滑逼近目标值
 * @param current   当前值
 * @param target    目标值
 * @param up_step   向非零目标逼近时的步长（正值）
 * @param down_step 回归零时的步长（正值，通常设得比 up_step 大以快速停车）
 * @return 斜坡后的新值
 */
static float kb_ramp(float current, float target, float up_step, float down_step)
{
	float step = (target == 0.0f) ? down_step : up_step;

	if (current < target) {
		current += step;
		if (current > target) current = target;
	} else if (current > target) {
		current -= step;
		if (current < target) current = target;
	}
	return current;
}

extern int head_moad;

Slow S_W;

uint8_t spin = 0,stop=0,Shift=0,other=0;

void mecanum_Resolving(void)
{//����һλ�����п��Ƶı�־λ����
	
	// Confirm F only after the raw state remains unchanged for 20 ms.
	if((rc.Q==1)&&rc.F_Q==0&&rc.L_Q==0)
	{
		rc.F_Q=1;
	}
	else if((rc.Q==1)&&rc.F_Q==1&&rc.L_Q==0)
	{
		rc.F_Q=0;
	}
	rc.L_Q=rc.Q;
	
	// spin控制：sr=1强制旋转；sr≠1时Ctrl切换状态机
	static uint8_t spin_toggle = 0U;
	static uint8_t ctrl_last   = 0U;
	static uint8_t sr_was_1    = 0U;

	if (rc.sr == 1U) {
		spin = 1;
		spin_toggle = 0U;
		sr_was_1 = 1U;
	} else {
		if (sr_was_1) {
			spin = 0;
			spin_toggle = 0U;
			sr_was_1 = 0U;
		}
		// Ctrl上升沿翻转
		if (rc.Ctrl == 1U && ctrl_last == 0U) {
			spin_toggle = !spin_toggle;
		}
		ctrl_last = rc.Ctrl;
		spin = spin_toggle;
	}	
   if(rc.sl == 2||rc.B == 1||VT13_USE.use_data.B)
	{
		stop = 1;
	}
	else 
	{
		stop = 0;
	}

	if(rc.Ctrl==1)//����       //����
	{
	  Shift = 1;
	}
	else
	{
	  Shift = 0;
	}
	
	float rc_x, rc_y, X, Y, aa;
	// WASD 目标：W=前进(-Y) S=后退(+Y)，同时按下则抵消
	float kb_y_target = 0.0f;
	if (rc.W && !rc.S)      kb_y_target = -KB_RAMP_MAX;
	else if (rc.S && !rc.W) kb_y_target =  KB_RAMP_MAX;

	// WASD 目标：A=左(+X) D=右(-X)，同时按下则抵消
	float kb_x_target = 0.0f;
	if (rc.A && !rc.D)      kb_x_target =  KB_RAMP_MAX;
	else if (rc.D && !rc.A) kb_x_target = -KB_RAMP_MAX;

	// 方向反转立刻归零，保证跟手：比如长按W后突然按S，前进量瞬间清零再反向
	if (s_kb_y * kb_y_target < 0.0f) s_kb_y = 0.0f;
	if (s_kb_x * kb_x_target < 0.0f) s_kb_x = 0.0f;

	s_kb_y = kb_ramp(s_kb_y, kb_y_target, KB_RAMP_UP_STEP, KB_RAMP_DOWN_STEP);
	s_kb_x = kb_ramp(s_kb_x, kb_x_target, KB_RAMP_UP_STEP, KB_RAMP_DOWN_STEP);

	rc_x = (rc.R_x) * remote_K;
	rc_y = (rc.R_y) * remote_K;

	Y = s_kb_y - rc_y;
	X = s_kb_x - rc_x;
	
	aa = sqrt(X*X+Y*Y);
	X = aa <limit ? X : X*(limit/aa);
	X = aa >-limit ? X : X*(limit/aa);
	Y = aa <limit ? Y : Y*(limit/aa);
	Y = aa >-limit ? Y : Y*(limit/aa);  

	
      uint16_t avg = (uint16_t)((abs(SHOOT_L.re_speed) + abs(SHOOT_R.re_speed)) * 0.5f);
      uint8_t shoot_speed = (avg >= 7650) ? 255U : (uint8_t)(avg * 0.03333333f);
      chassis_out_update(X,Y,All_control.shoot.fire,All_control.gesture_task,spin,stop,
                        rc.F, rc.G, shoot_speed);
}

//�����ж�

void USART3_IRQHandler(void)
{

	  if(USART3->SR & UART_FLAG_IDLE)//���յ�����
    {
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&huart3);

        if ((hdma_usart3_rx.Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            //disable DMA
            //ʧЧDMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //��ȡ�������ݳ���,���� = �趨���� - ʣ�೤��
            this_time_rx_len = 36 - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //�����趨���ݳ���
            hdma_usart3_rx.Instance->NDTR = 36;

            //set memory buffer 1
            //�趨������1
            hdma_usart3_rx.Instance->CR |= DMA_SxCR_CT;
            
            //enable DMA
            //ʹ��DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if(this_time_rx_len == 18)
            {
		HAL_UART_Receive_DMA(&huart3, (uint8_t*)dbus_recieve_1, 18);                                               
		deal_dbus(dbus_recieve_1);
            }
        }
        else
        {
            /* Current memory buffer used is Memory 1 */
            //disable DMA
            //ʧЧDMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //��ȡ�������ݳ���,���� = �趨���� - ʣ�೤��
            this_time_rx_len = 36 - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //�����趨���ݳ���
            hdma_usart3_rx.Instance->NDTR = 36;

            //set memory buffer 0
            //�趨������0
            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
            
            //enable DMA
            //ʹ��DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if(this_time_rx_len == 18)
            {
		         HAL_UART_Receive_DMA(&huart3, (uint8_t*)dbus_recieve_2, 18);                                               
		         deal_dbus(dbus_recieve_2);
            }
        }
    }

}


