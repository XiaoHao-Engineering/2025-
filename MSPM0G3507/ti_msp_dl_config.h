/**
  *************************************************************
  * @file     ti_msp_dl_config.h
  * @author   手动编写 (代替SysConfig生成)
  * @brief    MSPM0G3507 外设配置头文件
  *           定义所有外设实例宏、引脚映射、系统时钟配置
  *************************************************************
  */
#ifndef __TI_MSP_DL_CONFIG_H
#define __TI_MSP_DL_CONFIG_H

#include <ti/devices/msp/m0p/mspm0g3507.h>
#include <ti/driverlib/dl_adc12.h>
#include <ti/driverlib/dl_timer.h>
#include <ti/driverlib/dl_i2c.h>
#include <ti/driverlib/dl_uart.h>
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_sysctl.h>
#include <ti/driverlib/m0p/dl_core.h>

/* 外设实例宏定义 **********************************************/
#define GRAY_ADC_INST       ADC12_0          /* 8路灰度ADC */
#define MOTOR_TIM_INST      TIMA0            /* 电机PWM定时器 */
#define OLED_I2C_INST       I2C1             /* SSD1306 OLED */
#define UART_INST           UART1            /* MSPM0->STM32 */

/* 系统时钟配置 *************************************************/
#define SYS_CLOCK_FREQ      40000000UL       /* 系统时钟40MHz */
#define BUS_CLOCK_FREQ      40000000UL
#define MFCLOCK_FREQ        4000000UL
#define LFCLOCK_FREQ        32768UL

/**
  * @brief  系统初始化（时钟、电源、引脚复用）
  * @note   替代SysConfig的SYSCFG_DL_init()
  *         配置SYSPLL输出40MHz，外设时钟使能
  * @retval 无
  */
void SYSCFG_DL_init(void);

/**
  * @brief  中断初始化（如需要）
  */
void SYSCFG_DL_initInterrupts(void);

#endif
