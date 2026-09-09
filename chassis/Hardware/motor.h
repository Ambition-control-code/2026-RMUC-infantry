#ifndef MOTHR__H
#define MOTHR__H
#include "stm32f4xx.h"                  // Device header
#include "kalman.h"   

typedef struct{
	int32_t raw_value;   					 //����������������ԭʼֵ
	int32_t last_raw_value;					 //��һ�εı�����ԭʼֵ
	int32_t ecd_value;                       //���������������ı�����ֵ
	int32_t diff;							 //���α�����֮��Ĳ�ֵ
	int32_t temp_count;                      //������
	uint8_t buf_count;						 //�˲�����buf��
	int32_t ecd_bias;						 //��ʼ������ֵ	
	int32_t ecd_raw_rate;					 //ͨ������������õ����ٶ�ԭʼֵ
	int32_t rate_buf[6];	     //buf��for filter
	int32_t round_cnt;						 //Ȧ��
	int32_t filter_rate;					 //�ٶ�
	int32_t filter_rate_max;				 //����ٶ�
	float ecd_angle;						 //�Ƕ�
	float init_angle; 						 //��ʼ��һ������
	int16_t current;
	int16_t velo;
	int16_t electricity;
	int16_t re_speed;
	int8_t tempature;	
}dji_motor;
extern dji_motor Yaw;
extern dji_motor BR;
extern dji_motor BL;
extern dji_motor FL;
extern dji_motor FR;
extern dji_motor shoot_2006;
 
typedef struct
{
	float kp;
	float ki;
	float kd;
	float kf;
	
	float pout;
	float iout;
	float dout;
	float fout;
	float poutmax;
	float ioutmax;
	float foutmax;
	float I_Separate_Threshold;
	float doutmax;
	float outmax;
	uint8_t d_first_enable;
	float set;
	float preset;
	float real;
	float pre_real;
	float out;
	float out_last;
	float dead;
	float err;							
	float err_last;					
	float err_llast;				
	float integral;	
	float i_var_speed_a;              // variable-speed integral: ramp width
	float i_var_speed_b;              // variable-speed integral: full-speed window
  float dt;                         // controller period (s); if 0, treated as 1.0
  float lpf_alpha;                  // derivative low pass filter alpha
  first_order_filter_t d_filter;    // derivative low pass filter state
	
}PID_T;

typedef struct {
    float kp;
    float ki;
    float kd;
    float kf;
    float poutmax;
    float ioutmax;
    float doutmax;
    float foutmax;
    float outmax;
    float I_Separate_Threshold;
    float dead;
    float lpf_alpha;
    uint8_t d_first_enable;
    float i_var_speed_a;
    float i_var_speed_b;
    float dt;
} PID_Config_t;

extern PID_T BR_Spe;
extern PID_T BL_Spe;
extern PID_T FL_Spe;
extern PID_T FR_Spe;
extern PID_T follow_Yaw_speed_loop;
extern PID_T follow_Yaw_angle_loop;
extern PID_T spin_spe;
extern PID_T Buffer_power;
extern PID_T pid_shoot_2006;

// ���̿����ֽ������壺����̨�˱���һ��
typedef union {
	struct {
		uint8_t w          : 1;  // Bit 0: �����־
		uint8_t stop       : 1;  // Bit 1: ֹͣ
		uint8_t spin_mode  : 1;  // Bit 2: С����ģʽ
		uint8_t accelerate : 1;  // Bit 3: ����
		uint8_t tunnel     : 1;  // Bit 4: ������ģʽ
		uint8_t reserved   : 2;  // Bit 5-6: Ԥ��
		uint8_t shoot_speed_flag : 1;  // Bit 7: ������
	} bits;
	/* bits.reserved bit 0 maps to CAN Bit 5 (rc.G); bit 1 remains reserved. */
	uint8_t raw;
} chassis_ctrl_t;


typedef struct{
	int16_t VX;
	int16_t VY;
	uint8_t W;       // �����־ (���ں��ֽ�Bit0��ȡ)
	int8_t  Shift;   // ����
	int8_t  spin;    // С����ģʽ
	int8_t  stop;    // ֹͣ
	int8_t  tunnel;  // ������ģʽ
	uint16_t shoot_speed; // Ħ�����ٶ�ӳ�䵽8bit (0-255)
	int8_t tunnel_switch; // CAN Bit 5: rc.G
	uint16_t rx_watchdog_cnt; // control frame timeout counter
	uint16_t yaw_rx_watchdog_cnt; // yaw feedback timeout counter
}Data;
typedef struct{
 int yaw;
 int pitch2;
 int whe1;
 int whe2;
 int whe3;
 int whe4;
 int capcity;
 int shoot_2006;
 int c_board;
 uint8_t Cyaw;
 uint8_t Cpitch2;
 uint8_t Cwhe1;
 uint8_t Cwhe2;
 uint8_t Cwhe3;
 uint8_t Cwhe4;
 uint8_t Ccapcity;
 uint8_t Cshoot_2006;
 uint8_t Cc_board;
}ONLINE;
extern ONLINE Online;
extern volatile dji_motor FEED,SHOOT_L,SHOOT_R;
extern Data Data_C;
//void GMmotor_run (float setPit,float setYaw,float Pit_Groy,float Yaw_Groy, float Pit_ang, float Yaw_ang);
//void STmotor_run (float setPit,float setYaw,float Pit_Groy,float Yaw_Groy, float Pit_ang, float Yaw_ang);
void PID_Init_All(void);
void CAN_RxFifoMsgHandler(CAN_HandleTypeDef *hcan);

void Chassis_run(void);
void Chassis_DeviceWatchdog_Init(void);
void Chassis_DeviceWatchdog_Tick(void);
uint8_t Chassis_DeviceWatchdog_IsLost(void);
void Chassis_ControlData_Online(void);
void send_speed(void);
void Chassis_YawData_Online(void);
float PID_Cal(PID_T *pid, float get, double set);  // getΪʵ��ֵ  setΪĿ��ֵ
void Shoot_2006_SpeedLoop_SendCurrent(float target_speed);
void Shoot_2006_StopCurrent(void);


#endif
