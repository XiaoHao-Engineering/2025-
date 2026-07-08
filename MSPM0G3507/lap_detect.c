/**
  *************************************************************
  * @file     lap_detect.c
  * @author   2026电赛 E题 小组
  * @brief    圈数检测 模块实现
  *           算法：检测直角(全压黑线防抖) → 累计4直角=1圈 → 停车2s
  *************************************************************
  */

#include "lap_detect.h"
#include "grayscale.h"

/**
  * @brief  初始化圈数检测
  */
void LAP_Init(LapState_t *lap, uint8_t target)
{
    lap->cornerCount     = 0;
    lap->currentLap      = 0;
    lap->targetLaps      = target;
    lap->isPausing       = 0;
    lap->pauseTimer      = 0;
    lap->debounceCnt     = 0;
    lap->lastCornerState = 0;
}

/**
  * @brief  圈数检测更新
  * @retval 0=继续运行, 1=圈数完成, 2=圈间停顿
  */
uint8_t LAP_Update(LapState_t *lap, const uint16_t *grayBuf)
{
    uint8_t isCorner;

    /* 检查是否处于圈间停顿状态 */
    if (lap->isPausing)
    {
        /* 更新停顿计时（调用者需按ms调用或传入dt）*/
        lap->pauseTimer++;
        if (lap->pauseTimer >= LAP_PAUSE_MS / 5)  /* 假设5ms周期 */
        {
            /* 停顿结束 */
            lap->isPausing = 0;
            lap->pauseTimer = 0;
        }
        return 2;  /* 停顿中 */
    }

    /* 检测直角 */
    isCorner = GRAY_IsCorner(grayBuf);

    /* 防抖处理：连续检测到才确认 */
    if (isCorner)
    {
        if (!lap->lastCornerState)
        {
            lap->debounceCnt = 0;
        }
        lap->debounceCnt++;

        if (lap->debounceCnt >= LAP_CORNER_DEBOUNCE && lap->lastCornerState == 0)
        {
            /* 确认检测到一个直角弯 */
            lap->lastCornerState = 1;
            lap->cornerCount++;

            /* 检查是否完成一圈 */
            if (lap->cornerCount >= LAP_CORNERS_PER_LAP)
            {
                lap->cornerCount = 0;
                lap->currentLap++;

                /* 进入圈间停顿 */
                lap->isPausing  = 1;
                lap->pauseTimer = 0;

                /* 检查是否完成所有圈数 */
                if (lap->currentLap >= lap->targetLaps)
                {
                    return 1;  /* 全部完成 */
                }
                return 2;  /* 圈间停顿 */
            }
        }
    }
    else
    {
        lap->debounceCnt = 0;
        lap->lastCornerState = 0;
    }

    return 0;  /* 继续运行 */
}

/**
  * @brief  获取圈数检测状态
  */
uint8_t LAP_GetStatus(const LapState_t *lap)
{
    if (lap->currentLap >= lap->targetLaps)
        return 1;  /* 已完成 */
    if (lap->isPausing)
        return 2;  /* 停顿中 */
    return 0;      /* 运行中 */
}
