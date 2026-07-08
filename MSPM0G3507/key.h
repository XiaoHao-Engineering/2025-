/**
  *************************************************************
  * @file     key.h
  * @author   2026电赛 E题 小组
  * @brief    按键输入 模块头文件
  *           两按键：增加(N+) / 减少(N-)，带软件去抖
  *************************************************************
  */
#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

/* 按键引脚宏定义 ********************************************/
#define KEY_INC_PORT          GPIOC
#define KEY_INC_PIN           GPIO_PIN_2      /* PC2 - 增加键 */
#define KEY_DEC_PORT          GPIOC
#define KEY_DEC_PIN           GPIO_PIN_3      /* PC3 - 减少键 */

#define KEY_DEBOUNCE_MS       10              /* 消抖时间(ms) */
#define KEY_LONG_PRESS_MS     500             /* 长按判定时间(ms) */

/**
  * @brief  按键编码
  */
typedef enum
{
    KEY_NONE = 0,   /* 无按键 */
    KEY_INC  = 1,   /* 增加键 */
    KEY_DEC  = 2    /* 减少键 */
} KeyCode_t;

/**
  * @brief  初始化按键GPIO
  * @note   配置为输入，内部上拉
  * @retval 无
  */
void KEY_Init(void);

/**
  * @brief  扫描按键
  * @note   每次主循环调用，含10ms去抖逻辑
  * @retval KeyCode_t KEY_NONE / KEY_INC / KEY_DEC
  */
KeyCode_t KEY_Scan(void);

#endif
