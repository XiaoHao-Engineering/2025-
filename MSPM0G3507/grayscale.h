/**
  *************************************************************
  * @file     grayscale.h
  * @author   2026电赛 E题 小组
  * @brief    8路灰度传感器 模块头文件
  *           驱动8路模拟量灰度传感器，ADC采集+加权偏差计算
  *************************************************************
  */
#ifndef __GRAYSCALE_H
#define __GRAYSCALE_H

#include <stdint.h>

/* 灰度传感器配置参数 ***************************************/
#define GRAY_CHANNEL_NUM       8           /* 灰度探头数量 */
#define GRAY_BLACK_THRESHOLD   800         /* 黑线阈值(原始ADC值)，高于此值判断为黑线 */
#define GRAY_CORNER_THRESHOLD  5           /* 直角判定：至少N路同时"全黑" */
#define GRAY_ADC_MAX_VALUE     4095        /* 12位ADC最大值 */

/**
  * @brief  初始化8路灰度ADC
  * @note   需配合SysConfig配置ADC0_IN0~7，参考电压3.3V
  *         初始化ADC模块为单次转换模式，12位分辨率
  * @retval 无
  */
void GRAY_Init(void);

/**
  * @brief  读取全部8路灰度值
  * @param  buf : 存放8路原始ADC值的数组首地址
  * @note   buf[0]=最左路，buf[7]=最右路
  * @retval 无
  */
void GRAY_ReadAll(uint16_t *buf);

/**
  * @brief  计算加权偏差值
  * @param  buf : 8路原始ADC值数组
  * @retval int16_t 偏差 (-100 ~ +100)
  *         -100=严重偏左，0=居中，+100=严重偏右
  * @note   算法：sum((i-3.5) * val[i]) / sum(val[i]) * 100
  */
int16_t GRAY_GetDeviation(const uint16_t *buf);

/**
  * @brief  判断是否进入直角弯区域
  * @param  buf : 8路原始ADC值数组
  * @retval uint8_t 0=非直角，1=检测到直角
  * @note   当>=5路同时识别到黑线时判定为直角弯
  */
uint8_t GRAY_IsCorner(const uint16_t *buf);

#endif
