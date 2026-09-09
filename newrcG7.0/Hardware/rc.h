#ifndef RC_H
#define RC_H
#define keypad_K 16.5
#define remote_K 12.5
#define limit 8192
#define KB_RAMP_MAX         7100.0f
#define KB_RAMP_UP_STEP     42.9f
#define KB_RAMP_DOWN_STEP   66.3f
#include "main.h"
typedef struct
{
	int16_t R_x;  //����x		��1684 ��364 ��1024  1684-1024 = 1024 - 364 = 660
	int16_t R_y;	 //����y    ��1684 ��364 ��1024
	int16_t L_x;
	int16_t L_y;
	uint8_t sl;			//������  ��1 ��3 ��2
	uint8_t sr;
	
	uint8_t sl_change_flag;			//������  ��1 ��3 ��2
	uint8_t sr_change_flag;
	uint8_t mr_change_flag;
	uint8_t ml_change_flag;
	
	int16_t mouse_x;	//���
	int16_t mouse_y;
	int16_t mouse_z;
	uint8_t mouse_l;	//��갴��
	uint8_t mouse_r;
	uint16_t key;		//����
	u8 W;
	u8 S;
	u8 A;
	u8 D;
	u8 Shift;//head
	u8 Ctrl;//cup
	
	u8 Q;
	u8 L_Q;
	u8 F_Q;
	
	u8 E;
	u8 R;//shoot
	u8 F;//shoot
	u8 G;//shoot
	u8 Z;//feed
	u8 X;//feed
	u8 C;//feed
	u8 V;
	u8 L_V;
	
	u8 B;
	u8 L_B;
	
	uint8_t last_sr;
	uint8_t last_sl;
	uint8_t last_mouse_l;
	uint8_t last_mouse_r;
	uint8_t last_R;
	uint64_t cnts;
	uint16_t trigger;
	int16_t Pulley;
	uint8_t off_line_flag;
	

}RC;

typedef struct
{
	int16_t Set;
	int16_t Last;
	int16_t Err;
	int16_t Out;
}Slow;

extern Slow S_W;

void chassis_out_update(int16_t x, int16_t y, uint8_t w, int8_t accelerate, int8_t spin_mode, int8_t stop, int8_t tunnel, int8_t tunnel_switch, uint8_t shoot_speed_to8bit);
void mecanum_Resolving(void);    //���ֽ���


extern RC rc; 
extern uint8_t spin;
void rc_init(void);
void deal_dbus(u8 *RXBuff);
void rc_reset_keyboard_ramp(void);

#endif
