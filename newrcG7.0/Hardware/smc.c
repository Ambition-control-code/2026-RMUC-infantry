#include "smc.h"

static int8_t Signal(float y);
static float Sat(float y);
static float limit_max_min(float in,float min,float max);

//SMC YawSMC(20, 120, 0, 0.001, 25000, 0.8, 0.5); //这是一个yaw轴参考电机参数

/*****滑膜控制器设计*****/
//参考复旦、川大开源

//_差分求导一次 __差分求导两次

//力矩 = 转动惯量 * 角加速度
//力矩 = 电机转矩常数 * 电流
//解得角加速度为 （电机转矩常数 * 电流） / 转动惯量->将转动惯量带入到电机转矩常数，简化计算
//将电机模型简化为两个积分串联器 ：角加速度积分得到角速度，角速度积分得到角度
//引入角度误差变量 error = 角度实际值 - 角度目标值  _error = 角速度 - _目标角度
//引入控制量u后，用u代替电流I-> __error = 角加速度 - __目标角度-> __error = u/J - __目标角度

//选取滑模面
//s = c*error + _error
//_s = c*_error + __error = c * error + u/J - __目标角度
//选用指数趋近率 _s = -k*s - & * sign(s)
//解得u = J * (__目标角度 - c * error - (k * s + & * sign(s)))

//在实际代码中可以去掉角度的二次微分（差分），加大K值来减小超调（川大开源）
//用于YAW轴，C值根据误差角度动态调整

/**
* 参数1 ：滑膜控制结构体
* 参数2 ：加大c值提高响应速度，但会加大超调，c值太大大幅度缓慢震荡（川大开源）
* 参数3 ：加大k值减小超调并提高响应，k值太大会导致高频震荡（川大开源）
* 参数4 ：目标初始值
* 参数5 ：误差下限
* 参数6 ：输出限幅
* 参数7 ：转动惯量
* 参数7 ：干扰绝对值上限，对效果影响不大（川大开源）
* 注意 ：无
**/

void SMC_init(SMC_control *SMC,float C,float K,float ref,float error_eps,float u_max,float J,float epsilon)
{
	if(SMC == NULL)
	{
		return ;
	}
	SMC->C_IN = C;
	SMC->K_IN = K;
	SMC->ref = ref;
	SMC->error_eps = error_eps;
	SMC->u_max = u_max;
	SMC->J = J;
	SMC->epsilon = epsilon;	
}

/**
* 参数1 ：滑膜控制结构体
* 参数2 ：角度目标值
* 参数3 ：角度实际值
* 参数4 ：速度实际值
* 注意 ：无
**/
float smc_k_c,smc_k_k;
float SMC_Tick(SMC_control *SMC,float ref,float angle_now,float angle_vel) //anlge为当前位置(°),ang_vel为角速度(°/s)
{
	if(SMC == NULL)
	{
		return 0.0f;
	}
	//读取参数
	SMC->angle = angle_now;
	SMC->ang_vel = angle_vel;
	SMC->ref = ref;
	SMC->error = SMC->angle - SMC->ref;//计算误差
	
	smc_k_c = 0,smc_k_k = 0;
	
	
	//smc_k_c = limit_max_min(smc_k_c,0.4,1.5);
	//smc_k_k = limit_max_min(smc_k_k,0.4,1.5);
	
	//SMC->C = SMC->C_IN * smc_k_c;
	//SMC->K = SMC->K_IN * smc_k_k;
	
	//SMC->C = SMC->C_IN;
	smc_k_c = 180/(SMC->error-90);
	smc_k_c = limit_max_min(smc_k_c,0.5,1.2);
	SMC->C = SMC->C_IN * smc_k_c;
	SMC->K = SMC->K_IN;
	
	//这里对前馈进行了处理，没有严格单位统一
	SMC->ddref = (SMC->ref - SMC->last_ref) - SMC->dref; 
	SMC->dref = (SMC->ref - SMC->last_ref);
	
	//误差下限处理
	if (fabs(SMC->error) < SMC->error_eps)
	{
		SMC->u = 0;
		return 0;
	}
	
	SMC->s = SMC->C * SMC->error + (SMC->ang_vel - SMC->dref);
	//SMC->u = SMC->J * (SMC->ddref - SMC->C * (SMC->ang_vel - SMC->dref) - SMC->epsilon * Sat(SMC->s) - SMC->K * SMC->s);
	SMC->u = SMC->J * (-SMC->C * (SMC->ang_vel - SMC->dref) - SMC->epsilon * Sat(SMC->s) - SMC->K * SMC->s);
	
	//控制量限幅
	if (SMC->u > SMC->u_max)
		SMC->u = SMC->u_max;
	if (SMC->u < -SMC->u_max)
		SMC->u = -SMC->u_max;
	
	//善后处理
	SMC->last_ref = SMC->ref;
	
	return SMC->u;
}

static int8_t Signal(float y)
{
	if (y > 0)
		return 1;
	else if (y == 0)
		return 0;
	else
		return -1;
}

static float Sat(float y)
{
	if (fabs(y) <= 1)
		return y;
	else
		return Signal(y);
}

static float limit_max_min(float in,float min,float max)
{
	in > max ? in = max : 1;
	in < min ? in = min : 1;
	return in;
}

