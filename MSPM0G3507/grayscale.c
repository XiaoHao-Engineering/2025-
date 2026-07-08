/**
  *****************************************************************
  * @file     grayscale.c
  * @author   2026电赛 E题 小组
  * @brief    8路灰度传感器 模块实现
  *           SDK 2.10 正确API: configConversionMem + startConversion + getMemResult
  *****************************************************************
  */

#include \"grayscale.h\"
#include \"ti_msp_dl_config.h\"

void GRAY_Init(void)
{
    /* ADC配置已在 ti_msp_dl_config.c 中完成 */
    DL_ADC12_clearInterruptStatus(GRAY_ADC_INST, DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
}

void GRAY_ReadAll(uint16_t *buf)
{
    for (uint8_t i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        /* 重新配置MEM0切换通道 */
        DL_ADC12_configConversionMem(GRAY_ADC_INST, GRAY_ADCMEM_0,
            DL_ADC12_INPUT_CHAN_0 + i,
            DL_ADC12_REFERENCE_VOLTAGE_VDDA,
            DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0,
            DL_ADC12_AVERAGING_MODE_DISABLED,
            DL_ADC12_BURN_OUT_SOURCE_DISABLED,
            DL_ADC12_TRIGGER_MODE_AUTO_NEXT,
            DL_ADC12_WINDOWS_COMP_MODE_DISABLED);

        /* 启动转换 */
        DL_ADC12_startConversion(GRAY_ADC_INST);

        /* 等待转换完成（轮询中断标志）*/
        while (DL_ADC12_getPendingInterrupt(GRAY_ADC_INST) != DL_ADC12_IIDX_MEM0_RESULT_LOADED);

        /* 读取结果 */
        buf[i] = (uint16_t)DL_ADC12_getMemResult(GRAY_ADC_INST, GRAY_ADCMEM_0);

        /* 清除中断标志 */
        DL_ADC12_clearInterruptStatus(GRAY_ADC_INST, DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
    }
}

int16_t GRAY_GetDeviation(const uint16_t *buf)
{
    uint8_t i;
    float weightedSum = 0.0f;
    float totalValue  = 0.0f;

    for (i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        weightedSum += (float)(i - 3.5f) * (float)buf[i];
        totalValue  += (float)buf[i];
    }

    if (totalValue < 0.1f) return 0;

    float deviation = (weightedSum / totalValue) * 100.0f;
    if (deviation > 100.0f)  deviation = 100.0f;
    if (deviation < -100.0f) deviation = -100.0f;

    return (int16_t)deviation;
}

uint8_t GRAY_IsCorner(const uint16_t *buf)
{
    uint8_t blackCount = 0;
    for (uint8_t i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        if (buf[i] > GRAY_BLACK_THRESHOLD)
            blackCount++;
    }
    return (blackCount >= GRAY_CORNER_THRESHOLD) ? 1 : 0;
}
