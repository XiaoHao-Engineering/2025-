/**
  *************************************************************
  * @file     uart_comm.h
  * @author   2026电赛 E题 小组
  * @brief    UART通信 模块头文件
  *           MSPM0 → STM32 单向命令帧
  *************************************************************
  */
#ifndef __UART_COMM_H
#define __UART_COMM_H

#include <stdint.h>

/* UART引脚宏定义 ********************************************/
#define UART_TX_PORT           GPIOA
#define UART_TX_PIN            GPIO_PIN_8      /* PA8 - TX */
#define UART_RX_PORT           GPIOA
#define UART_RX_PIN            GPIO_PIN_9      /* PA9 - RX */

#define UART_BAUDRATE          115200

/**
  * @brief  命令编码
  */
typedef enum
{
    CMD_LASER_ON   = 0x01,   /* 激光常开（基础1用）*/
    CMD_AIM_START  = 0x02,   /* 启动瞄准（基础2/3用）*/
    CMD_AIM_DONE   = 0x03,   /* 瞄准完成通知 */
    CMD_ACK        = 0xAA    /* 应答帧 */
} CmdCode_t;

/**
  * @brief  初始化UART
  * @note   115200-8N1，SysConfig配置UART1
  * @retval 无
  */
void UART_Init(void);

/**
  * @brief  发送命令帧
  * @param  cmd   : 命令码
  * @param  param : 参数（保留，暂填0x00）
  * @note   帧协议: 0xAA + cmd + param + sum(AA+cmd+param)
  * @retval 无
  */
void UART_SendCmd(CmdCode_t cmd, uint8_t param);

#endif
