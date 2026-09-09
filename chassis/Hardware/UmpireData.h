#ifndef _UMPIREDATA_H_
#define _UMPIREDATA_H_

#include  "main.h"
//#include  "CRC8_16.h"


typedef  struct
{ //0001
	u8   game_type:4;       //��������
	u8   game_progress:4;   //��ǰ�����׶�
	u16  remain_time;       //��ǰ�׶�ʣ��ʱ��     *
	//0002
	u8   winner:4;          //�������
	//0003
	u16  red_1_HP;          //��1Ӣ��Ѫ��
	u16  red_2_HP;          //��2����Ѫ��
	u16  red_3_HP;          //��3����Ѫ��
	u16  red_4_HP;          //��4����Ѫ��
	u16  red_5_HP;          //��5����Ѫ��
	u16  red_7_HP;          //��7�ڱ�Ѫ��
	u16  red_outpost_HP;    //�췽ǰ��վѪ��
	u16  red_base_HP;       //�췽����Ѫ��
	u16  blue_1_HP;         //��1Ӣ��Ѫ��
	u16  blue_2_HP;         //��2����Ѫ��
  u16  blue_3_HP; 	      //��3����Ѫ��
	u16  blue_4_HP;         //��������
	u16  blue_5_HP;         //��������
	u16  blue_7_HP;         //��������
	u16  blue_outpost_HP;   //����ǰ��վѪ��
	u16  blue_base_HP;
	//0004
	u8   dart_belong;             //�����������
	u16  stage_remaining_time;    //����ʣ��ʱ��
	//0101
	u32  event_type;              //�����¼�����
	//0102
	u8   supply_projectile_id;   //����վ��ID
	u8   supply_robot_id;        //����������
	u8   supply_projectile_step; //������״̬
	u8   supply_projectile_num;  //��������
	//0103

	//0104
	u8   level;                  //����ȼ�
	u8   foul_id;                //���������ID
	//0105
	u8 dart_remaining_time;       //���ڷ���ڵ���ʱ
	//0201
	u8 robot_id;
	u8 robot_level;
	u16 remain_HP;
	u16 max_HP;
	u16 v;
	u16 shooter_id1_17mm_cooling_limit;
	u16 shooter_id1_17mm_speed_limit;
  u16 shooter_id2_17mm_cooling_rate;
	u16 shooter_id2_17mm_cooling_limit;
	u16 shooter_id2_17mm_speed_limit;
  u16 shooter_id1_42mm_cooling_rate;
	u16 shooter_id1_42mm_cooling_limit;
	u16 shooter_id1_42mm_speed_limit;
  u16 chassis_power_limit;
  u8 mains_power_gimbal_output : 1;
  u8 mains_power_chassis_output : 1;
  u8 mains_power_shooter_output : 1;
	//0202
	u16  classis_voit;                            //���������ѹmv
	u16  classis_current;                         //���������ѹma
float  classis_power;                           //�����������w
	u16  classis_power_buffer;                    //���̹��ʻ���J     **
	u16  shooter_heat0;                           //17mmǹ������      **
	u16  shooter_heat1;                           //42mmǹ������
	u16  mobile_shooter_heat2;                    //����17mmǹ������
	//0x0203
  float x;                                      //λ��x m
  float y;
  float z;
	float yaw;                                    //λ��ǹ�� ��
	//0x0204
	u8  power_rune_buff;                           //����������        *
	//0x0205
	u16  energy_point;                              //���л�����������
	u8   attack_time;                               //�ɹ���ʱ��
	//0x0206
	u8 armar_id :4;                                 //����װ��id       *
	u8 hurt_type:4;                                 //�˺�����         *
	//0x0207
	u8 bullet_type;                                 //�ӵ�����
	u8 bullet_freq;                                 //�ӵ���Ƶ         *
	float bullet_speed;                             //�ӵ�����         *
	//0x0208
	u16 bullet_remaining_num;                       //����ʣ��         *
	//0x0209
	u32 rfid_status;                                //RFID״̬
	uint8_t fortress;                               //bit24 前哨站状态
	//0x020A
	u8  dart_launch_opening_status;                 //���ڿڷ���״̬
	u8  dart_attack_target;                         //���ڴ��Ŀ��
	u16	target_change_time;                         //�л�Ŀ��ʱʣ��ʱ��
	u8  first_dart_speed;                           //����һö�����ٶ�
	u8  second_dart_speed;                          //���ڶ�ö�����ٶ�
	u8  third_dart_speed;                           //3
	u8  fourth_dart_speed;                          //4
	u16 last_dart_launch_time;                      //���һ�η������ʣ��ʱ��
	u16 operate_launch_cmd_time;                    //���һ�β�����ȷ������ʱ����ʱ��
	//0x0301

}Robot_Para;//��Ż����˲����ṹ��

void Umpire_UsartInit(void);

void DMA_USART1_Send(u8 *data, u16 size);
void deal_data (void);

extern Robot_Para robot;
extern volatile uint16_t g_umpire_watchdog_cnt;

#endif

