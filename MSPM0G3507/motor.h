/**
  *************************************************************
  * @file     motor.h
  * @author   2026电赛 E题 小组
  * @brief    TB6612电机驱动 模块头文件
  *           双路直流电机PWM+方向控制
  *************************************************************
  */
#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

/* TB6612引脚宏定义（根据实际接线修改）***********************/
#define MOTOR_PWMA_PORT      GPIOB
#define MOTOR_PWMA_PIN       GPIO_PIN_0      /* PB0 - 左电机PWM */
#define MOTOR_PWMB_PORT      GPIOB
#define MOTOR_PWMB_PIN       GPIO_PIN_1      /* PB1 - 右电机PWM */

#define MOTOR_AIN1_PORT      GPIOB
#define MOTOR_AIN1_PIN       GPIO_PIN_2      /* PB2 */
#define MOTOR_AIN2_PORT      GPIOB
#define MOTOR_AIN2_PIN       GPIO_PIN_3      /* PB3 */

#define MOTOR_BIN1_PORT      GPIOB
#define MOTOR_BIN1_PIN       GPIO_PIN_4      /* PB4 */
#define MOTOR_BIN2_PORT      GPIOB
#define MOTOR_BIN2_PIN       GPIO_PIN_5      /* PB5 */

#define MOTOR_STBY_PORT      GPIOB
#define MOTOR_STBY_PIN       GPIO_PIN_6      /* PB6 - 待机控制 */

#define MOTOR_PWM_MAX        999             /* PWM周期计数值 (1000-1) */
#define MOTOR_SPEED_MAX      1000            /* 最大速度值 */

/**
  * @brief  初始化TB6612电机驱动
  * @note   配置PWM定时器和方向控制引脚，拉高STBY使能
  * @retval 无
  */
void MOTOR_Init(void);

/**
  * @brief  设置左右电机速度
  * @param  left  : 左轮速度 (-1000 ~ +1000)
  *         >0 正转前进，<0 反转后退，0 停止
  * @param  right : 右轮速度 (-1000 ~ +1000)
  * @retval 无
  */
void MOTOR_SetSpeed(int16_t left, int16_t right);

/**
  * @brief  紧急停止
  * @note   电机刹车（所有控制引脚置低）
  * @retval 无
  */
void MOTOR_Stop(void);

#endif
