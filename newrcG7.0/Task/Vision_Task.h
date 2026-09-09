#ifndef __VISION_TASK_H
#define __VISION_TASK_H
#include "main.h"
#include "CRC.h"
#pragma pack(push, 1)
typedef struct
{
	uint8_t header[2];
	uint8_t mode;
	float q[4];
	float yaw;
	float gyro_z;
	float pitch;
	float gyro_y;
	float current_v;
	uint16_t bullet_count;
	uint8_t checksum[2];//存放crc计算结果，不参与运算
} send_struct;
typedef struct 
{
  uint8_t header[2]   ;
//  uint8_t reserved : 1;
	uint8_t mode;
  float yaw;
  float v_yaw;
  float a_yaw;
	float pitch;
  float v_pitch;
  float a_pitch;
  uint8_t checksum[2] ;//接收的视觉crc的计算结果
} receive_struct;
#pragma pack(pop)
typedef union {
    receive_struct receive_vision;
    u8 vision_data[29];
} VisionUnion;
//#pragma pack()

void receive_vision_task(uint8_t* Buf, uint32_t *Len);
void vision_task(void const * argument);
void send_vision_task(void);
void send_with_crc16(send_struct *data);
void send_with_crc16(send_struct *data);
void recieve_vision_cdc(uint8_t *buf);
void deal_zhuantou(void);
extern VisionUnion vision_recieve_union;
#endif
