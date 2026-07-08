/**
  *************************************************************
  * @file     oled.h
  * @author   2026电赛 E题 小组
  * @brief    SSD1306 OLED显示 模块头文件
  *           I2C接口，128x64像素，显示圈数和状态
  *************************************************************
  */
#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>

/* OLED引脚宏定义 ********************************************/
#define OLED_I2C_INST          I2C1            /* I2C1外设 */
#define OLED_I2C_SDA_PORT      GPIOC
#define OLED_I2C_SDA_PIN       GPIO_PIN_0      /* PC0 - SDA */
#define OLED_I2C_SCL_PORT      GPIOC
#define OLED_I2C_SCL_PIN       GPIO_PIN_1      /* PC1 - SCL */

#define OLED_ADDR              0x78            /* SSD1306 I2C地址(7位0x3C << 1) */
#define OLED_WIDTH              128
#define OLED_HEIGHT             64

/**
  * @brief  初始化OLED显示屏
  * @note   配置I2C并发送初始化序列
  * @retval 无
  */
void OLED_Init(void);

/**
  * @brief  清屏
  * @retval 无
  */
void OLED_Clear(void);

/**
  * @brief  显示字符串
  * @param  x   : 列坐标 (0~127)
  * @param  y   : 行坐标 (0~7，每行8像素)
  * @param  str : 要显示的字符串
  * @retval 无
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);

/**
  * @brief  显示数字
  * @param  x   : 列坐标 (0~127)
  * @param  y   : 行坐标 (0~7)
  * @param  num : 要显示的数字
  * @param  len : 显示位数 (不足前补零)
  * @retval 无
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

/**
  * @brief  显示圈数信息
  * @param  target  : 设定圈数 N
  * @param  current : 当前已完成圈数
  * @param  state   : 运行状态字符串（如"Running" "Pausing" "Done"）
  * @retval 无
  */
void OLED_ShowLapInfo(uint8_t target, uint8_t current, const char *state);

#endif
