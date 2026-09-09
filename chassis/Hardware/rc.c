#include "rc.h"
#include "main.h"

#define RX_BUF_SIZE 18		//DMA接收缓冲区大小
u8 RXBuff[RX_BUF_SIZE];		//DMA接收缓冲区
u8 sendchassic[5];

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


float keyboard_jump_flag_shift,keyboard_jump_flag_e,key_shift_last,key_e_last;

void deal_dbus(u8 *RXBuff)
{
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
		temp = 1024 - (RXBuff[16]|(RXBuff[17] << 8));//遥控器侧面的拨轮
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
		//遥控器值抛出异常
		rc.R_x = rc.R_x > 660 ? 0 : rc.R_x;
		rc.R_x = rc.R_x < -660 ? 0 : rc.R_x;
		
		rc.R_y = rc.R_y > 660 ? 0 : rc.R_y;
		rc.R_y = rc.R_y < -660 ? 0 : rc.R_y;
		
		rc.L_x = rc.L_x > 660 ? 0 : rc.L_x;
		rc.L_x = rc.L_x < -660 ? 0 : rc.L_x;
		
		rc.L_y = rc.L_y > 660 ? 0 : rc.L_y;
		rc.L_y = rc.L_y < -660 ? 0 : rc.L_y;
		
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


//串口中断
uint16_t rrr;
void USART3_IRQHandler(void)
{
	rrr++;
	  if(USART3->SR & UART_FLAG_IDLE)//接收到数据
    {
			
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&huart3);

        if ((hdma_usart3_rx.Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = 36 - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart3_rx.Instance->NDTR = 36;

            //set memory buffer 1
            //设定缓冲区1
            hdma_usart3_rx.Instance->CR |= DMA_SxCR_CT;
            
            //enable DMA
            //使能DMA
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
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = 36 - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart3_rx.Instance->NDTR = 36;

            //set memory buffer 0
            //设定缓冲区0
            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
            
            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if(this_time_rx_len == 18)
            {
		         HAL_UART_Receive_DMA(&huart3, (uint8_t*)dbus_recieve_2, 18);                                               
		         deal_dbus(dbus_recieve_2);
            }
        }
    }

}


