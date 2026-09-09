#include "main.h"
#include "Gimbal_task.h"
#include "usbd_cdc_if.h"
extern float INS_quat[4];
extern IMU_angle imu_angle;
send_struct send_vision;
VisionUnion vision_recieve_union;
static volatile uint8_t g_vision_frame_valid = 0;


void Vision_Control(void const * argument)
{
  for(;;)	
	{
		send_vision_task();
		if (g_vision_frame_valid)
		{
			Status_zimiao vision_status = (Status_zimiao)vision_recieve_union.receive_vision.mode;

			All_control.zimiao.status_zimiaoRX = vision_status;
			// Hold the last valid target when vision reports "not locked" (upstream may output 0).
			if (vision_status == MIAODAO || vision_status == MIAODAOKAIHUO)
			{
				All_control.zimiao.pitch_zimiao = vision_recieve_union.receive_vision.pitch * 57.3f;
				All_control.zimiao.yaw_zimiao = vision_recieve_union.receive_vision.yaw * 57.3f;
			}
			else

			{
				All_control.zimiao.yaw_zimiao = imu_angle.yaw_real;
			}
		}
        osDelay(1);
  }
}
u8 vsi;
void recieve_vision_cdc(u8 *buf)
{   vsi++;
	memcpy(&vision_recieve_union.vision_data,buf,29);
	g_vision_frame_valid =
		(vision_recieve_union.vision_data[0] == 0x0d) &&
		(vision_recieve_union.vision_data[1] == 0x0d) &&
		(vision_recieve_union.vision_data[27] == 0xff) &&
		(vision_recieve_union.vision_data[28] == 0xff);
}
void send_vision_task()
{
	send_vision.header[0]=0x0d;
	send_vision.header[1]=0x0d;
	//send_vision.mode=All_control.zimiao.status_C;
	send_vision.mode=0x01;
	send_vision.bullet_count=All_control.shoot.shoot_count;
	send_vision.q[0]=INS_quat[0];
	send_vision.q[1]=INS_quat[1];
	send_vision.q[2]=INS_quat[2];
	send_vision.q[3]=INS_quat[3];
	send_vision.pitch=imu_angle.pitch/57.3f;
	send_vision.yaw=imu_angle.yaw/57.3f;
	send_vision.gyro_y=imu_angle.gyro_y;
	send_vision.gyro_z=imu_angle.gyro_z;
	send_vision.current_v=speed_17mm;
	send_vision.checksum[0]=0xff;
	send_vision.checksum[1]=0xff;
	u8 buf[43];
	memcpy(&buf,&send_vision,43);
	CDC_Transmit_FS(buf,43);
}

