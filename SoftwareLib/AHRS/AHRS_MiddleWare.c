/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       AHRS_MiddleWare.c/h
  * @brief      ��̬�����м�㣬Ϊ��̬�����ṩ��غ���
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */

#include "AHRS_MiddleWare.h"
#include "AHRS.h"
#include "math.h"

#define Latitude_At_ShenZhen 22.57025f

/**
  * @brief          ���ڻ�ȡ��ǰ�߶�
  * @author         RM
  * @param[in]      �߶ȵ�ָ�룬fp32
  * @retval         ���ؿ�
  */

void AHRS_get_height(fp32 *high)
{
    if (high != NULL)
    {
        *high = 0.0f;
    }
}

/**
  * @brief          ���ڻ�ȡ��ǰγ��
  * @author         RM
  * @param[in]      γ�ȵ�ָ�룬fp32
  * @retval         ���ؿ�
  */

void AHRS_get_latitude(fp32 *latitude)
{
    if (latitude != NULL)
    {
        *latitude = Latitude_At_ShenZhen;
    }
}

/**
  * @brief          ���ٿ���������
  * @author         RM
  * @param[in]      ������Ҫ�����ĸ�������fp32
  * @retval         ����1/sqrt ������ĵ���
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
  * @brief          sin����
  * @author         RM
  * @param[in]      �Ƕ� ��λ rad
  * @retval         ���ض�Ӧ�Ƕȵ�sinֵ
  */

fp32 AHRS_sinf(fp32 angle)
{
    return sinf(angle);
}
/**
  * @brief          cos����
  * @author         RM
  * @param[in]      �Ƕ� ��λ rad
  * @retval         ���ض�Ӧ�Ƕȵ�cosֵ
  */

fp32 AHRS_cosf(fp32 angle)
{
    return cosf(angle);
}

/**
  * @brief          tan����
  * @author         RM
  * @param[in]      �Ƕ� ��λ rad
  * @retval         ���ض�Ӧ�Ƕȵ�tanֵ
  */

fp32 AHRS_tanf(fp32 angle)
{
    return tanf(angle);
}
/**
  * @brief          ����32λ�������ķ����Ǻ��� asin����
  * @author         RM
  * @param[in]      ����sinֵ�����1.0f����С-1.0f
  * @retval         ���ؽǶ� ��λ����
  */

fp32 AHRS_asinf(fp32 sin)
{

    return asinf(sin);
}

/**
  * @brief          �����Ǻ���acos����
  * @author         RM
  * @param[in]      ����cosֵ�����1.0f����С-1.0f
  * @retval         ���ض�Ӧ�ĽǶ� ��λ����
  */

fp32 AHRS_acosf(fp32 cos)
{

    return acosf(cos);
}

/**
  * @brief          �����Ǻ���atan����
  * @author         RM
  * @param[in]      ����tanֵ�е�yֵ ����������С������
  * @param[in]      ����tanֵ�е�xֵ ����������С������
  * @retval         ���ض�Ӧ�ĽǶ� ��λ����
  */

fp32 AHRS_atan2f(fp32 y, fp32 x)
{
    return atan2f(y, x);
}
