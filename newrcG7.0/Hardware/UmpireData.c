#include "main.h"
#include "string.h"

#define  Dma_BufferSize     128  //DMA缓冲区大小，必须为2的幂
#define  FrameSize          128  //帧大小

typedef struct {
    u8  HC;           //CPU处理进度
    u8  dizhi:7;      //地址
    u8  jindu;        //CPU处理进度
    u16 huan;         //CPU剩余处理字节数
} shujiu;

shujiu JISHU;
extern send_struct send_vision;
Robot_Para robot;              //机器人参数结构体
u8  DMA_Buffer[Dma_BufferSize]; //DMA缓冲区

u8  Frame     [FrameSize];      //帧缓冲区
u8  DMA_TX_Buffer[Dma_BufferSize];
u8  DMA_RX_Buffer[Dma_BufferSize];
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

// 初始化函数
void Umpire_UsartInit()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    // USART1 GPIO配置
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // USART6 GPIO配置
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_14;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // USART1初始化
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    // USART6初始化
    huart6.Instance = USART6;
    huart6.Init.BaudRate = 115200;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX_RX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart6) != HAL_OK)
    {
        Error_Handler();
    }

    // USART1 RX DMA初始化
    hdma_usart1_rx.Instance = DMA2_Stream5;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

    // USART1 TX DMA初始化
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);

    // 使能USART1 DMA接收
    if (HAL_UART_Receive_DMA(&huart1, DMA_Buffer, Dma_BufferSize) != HAL_OK)
    {
        Error_Handler();
    }
// // 使能串口 1 的 DMA 发送
//    if (HAL_UART_Transmit_DMA(&huart1, DMA_TX_Buffer, Dma_BufferSize) != HAL_OK)
//    {
//        Error_Handler();
//    }
    // 使能USART6接收中断
    HAL_NVIC_SetPriority(USART6_IRQn, 2, 3);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);

    // 使能DMA中断
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 2, 3);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

u16 heceng16 (u8 a, u8 b)   // 8位转16位
{
    u16 c = 0;
    c = ((c + a) << 8) + b;
    return c;
}

u32 heceng32(u8 a, u8 b, u8 c, u8 d)  // 8位转32位
{
    u32 chu = 0;
    u16 zhong1 = 0;
    u16 zhong2 = 0;
    zhong1 = heceng16(a, b);
    zhong2 = heceng16(c, d);
    chu = ((0x00000000 + zhong1) << 16) + zhong2;
    return chu;
}

u8 tiqv (u8 shuru, u8 wei, u8 shu)  
{
    // 从1字节数据中提取指定位数的数据
    // shuru 输入的1字节数据
    // wei   起始位（从左到右）
    // shu   提取的位数
    u8 shuchu;
    u8 weiyi;
    weiyi = 8 - wei + 1 - shu;
    shuchu = shuru << weiyi;
    shuchu = shuchu >> (8 - shu);
    return shuchu;
}

u16 cmd_id;
u16 data_length;
u8 a_ = 1; // 数组索引

volatile union zhuanhua  // float和字节数组转换
{
    float powerr;
    unsigned char g[4];
} ppp;

int16_t power_limit_hero;

// DMA 传输完成回调函数
void HAL_UART_TxCpltCallback (UART_HandleTypeDef *huart)
{	
if (huart == &huart1)
{
// 禁用 DMA
HAL_DMA_Abort (&hdma_usart1_tx);
// 使能串口发送完成中断
__HAL_UART_ENABLE_IT (&huart1, UART_IT_TC);
}
}
// 使用 DMA 发送数据的函数
void DMA_USART1_Send (uint8_t *data, uint16_t size)
{
  memcpy(DMA_TX_Buffer, data, size); // 复制数据到DMA发送缓冲区
// 等待 DMA 停止
//while (HAL_DMA_GetState (&hdma_usart1_tx) != HAL_DMA_STATE_READY);
// 使用 DMA 发送数据
HAL_UART_Transmit_IT (&huart1, data, size);
	//    memcpy(DMA_TX_Buffer, data, size); // 复制数据到DMA发送缓冲区

}

//void USART6_IRQHandler();// USART6中断处理函数

void DMA2_Stream5_IRQHandler();// DMA2 Stream5中断处理函数    接收

void DMA2_Stream7_IRQHandler();// DMA2 Stream7中断处理函数     发送


//// DMA发送完成中断处理函数
//u16 USART_RX_STA=0;       //接收状态标记	
//u8 Res;
//u8 USART_RX_BUF[200];     //接收缓冲,最大USART_REC_LEN个字节.

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart == &huart1) {
//        if ((USART_RX_STA & 0x8000) == 0) { // 接收未完成
//            if (USART_RX_STA & 0x4000) { // 接收到了 0x0d
//                if (Res != 0x0a) {
//                    USART_RX_STA = 0; // 接收错误，重新开始
//                } else {
//                    USART_RX_STA |= 0x8000; // 接收完成了
//                }
//            } else { // 还没收到 0X0D
//                if (Res == 0x0d) {
//                    USART_RX_STA |= 0x4000;
//                } else {
//                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
//                    USART_RX_STA++;
//                    if (USART_RX_STA > (200 - 1)) {
//                        USART_RX_STA = 0; // 接收数据错误，重新开始接收
//                    }
//                }
//            }
//        }

//        // 再次使能接收中断以继续接收下一个字节
//        HAL_UART_Receive_IT(huart, &Res, 1);
//    }
//}    

// DMA接收半传输和传输完成中断处理函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        JISHU.HC += Dma_BufferSize / 2;               
       while(JISHU.HC>0)//dma????????????
	{
		 if(JISHU.huan>0)               //?????????????
		 {
	      Frame[a_]=DMA_Buffer[JISHU.dizhi];//???????
			  a_++;
			  JISHU.huan--;
		 
		 }
		 if(JISHU.huan<=0)
		 {
		 
		   switch (JISHU.jindu)
		   {
			   case 0://??0:???????0xA5
			   {
           if(DMA_Buffer[JISHU.dizhi]==0xA5)
           {
						  Frame[0]=0xa5;
							JISHU.jindu++;//???????
						  JISHU.huan=4; //????????4??????
						  a_=1;          //??????
						  break;
					 }
					 else{ a_=1;JISHU.jindu=0;break;}
			   }
		     case 1://??1:??????????,??????
		  	 {
				   if(!Verify_CRC8_Check_Sum(Frame,5)){Frame[0]=0;JISHU.jindu=0;a_=1;break;} //??CRC8??????,????
           data_length=heceng16(Frame[2],Frame[1]);                                 //???????
           JISHU.huan=data_length+4;	//cmd_id,data,frame_tail???	                                            //??????????
					 JISHU.jindu++;
					 break;
		     }
			   case 2://??2:??????????????
			   {
					 cmd_id=heceng16(Frame[6],Frame[5]);
			     if(!Verify_CRC16_Check_Sum(Frame,5+data_length+4))  {Frame[0]=0;JISHU.jindu=0;a_=1;break;}//CRC16??????,????
				   
			     switch (cmd_id)//?????
			     {
			        case 0x0001://????
				      {
//				          robot.game_type=tiqv(Frame[7],1,3);  
//					        robot.game_progress=Frame[7]>>3;
//					        robot.remain_time=heceng16(Frame[9],Frame[8]);
					          break;
				      }
			        case 0x0002://????
				      {
				            robot.winner=Frame[7];
					          break;
				      }
              case 0x0003://???????
              {
				          
//									  robot.red_1_HP=heceng16(Frame[8],Frame[7]);
//							    	robot.red_2_HP=heceng16(Frame[10],Frame[9]);
							    	robot.red_3_HP=heceng16(Frame[12],Frame[11]);
//							    	robot.red_4_HP=heceng16(Frame[14],Frame[13]);
//							    	robot.red_5_HP=heceng16(Frame[16],Frame[15]);
//							    	robot.red_7_HP=heceng16(Frame[18],Frame[17]);
//								    robot.red_outpost_HP=heceng16(Frame[20],Frame[19]);
//								    robot.red_base_HP=heceng16(Frame[22],Frame[21]);
//								    robot.blue_1_HP=heceng16(Frame[24],Frame[23]);
//							    	robot.blue_2_HP=heceng16(Frame[26],Frame[25]);
							    	robot.blue_3_HP=heceng16(Frame[28],Frame[27]);
//							    	robot.blue_4_HP=heceng16(Frame[30],Frame[29]);
//							    	robot.blue_5_HP=heceng16(Frame[32],Frame[31]);
//							    	robot.blue_7_HP=heceng16(Frame[34],Frame[33]);
//								    robot.blue_outpost_HP=heceng16(Frame[36],Frame[35]);
//								    robot.blue_base_HP=heceng16(Frame[38],Frame[37]);
					          break;
									
				      }
							case 0x0004://??????
							{
//							      robot.dart_belong=Frame[7];
//								    robot.stage_remaining_time=heceng16(Frame[9],Frame[8]);
								    break;
							}
              case 0x0101://????
              {
//				            robot.event_type=Frame[7];
					          break;
				      }
              case 0x0102://????
              {
//				            robot.supply_projectile_id=Frame[7];
//	                  robot.supply_robot_id=Frame[8];
//	                  robot.supply_projectile_step=Frame[9];
//	                  robot.supply_projectile_num=Frame[10];
					          break;
	
				      }
              case 0x0103://????
              {
                    //?
								    break;
				      }	
              case 0x0104://????
              {
//				           robot.level=Frame[7];
//					         robot.foul_id=Frame[8];
					         break;
				      }	
							case 0x0105:
							{
//							     robot.dart_remaining_time=Frame[7];
							     break;
							}
 				      case 0x0201://?????
              {
	               
								robot.robot_id=Frame[7];
                 robot.robot_level=Frame[8];
								robot.remain_HP=heceng16(Frame[10],Frame[9]);
                 robot.max_HP=heceng16(Frame[12],Frame[11]);
                 robot.shooter_id1_17mm_cooling_rate=heceng16(Frame[14],Frame[13]);//每秒冷却值
                 robot.shooter_id1_17mm_cooling_limit=heceng16(Frame[16],Frame[15]);//枪口热量剩余//
                 robot.chassis_power_limit =heceng16(Frame[18],Frame[17]);//底盘功率
								robot.mains_power_shooter_output=(Frame[19]&0x04)>>2;     //发射机构电
								robot.mains_power_gimbal_output=Frame[19]&0x01;         //云台电
								robot.mains_power_chassis_output=(Frame[19]&0x02)>>1;     //底盘电
								
				      	 break;
				      }
				      case 0x0202://????
              {
				         robot.classis_voit=heceng16(Frame[8],Frame[7]);//
	               robot.classis_current=heceng16(Frame[10],Frame[9]);
								 ppp.g[0]=Frame[11];
								 ppp.g[1]=Frame[12];
								 ppp.g[2]=Frame[13];
								 ppp.g[3]=Frame[14];
								 robot.classis_power=ppp.powerr;
                 robot.classis_power_buffer=heceng16(Frame[16],Frame[15]);//????
	               robot.shooter_heat0=heceng16(Frame[18],Frame[17]);
	               robot.shooter_heat1=heceng16(Frame[19],Frame[20]);
								 robot.mobile_shooter_heat2=heceng16(Frame[21],Frame[22]);
								 break;
	
				       }	
				      case 0x0203://?????
              {
//								 ppp.g[0]=Frame[7];
//								 ppp.g[1]=Frame[8];
//								 ppp.g[2]=Frame[9];
//								 ppp.g[3]=Frame[10];
//				         robot.x=ppp.powerr;
//					       ppp.g[0]=Frame[11];
//								 ppp.g[1]=Frame[12];
//								 ppp.g[2]=Frame[13];
//								 ppp.g[3]=Frame[14];
//				         robot.y=ppp.powerr;
//                 ppp.g[0]=Frame[15];
//								 ppp.g[1]=Frame[16];
//								 ppp.g[2]=Frame[17];
//								 ppp.g[3]=Frame[18];
//				         robot.z=ppp.powerr;
//				      	   ppp.g[0]=Frame[19];
//								 ppp.g[1]=Frame[20];
//								 ppp.g[2]=Frame[21];
//								 ppp.g[3]=Frame[22];
//				         robot.yaw=ppp.powerr;
								 break;
				      }
				      case 0x0204://?????
              {
//			           robot.power_rune_buff=Frame[7];
								 break;
				      }
			      	 case 0x0205://????
              {
//				         robot.energy_point=heceng16(Frame[8],Frame[7]);
//	               robot.attack_time=Frame[9];
							   break;
				      }
				      case 0x0206://????
              {
//				         robot.armar_id=tiqv(Frame[7],1,3);
//	               robot.hurt_type=Frame[7]>>4;
								 break;
				      }
              case	0x0207://????
				      {
                 robot.bullet_type=Frame[7];
	               robot.bullet_freq=Frame[9];
								 ppp.g[0]=Frame[10];
								 ppp.g[1]=Frame[11];
								 ppp.g[2]=Frame[12];
							   ppp.g[3]=Frame[13];
	               robot.bullet_speed=ppp.powerr;
								send_vision.current_v=robot.bullet_speed;
								//send_vision.bullet_count+=robot.bullet_freq;
								
							   break;
				      }
 				      case 0x0208://????
				      {
//				         robot.bullet_remaining_num=heceng16(Frame[8],Frame[7]);
							   break;
				      }
							case 0x0209://RFID??
				      {
				         robot.rfid_status=Frame[7];       
							   break;
				      }
							case 0x020A://RFID??
				      {
//				         robot.rfid_status=Frame[7];
							   break;
				      }
				      case 0x0301://?????
              {
				         //?
								break;
				      }
				      default:;
							
			     }	
					  JISHU.jindu=0;
					  a_=1;
					  Frame[0]=0;
					 
				 }
				  default://????,????????
			   {
			      JISHU.jindu=0;
					  a_=1;
					  Frame[0]=0;
			      break;
	
			   }
		   }
		 }
     JISHU.dizhi++;//????????
		 JISHU.HC--;   //???????
	 }
}
}	

