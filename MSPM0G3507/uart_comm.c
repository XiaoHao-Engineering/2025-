/**
  *************************************************************
  * @file     uart_comm.c
  * @author   2026电赛 E题 小组
  * @brief    UART通信 模块实现
  *           帧协议: 帧头(0xAA) + 命令(1B) + 参数(1B) + 和校验(1B)
  *************************************************************
  */

#include "uart_comm.h"
#include "ti_msp_dl_config.h"

/* 外部引用的UART实例（由SysConfig生成）*/

/**
  * @brief  初始化UART
  */
void UART_Init(void)
{
    /* SysConfig已配置UART1为115200-8N1，此处使能 */
    DL_UART_enable(UART_INST);

    /* 清空接收FIFO */
    
}

/**
  * @brief  发送命令帧
  * @param  cmd   : 命令码
  * @param  param : 参数
  */
void UART_SendCmd(CmdCode_t cmd, uint8_t param)
{
    uint8_t frame[4];
    uint8_t checksum;

    /* 组帧 */
    frame[0] = 0xAA;              /* 帧头 */
    frame[1] = (uint8_t)cmd;      /* 命令 */
    frame[2] = param;             /* 参数 */

    /* 和校验 */
    checksum = frame[0] + frame[1] + frame[2];
    frame[3] = checksum;

    /* 逐字节发送 */
    for (uint8_t i = 0; i < 4; i++)
    {
        /* 等待发送FIFO有空位 */
        while (!DL_UART_isTXFIFOEmpty(UART_INST));
        DL_UART_transmitData(UART_INST, frame[i]);
    }
}
