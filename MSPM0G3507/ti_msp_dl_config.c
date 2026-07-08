#include \"ti_msp_dl_config.h\"

SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_ADC12_0_init();
    SYSCFG_DL_TIMER_init();
    SYSCFG_DL_I2C_init();
    SYSCFG_DL_UART_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_ADC12_reset(ADC12_0_INST);
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_ADC12_enablePower(ADC12_0_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    /* PB0-PB1: TIMA0_C0/C1 motor PWM */
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM16, IOMUX_PINCM_FUNC_TIMER);
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM17, IOMUX_PINCM_FUNC_TIMER);
    /* PB2-PB6: motor dir + STBY as GPIO output */
    DL_GPIO_initDigitalOutput(IOMUX_PINCM18);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM19);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM20);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM21);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM22);
    /* PC0-PC1: I2C1 for OLED */
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM32, IOMUX_PINCM_FUNC_I2C);
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM33, IOMUX_PINCM_FUNC_I2C);
    /* PC2-PC3: buttons with pullup */
    DL_GPIO_initDigitalInput(IOMUX_PINCM34, IOMUX_PINCM_INPUT_ENABLE);
    DL_GPIO_enablePullUp(GPIOC, GPIO_PIN_2);
    DL_GPIO_initDigitalInput(IOMUX_PINCM35, IOMUX_PINCM_INPUT_ENABLE);
    DL_GPIO_enablePullUp(GPIOC, GPIO_PIN_3);
    /* PA8-PA9: UART1 for STM32 */
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM8, IOMUX_PINCM_FUNC_UART);
    DL_GPIO_initPeripheralFunction(IOMUX_PINCM9, IOMUX_PINCM_FUNC_UART);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_disableHFXT();
    DL_SYSCTL_disableSYSPLL();
}

/* ADC12_0: single conversion for grayscale sensors */
static const DL_ADC12_ClockConfig gADC12_0ClockConfig = {
    .clockSel    = DL_ADC12_CLOCK_ULPCLK,
    .divideRatio = DL_ADC12_CLOCK_DIVIDE_8,
    .freqRange   = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};

SYSCONFIG_WEAK void SYSCFG_DL_ADC12_0_init(void)
{
    DL_ADC12_setClockConfig(ADC12_0_INST, (DL_ADC12_ClockConfig*)&gADC12_0ClockConfig);
    DL_ADC12_configConversionMem(ADC12_0_INST, GRAY_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA,
        DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT,
        DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setPowerDownMode(ADC12_0_INST, DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC12_0_INST, 500);
    DL_ADC12_clearInterruptStatus(ADC12_0_INST, DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
    DL_ADC12_enableConversions(ADC12_0_INST);
}

/* TIMA0: 10kHz PWM for TB6612 motor drive */
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

/* I2C1: 100kHz for SSD1306 OLED */
SYSCONFIG_WEAK void SYSCFG_DL_I2C_init(void)
{
    DL_I2C_init(OLED_I2C_INST,
        DL_I2C_CONTROLLER_MAX_TIMEOUT_DISABLE,
        DL_I2C_CONTROLLER_CLOCK_100KHZ);
}

/* UART1: 115200-8N1 to STM32 */
SYSCONFIG_WEAK void SYSCFG_DL_UART_init(void)
{
    DL_UART_Config uartCfg = {
        .mode = DL_UART_MODE_NORMAL,
        .direction = DL_UART_DIRECTION_BOTH,
        .flowControl = DL_UART_FLOW_CONTROL_NONE,
        .parity = DL_UART_PARITY_NONE,
        .wordLength = DL_UART_WORD_LENGTH_8,
        .stopBits = DL_UART_STOP_BITS_ONE,
    };
    DL_UART_init(UART_INST, &uartCfg);
    DL_UART_configBaudRate(UART_INST, 115200);
    DL_UART_enable(UART_INST);
}
