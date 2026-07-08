/**
  *************************************************************
  * @file     grayscale.c
  * @author   2026电赛 E题 小组
  * @brief    8路灰度传感器 模块实现
  *           基于MSPM0G3507 DriverLib的ADC12模块
  *************************************************************
  */

#include "grayscale.h"
#include "ti_msp_dl_config.h"   /* SysConfig生成的头文件 */

/* ADC12模块句柄（由SysConfig生成，在ti_msp_dl_config.h中声明）*/
extern ADC12_Regs* GRAY_ADC_INST;

/**
  * @brief  初始化8路灰度ADC
  * @note   SysConfig已配置ADC0_IN0~IN7，本函数仅使能ADC模块
  */
void GRAY_Init(void)
{
    /* SysConfig已生成DL_ADC12_init()，直接使能即可 */
    DL_ADC12_enableConversions(GRAY_ADC_INST);
}

/**
  * @brief  读取全部8路灰度值
  * @param  buf : 存放8路原始ADC值的数组首地址
  */
void GRAY_ReadAll(uint16_t *buf)
{
    uint8_t i;
    uint32_t result;

    for (i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        /* 选择ADC通道 */
        DL_ADC12_setChannelMux(GRAY_ADC_INST, DL_ADC12_CHANNEL_0 + i);
        DL_ADC12_startConversion(GRAY_ADC_INST);

        /* 等待转换完成 */
        while (!DL_ADC12_isConversionComplete(GRAY_ADC_INST));

        /* 读取转换结果 */
        result = DL_ADC12_getMemResult(GRAY_ADC_INST, DL_ADC12_MEM_IDX_0);
        buf[i] = (uint16_t)result;
    }
}

/**
  * @brief  计算加权偏差值
  * @param  buf : 8路原始ADC值数组
  * @retval int16_t 偏差 (-100 ~ +100)
  */
int16_t GRAY_GetDeviation(const uint16_t *buf)
{
    uint8_t i;
    float weightedSum = 0.0f;
    float totalValue  = 0.0f;
    float deviation;

    /* 加权偏差计算：中心为索引3.5(介于3和4之间) */
    for (i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        weightedSum += (float)(i - 3.5f) * (float)buf[i];
        totalValue  += (float)buf[i];
    }

    /* 防止除零 */
    if (totalValue < 0.1f)
        return 0;

    deviation = (weightedSum / totalValue) * 100.0f;

    /* 限幅到 -100 ~ +100 */
    if (deviation > 100.0f)  deviation = 100.0f;
    if (deviation < -100.0f) deviation = -100.0f;

    return (int16_t)deviation;
}

/**
  * @brief  判断是否进入直角弯区域
  * @param  buf : 8路原始ADC值数组
  * @retval 0=非直角，1=检测到直角
  */
uint8_t GRAY_IsCorner(const uint16_t *buf)
{
    uint8_t i;
    uint8_t blackCount = 0;

    for (i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        if (buf[i] > GRAY_BLACK_THRESHOLD)
            blackCount++;
    }

    return (blackCount >= GRAY_CORNER_THRESHOLD) ? 1 : 0;
}
