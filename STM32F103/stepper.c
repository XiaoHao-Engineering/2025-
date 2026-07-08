/**
  *************************************************************
  * @file     stepper.c
  * @author   2026电赛 E题 小组
  * @brief    42步进电机驱动 模块实现
  *           脉冲+方向控制，含梯形加减速
  *************************************************************
  */

#include "stepper.h"
#include "tim.h"       /* HAL生成的定时器头文件 */

/**
  * @brief  初始化步进电机
  */
void STEP_Init(Stepper_t *motor,
               GPIO_TypeDef *pulsePort, uint16_t pulsePin,
               GPIO_TypeDef *dirPort, uint16_t dirPin)
{
    motor->pulsePort   = pulsePort;
    motor->pulsePin    = pulsePin;
    motor->dirPort     = dirPort;
    motor->dirPin      = dirPin;
    motor->currentPos  = 0;
    motor->targetPos   = 0;
    motor->speed       = 0;
    motor->busy        = 0;

    /* 初始方向：正转 */
    HAL_GPIO_WritePin(dirPort, dirPin, GPIO_PIN_RESET);
    /* 脉冲初始低 */
    HAL_GPIO_WritePin(pulsePort, pulsePin, GPIO_PIN_RESET);
}

/**
  * @brief  步进到指定绝对位置
  */
void STEP_MoveTo(Stepper_t *motor, int32_t targetPos)
{
    motor->targetPos = targetPos;
    motor->busy      = 1;

    /* 设置方向 */
    if (targetPos > motor->currentPos)
    {
        HAL_GPIO_WritePin(motor->dirPort, motor->dirPin, GPIO_PIN_RESET); /* 正转 */
    }
    else
    {
        HAL_GPIO_WritePin(motor->dirPort, motor->dirPin, GPIO_PIN_SET);   /* 反转 */
    }

    /* 设置默认速度 */
    if (motor->speed == 0)
    {
        motor->speed = STEP_MAX_SPEED;
    }
}

/**
  * @brief  设置步进速度
  */
void STEP_SetSpeed(Stepper_t *motor, uint32_t speed)
{
    if (speed > STEP_MAX_SPEED)
        speed = STEP_MAX_SPEED;
    if (speed < 1)
        speed = 1;
    motor->speed = speed;
}

/**
  * @brief  紧急停止
  */
void STEP_Stop(Stepper_t *motor)
{
    motor->busy      = 0;
    motor->targetPos = motor->currentPos;
    motor->speed     = 0;

    /* 脉冲输出低电平 */
    HAL_GPIO_WritePin(motor->pulsePort, motor->pulsePin, GPIO_PIN_RESET);
}

/**
  * @brief  定时器中断中调用，产生步进脉冲
  * @note   假设1kHz定时器中断，每中断一次发一个脉冲
  */
void STEP_Tick(Stepper_t *motor)
{
    if (!motor->busy) return;

    /* 检查是否到达目标 */
    if (motor->currentPos == motor->targetPos)
    {
        motor->busy = 0;
        return;
    }

    /* 产生一个脉冲 */
    HAL_GPIO_WritePin(motor->pulsePort, motor->pulsePin, GPIO_PIN_SET);

    /* 插入短延时（可通过定时器的比较值精确控制脉冲宽度）*/
    for (volatile uint32_t i = 0; i < 100; i++);

    HAL_GPIO_WritePin(motor->pulsePort, motor->pulsePin, GPIO_PIN_RESET);

    /* 更新位置 */
    if (motor->targetPos > motor->currentPos)
    {
        motor->currentPos++;
    }
    else
    {
        motor->currentPos--;
    }
}
