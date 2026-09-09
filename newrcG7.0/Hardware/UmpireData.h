#ifndef _UMPIREDATA_H_
#define _UMPIREDATA_H_

#include  "main.h"
//#include  "CRC8_16.h"


typedef  struct
{ //0001
	u8   game_type:4;       //比赛类型
	u8   game_progress:4;   //当前比赛阶段         
	u16  remain_time;       //当前阶段剩余时间     *
	//0002
	u8   winner:4;          //比赛结果
	//0003
	u16  red_1_HP;          //红1英雄血量
	u16  red_2_HP;          //红2工程血量
	u16  red_3_HP;          //红3步兵血量
	u16  red_4_HP;          //红4步兵血量
	u16  red_5_HP;          //红5步兵血量
	u16  red_7_HP;          //红7哨兵血量
	u16  red_outpost_HP;    //红方前哨站血量
	u16  red_base_HP;       //红方基地血量
	u16  blue_1_HP;         //蓝1英雄血量
	u16  blue_2_HP;         //蓝2工程血量
  u16  blue_3_HP; 	      //蓝3步兵血量
	u16  blue_4_HP;         //。。。。
	u16  blue_5_HP;         //。。。。
	u16  blue_7_HP;         //。。。。
	u16  blue_outpost_HP;   //蓝方前哨站血量
	u16  blue_base_HP;
	//0004
	u8   dart_belong;             //发射飞镖任务
	u16  stage_remaining_time;    //发射剩余时间
	//0101 
	u32  event_type;              //场地事件数据
	//0102 
	u8   supply_projectile_id;   //补给站口ID
	u8   supply_robot_id;        //补单机器人
	u8   supply_projectile_step; //出弹口状态
	u8   supply_projectile_num;  //补单数量
	//0103
	
	//0104
	u8   level;                  //警告等级
	u8   foul_id;                //犯规机器人ID
	//0105
	u8 dart_remaining_time;       //飞镖发射口倒计时
	//0201
	u8 robot_id;
	u8 robot_level;   
	u16 remain_HP;
	u16 max_HP;
	u16 shooter_id1_17mm_cooling_rate;
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
	u16  classis_voit;                            //底盘输出电压mv
	u16  classis_current;                         //底盘输出电压ma
float  classis_power;                           //底盘输出功率w
	u16  classis_power_buffer;                    //底盘功率缓冲J     **
	u16  shooter_heat0;                           //17mm枪口热量      **
	u16  shooter_heat1;                           //42mm枪口热量
	u16  mobile_shooter_heat2;                    //机动17mm枪口热量
	//0x0203 
  float x;                                      //位置x m
  float y; 
  float z;
	float yaw;                                    //位置枪口 度
	//0x0204
	u8  power_rune_buff;                           //机器人增益        *
	//0x0205
	u16  energy_point;                              //空中机器人能量点
	u8   attack_time;                               //可攻击时间
	//0x0206
	u8 armar_id :4;                                 //被打装甲id       *
	u8 hurt_type:4;                                 //伤害类型         *
	//0x0207
	u8 bullet_type;                                 //子弹类型
	u8 bullet_freq;                                 //子弹射频         *
	float bullet_speed;                             //子弹射速         *
	//0x0208 
	u16 bullet_remaining_num;                       //弹丸剩余         *
	//0x0209
	u32 rfid_status;                                //RFID状态
	//0x020A
	u8  dart_launch_opening_status;                 //飞镖口发射状态 
	u8  dart_attack_target;                         //飞镖打击目标
	u16	target_change_time;                         //切换目标时剩余时间
	u8  first_dart_speed;                           //检测第一枚飞镖速度
	u8  second_dart_speed;                          //检测第二枚飞镖速度
	u8  third_dart_speed;                           //3
	u8  fourth_dart_speed;                          //4
	u16 last_dart_launch_time;                      //最近一次发射比赛剩余时间
	u16 operate_launch_cmd_time;                    //最近一次操作手确定发射时比赛时间     
	//0x0301
	
}Robot_Para;//存放机器人参数结构体

void Umpire_UsartInit(void);

void DMA_USART1_Send(u8 *data, u16 size);
void deal_data (void);

extern Robot_Para robot; 


#endif


