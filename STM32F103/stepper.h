/**
  *************************************************************
  * @file     stepper.h
  * @author   2026电赛 E题 小组
  * @brief    42步进电机驱动 模块头文件
  *           基于TIM PWM脉冲+方向GPIO控制
  *************************************************************
  */
#ifndef __STEPPER_H
#define __STEPPER_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

/* 步进电机配置 **********************************************/
#define STEP_PULSE_PORT         GPIOA
#define STEP_PULSE_PAN_PIN      GPIO_PIN_0      /* PA0 - 水平步进脉冲 */
#define STEP_DIR_PAN_PIN        GPIO_PIN_1      /* PA1 - 水平步进方向 */
#define STEP_PULSE_TILT_PIN     GPIO_PIN_2      /* PA2 - 俯仰步进脉冲 */
#define STEP_DIR_TILT_PIN       GPIO_PIN_3      /* PA3 - 俯仰步进方向 */

#define STEP_PULSE_WIDTH_US     10              /* 脉冲宽度(us) */
#define STEP_MAX_SPEED          2000            /* 最大速度(步/s) */
#define STEP_ACCEL_STEPS        200             /* 加减速步数 */

/**
  * @brief  步进电机句柄
  */
typedef struct
{
    GPIO_TypeDef  *pulsePort;     /* 脉冲引脚PORT */
    uint16_t       pulsePin;      /* 脉冲引脚 */
    GPIO_TypeDef  *dirPort;       /* 方向引脚PORT */
    uint16_t       dirPin;        /* 方向引脚 */
    int32_t        currentPos;    /* 当前位置(步数) */
    int32_t        targetPos;     /* 目标位置(步数) */
    uint32_t       speed;         /* 当前速度(步/s) */
    uint8_t        busy;          /* 正在运动中标志 */
} Stepper_t;

/**
  * @brief  初始化步进电机
  * @param  motor   : 步进电机句柄指针
  * @param  pulsePort : 脉冲引脚PORT
  * @param  pulsePin  : 脉冲引脚
  * @param  dirPort   : 方向引脚PORT
  * @param  dirPin    : 方向引脚
  * @retval 无
  */
void STEP_Init(Stepper_t *motor,
               GPIO_TypeDef *pulsePort, uint16_t pulsePin,
               GPIO_TypeDef *dirPort, uint16_t dirPin);

/**
  * @brief  步进到指定绝对位置
  * @param  motor    : 步进电机句柄指针
  * @param  targetPos: 目标位置(步数)
  * @retval 无
  */
void STEP_MoveTo(Stepper_t *motor, int32_t targetPos);

/**
  * @brief  设置步进速度
  * @param  motor : 步进电机句柄指针
  * @param  speed : 速度(步/s)
  * @retval 无
  */
void STEP_SetSpeed(Stepper_t *motor, uint32_t speed);

/**
  * @brief  紧急停止
  * @param  motor : 步进电机句柄指针
  * @retval 无
  */
void STEP_Stop(Stepper_t *motor);

/**
  * @brief  定时器中断中调用(用于产生步进脉冲)
  * @param  motor : 步进电机句柄指针
  * @retval 无
  */
void STEP_Tick(Stepper_t *motor);

#endif
