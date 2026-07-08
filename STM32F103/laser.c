/**
  *************************************************************
  * @file     laser.c
  * @author   2026电赛 E题 小组
  * @brief    405nm蓝紫激光控制 模块实现
  *************************************************************
  */

#include "laser.h"
#include "gpio.h"       /* HAL生成的GPIO头文件 */

/**
  * @brief  初始化激光IO
  */
void LASER_Init(void)
{
    /* GPIO已在CubeMX中配置为推挽输出 */
    HAL_GPIO_WritePin(LASER_PORT, LASER_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  开启激光
  */
void LASER_On(void)
{
    HAL_GPIO_WritePin(LASER_PORT, LASER_PIN, GPIO_PIN_SET);
}

/**
  * @brief  关闭激光
  */
void LASER_Off(void)
{
    HAL_GPIO_WritePin(LASER_PORT, LASER_PIN, GPIO_PIN_RESET);
}
