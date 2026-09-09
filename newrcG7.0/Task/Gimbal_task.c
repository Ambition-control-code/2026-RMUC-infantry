#include "Gimbal_task.h"
#include "kalman.h"

extern VT13_USE_t  VT13_USE;
extern uint16_t    new_live_3;

#define PITCH_ZERO_LPF_ALPHA 0.01f
#define PITCH2_TARGET_MIN    2.0f
#define PITCH2_TARGET_MAX    69.9f
#define DJ_HANDLER_TIMEOUT_CNT   2000U
#define PITCH2_STARTUP_TARGET    65.0f

#define PITCH2_STALL_VEL_THRESHOLD   0.5f
#define PITCH2_STALL_TOR_THRESHOLD   7.2f
#define PITCH2_STALL_DURATION_MS     400U
#define PITCH2_STALL_LOCKOUT_MS      3000U

#define RC_OFFLINE_MS        500U
#define VT13_OFFLINE_MS      500U
#define SWITCH_CONFIRM_MS    100U

typedef enum {
    CTRL_SRC_RC = 0,
    CTRL_SRC_VT13,
    CTRL_SRC_NONE
} CtrlSource_T;

static void pitch2_stall_protect(void);
static void gimbal_yaw_reinit(void);
static void gimbal_pitch2_reinit(void);

static first_order_filter_t g_pitch_zero_filter = {0};
static Gesture g_last_gesture = normal;

// ���鿨�����˲���

static CtrlSource_T g_ctrl_src           = CTRL_SRC_RC;
static uint16_t     g_switch_confirm_cnt = 0;
static uint8_t      g_poweroff_latched   = 0;
static uint8_t      g_vt13_first_frame   = 1;

static uint16_t g_pitch2_stall_cnt     = 0;
static uint16_t g_pitch2_stall_lockout = 0;
static uint8_t  g_pitch2_stall_active  = 0;

ALL_control All_control = {
	.DJ_handler = {
		.PITCH = DJ_HANDLER_TIMEOUT_CNT,
		.PITCH_flag = 1,
	},
};
SMC_control YAW_SMC;

static float wrap_deg_180(float angle_deg)
{
	while (angle_deg > 180.0f)
	{
		angle_deg -= 360.0f;
	}
	while (angle_deg < -180.0f)
	{
		angle_deg += 360.0f;
	}
	return angle_deg;
}

static float pitch2_clamp(float value)
{
	if (value > PITCH2_TARGET_MAX)
	{
		return PITCH2_TARGET_MAX;
	}
	if (value < PITCH2_TARGET_MIN)
	{
		return PITCH2_TARGET_MIN;
	}
	return value;
}

static uint8_t pitch_motor_is_enabled(void)
{
	return (uint8_t)((All_control.DJ_handler.PITCH_flag == 0U) &&
	                 (All_control.DJ_handler.PITCH < DJ_HANDLER_TIMEOUT_CNT));
}

static void apply_pitch_startup_protection(ALL_control *all_control)
{
	if (pitch_motor_is_enabled())
	{
		return;
	}
	all_control->gimbal.pitch2_target=motor[MotorPitch2].para.sum_angle>PITCH2_STARTUP_TARGET?PITCH2_STARTUP_TARGET:motor[MotorPitch2].para.sum_angle;
	all_control->gimbal.pitch_target = first_order_filter(
		&g_pitch_zero_filter,
		0.0f,
		PITCH_ZERO_LPF_ALPHA
	);
}
extern float debus_jishi;
void Gimbal_Control(void const *argument)
{

	gimbal_init();// Yaw index0, Pitch2 index1
	for (;;)
	{
		handler();


		uint8_t zimiao_enable = (rc.sl==1||rc.mouse_r == 1);
		uint8_t zimiao_locked = zimiao_enable && 
		                        (All_control.zimiao.status_zimiaoRX == MIAODAO || 
		                         All_control.zimiao.status_zimiaoRX == MIAODAOKAIHUO);


		if (zimiao_locked)
		{
			into_zimiao();
			//All_control.shoot.fire = (All_control.zimiao.status_zimiaoRX == MIAODAOKAIHUO) ? 0x01 : rc.mouse_l;
			All_control.shoot.fire = ((rc.trigger != 0) || (rc.mouse_l == 1)) ? 0x01 : 0x00;
			if(rc.R==1)
			{All_control.shoot.fire = 0;}
		}
		else
		{
			All_control.gimbal.pitch_target -= (-rc.L_y / 2000.0f + rc.mouse_y / 1000.0f) * Stop;
			All_control.gimbal.yaw_target   -= (rc.L_x /1000.0f  + rc.mouse_x / 1000.0f) * Stop;
			All_control.shoot.fire = ((rc.trigger != 0) || (rc.mouse_l == 1)) ? 0x01 : 0x00;
			if(rc.R==1)
			{All_control.shoot.fire = 0;}
		}

		All_control.gimbal.pitch_target = 
		All_control.gimbal.pitch_target > 20 ?
		 20 : All_control.gimbal.pitch_target;
		All_control.gimbal.pitch_target = 
		All_control.gimbal.pitch_target < -28 ?
		 -28 : All_control.gimbal.pitch_target;

		pitch2_stall_protect();
		justice_gesture(&All_control);
		Motor_Control_Run(
			All_control.gimbal.pitch_target,
			All_control.gimbal.yaw_target,
			Stop*All_control.shoot.shoot_speed,
			Stop*All_control.shoot.shoot_speed,
			All_control.gimbal.pitch2_target
		);

		ctrl_send();
		osDelay(1);
	}
}
void justice_gesture(ALL_control *all_control)
{
		// Shift切换goudong/normal，3ms去抖
		static uint8_t  s_last_shift   = 0;
		static uint32_t s_debounce_cnt = 0;
		if (rc.Shift == 1 && s_last_shift == 0 && s_debounce_cnt == 0 && !g_pitch2_stall_active)
		{
			all_control->gesture_task = (all_control->gesture_task == goudong) ? normal : goudong;
			s_debounce_cnt = 14;
		}
		if (s_debounce_cnt > 0)
		{
			s_debounce_cnt--;
		}
		s_last_shift = rc.Shift;

	if (g_pitch2_stall_active)
	{all_control->gesture_task = normal;}
	if(rc.sl==2||rc.B==1||VT13_USE.use_data.B==1)
	{all_control->gesture_task = goudong;}
	if (all_control->gesture_task != g_last_gesture)
	{
		if (g_last_gesture == goudong && all_control->gesture_task == normal)
		{
			// goudong切换到normal时，Pitch目标角度归零，避免伸缩过程发生机械干涉
			all_control->gimbal.pitch_target = 0.0f;
			g_pitch_zero_filter.out = 0.0f;
		}
		else
		{
			g_pitch_zero_filter.out = all_control->gimbal.pitch_target;
		}
	}
	if (all_control->gesture_task == goudong)
	{
		all_control->gimbal.pitch2_target = PITCH2_TARGET_MAX;
		if(spin==0)
	{
			all_control->gimbal.pitch_target = first_order_filter(
			&g_pitch_zero_filter,
			5.0f,
			PITCH_ZERO_LPF_ALPHA);
			GMPitchPid_Spe.outmax=8000.0f;
	 }
		if(spin==1)
	{
		    all_control->gimbal.pitch_target = first_order_filter(
			&g_pitch_zero_filter,
			15.0f,
			0.005f);
			GMPitchPid_Spe.outmax=5000.0f;
	}
	}
	else
	{
		all_control->gimbal.pitch2_target = PITCH2_TARGET_MIN;
		GMPitchPid_Spe.outmax=16384.0f;
	}

	all_control->gimbal.pitch2_target = pitch2_clamp(all_control->gimbal.pitch2_target);

	g_last_gesture = all_control->gesture_task;
	apply_pitch_startup_protection(&All_control);
}

static void pitch2_stall_protect(void)
{
	if (g_pitch2_stall_active)
	{
		if (--g_pitch2_stall_lockout == 0U)
		{
			g_pitch2_stall_active = 0;
		}
		return;
	}

	float vel_abs = motor[MotorPitch2].para.vel;
	if (vel_abs < 0.0f) vel_abs = -vel_abs;
	float tor_abs = motor[MotorPitch2].para.tor;
	if (tor_abs < 0.0f) tor_abs = -tor_abs;

	if (vel_abs < PITCH2_STALL_VEL_THRESHOLD && tor_abs > PITCH2_STALL_TOR_THRESHOLD)
	{
		if (++g_pitch2_stall_cnt >= PITCH2_STALL_DURATION_MS)
		{
			g_pitch2_stall_active  = 1;
			g_pitch2_stall_lockout = PITCH2_STALL_LOCKOUT_MS;
			g_pitch2_stall_cnt     = 0;
		}
	}
	else
	{
		g_pitch2_stall_cnt = 0;
	}
}
//?????
//void justice_gesture(ALL_control *all_control)
//{
//	all_control->gimbal.pitch2_target += rc.mouse_z/500.0f;
//	all_control->gimbal.pitch2_target = 
//	all_control->gimbal.pitch2_target >= 10.0f ?
//	             10.0f : all_control->gimbal.pitch2_target;
//	all_control->gimbal.pitch2_target = 
//	all_control->gimbal.pitch2_target <= -50.0f ?
//		           -50.0f : All_control.gimbal.pitch2_target;
//}
void into_zimiao(void)
{	
	float diff = wrap_deg_180(All_control.zimiao.yaw_zimiao - imu_angle.yaw_real);
	float yaw_cmd = imu_angle.yaw_real + diff;

	// 拒绝与当前角度差距超过50°的自瞄目标，防止视觉野值跳变
	if (fabsf(diff) > 55.0f)
	{
		yaw_cmd = imu_angle.yaw_real;
	}

	All_control.gimbal.pitch_target =All_control.zimiao.pitch_zimiao;
	All_control.gimbal.yaw_target   =yaw_cmd;
}

static void controller_arbitrate(void)
{
    uint8_t rc_alive   = (debus_jishi < RC_OFFLINE_MS);
    uint8_t vt13_alive = (new_live_3 > 0);

    switch (g_ctrl_src) {

    case CTRL_SRC_RC:
        if (!rc_alive && vt13_alive) {
            if (++g_switch_confirm_cnt >= SWITCH_CONFIRM_MS) {
                g_ctrl_src = CTRL_SRC_VT13;
                g_switch_confirm_cnt = 0;
                g_vt13_first_frame = 1;
                rc_reset_keyboard_ramp();
            }
        } else {
            g_switch_confirm_cnt = 0;
        }
        break;

    case CTRL_SRC_VT13:
        if (VT13_USE.use_data.B)      g_poweroff_latched = 1;
        if (g_poweroff_latched && !VT13_USE.use_data.B) g_poweroff_latched = 0;

        if (rc_alive) {
            if (++g_switch_confirm_cnt >= SWITCH_CONFIRM_MS) {
                g_ctrl_src = CTRL_SRC_RC;
                g_switch_confirm_cnt = 0;
                g_poweroff_latched = 0;
            }
        } else if (!vt13_alive) {
            if (++g_switch_confirm_cnt >= SWITCH_CONFIRM_MS) {
                g_ctrl_src = CTRL_SRC_NONE;
                g_switch_confirm_cnt = 0;
            }
        } else {
            g_switch_confirm_cnt = 0;
        }
        break;

    case CTRL_SRC_NONE:
        if (rc_alive) {
            if (++g_switch_confirm_cnt >= SWITCH_CONFIRM_MS) {
                g_ctrl_src = CTRL_SRC_RC;
                g_switch_confirm_cnt = 0;
            }
        } else if (vt13_alive) {
            if (++g_switch_confirm_cnt >= SWITCH_CONFIRM_MS) {
                g_ctrl_src = CTRL_SRC_VT13;
                g_switch_confirm_cnt = 0;
                g_vt13_first_frame = 1;
                rc_reset_keyboard_ramp();
            }
        } else {
            g_switch_confirm_cnt = 0;
        }
        break;
    }
}

static void vt13_to_rc(void)
{
    if (g_vt13_first_frame) {
        rc.mouse_x = 0;
        rc.mouse_y = 0;
        g_vt13_first_frame = 0;
    } else {
        rc.mouse_x = VT13_USE.use_data.mouse_x;
        rc.mouse_y = VT13_USE.use_data.mouse_y;
    }

    rc.W = VT13_USE.use_data.W;
    rc.S = VT13_USE.use_data.S;
    rc.A = VT13_USE.use_data.A;
    rc.D = VT13_USE.use_data.D;

    rc.Shift = VT13_USE.use_data.SHIFT;
    rc.Ctrl  = VT13_USE.use_data.CTRL;

    rc.mouse_l = (VT13_USE.use_data.mouse_left  != 0) ? 1 : 0;
    rc.mouse_r = (VT13_USE.use_data.mouse_right != 0) ? 1 : 0;
    rc.B = VT13_USE.use_data.B;
	rc.Z = VT13_USE.use_data.Z;
    rc.X  = VT13_USE.use_data.X;
	rc.C  = VT13_USE.use_data.C;
    rc.R_x     = 0;
    rc.R_y     = 0;
    rc.L_x     = 0;
    rc.L_y     = 0;
    rc.trigger = 0;
    rc.Q = 0;  rc.E = 0;  rc.R = 0;  rc.F = 0;  rc.G = 0;
    rc.Z = 0;  rc.X = 0;  rc.C = 0;  rc.V = 0;
}

void handler()
{
    All_control.DJ_handler.PITCH2++;
    All_control.DJ_handler.YAW++;
    All_control.DJ_handler.PITCH++;
    All_control.DJ_handler.SHOOT_L++;
    All_control.DJ_handler.SHOOT_R++;
    debus_jishi++;
    if (new_live_3 > 0) new_live_3--;

    controller_arbitrate();

    if (g_ctrl_src == CTRL_SRC_VT13) {
        vt13_to_rc();
    } else if (g_ctrl_src == CTRL_SRC_NONE) {
        rc.sl = 2;
        rc.sr = 3;
    }

    uint8_t yaw_feedback_lost =
            (All_control.DJ_handler.YAW > GIMBAL_YAW_REINIT_CNT);
    uint8_t pitch2_feedback_lost =
            (All_control.DJ_handler.PITCH2 > GIMBAL_PITCH2_REINIT_CNT);
    if (yaw_feedback_lost || pitch2_feedback_lost)
    {
            All_control.gimbal.yaw_target = imu_angle.yaw_real;
            All_control.gimbal.pitch_target = imu_angle.pitch;
            All_control.gimbal.pitch2_target = pitch2_clamp(All_control.gimbal.pitch2_target);
            if (yaw_feedback_lost)
            {
                gimbal_yaw_reinit();
            }
            if (pitch2_feedback_lost)
            {
                gimbal_pitch2_reinit();
            }
    }

    if (debus_jishi >= 2000 && g_ctrl_src != CTRL_SRC_VT13) {
        rc.sl = 2;
    }

    if (rc.sl == 2||rc.B==1) {
        All_control.gimbal.yaw_target = imu_angle.yaw_real;
		All_control.gimbal.pitch_target = imu_angle.pitch;
    }

    if (All_control.DJ_handler.PITCH > DJ_HANDLER_TIMEOUT_CNT)
    {
        All_control.DJ_handler.PITCH_flag = 1;
    }
}

void gimbal_init(void)
{
	PID_Init_All();
	dm4340_motor_init();
	float jishi = 0.0f;
	while ((motor[MotorYaw].para.state+motor[MotorPitch2].para.state) < 2)
	{
		jishi++;
		dm4310_enable(&hcan1, &motor[MotorYaw]);//yaw
		//keep_zero(&hcan1, motor[MotorYaw].id, MIT_MODE);
		osDelay(2);
		dm4310_enable(&hcan1, &motor[MotorPitch2]);//pitch2
		//keep_zero(&hcan1, motor[MotorPitch2].id,POS_MODE);
		motor[MotorYaw].ctrl.tor_set=0;
		dm4310_ctrl_send(&hcan1, &motor[MotorYaw]);
		if (jishi >= 500)
		{
			break;
		}
		osDelay(2);
	}
}

static void gimbal_yaw_reinit(void)
{
	float jishi = 0.0f;

	PID_Init_GimbalYaw();
	dm4340_motor_init_yaw();
	while (motor[MotorYaw].para.state < 1)
	{
		jishi++;
		dm4310_enable(&hcan1, &motor[MotorYaw]);
		motor[MotorYaw].ctrl.tor_set = 0.0f;
		if (jishi >= 500)
		{
			break;
		}
		osDelay(2);
	}
}

static void gimbal_pitch2_reinit(void)
{
	float jishi = 0.0f;

	PID_Init_GimbalPitch2();
	dm4340_motor_init_pitch2();
	while (motor[MotorPitch2].para.state < 1)
	{
		jishi++;
		dm4310_enable(&hcan1, &motor[MotorPitch2]);
		if (jishi >= 500)
		{
			break;
		}
		osDelay(2);
	}
}
