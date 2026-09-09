#include "UmpireDraw.h"
#include "stdlib.h"
#include "string.h"  
#include "main.h" 
#include "stm32f4xx_hal_uart.h"

//extern Chassic_hero HERO_2024;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
//extern bucket_imu imu;
extern u8 recieve_bdp_err,recieve_pitch;
extern CHASSIS_Task chassis_task;

/**
  * @brief          ��С���˷���ʼ��
  * @param[in]      ��С���˷��ṹ��
  * @param[in]      ������
  * @retval         ���ؿ�
  */
void OLS_Init(Ordinary_Least_Squares_t *OLS, uint16_t order)
{
    OLS->Order = order;
    OLS->Count = 0;
    OLS->x = (float *)malloc(sizeof(float) * order);
    OLS->y = (float *)malloc(sizeof(float) * order);
    OLS->k = 0;
    OLS->b = 0;
    memset((void *)OLS->x, 0, sizeof(float) * order);
    memset((void *)OLS->y, 0, sizeof(float) * order);
    memset((void *)OLS->t, 0, sizeof(float) * 4);
}

/**
  * @brief          ��С���˷�ƽ���ź�
  * @param[in]      ��С���˷��ṹ��
  * @param[in]      �ź�����������һ������ʱ����
  * @param[in]      �ź�ֵ
  * @retval         ����ƽ�����
  */
float OLS_Smooth(Ordinary_Least_Squares_t *OLS, float deltax, float y)
{
    static float temp = 0;
    temp = OLS->x[1];
    for (uint16_t i = 0; i < OLS->Order - 1; ++i)
    {
        OLS->x[i] = OLS->x[i + 1] - temp;
        OLS->y[i] = OLS->y[i + 1];
    }
    OLS->x[OLS->Order - 1] = OLS->x[OLS->Order - 2] + deltax;
    OLS->y[OLS->Order - 1] = y;

    if (OLS->Count < OLS->Order)
    {
        OLS->Count++;
    }

    memset((void *)OLS->t, 0, sizeof(float) * 4);
    for (uint16_t i = OLS->Order - OLS->Count; i < OLS->Order; ++i)
    {
        OLS->t[0] += OLS->x[i] * OLS->x[i];
        OLS->t[1] += OLS->x[i];
        OLS->t[2] += OLS->x[i] * OLS->y[i];
        OLS->t[3] += OLS->y[i];
    }

    OLS->k = (OLS->t[2] * OLS->Order - OLS->t[1] * OLS->t[3]) / (OLS->t[0] * OLS->Order - OLS->t[1] * OLS->t[1]);
    OLS->b = (OLS->t[0] * OLS->t[3] - OLS->t[1] * OLS->t[2]) / (OLS->t[0] * OLS->Order - OLS->t[1] * OLS->t[1]);

    OLS->StandardDeviation = 0;
    for (uint16_t i = OLS->Order - OLS->Count; i < OLS->Order; ++i)
    {
        OLS->StandardDeviation += fabsf(OLS->k * OLS->x[i] + OLS->b - OLS->y[i]);
    }
    OLS->StandardDeviation /= OLS->Order;

		OLS->result=OLS->k * OLS->x[OLS->Order - 1] + OLS->b;
		
    return OLS->result;
}

#define   fh    5      //frame_header ֡ͷ���ݳ���
#define   ci    2      //cmd_id       cmdid����
#define   crc16 2      //CRC16        crc16����

//���ֽڴ��ڷ��ͳ���
//touΪ��������ĵ�ַ
//lengthΪ���鳤��
void SendData (uint8_t *tou,int length)
{
HAL_UART_Transmit(&huart6,tou,length,0xFFF);
}

//Ϊ���͵�һ֡��������crcУ��
//������ĵ�5���ֽ�����CRC8У���룬��������ֽ�����CRC16У����
//touΪ��������ĵ�ַ
//lengthΪ���鳤��
void addCRC(uint8_t* tou,int length)
{
	    Append_CRC8_Check_Sum(tou,5);
	    Append_CRC16_Check_Sum(tou,length);
}

//copy���ƺ���
//tou Ҫ���Ƶĵ�ַ
//mubiao  ���Ƶĵ�
//changdu �����ֽ���
void copy(uint8_t* tou,u8* mubiao,int changdu)
{
	int i=0;
  while(changdu>i)
	{
	  tou[i]=mubiao[i];
		i++;
	}
}
//drawOnePicture  ����һ��ͼ�κ���
//ID              ָ��ID
//sendID          ������ID
//receptionID     ������ID
//PICTURE         ͼ������
uint8_t bao=0;  
void drawOnePicture(u16 ID,u16 sendID,u16 receptionID,uint8_t* PICTURE)
{
	  draw1picture one; 
	  uint8_t fachu[30];//���巢�ͱ���
	  one.SOF=0xA5;                         //����֡ͷ0XA5
	  one.data_length=0x0015;                //��������֡����
	  one.seq=bao++;                         //����
	  one.CRC8=0;                            //CRC8֡ͷУ��
	  one.cmd_id= RtoR;                      //ͨ��ָ��������������
	  one.ID = ID;                           //ָ��ID
	  one.sendID=sendID;                     //������ID
	  one.receptionID=receptionID;           //������ID
	 
    one.CRC16=0;                           //CRC16У��

	
	   fachu[0]=0xA5;
	   copy(&fachu[1],(u8*)&one.data_length,2);
	   fachu[3]=one.seq;
	   copy(&fachu[fh],(u8*)&one.cmd_id,2);
	   copy(&fachu[fh+2],(u8*)&one.ID,2);
	   copy(&fachu[fh+4],(u8*)&one.sendID,2);
	   copy(&fachu[fh+6],(u8*)&one.receptionID,2);
     copy(&fachu[fh+8],(u8*)PICTURE+1,15);
		 
       addCRC(fachu,30);             //����У����
	   SendData(fachu,30);           //�������
}
//drawOnePicture  �������ͼ�κ���
//ID              ָ��ID
//sendID          ������ID
//receptionID     ������ID
//PICTURE         ͼ������
void drawFivePicture(u16 ID,u16 sendID,u16 receptionID,u8	*PICTURE)
{
     u8 fachu[90];  
	   draw1picture one; 
	   one.SOF=0xA5;                          //����֡ͷ0XA5
	                            //���巢�ͱ���
	   one.data_length=0x0024;                //��������֡����
	   one.seq=bao++;                         //����
	   one.CRC8=0;                            //CRC8֡ͷУ��
	   one.cmd_id= RtoR;                      //ͨ��ָ��������������
	   one.ID = ID;                           //ָ��ID
	   one.sendID=sendID;                     //������ID
	   one.receptionID=receptionID;           //������ID
       one.CRC16=0;                           //CRC16У��

	   fachu[0]=0xA5;
	   copy(&fachu[1],(u8*)&one.data_length,2);
	   fachu[3]=one.seq;
	   copy(&fachu[fh],(u8*)&one.cmd_id,2);
	   copy(&fachu[fh+2],(u8*)&one.ID,2);
	   copy(&fachu[fh+4],(u8*)&one.sendID,2);
	   copy(&fachu[fh+6],(u8*)&one.receptionID,2);
     copy(&fachu[fh+8] ,PICTURE+1,15);
		 copy(&fachu[fh+23],PICTURE+1+16,15);
		 //copy(&fachu[fh+38],PICTURE+1+32,15);
		 //copy(&fachu[fh+53],PICTURE+1+48,15);
		 //copy(&fachu[fh+68],PICTURE+1+64,15);
     addCRC((unsigned char*)fachu,45);             //����У����
	   SendData((unsigned char*)fachu,45);           //�������
}

u16 R;
u16 CR;
u16 id;
u32 text;
picture LINE1[5];      //����һ��ͼ��Ϊֱ��
picture zhengfang;     //����һ������Ϊzhengfang
picture yuanhu;        //����һ��Բ��
word V1;               //����һ������Ϊ��ѹv
word V2;                //����һ������Ϊ����d
int hudu;              //Բ����ʾ�Ƕ�
struct 
{
	u8 fool:1;
}luoji;
int zhunxingColor =green;
int zhunxingZuangTai=add;
void pictureInit()
{  
		 static u32 i;
	 u8 a;
	  i++;
	  if((i%2)==0) {a=1;} else {a=2;}
	  luoji.fool++;
	  LINE1[1].name=            288;             //ֱ����
	  LINE1[1].width=           30;             //�߿� 
	  LINE1[1].start_x=     ScreenX/2-450+0.3*500;;        //ֱ�����x����

	  LINE1[1].start_y=         ScreenY/2-400;            //ֱ�����y����
	  LINE1[1].end_x  =         ScreenX/2-450+(chassis_task.D_R.CAP_U_TX_DATA-1300)/1766*500;        //ֱ���յ�x����
	  LINE1[1].end_y  =         ScreenY/2-400;            //ֱ���յ�y����
    LINE1[1].color=           5;         //ֱ����ɫ
    LINE1[1].operate_tpye=    a;              //ͼ�β�������ͼ��
    LINE1[1].graphic_tpye=    line;           //ͼ������ֱ��
    LINE1[1].layer=           1;              //ͼ����1
		//////////////////////////׼��///////////////////////////////////
		LINE1[0].name=            5700;             //ֱ����          
	  LINE1[0].width=           3;             //�߿� 
    LINE1[0].start_x=         ScreenX/2-1;        //ֱ�����x����
	  LINE1[0].start_y=         ScreenY/2-36;            //ֱ�����y����
	  LINE1[0].end_x  =         ScreenX/2+9;        //ֱ���յ�x����
	  LINE1[0].end_y  =         ScreenY/2-36;            //ֱ���յ�y����
    LINE1[0].color=           zhunxingColor;         //ֱ����ɫ
    LINE1[0].operate_tpye=    a;            //ͼ�β�������ͼ��
    LINE1[0].graphic_tpye=    line;           //ͼ������ֱ��
    LINE1[0].layer=           0;              //ͼ����1
//	if(Data_C.spin==1)
//	{
	    V1.name=9999;
		V1.start_angle =70;              //�����С
		V1.end_angle   = 3;              //С����Чλ��
		V1.width       = 9;              //�߿�
		V1.start_x     = ScreenX/2+600;
		V1.start_y     = ScreenY/2;
    V1.W.zhengshu  = (int)(Data_C.W);
    //V1.W.zhengshu  = (int)0;

		V1.color       =  5;
		if(Data_C.spin==1){V1.color       =  1;}
		V1.operate_tpye = a;
	V1.graphic_tpye = ZS;
		V1.layer        = 0;
	}
	
//	 else{ V1.name=9999;
//		V1.start_angle =70;              //�����С
//		V1.end_angle   = 3;              //С����Чλ��
//		V1.width       = 9;              //�߿�
//		V1.start_x     = ScreenX/2+600;
//		V1.start_y     = ScreenY/2;
//    V1.W.zhengshu  = (int)10*(recieve_pitch);
//    V1.W.zhengshu  = (int)1;

//		V1.color   
//		 = 8 ;
//		V1.operate_tpye = a;
//		V1.graphic_tpye = ZS;
//		V1.layer        = 0;
//	 }


extern int ui_count;
extern Robot_Para robot;              //�����˲����ṹ��

void drawPictureDemo(void)
{
	  static int i;
	  pictureInit(); 
//id= HERO_2024.hero_umpire.id;
id= robot.robot_id;
	
    if(id<0X100)
		{
		   R=id;
		   CR=id+0x100;
		}
		else if(id>=0x100)
		{
		   R=id;
			 CR=R-100+0X164;
		}
		
		if(i>3){i=0;}
		else {i++;}


  //		drawPicture(0x101,103,0x167,(8*)&yuanhu);          //��ͼ����
			switch(i)
			{
				case 1:drawFivePicture(draw5P,R,CR,(u8*)&LINE1);break;
				case 2:drawOnePicture(draw1P,R,CR,(u8*)&V1);break;
			}
//		}         //��ͼ����
	    drawOnePicture(draw1P,R,CR,(u8*)&V1);        //��ͼ����
		  //drawOnePicture(draw1P,R,CR,(u8*)&yuanhu);      //��ͼ����
  		//drawOnePicture(draw1P,R,CR,(u8*)&zhengfang);   //��ͼ����
    
    
    
}

void hero_ui()
{


}

