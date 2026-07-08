/**
  *************************************************************
  * @file     cam_comm.h
  * @author   2026电赛 E题 小组
  * @brief    OpenMV视觉通信 模块头文件
  *           STM32与OpenMV通过USART2通信
  *************************************************************
  */
#ifndef __CAM_COMM_H
#define __CAM_COMM_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define CAM_UART_BAUDRATE       115200

#define CAM_FRAME_HEADER        0xAA    /* 查询帧头 */
#define CAM_FRAME_RESP_HEADER   0xBB    /* 应答帧头 */
#define CAM_FRAME_LEN           6       /* 应答帧长度: 0xBB + x + y + w + h + sum */

/**
  * @brief  初始化与OpenMV的通信
  * @param  huart : UART句柄指针（USART2）
  * @retval 无
  */
void CAM_Init(UART_HandleTypeDef *huart);

/**
  * @brief  查询并获取靶心坐标
  * @param  x : 靶心X坐标(归一化0.0~1.0)
  * @param  y : 靶心Y坐标(归一化0.0~1.0)
  * @param  w : 靶心宽度(归一化)
  * @param  h : 靶心高度(归一化)
  * @retval uint8_t 0=未识到目标, 1=已识别
  */
uint8_t CAM_GetTarget(float *x, float *y, float *w, float *h);

#endif
