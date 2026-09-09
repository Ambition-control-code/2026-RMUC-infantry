#ifndef AHRS_MIDDLEWARE_H
#define AHRS_MIDDLEWARE_H
#include "struct_typedef.h"
/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       AHRS_MiddleWare.c/h
  * @brief      ×ËÌ¬½âËãÖÐ¼ä²ã£¬Îª×ËÌ¬½âËãÌá¹©Ïà¹Øº¯Êý
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. Íê³É
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */

//¶¨Òå NULL
#ifndef NULL
#define NULL 0
#endif

//¶¨ÒåPI Öµ
#ifndef PI
#define PI 3.14159265358979f
#endif


//¶¨Òå ½Ç¶È(¶È)×ª»»µ½ »¡¶ÈµÄ±ÈÀý
#ifndef ANGLE_TO_RAD
#define ANGLE_TO_RAD 0.01745329251994329576923690768489f
#endif

//¶¨Òå »¡¶È ×ª»»µ½ ½Ç¶ÈµÄ±ÈÀý
#ifndef RAD_TO_ANGLE
#define RAD_TO_ANGLE 57.295779513082320876798154814105f
#endif

extern void AHRS_get_height(fp32 *high);
extern void AHRS_get_latitude(fp32 *latitude);
extern fp32 AHRS_invSqrt(fp32 num);
extern fp32 AHRS_sinf(fp32 angle);
extern fp32 AHRS_cosf(fp32 angle);
extern fp32 AHRS_tanf(fp32 angle);
extern fp32 AHRS_asinf(fp32 sin);
extern fp32 AHRS_acosf(fp32 cos);
extern fp32 AHRS_atan2f(fp32 y, fp32 x);
#endif
