#ifndef GIMBAL_TASK_H
#define GIMBAL_TASK_H
#define Stop ((rc.B || rc.sl == 2) ? 0.0f : 1.0f)
#define GIMBAL_YAW_REINIT_CNT    200U
#define GIMBAL_PITCH2_REINIT_CNT 800U
#include "main.h"
#include "Vision_Task.h"
typedef enum{
	MEIMIAODAO=0,
    MIAODAO,
	MIAODAOKAIHUO
}Status_zimiao;
typedef enum{
    ZIMIAO=0,
	XIAOFU,
	DAFU
}Status_c;
typedef enum {
	normal = 0,
	goudong,
} Gesture;
typedef struct 
{
	float ZM_yaw_counts;
} Zhuantou;
typedef struct {
	float yaw_target;
	float pitch_target;
	float pitch2_target;
} GIMBAL;
typedef struct {
	float yaw_zimiao;
	float pitch_zimiao;
    Zhuantou zhuantou;
	Status_zimiao status_zimiaoRX;
	Status_c status_C;
} Zimiao;
typedef struct {
	float shoot_speed;
	float feed_speed;
	uint8_t fire;
	float shoot_count;
} SHOOT;
typedef struct {
	uint16_t PITCH;
	uint16_t PITCH2;
	uint16_t YAW;
	uint16_t SHOOT_L;
	uint16_t SHOOT_R;
	uint16_t PITCH_flag;
	uint16_t PITCH2_flag;
	uint16_t YAW_flag;
	uint16_t SHOOT_L_flag;
	uint16_t SHOOT_R_flag;
} DJ_HANDLE;
typedef struct {
	GIMBAL gimbal;
	Zimiao zimiao;
	SHOOT  shoot;
	Gesture gesture_task;
    DJ_HANDLE DJ_handler;
} ALL_control;
void justice_gesture(ALL_control* all_control);
void gimbal_init(void);

extern ALL_control All_control;
void into_zimiao(void);
void handler();
#endif
