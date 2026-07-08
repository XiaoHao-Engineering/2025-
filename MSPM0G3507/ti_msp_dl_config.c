/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_ADC_init();
    SYSCFG_DL_TIMER_init();
    SYSCFG_DL_I2C_init();
    SYSCFG_DL_UART_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    /* PA0-PA7: ADC0_IN0~7 (grayscale) */
    for (uint8_t i = 0; i < 8; i++) {
        DL_GPIO_initPeripheralFunction(IOMUX_PINCM0 + i, IOMUX_PINCM_FUNC_ADC);
    }
    /* PB0-PB1: TIMA0_C0~C1 (motor PWM) */
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM16, IOMUX_PINCM_FUNC_TIMER);
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM17, IOMUX_PINCM_FUNC_TIMER);
    /* PB2-PB6: motor direction/STBY (GPIO output) */
    DL_GPIO_initDigitalOutput(IOMUX_PINCM18);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM19);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM20);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM21);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM22);
    /* PC0-PC1: I2C1_SCL/SDA (OLED) */
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM32, IOMUX_PINCM_FUNC_I2C);
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM33, IOMUX_PINCM_FUNC_I2C);
    /* PC2-PC3: button input with pull-up */
    DL_GPIO_initDigitalInput(IOMUX_PINCM34, IOMUX_PINCM_INPUT_ENABLE);
    DL_GPIO_enablePullUp(GPIOC, GPIO_PIN_2);
    DL_GPIO_initDigitalInput(IOMUX_PINCM35, IOMUX_PINCM_INPUT_ENABLE);
    DL_GPIO_enablePullUp(GPIOC, GPIO_PIN_3);
    /* PA8-PA9: UART1_TX/RX */
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM8, IOMUX_PINCM_FUNC_UART);
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM9, IOMUX_PINCM_FUNC_UART);
}


SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_disableHFXT();
    /* Configure SYSPLL: SYSOSC(32MHz) * 5 / 4 = 40MHz */
    DL_SYSCTL_enableSYSOSC();
    DL_SYSCTL_configSYSPLL(DL_SYSCTL_SYSPLL_SRC_SYSOSC, 5, 4, 1);
    while (!DL_SYSCTL_isSYSPLLLocked());
    DL_SYSCTL_setMCLKSource(DL_SYSCTL_MCLK_SOURCE_SYSPLL);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    /* Enable peripheral clocks */
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_ADC);
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_TIMG0);
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_I2C1);
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_UART1);
}


SYSCONFIG_WEAK void SYSCFG_DL_ADC_init(void)
{
    DL_ADC12_setClockConfig(GRAY_ADC_INST,
        DL_ADC12_CLOCK_SRC_BUSCLK, DL_ADC12_CLOCK_DIVIDER_8);
    DL_ADC12_initSingleSample(GRAY_ADC_INST,
        DL_ADC12_SAMP_MODE_SINGLE, DL_ADC12_TRIG_SRC_SOFTWARE,
        DL_ADC12_SAMP_TIME_16);
    for (uint8_t ch = 0; ch < 8; ch++)
    {
        DL_ADC12_configMem(GRAY_ADC_INST, DL_ADC12_MEM_IDX_0,
            (DL_ADC12_CHANNEL_0 + ch),
            DL_ADC12_REF_VOLTAGE_VDDA,
            DL_ADC12_SAMP_TIME_16);
    }
}

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_init(void)
{
    DL_TimerA_initUpMode(MOTOR_TIM_INST,
        DL_TIMER_CLOCK_DIVIDE_4, 999,
        DL_TIMER_CONTROL_MODE_EDGE_ALIGN_WITH_COMPARE_ONLY);
    DL_TimerA_enableCaptureCompareOutput(MOTOR_TIM_INST, DL_TIMERA_MUX_GENERATOR_0);
    DL_TimerA_setCaptureCompareValue(MOTOR_TIM_INST, 0, DL_TIMERA_MUX_MATCH_0);
    DL_TimerA_enableCaptureCompareOutput(MOTOR_TIM_INST, DL_TIMERA_MUX_GENERATOR_1);
    DL_TimerA_setCaptureCompareValue(MOTOR_TIM_INST, 0, DL_TIMERA_MUX_MATCH_1);
    DL_TimerA_enableStart(MOTOR_TIM_INST);
}

SYSCONFIG_WEAK void SYSCFG_DL_I2C_init(void)
{
    DL_I2C_init(OLED_I2C_INST,
        DL_I2C_CONTROLLER_MAX_TIMEOUT_DISABLE,
        DL_I2C_CONTROLLER_CLOCK_100KHZ);
}

SYSCONFIG_WEAK void SYSCFG_DL_UART_init(void)
{
    DL_UART_init(UART_INST,
        DL_UART_MAJORITY_15, DL_UART_PARITY_NONE, DL_UART_STOP_BITS_ONE);
    DL_UART_setBaudRate(UART_INST, 115200);
    DL_UART_enable(UART_INST);
}
