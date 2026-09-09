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

#include "AHRS_MiddleWare.h"
#include "AHRS.h"
#include "arm_math.h"
#include "main.h"
/**
  * @brief          ÓÃÓÚ»ñÈ¡µ±Ç°¸ß¶È
  * @author         RM
  * @param[in]      ¸ß¶ÈµÄÖ¸Õë£¬fp32
  * @retval         ·µ»Ø¿Õ
  */

void AHRS_get_height(fp32 *high)
{
    if (high != NULL)
    {
        *high = 0.0f;
    }
}

/**
  * @brief          ÓÃÓÚ»ñÈ¡µ±Ç°Î³¶È
  * @author         RM
  * @param[in]      Î³¶ÈµÄÖ¸Õë£¬fp32
  * @retval         ·µ»Ø¿Õ
  */

void AHRS_get_latitude(fp32 *latitude)
{
    if (latitude != NULL)
    {
//        *latitude = Latitude_At_ShenZhen;
    }
}

/**
  * @brief          ¿ìËÙ¿ª·½º¯Êý£¬
  * @author         RM
  * @param[in]      ÊäÈëÐèÒª¿ª·½µÄ¸¡µãÊý£¬fp32
  * @retval         ·µ»Ø1/sqrt ¿ª·½ºóµÄµ¹Êý
  */

fp32 AHRS_invSqrt(fp32 num)
{
    fp32 halfnum = 0.5f * num;
    fp32 y = num;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(fp32 *)&i;
    y = y * (1.5f - (halfnum * y * y));
    return y;
}

/**
  * @brief          sinº¯Êý
  * @author         RM
  * @param[in]      ½Ç¶È µ¥Î» rad
  * @retval         ·µ»Ø¶ÔÓ¦½Ç¶ÈµÄsinÖµ
  */

fp32 AHRS_sinf(fp32 angle)
{
    return arm_sin_f32(angle);
}
/**
  * @brief          cosº¯Êý
  * @author         RM
  * @param[in]      ½Ç¶È µ¥Î» rad
  * @retval         ·µ»Ø¶ÔÓ¦½Ç¶ÈµÄcosÖµ
  */

fp32 AHRS_cosf(fp32 angle)
{
    return arm_cos_f32(angle);
}

/**
  * @brief          tanº¯Êý
  * @author         RM
  * @param[in]      ½Ç¶È µ¥Î» rad
  * @retval         ·µ»Ø¶ÔÓ¦½Ç¶ÈµÄtanÖµ
  */

fp32 AHRS_tanf(fp32 angle)
{
    return tanf(angle);
}
/**
  * @brief          ÓÃÓÚ32Î»¸¡µãÊýµÄ·´Èý½Çº¯Êý asinº¯Êý
  * @author         RM
  * @param[in]      ÊäÈësinÖµ£¬×î´ó1.0f£¬×îÐ¡-1.0f
  * @retval         ·µ»Ø½Ç¶È µ¥Î»»¡¶È
  */

fp32 AHRS_asinf(fp32 sin)
{
    return asinf(sin);
}

/**
  * @brief          ·´Èý½Çº¯Êýacosº¯Êý
  * @author         RM
  * @param[in]      ÊäÈëcosÖµ£¬×î´ó1.0f£¬×îÐ¡-1.0f
  * @retval         ·µ»Ø¶ÔÓ¦µÄ½Ç¶È µ¥Î»»¡¶È
  */

fp32 AHRS_acosf(fp32 cos)
{

    return acosf(cos);
}

/**
  * @brief          ·´Èý½Çº¯Êýatanº¯Êý
  * @author         RM
  * @param[in]      ÊäÈëtanÖµÖÐµÄyÖµ ×î´óÕýÎÞÇî£¬×îÐ¡¸ºÎÞÇî
  * @param[in]      ÊäÈëtanÖµÖÐµÄxÖµ ×î´óÕýÎÞÇî£¬×îÐ¡¸ºÎÞÇî
  * @retval         ·µ»Ø¶ÔÓ¦µÄ½Ç¶È µ¥Î»»¡¶È
  */

fp32 AHRS_atan2f(fp32 y, fp32 x)
{
    return atan2f(y, x);
}
void _ttywrch(int ch)
{
	
}
