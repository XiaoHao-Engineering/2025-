/**
  *************************************************************
  * @file     ti_msp_dl_config.c
  * @author   手动编写 (代替SysConfig生成)
  * @brief    MSPM0G3507 外设配置实现
  *           时钟、GPIO引脚复用、ADC/PWM/I2C/UART初始化
  *************************************************************
  */

#include "ti_msp_dl_config.h"

/* IOMUX引脚索引定义 (MSPM0G3507) *******************************/
/* PA0~PA9 = PINCM0~9, PB0~PB6 = PINCM16~22, PC0~PC3 = PINCM32~35 */
#define IOMUX_PCM(_idx)       (IOMUX_PINCM0 + (_idx))

/* 外设时钟使能宏 **********************************************/
#define SYSCTL_POWER_BASE     0x40080000UL

/* 引脚功能配置表 **********************************************/
/* PA0~PA7: ADC0_IN0~7 (灰度传感器) */
/* PB0~PB1: TIMA0_C0~C1 (电机PWM) */
/* PB2~PB6: GPIO (电机方向/STBY) */
/* PC0~PC1: I2C1_SCL/SDA (OLED) */
/* PC2~PC3: GPIO (按键输入) */
/* PA8~PA9: UART1_TX/RX */

/**
  * @brief  系统时钟初始化
  * @note   配置SYSPLL输出40MHz，MCLK=40MHz，ULPCLK=40MHz
  */
void SYSCFG_DL_init(void)
{
    /* ========== 1. 电源配置 ========== */
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);


    /* ========== 2. SYSPLL配置 (40MHz) ========== */
    /* 使用SYSOSC(32MHz)作为时钟源 */
    DL_SYSCTL_enableSYSOSC();

    /* 配置SYSPLL: 32MHz -> VCO = 32*5/4 = 40MHz -> 输出40MHz */
    DL_SYSCTL_configSYSPLL(
        DL_SYSCTL_SYSPLL_SRC_SYSOSC,    /* 时钟源: SYSOSC 32MHz */
        5,                               /* 反馈分频 FBDIV */
        4,                               /* 预分频 PREDIV */
        1                                /* 后分频 POSTDIV */
    );

    /* 等待PLL锁定 */
    while (!DL_SYSCTL_isSYSPLLLocked());

    /* 设置MCLK = SYSPLL (40MHz) */
    DL_SYSCTL_setMCLKSource(DL_SYSCTL_MCLK_SOURCE_SYSPLL);

    /* 设置ULPCLK = MCLK / 1 (40MHz) */
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);

    /* 使能外设时钟 (ADC, TIMER, I2C, UART) */
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_ADC);
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_TIMG0);
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_I2C1);
    DL_SYSCTL_enablePeripheralClock(DL_SYSCTL_PERIPH_UART1);


    /* ========== 3. GPIO引脚复用配置 ========== */

    /* PA0~PA7: ADC0_IN0~IN7 */
    for (uint8_t i = 0; i < 8; i++)
    {
        DL_GPIO_initPeripheralFunction(IOMUX_PCM(i), IOMUX_PINCM_FUNC_ADC);
    }

    /* PB0~PB1: TIMA0_C0~C1 (PWM输出) */
    DL_GPIO_initPeripheralFunction(IOMUX_PCM(16), IOMUX_PINCM_FUNC_TIMER);
    DL_GPIO_initPeripheralFunction(IOMUX_PCM(17), IOMUX_PINCM_FUNC_TIMER);

    /* PB2: AIN1 (GPIO输出) */
    DL_GPIO_initDigitalOutput(IOMUX_PCM(18));
    DL_GPIO_clearPins(GPIOB, GPIO_PIN_2);

    /* PB3: AIN2 (GPIO输出) */
    DL_GPIO_initDigitalOutput(IOMUX_PCM(19));
    DL_GPIO_clearPins(GPIOB, GPIO_PIN_3);

    /* PB4: BIN1 (GPIO输出) */
    DL_GPIO_initDigitalOutput(IOMUX_PCM(20));
    DL_GPIO_clearPins(GPIOB, GPIO_PIN_4);

    /* PB5: BIN2 (GPIO输出) */
    DL_GPIO_initDigitalOutput(IOMUX_PCM(21));
    DL_GPIO_clearPins(GPIOB, GPIO_PIN_5);

    /* PB6: STBY (GPIO输出，高有效) */
    DL_GPIO_initDigitalOutput(IOMUX_PCM(22));
    DL_GPIO_clearPins(GPIOB, GPIO_PIN_6);

    /* PC0~PC1: I2C1_SCL/SDA */
    DL_GPIO_initPeripheralFunction(IOMUX_PCM(32), IOMUX_PINCM_FUNC_I2C);
    DL_GPIO_initPeripheralFunction(IOMUX_PCM(33), IOMUX_PINCM_FUNC_I2C);

    /* PC2~PC3: 按键输入 (上拉) */
    DL_GPIO_initDigitalInput(IOMUX_PCM(34), IOMUX_PINCM_INPUT_ENABLE);
    DL_GPIO_enablePullUp(GPIOC, GPIO_PIN_2);
    DL_GPIO_initDigitalInput(IOMUX_PCM(35), IOMUX_PINCM_INPUT_ENABLE);
    DL_GPIO_enablePullUp(GPIOC, GPIO_PIN_3);

    /* PA8~PA9: UART1_TX/RX */
    DL_GPIO_initPeripheralFunction(IOMUX_PCM(8), IOMUX_PINCM_FUNC_UART);
    DL_GPIO_initPeripheralFunction(IOMUX_PCM(9), IOMUX_PINCM_FUNC_UART);


    /* ========== 4. 外设初始化 ========== */

    /* --- ADC12初始化 (8路灰度, 单次转换, 12位) --- */
    DL_ADC12_setClockConfig(GRAY_ADC_INST,
        DL_ADC12_CLOCK_SRC_BUSCLK, DL_ADC12_CLOCK_DIVIDER_8);
    DL_ADC12_initSingleSample(GRAY_ADC_INST,
        DL_ADC12_SAMP_MODE_SINGLE, DL_ADC12_TRIG_SRC_SOFTWARE,
        DL_ADC12_SAMP_TIME_16);
    /* 配置通道0~7: 12位, VDDA参考 */
    for (uint8_t ch = 0; ch < 8; ch++)
    {
        DL_ADC12_configMem(GRAY_ADC_INST, DL_ADC12_MEM_IDX_0,
            (DL_ADC12_CHANNEL_0 + ch),
            DL_ADC12_REF_VOLTAGE_VDDA,
            DL_ADC12_SAMP_TIME_16);
    }

    /* --- TIMA0初始化 (电机PWM, 10kHz, 边沿对齐) --- */
    /* 时钟=40MHz, 预分频=3 -> 10MHz, 周期=1000 -> 10kHz */
    DL_TimerA_initUpMode(MOTOR_TIM_INST,
        DL_TIMER_CLOCK_DIVIDE_4,           /* 40MHz/4=10MHz */
        999,                                /* 周期=1000 */
        DL_TIMER_CONTROL_MODE_EDGE_ALIGN_WITH_COMPARE_ONLY);
    /* 使能两路PWM输出 */
    DL_TimerA_enableCaptureCompareOutput(MOTOR_TIM_INST, DL_TIMERA_MUX_GENERATOR_0);
    DL_TimerA_setCaptureCompareValue(MOTOR_TIM_INST, 0, DL_TIMERA_MUX_MATCH_0);
    DL_TimerA_enableCaptureCompareOutput(MOTOR_TIM_INST, DL_TIMERA_MUX_GENERATOR_1);
    DL_TimerA_setCaptureCompareValue(MOTOR_TIM_INST, 0, DL_TIMERA_MUX_MATCH_1);

    /* --- I2C1初始化 (OLED, 100kHz) --- */
    DL_I2C_init(OLED_I2C_INST,
        DL_I2C_CONTROLLER_MAX_TIMEOUT_DISABLE,
        DL_I2C_CONTROLLER_CLOCK_100KHZ);

    /* --- UART1初始化 (115200-8N1) --- */
    DL_UART_init(UART_INST,
        DL_UART_MAJORITY_15,
        DL_UART_PARITY_NONE,
        DL_UART_STOP_BITS_ONE);
    DL_UART_setBaudRate(UART_INST, 115200);

    /* 使能定时器计数 */
    DL_TimerA_enableStart(MOTOR_TIM_INST);
}

/**
  * @brief  中断初始化
  * @note   目前未使用中断，保留为空
  */
void SYSCFG_DL_initInterrupts(void)
{
    /* 当前基础要求无需中断，保留占位 */
}
