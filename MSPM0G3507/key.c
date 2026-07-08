/**
  *************************************************************
  * @file     key.c
  * @author   2026电赛 E题 小组
  * @brief    按键输入 模块实现
  *           支持短按消抖，通过定时器中断提供时间基准
  *************************************************************
  */

#include "key.h"
#include "ti_msp_dl_config.h"

static uint8_t  key_inc_last   = 1;   /* 上次INC键状态 (1=未按下) */
static uint8_t  key_dec_last   = 1;   /* 上次DEC键状态 */
static uint16_t key_inc_timer  = 0;   /* INC键消抖计时 */
static uint16_t key_dec_timer  = 0;   /* DEC键消抖计时 */

/**
  * @brief  初始化按键GPIO
  */
void KEY_Init(void)
{
    /* SysConfig已配置GPIO引脚为输入上拉模式，此处无需额外操作 */
    key_inc_last = 1;
    key_dec_last = 1;
}

/**
  * @brief  扫描按键
  * @retval KeyCode_t
  */
KeyCode_t KEY_Scan(void)
{
    uint8_t key_inc_now = DL_GPIO_readPins(KEY_INC_PORT, KEY_INC_PIN) ? 0 : 1;
    uint8_t key_dec_now = DL_GPIO_readPins(KEY_DEC_PORT, KEY_DEC_PIN) ? 0 : 1;

    /* INC键去抖处理 */
    if (key_inc_now == key_inc_last)
    {
        if (key_inc_now)
            key_inc_timer++;
        else
            key_inc_timer = 0;
    }
    else
    {
        key_inc_timer = 0;
    }

    /* 确认按下（消抖时间到达）*/
    if (key_inc_now && key_inc_timer == KEY_DEBOUNCE_MS)
    {
        key_inc_last = key_inc_now;
        return KEY_INC;
    }

    /* DEC键去抖处理 */
    if (key_dec_now == key_dec_last)
    {
        if (key_dec_now)
            key_dec_timer++;
        else
            key_dec_timer = 0;
    }
    else
    {
        key_dec_timer = 0;
    }

    /* 确认按下 */
    if (key_dec_now && key_dec_timer == KEY_DEBOUNCE_MS)
    {
        key_dec_last = key_dec_now;
        return KEY_DEC;
    }

    /* 更新上次状态 */
    key_inc_last = key_inc_now;
    key_dec_last = key_dec_now;

    return KEY_NONE;
}
