//#include "USE_DRAW.h"
//#include "main.h"
////#include "motor.h"

//#define chongfa 1
//u16 R;
//u16 CR;
//u16 id;
//u32 text;
//picture LINE1[7];      //定义一个图像为直线
//picture zhengfang;     //定义一个矩形为zhengfang
//picture yuanhu;        //定义一个圆弧
//word V1;               //定义一个数据为电压v
//picture LINE2[3];      //定义一个图像为直线
//word D;                //定义一个数据为弹速d
//picture yuan;  
//extern capacity D_R;
//int hudu;              //圆弧显示角度

//extern volatile dji_motor Yaw;
//extern CHASSIS_Task chassis_task;


//struct 
//{
//	u8 fool:1;
//}luoji;
//int zhunxingColor =yellow;
//int zhunxingZuangTai=add;
//void pictureInit()
//{  
//  //int pwm=1400;	  
//	 // luoji.fool++;
//	  LINE1[0].name=            2999;                 //直线名
//	  LINE1[0].width=           2;                    //线宽 
//    LINE1[0].start_x=         ScreenX/2;             //直线起点x坐标
//	  LINE1[0].start_y=         ScreenY/2-50;          //直线起点y坐标
//	  LINE1[0].end_x  =         ScreenX/2;            //直线终点x坐标
//	  LINE1[0].end_y  =         ScreenY/2-100;        //直线终点y坐标
//    LINE1[0].color=           zhunxingColor;         //直线颜色
//    LINE1[0].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE1[0].graphic_tpye=    line;           //图形类型直线
//    LINE1[0].layer=           1;              //图层数1
//		 
//	  LINE1[1].name=            288;             //直线名
//	  LINE1[1].width=           2;             //线宽 
//    LINE1[1].start_x=         ScreenX/2+35;        //直线起点x坐标
//	  LINE1[1].start_y=         ScreenY/2+15-50;            //直线起点y坐标
//	  LINE1[1].end_x  =         ScreenX/2+35;        //直线终点x坐标
//	  LINE1[1].end_y  =         ScreenY/2-15-50;            //直线终点y坐标
//    LINE1[1].color=           zhunxingColor;         //直线颜色
//    LINE1[1].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE1[1].graphic_tpye=    line;           //图形类型直线
//    LINE1[1].layer=           1;              //图层数1
//		
//		LINE1[2].name=            3788;             //直线名
//	  LINE1[2].width=           2;             //线宽 
//    LINE1[2].start_x=         ScreenX/2-55;        //直线起点x坐标
//	  LINE1[2].start_y=         ScreenY/2-50;            //直线起点y坐标
//	  LINE1[2].end_x  =         ScreenX/2+55;        //直线终点x坐标
//	  LINE1[2].end_y  =         ScreenY/2-50;            //直线终点y坐标
//    LINE1[2].color=           zhunxingColor;         //直线颜色
//    LINE1[2].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE1[2].graphic_tpye=    line;           //图形类型直线
//    LINE1[2].layer=           1;              //图层数1
//		
//		LINE1[3].name=            4349;             //直线名
//	  LINE1[3].width=           2;             //线宽 
//    LINE1[3].start_x=         ScreenX/2-47;        //直线起点x坐标
//	  LINE1[3].start_y=         ScreenY/2-100;            //直线起点y坐标
//	  LINE1[3].end_x  =         ScreenX/2+47;        //直线终点x坐标
//	  LINE1[3].end_y  =         ScreenY/2-100;            //直线终点y坐标
//    LINE1[3].color=           zhunxingColor;         //直线颜色
//    LINE1[3].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE1[3].graphic_tpye=    line;           //图形类型直线
//    LINE1[3].layer=           1;              //图层数1
//		
//		LINE1[4].name=            5700;             //直线名
//	  LINE1[4].width=           2;             //线宽 
//    LINE1[4].start_x=         ScreenX/2-35;        //直线起点x坐标
//	  LINE1[4].start_y=         ScreenY/2+15-50;            //直线起点y坐标
//	  LINE1[4].end_x  =         ScreenX/2-35;        //直线终点x坐标
//	  LINE1[4].end_y  =         ScreenY/2-15-50;            //直线终点y坐标
//    LINE1[4].color=           zhunxingColor;         //直线颜色
//    LINE1[4].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE1[4].graphic_tpye=    line;           //图形类型直线
//    LINE1[4].layer=           1;              //图层数1
//		
//	  LINE2[0].name=            6666;             //直线名
//	  LINE2[0].width=           2;             //线宽 
//    LINE2[0].start_x=         ScreenX/2-50;            //直线起点x坐标
//	  LINE2[0].start_y=         ScreenY/2-50;            //直线起点y坐标
//	  LINE2[0].end_x  =         ScreenX/2+50;            //直线终点x坐标
//	  LINE2[0].end_y  =         ScreenY/2+50;          //直线终点y坐标
//    LINE2[0].color=           white;        //直线颜色
//    LINE2[0].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE2[0].graphic_tpye=    line;           //图形类型直线
//    LINE2[0].layer=           2;              //图层数1
//		 
//	  LINE2[1].name=            8989;             //直线名
//	  LINE2[1].width=           2;             //线宽 
//    LINE2[1].start_x=         ScreenX/2+50;        //直线起点x坐标
//	  LINE2[1].start_y=         ScreenY/2-50;            //直线起点y坐标
//	  LINE2[1].end_x  =         ScreenX/2-50;        //直线终点x坐标
//	  LINE2[1].end_y  =         ScreenY/2+50;            //直线终点y坐标
//    LINE2[1].color=           white;         //直线颜色
//    LINE2[1].operate_tpye=    zhunxingZuangTai;            //图形操作添加图形
//    LINE2[1].graphic_tpye=    line;           //图形类型直线
//    LINE2[1].layer=           2;              //图层数1
//  	
//		zhengfang.name=         1275;
//		zhengfang.width=        10;
//		zhengfang.start_x=      ScreenX/2+10;
//		zhengfang.start_y=      ScreenY/2-435;
//		zhengfang.end_x=        ScreenX/2-10;
//		zhengfang.end_y=        ScreenY/2-455;
//		zhengfang.color=        white;
//		zhengfang.operate_tpye= zhunxingZuangTai;
//		zhengfang.graphic_tpye= rele;
//		zhengfang.layer       = 1;
//		
//		yuanhu.name =           7878;
//		yuanhu.start_angle   =  330;
//		yuanhu.end_angle     =  30;
//		yuanhu.width         =  5;
//		yuanhu.start_x       =  ScreenX/2;
//		yuanhu.start_y       =  ScreenY/2-450;
//		yuanhu.end_x         =  70;
//		yuanhu.end_y         =  70;
//		yuanhu.color         =  green;
//		yuanhu.operate_tpye  =  add;
//		yuanhu.graphic_tpye  =  ciar; 
//		yuanhu.layer         =  4;
//		
//		yuan.name =  2523;
//		yuan.width=5;
//		yuan.start_x=ScreenX/2-15;
//		yuan.start_y=ScreenY/2-50;
////		yuan.end_x         =  70;
////		yuan.end_y         =  70;
//		yuan.color =        green;
//		yuan.operate_tpye= add;
//		yuan.graphic_tpye = _round;
//		yuan.radius = 10;
//		yuan.layer = 4;
//		
//		
//	  V1.name=9999;
//		V1.start_angle =20;              //字体大小
//		V1.end_angle   = 3;              //小数有效位数
//		V1.width       = 3;              //线宽
//		V1.start_x     = ScreenX/2+300;
//		V1.start_y     = ScreenY/2+300;
//		V1.W.zhengshu   = text;
//		V1.color       =  white;
//		V1.operate_tpye = add;
//		V1.graphic_tpye = ZS;
//		V1.layer        = 3;
//		
//		

//		D.start_angle =   40;              //字体大小
//		D.end_angle   =   2;              //小数有效位数
//		D.width       =   4;              //线宽
//		D.start_x     =   ScreenX/2+340;
//		D.start_y     =   ScreenY/2+270;
//		D.W.zhengshu   =  1400*1;
//	  D.color        =  green;
//		D.operate_tpye =  add;
//		D.graphic_tpye =  ZS;
//		D.layer        =  3;
//}
//u8 zuce=0;
////大疆ui显示图像步骤 ，添加显示图像和修改显示图像，通过添加显示图像实现显示，修改图像实现刷新
//void drawPictureDemo(void)
//{
//	  pictureInit(); 
////		id=robot.robot_id;
//	id=3;
//    if(id<0X100)
//		{
//		   R=id;
//			 CR=id+0x100;
//		}
//		
//		
//		else if(id>=0x100)
//		{
//		   R=id;
//			 CR=R-100+0X164;
//		}
// int sd=0;
//	
//		
//		switch(zuce)
//		{
//			case 0:
//					 for(sd=0;sd<chongfa;sd++)
//								{
//									drawPicture(draw1P,R,CR,(u8*)&V1);          //绘图函数
//								}
//					break;
//		  case 1:			
//					for(sd=0;sd<chongfa;sd++)
//								{
//									drawPicture(draw1P,R,CR,(u8*)&D);           //绘图函数
//								}
//					break;
//			case 2:
//						for(sd=0;sd<chongfa;sd++)
//							 {
//									drawPicture(draw5P,R,CR,(u8*)LINE1);        //绘图函数
//								}
//					break;
//			case 3:		
//					for(sd=0;sd<chongfa;sd++)
//							{
//								drawPicture(draw1P,R,CR,(u8*)&yuanhu);      //绘图函数
//							}
//					break;
//			case 4:
//					for(sd=0;sd<chongfa;sd++)
//							{
//								drawPicture(draw2P,R,CR,(u8*)LINE2);        //绘图函数
//							}
//					break;
//			case 5:
//						for(sd=0;sd<chongfa;sd++)
//							{
//								drawPicture(draw1P,R,CR,(u8*)&zhengfang);   //绘图函数
//							}
//					break;
//			case 6:
//						for(sd=0;sd<chongfa;sd++)
//							{
//								drawPicture(draw1P,R,CR,(u8*)&yuan);   //绘图函数
//							}
//					break;
//					
//					
//			default: break;
//		}
//		zuce++;
//		if(zuce>6){zuce=0;}
//    
//    
//    
//}
////  for(sd=0;sd<chongfa;sd++)
////	{
////		drawPicture(draw1P,R,CR,(u8*)&V1);          //绘图函数
////	}
////		vTaskDelay(1);
////	for(sd=0;sd<chongfa;sd++)
////	{
////		drawPicture(draw1P,R,CR,(u8*)&D);           //绘图函数
////	}
////		vTaskDelay(1);
////	for(sd=0;sd<chongfa;sd++)
////	{
////		drawPicture(draw5P,R,CR,(u8*)LINE1);        //绘图函数
////	}
////		vTaskDelay(1);
////	for(sd=0;sd<chongfa;sd++)
////	{
////		drawPicture(draw1P,R,CR,(u8*)&yuanhu);      //绘图函数
////	}
////	vTaskDelay(1);
////	
////	for(sd=0;sd<chongfa;sd++)
////	{
////		drawPicture(draw2P,R,CR,(u8*)LINE2);        //绘图函数
////	}
////		vTaskDelay(2);
////		for(sd=0;sd<10;sd++)
////	{
////		drawPicture(draw1P,R,CR,(u8*)&zhengfang);   //绘图函数
////	}
////	vTaskDelay(1);
////		for(sd=0;sd<chongfa;sd++)
////	{
////    deletePicture(draw1P,R ,CR,delAll,1);
////	}	
//    

// u8 ji=0;
//int abc=0;


//void drawPictureDemoppxu(void)
//{
//	  pictureInit(); 
//    D.operate_tpye =  change;
//	  zhengfang.operate_tpye= change;
//		yuanhu.operate_tpye  =  change;
//		V1.operate_tpye = change;
//	  V1.W.zhengshu=abc++;
//	  D.W.zhengshu=D_R.V*100;
//	
//	  if  (D.W.zhengshu>2000)
//		{
//		  D.color   =  green;
//		}
//		else if(D.W.zhengshu>1500)
//		{
//		  D.color   =  yellow;
//		}
//		else
//		{
//		   D.color  =  white;
//		}
//	  
//	 	//deletePicture(delP,R,CR,delOne,0);
//		id=robot.robot_id;
//    if(id<0X100)
//		{
//		   R=id;
//			 CR=id+0x100;
//		}
//		else if(id>=0x100)
//		{
//		   R=id;
//			 CR=R-100+0X164;
//		}
//		if ((Data_C.key_set&0x02)==0x02)
//		{
//		   	zhengfang.width=        5;
//		    zhengfang.start_x=      ScreenX/2+15;
//		    zhengfang.start_y=      ScreenY/2-430;
//		    zhengfang.end_x=        ScreenX/2-15;
//		    zhengfang.end_y=        ScreenY/2-460;
//			
//		}
//		else
//		{
//		    zhengfang.width=        15;
//	    	zhengfang.start_x=      ScreenX/2+10;
//		    zhengfang.start_y=      ScreenY/2-440;
//		    zhengfang.end_x=        ScreenX/2-10;
//		    zhengfang.end_y=        ScreenY/2-450;
//		}
//		
//		if (Data_C.spin==1)
//		{
//		
//		  hudu+=15;
//			if(hudu>360)
//			{hudu=0;}
//			yuanhu.color         =  yellow;
//		
//		}
//		else
//		{		 
//		    hudu=0;	
//         yuanhu.color         =  green;      
//		}
//       hudu=fabs(fmodf(Yaw.ecd_angle-chassis_task.follow_angle_set+360,360.0f));
//		  
//	     yuanhu.start_angle   =  (hudu-30)<0?(hudu+360-30):(hudu-30);
//		   yuanhu.end_angle     =  (hudu+30)>360?(hudu-360+30):(hudu+30);

//		
//		if ((Data_C.key_set&0x04)==0x04&&robot.mains_power_shooter_output==1)//发射机构
//		{
//			 LINE2[1].start_x=         ScreenX/2;            //直线起点x坐标
//			 LINE2[1].start_y=         ScreenY/2;            //直线起点y坐标
//			 LINE2[1].end_x  =         ScreenX/2;            //直线终点x坐标
//			 LINE2[1].end_y  =         ScreenY/2;          //直线终点y坐标

//			 LINE2[0].start_x=         ScreenX/2;            //直线起点x坐标			
//			 LINE2[0].start_y=         ScreenY/2;            //直线起点y坐标
//			 LINE2[0].end_x  =         ScreenX/2;            //直线终点x坐标
//			 LINE2[0].end_y  =         ScreenY/2;          //
//			 LINE2[1].operate_tpye=    change;            //图形操作添加图形
//			 LINE2[0].operate_tpye=    change;            //图形操作添加图形
//		}
//		else
//		{
//			 LINE2[0].start_x=         ScreenX/2-50;            //直线起点x坐标
//			 LINE2[0].start_y=         ScreenY/2-50;            //直线起点y坐标
//			 LINE2[0].end_x  =         ScreenX/2+50;            //直线终点x坐标
//			 LINE2[0].end_y  =         ScreenY/2+50;          //直线终点y坐标
//		
//			 LINE2[1].start_x=         ScreenX/2+50;        //直线起点x坐标
//			 LINE2[1].start_y=         ScreenY/2-50;            //直线起点y坐标
//			 LINE2[1].end_x  =         ScreenX/2-50;        //直线终点x坐标
//			 LINE2[1].end_y  =         ScreenY/2+50;            //直线终点y坐标
//			 LINE2[1].operate_tpye=    change;            //图形操作添加图形
//			 LINE2[0].operate_tpye=    change;            //图形操作添加图形    
//		}
// int sd=0;
//		switch(ji)
//		{
//			case 0:
//					for(sd=0;sd<chongfa;sd++)
//					{
//						drawPicture(draw1P,R,CR,(u8*)&V1);           //绘图函数
//					}	
//					break;
//		  case 1:			
//					for(sd=0;sd<chongfa;sd++)
//					{	
//						drawPicture(draw1P,R,CR,(u8*)&D);          //绘图函数
//					}
//					break;
//			case 2:
//					for(sd=0;sd<chongfa;sd++)
//					{
//						drawPicture(draw2P,R,CR,(u8*)LINE2);        //绘图函数
//					}
//					break;
//			case 3:		
//					for(sd=0;sd<chongfa;sd++)
//					{
//						drawPicture(draw1P,R,CR,(u8*)&yuanhu);      //绘图函数
//					}
//					break;
//			case 4:
//						for(sd=0;sd<10;sd++)
//					{
//						drawPicture(draw1P,R,CR,(u8*)&zhengfang);   //绘图函数
//					}
//					break;
//					
//					
//			default: break;
//		}
//		ji++;
//		if(ji>4){ji=0;}   
//}

