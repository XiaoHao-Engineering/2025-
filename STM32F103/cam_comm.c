/**
  *************************************************************
  * @file     cam_comm.c
  * @author   2026电赛 E题 小组
  * @brief    OpenMV视觉通信 模块实现
  *           协议：STM32发0xAA 0x01 -> OpenMV回0xBB x y w h sum
  *************************************************************
  */

#include "cam_comm.h"

static UART_HandleTypeDef *g_camUart = NULL;

/**
  * @brief  初始化与OpenMV的通信
  */
void CAM_Init(UART_HandleTypeDef *huart)
{
    g_camUart = huart;
}

/**
  * @brief  查询并获取靶心坐标
  */
uint8_t CAM_GetTarget(float *x, float *y, float *w, float *h)
{
    uint8_t query[2] = {0xAA, 0x01};
    uint8_t resp[CAM_FRAME_LEN];
    uint8_t sum;

    /* 发送查询帧 */
    HAL_UART_Transmit(g_camUart, query, 2, 100);

    /* 接收应答帧 */
    if (HAL_UART_Receive(g_camUart, resp, CAM_FRAME_LEN, 50) != HAL_OK)
    {
        return 0;  /* 超时，未收到 */
    }

    /* 校验帧头 */
    if (resp[0] != CAM_FRAME_RESP_HEADER)
    {
        return 0;
    }

    /* 和校验 */
    sum = 0;
    for (uint8_t i = 0; i < 5; i++)
    {
        sum += resp[i];
    }
    if (sum != resp[5])
    {
        return 0;  /* 校验失败 */
    }

    /* 解析坐标（归一化值，0~255映射到0.0~1.0）*/
    *x = resp[1] / 255.0f;
    *y = resp[2] / 255.0f;
    *w = resp[3] / 255.0f;
    *h = resp[4] / 255.0f;

    /* 如果x和y都为0，视为无目标 */
    if (resp[1] == 0 && resp[2] == 0)
    {
        return 0;
    }

    return 1;  /* 成功获取目标 */
}
