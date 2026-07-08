/**
  *************************************************************
  * @file     lap_detect.h
  * @author   2026电赛 E题 小组
  * @brief    圈数检测 模块头文件
  *           基于直角弯计数实现闭环圈数计量
  *************************************************************
  */
#ifndef __LAP_DETECT_H
#define __LAP_DETECT_H

#include <stdint.h>

/* 圈数检测配置 ************************************************/
#define LAP_CORNERS_PER_LAP     4           /* 每圈直角数 */
#define LAP_PAUSE_MS            2000        /* 每圈停顿时间(ms) */
#define LAP_CORNER_DEBOUNCE     3           /* 直角防抖：连续N次检测到才算 */
#define LAP_MIN_LAPS            1           /* 最小圈数 */
#define LAP_MAX_LAPS            5           /* 最大圈数 */

/**
  * @brief  圈数检测状态结构体
  */
typedef struct
{
    uint8_t  cornerCount;       /* 当前圈内已检测到的直角数 (0~3) */
    uint8_t  currentLap;        /* 已完成圈数 */
    uint8_t  targetLaps;        /* 目标圈数 N */
    uint8_t  isPausing;         /* 是否处于每圈停顿中 */
    uint16_t pauseTimer;        /* 停顿计时器(ms) */
    uint8_t  debounceCnt;       /* 直角防抖计数器 */
    uint8_t  lastCornerState;   /* 上一次直角检测状态 */
} LapState_t;

/**
  * @brief  初始化圈数检测
  * @param  lap    : 圈数状态结构体指针
  * @param  target : 目标圈数 N (1~5)
  * @retval 无
  */
void LAP_Init(LapState_t *lap, uint8_t target);

/**
  * @brief  圈数检测更新（每控制周期调用一次）
  * @param  lap     : 圈数状态结构体指针
  * @param  grayBuf : 8路灰度ADC值数组
  * @retval uint8_t 系统指令
  *         0=继续运行, 1=圈数完成(停车), 2=圈间停顿中
  */
uint8_t LAP_Update(LapState_t *lap, const uint16_t *grayBuf);

/**
  * @brief  获取圈数检测状态
  * @param  lap : 圈数状态结构体指针
  * @retval uint8_t 0=运行中, 1=已完成, 2=停顿中
  */
uint8_t LAP_GetStatus(const LapState_t *lap);

#endif
