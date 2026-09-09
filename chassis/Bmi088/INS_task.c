#include "main.h"
#include "INS_task.h"

u8 offset_OK_flag=1;//У׼ʱ0��main.c�аѶ�ȡ���ˣ�У׼���1��ע����read
extern uint16_t Man,WCIS;
float OFFSET_Buffer[3];
bmi088_real_data_t bmi088_real_data;
IMU_angle imu_angle = {0};
 fp32 Gyro_Scale_Factor[3][3] = {IMU_BOARD_INSTALL_SPIN_MATRIX}; //������У׼���Զ�
 fp32 gyro_cali_offset[3] ={0.0100966273f, 0.0f, 0.0f};
 fp32 gyro_cali_debug_result[3] ={-0.0f, 0.0f, 0.0f};
 fp32 Gyro_Offset[3] = {0.0f, 0.0f, 0.0f};            //��������Ư
 fp32 Accel_Scale_Factor[3][3] = {IMU_BOARD_INSTALL_SPIN_MATRIX}; //���ٶ�У׼���Զ�
 const float TimingTime = INS_DELTA_TICK * 0.001f;   //�������е�ʱ�� ��λ s
 fp32 INS_quat[4] = {0.0f, 0.0f, 0.0f, 0.0f}; //��Ԫ��
 fp32 INS_gyro[3] = {0.0f, 0.0f, 0.0f};
 fp32 INS_accel[3] = {0.0f, 0.0f, 0.0f};
 fp32 INS_mag[3] = {0.0f, 0.0f, 0.0f};
//static fp32 INS_tempature = 0.0f;
fp32 INS_Angle[3] = {0.0f, 0.0f, 0.0f};      //ŷ���� ��λ rad

static void IMU_Cali_Slove(fp32 gyro[3], fp32 accel[3], fp32 mag[3], bmi088_real_data_t *bmi088)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        gyro[i] = bmi088->gyro[0] * Gyro_Scale_Factor[i][0] + bmi088->gyro[1] * Gyro_Scale_Factor[i][1] + bmi088->gyro[2] * Gyro_Scale_Factor[i][2] + Gyro_Offset[i];
        accel[i] = bmi088->accel[0] * Accel_Scale_Factor[i][0] + bmi088->accel[1] * Accel_Scale_Factor[i][1] + bmi088->accel[2] * Accel_Scale_Factor[i][2];
    }
}


void man(void)
{
		BMI088_read_(&bmi088_real_data);
  //��ȥ��Ư�Լ���ת����ϵ
  IMU_Cali_Slove(INS_gyro, INS_accel, INS_mag, &bmi088_real_data);
}

void INS_task()
{
	BMI088_read_(&bmi088_real_data);
  //��ȥ��Ư�Լ���ת����ϵ
  IMU_Cali_Slove(INS_gyro, INS_accel, INS_mag, &bmi088_real_data);
	
		//���ٶȼƵ�ͨ�˲�
	static fp32 accel_fliter_1[3] = {0.0f, 0.0f, 0.0f};
	static fp32 accel_fliter_2[3] = {0.0f, 0.0f, 0.0f};
	static fp32 accel_fliter_3[3] = {0.0f, 0.0f, 0.0f};
	static const fp32 fliter_num[3] = {1.929454039488895f, -0.93178349823448126f, 0.002329458745586203f};
	
		//�ж��Ƿ��һ�ν��룬�����һ�����ʼ����Ԫ����֮�������Ԫ������Ƕȵ�λrad
	static uint8_t updata_count = 0;
	if (updata_count == 0)
	{
			//��ʼ����Ԫ��		
			AHRS_init(INS_quat, INS_accel, INS_mag);
			get_angle(INS_quat, INS_Angle, INS_Angle + 1, INS_Angle + 2);

			accel_fliter_1[0] = accel_fliter_2[0] = accel_fliter_3[0] = INS_accel[0];
			accel_fliter_1[1] = accel_fliter_2[1] = accel_fliter_3[1] = INS_accel[1];
			accel_fliter_1[2] = accel_fliter_2[2] = accel_fliter_3[2] = INS_accel[2];
			updata_count++;
	}
	else
	{
			//���ٶȼƵ�ͨ�˲�
			accel_fliter_1[0] = accel_fliter_2[0];
			accel_fliter_2[0] = accel_fliter_3[0];

			accel_fliter_3[0] = accel_fliter_2[0] * fliter_num[0] + accel_fliter_1[0] * fliter_num[1] + INS_accel[0] * fliter_num[2];

			accel_fliter_1[1] = accel_fliter_2[1];
			accel_fliter_2[1] = accel_fliter_3[1];

			accel_fliter_3[1] = accel_fliter_2[1] * fliter_num[0] + accel_fliter_1[1] * fliter_num[1] + INS_accel[1] * fliter_num[2];

			accel_fliter_1[2] = accel_fliter_2[2];
			accel_fliter_2[2] = accel_fliter_3[2];

			accel_fliter_3[2] = accel_fliter_2[2] * fliter_num[0] + accel_fliter_1[2] * fliter_num[1] + INS_accel[2] * fliter_num[2];

		
			//������Ԫ��
			AHRS_update(INS_quat, TimingTime, INS_gyro, accel_fliter_3, INS_mag);
			get_angle(INS_quat, INS_Angle, INS_Angle + 1, INS_Angle + 2);
			
			imu_angle.last_yaw = imu_angle.yaw;
			imu_angle.yaw = INS_Angle[0] * 57.3f;//yaw
			imu_angle.pitch = INS_Angle[1] * 57.3f;//pitch
			imu_angle.roll = INS_Angle[2]* 57.3f;//roll
			imu_angle.yaw_diff = imu_angle.yaw - imu_angle.last_yaw;
			if(imu_angle.yaw_diff > 150.0f)
				imu_angle.yaw_counts --;
			else if(imu_angle.yaw_diff < -150.0f)
				imu_angle.yaw_counts ++;
			imu_angle.yaw_real = imu_angle.yaw + imu_angle.yaw_counts * 360.0f;
			imu_angle.gyro_z = INS_gyro[2] * 57.3f;//yaw
			imu_angle.gyro_y = INS_gyro[1] * 57.3f;//pitch
			imu_angle.gyro_x = INS_gyro[0] * 57.3f;//roll
			//�����ǿ���У׼
			if(offset_OK_flag == 1)
			{
					static uint16_t start_gyro_cali_time = 0;
					if(start_gyro_cali_time == 0)
					{		
							Gyro_Offset[0] = gyro_cali_offset[0];
							Gyro_Offset[1] = gyro_cali_offset[1];
							Gyro_Offset[2] = gyro_cali_offset[2];
							start_gyro_cali_time++;
							offset_OK_flag = 2;
					}

			}//�����ǿ���У׼   code end

	}          
}

uint16_t count_time = 0;
int8_t cali_gyro_hook(void)
{
	imu_cali_t local_cali_t;
	INS_cali_gyro(local_cali_t.scale, local_cali_t.offset, &count_time);
	if (count_time > GYRO_CALIBRATE_TIME)
	{
		count_time = GYRO_CALIBRATE_TIME+1;
		OFFSET_Buffer[0] = local_cali_t.offset[0];
		OFFSET_Buffer[1] = local_cali_t.offset[1];
		OFFSET_Buffer[2] = local_cali_t.offset[2];
		gyro_cali_debug_result[0] = local_cali_t.offset[0];
		gyro_cali_debug_result[1] = local_cali_t.offset[1];
		gyro_cali_debug_result[2] = local_cali_t.offset[2];
		gyro_cali_offset[0] = local_cali_t.offset[0];
		gyro_cali_offset[1] = local_cali_t.offset[1];
		gyro_cali_offset[2] = local_cali_t.offset[2];
		
		if(WCIS==0)
		{
		return 1;
		}
		else
		{
			WCIS=0;
			count_time=0;
		OFFSET_Buffer[0]=0.0f;
		OFFSET_Buffer[1]=0.0f;
		OFFSET_Buffer[2]=0.0f;
			return 0;
		}
	}
	else
	{
//		led_red_on();
		return 0;
	}
	
}
/**
  * @brief          У׼������
  * @author         RM
  * @param[in]      �����ǵı������ӣ�1.0fΪĬ��ֵ�����޸�
  * @param[in]      �����ǵ���Ư���ɼ������ǵľ�ֹ�������Ϊoffset
  * @param[in]      �����ǵ�ʱ�̣�ÿ����gyro_offset���û��1,
  * @retval         ���ؿ�
  */

void INS_cali_gyro(fp32 cali_scale[3], fp32 cali_offset[3], uint16_t *time_count)
{
	if( *time_count == 0)
	{
			Gyro_Offset[0] = gyro_cali_offset[0];
			Gyro_Offset[1] = gyro_cali_offset[1];
			Gyro_Offset[2] = gyro_cali_offset[2];
	}
	gyro_offset(Gyro_Offset, INS_gyro, 0, time_count);

	cali_offset[0] = Gyro_Offset[0];
	cali_offset[1] = Gyro_Offset[1];
	cali_offset[2] = Gyro_Offset[2];
	

}
void gyro_offset(float gyro_offset[3], float gyro[3], uint8_t imu_status, uint16_t *offset_time_count)
{

		gyro_offset[0] = gyro_offset[0] - GYRO_OFFSET_KP * gyro[0];
		gyro_offset[1] = gyro_offset[1] - GYRO_OFFSET_KP * gyro[1];
		gyro_offset[2] = gyro_offset[2] - GYRO_OFFSET_KP * gyro[2];
//		gyro_offset[0] = gyro_offset[0] - gyro[0]/GYRO_CALIBRATE_TIME;
//		gyro_offset[1] = gyro_offset[1] - gyro[1]/GYRO_CALIBRATE_TIME;
//		gyro_offset[2] = gyro_offset[2] - gyro[2]/GYRO_CALIBRATE_TIME;
		(*offset_time_count)++;
}


/**
  * @brief          У׼���������ã�����flash���������ط�����У׼ֵ
  * @author         RM
  * @param[in]      �����ǵı������ӣ�1.0fΪĬ��ֵ�����޸�
  * @param[in]      �����ǵ���Ư
  * @retval         ���ؿ�
  */

int16_t ins_set_counts = 0;
void INS_set_cali_gyro(fp32 cali_scale[3], fp32 cali_offset[3])
{
	  ins_set_counts++;
    gyro_cali_offset[0]  = cali_offset[0];
    gyro_cali_offset[1]  = cali_offset[1];
    gyro_cali_offset[2]  = cali_offset[2];
		for(int i = 0; i < 3; i++)
		{
			if(!(fabs(gyro_cali_offset[i]) <= 999.0f))
				gyro_cali_offset[0] = gyro_cali_offset[1] = gyro_cali_offset[2] = 0.0f;
		}

}
