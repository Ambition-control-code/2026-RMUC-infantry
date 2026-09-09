#ifndef ulitility_tools_H
#define ulitility_tools_H
#include "main.h"

//大疆目前对1920*1080分辨率支持比较友好
#define ScreenX 1920
#define ScreenY 1080              //显示分辨率

#define MAXB_S   3920       //每秒字节传输速率上限





#define  Cred1   0x0101     //红1英雄客户端ID
#define  Cred2   0x0102     //红2工程客户端ID
#define  Cred3   0x0103     //红3步兵客户端ID
#define  Cred4   0x0104     //红4步兵客户端ID
#define  Cred5   0x0105     //红5步兵客户端ID
#define  Cred6   0x0106     //红6无人机客户端ID
//computer red 
#define  Cblue1  0x0165     //蓝1英雄客户端ID
#define  Cblue2  0x0166     //蓝2工程客户端ID
#define  Cblue3  0x0167     //蓝3步兵客户端ID
#define  Cblue4  0x0168     //蓝4步兵客户端ID
#define  Cblue5  0x0169     //蓝5步兵客户端ID
#define  Cblue6  0x016A     //蓝6无人机客户端ID
//computer blue
#define  red1    1          //红1机器人ID
#define  red2    2          //红2机器人ID
#define  red3    3          //红3机器人ID
#define  red4    4          //红4机器人ID
#define  red5    5          //红5机器人ID
#define  red6    6          //红6机器人ID
#define  red7    7  
#define  red9    9
//robot red
#define  blue1   101        //蓝1机器人ID
#define  blue2   102        //蓝2机器人ID
#define  blue3   103        //蓝3机器人ID
#define  blue4   104        //蓝4机器人ID
#define  blue5   105        //蓝5机器人ID
#define  blue6   106        //蓝6机器人ID
#define  blue7   107        //蓝6机器人ID
#define  blue9   109        //蓝6机器人ID
//robot blue
#define  delP     0x0100     //删除图形指令ID         del picture    ID
#define  draw1P   0x0101     //绘制一个图形指令ID     draw 1 pixture ID
#define  draw2P   0x0102     //绘制两个图形指令ID     draw 2 pixture ID
#define  draw5P   0x0103     //绘制五个图形指令ID     draw 5 pixture ID
#define  draw7P   0x0104     //绘制七个图形指令ID     draw 7 pixture ID
#define  drawWord 0x0110     //绘制字符               draw word
#define  RtoR     0x0301     //机器人对机器人通信cmdid标识
//robot to robot
#define  red      0   //红色
#define  blue     0   //蓝色
#define  yellow   1   //黄色
#define  green    2   //绿色
#define  orange   3   //橘色
#define  amaranth 4   //紫红色
#define  pink     5   //粉色
#define  cyan     6   //青色
#define  black    7   //黑色
#define  white    8   //白色

#define  line     0   //直线
#define  rele     1   //矩形    rectangle
#define  _round    2   //正圆  
#define  elal     3   //椭圆    elliptical
#define  ciar     4   //圆弧    circular arc
#define  FD       5   //小树
#define  ZS       6   //整数
//图层删除操作标识
#define  null     0   //空操作
#define  delOne   1   //删除图层
#define  delAll   2   //删除所有
//图层操作标识
#define  null     0   //空操作
#define  add      1   //增加
#define  change   2   //更改
#define  del      3   //删除



typedef struct//发送图形数据段
{
	u32 name;
  u32 operate_tpye:3; 
  u32 graphic_tpye:3; 
  u32 layer:4; 
  u32 color:4; 
  u32 start_angle:9;
  u32 end_angle:9;
  u32 width:10; 
  u32 start_x:11; 
  u32 start_y:11; 
  u32 radius:10; 
  u32 end_x:11; 
  u32 end_y:11;
}picture; 
typedef union
{
  float xiaoshu;
	int  zhengshu;
}ZZ;
typedef struct//发送图形数据段
{
	u32 name;
  u32 operate_tpye:3; 
  u32 graphic_tpye:3; 
  u32 layer:4; 
  u32 color:4; 
  u32 start_angle:9;//字体大小
  u32 end_angle:9;//小数有效位数
  u32 width:10; //线宽
  u32 start_x:11; 
  u32 start_y:11; 
  ZZ W ;
}word; 
typedef struct//整体数据帧
{
	 u8 SOF;                  //帧头
	 u16 data_length;         //数据长度
   u8 seq;                  //包数
   u8 CRC8;                 //CRC8校验
   u16 cmd_id;	            //命令码ID
   u16 ID;                  //数据内容ID
	 u16 sendID;              //发送者ID
   u16 receptionID;         //接收者ID
	 picture picture1;        //图形1
	 u16 CRC16;               //CRC16校验

}draw1picture;
typedef struct//整体数据帧
{
	 u8  SOF;                  //帧头
	 u16 data_length;          //数据长度
   u8  seq;                  //包数
   u8  CRC8;                 //CRC8校验
   u16 cmd_id;	             //命令码ID
   u16 ID;                   //数据内容ID
	 u16 sendID;               //发送者ID
   u16 receptionID;          //接收者ID
	 u8  operate_tpye;         //图形操作
	 u8  layer;                //图层数
	 u16 CRC16;                //CRC16校验

}deletepicture;

typedef __packed struct
{
    uint16_t Order;
    uint32_t Count;

    float *x;
    float *y;

    float k;
    float b;

    float StandardDeviation;

    float t[4];
	
	  float result;
} Ordinary_Least_Squares_t;

float gimbal_pitch_replay(float now_pitch,float *a);
void  OLS_Init(Ordinary_Least_Squares_t *OLS, uint16_t order);
float OLS_Smooth(Ordinary_Least_Squares_t *OLS, float deltax, float y);
void drawPictureDemo(void);

#endif
