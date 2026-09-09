#ifndef MOTHR__H
#define MOTHR__H
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"
#include "kalman.h"
#define PITCH2_CTRL_DIV 10U  // Gimbal loop 1kHz -> Pitch2 CAN 50Hz

typedef struct{
	int32_t raw_value;   					 //����������������ԭʼֵ
	int32_t last_raw_value;					 //��һ�εı�����ԭʼֵ
	int32_t ecd_value;                 //���������������ı�����ֵ
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
	int16_t electricity;
	int16_t re_speed;
	int8_t tempature;	
//	float real_angle;
//	float zero_angle;
//	float relative_zero_angle;
//	float ALL_angle;
//	int tool_flag;
}dji_motor;


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

typedef struct
{
	float kp;
	float ki;
	float kd;
	float kf;
	uint8_t d_first_enable;
	float dead;
	float I_Separate_Threshold;
	float i_var_speed_a;
	float i_var_speed_b;
	float dt;
	float poutmax;
	float ioutmax;
	float doutmax;
	float foutmax;
	float outmax;
	float lpf_alpha;
} PID_Config_t;
typedef struct {
    float C;		   // ��Ĥ��ϵ��
    float K;		   // ��Ĥ����	Խ����ӦԽ��
    float ref;         // Ŀ��ֵ
    float dead;   	   // ����
    float u_max;       // ������ֵ
    float J;           // ���ƹ���
    float epsilon;	   // ��Ĥ������	���Ƴ�����������Ӧ

    float angle;       // ��ǰ�Ƕȷ���
    float ang_vel;     // ��ǰ���ٶȷ���
    float u;           // �������

    // �ڲ�����
    float error;
    float error_last;
    float dref;        // Ŀ��ֵһ�׵�
    float ddref;       // Ŀ��ֵ���׵�
    float refl;        // ��һ��Ŀ��ֵ
    float s;           // ��ģ��
} SMC_HandleTypeDef;
extern PID_T GMPitchPid_Ang;
extern PID_T motor2006;
extern PID_T GMPitchPid_Spe;
extern PID_T GMYawPid_Ang;
extern PID_T GMYawPid_Spe;

extern PID_T GMYawPid_Ang_ZM;
extern PID_T GMYawPid_Spe_ZM;
extern PID_T STLpid_Spe;
extern PID_T STRpid_Spe;
extern PID_T Feedpid_Spe;
extern PID_T PITCH2_Ang;
extern PID_T PITCH2_Spe;
extern float PID_Cal(PID_T *pid, float get, double set);
void PID_InitFromConfig(PID_T *pid, const PID_Config_t *config);
extern volatile dji_motor FEED,SHOOT_L,SHOOT_R;
extern float GMPitchTorqueFeedforward;
extern float speed_17mm;
extern PID_T GMPitchPid_Ang, GMPitchPid_Spe, GMYawPid_Ang, GMYawPid_Spe, GMYawPid_Ang_ZM, GMYawPid_Spe_ZM, STLpid_Spe, STRpid_Spe, Feedpid_Spe, Dm4310_Ang, Dm4310_Spe;

/*============ ����Ħ��ǰ���ṹ�� ============*/
typedef struct {
    float Fc;           // ����Ħ�����س���
    float tanh_coeff;   // tanhϵ����������㸽��ƽ�����ɵĶ��ͳ̶�
    float lpf_alpha;    // �ٶ��˲�ϵ��
    first_order_filter_t vel_filter;  // �ٶ��˲���
    float vel_filtered; // �˲����ٶȣ������ⲿ���ʣ�
    float output;       // ǰ�����ֵ�����ڵ��Թ۲죩
} CoulombFrictionFF_t;

void CoulombFrictionFF_Init(CoulombFrictionFF_t *ff, float Fc, float tanh_coeff, float lpf_alpha);
float CoulombFrictionFF_Calc(CoulombFrictionFF_t *ff, float velocity);

typedef struct {
    float vel_scale;                   // vision velocity -> speed loop setpoint
    float acc_scale;                   // vision acceleration -> torque feedforward
    float friction_scale;              // friction model gain, 0 disables it
    CoulombFrictionFF_t *friction_ff;   // optional friction feedforward model
} AxisFeedforwardConfig_t;

typedef struct {
    float speed_set;
    float speed_ff;
    float torque_ff;
    float friction_ff;
    float torque_total_ff;
} AxisFeedforwardOutput_t;

AxisFeedforwardOutput_t AxisFeedforward_Calc(const AxisFeedforwardConfig_t *config,
                                             uint8_t enable,
                                             float base_speed_set,
                                             float vision_velocity,
                                             float vision_acceleration,
                                             float friction_velocity);

void CAN_RxFifoMsgHandler(CAN_HandleTypeDef *hcan);
void PID_Init_All(void);
void PID_Init_GimbalYaw(void);
void PID_Init_GimbalPitch2(void);
void Motor_Control_Run(float setPit, float setYaw, float L_setST, float R_setST,float DMPit);
#endif
