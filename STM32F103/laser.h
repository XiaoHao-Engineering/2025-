/**
  *************************************************************
  * @file     laser.h
  * @author   2026电赛 E题 小组
  * @brief    405nm蓝紫激光控制 模块头文件
  *************************************************************
  */
#ifndef __LASER_H
#define __LASER_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

/* 激光引脚 **************************************************/
#define LASER_PORT              GPIOA
#define LASER_PIN               GPIO_PIN_4      /* PA4 */

/**
  * @brief  初始化激光IO
  * @retval 无
  */
void LASER_Init(void);

/**
  * @brief  开启激光
  * @retval 无
  */
void LASER_On(void);

/**
  * @brief  关闭激光
  * @retval 无
  */
void LASER_Off(void);

#endif
