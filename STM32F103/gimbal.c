/**
  *************************************************************
  * @file     gimbal.c
  * @author   2026电赛 E题 小组
  * @brief    二维云台控制 模块实现
  *************************************************************
  */

#include "gimbal.h"

/**
  * @brief  初始化云台
  */
void GIMBAL_Init(Gimbal_t *gimbal)
{
    STEP_Init(&gimbal->pan,
              STEP_PULSE_PORT, STEP_PULSE_PAN_PIN,
              STEP_PULSE_PORT, STEP_DIR_PAN_PIN);
    STEP_Init(&gimbal->tilt,
              STEP_PULSE_PORT, STEP_PULSE_TILT_PIN,
              STEP_PULSE_PORT, STEP_DIR_TILT_PIN);

    gimbal->panAngle  = 0.0f;
    gimbal->tiltAngle = 0.0f;

    STEP_SetSpeed(&gimbal->pan, 800);
    STEP_SetSpeed(&gimbal->tilt, 800);
}

/**
  * @brief  设定云台角度
  */
void GIMBAL_SetAngles(Gimbal_t *gimbal, float panDeg, float tiltDeg)
{
    /* 限幅 */
    if (panDeg  < GIMBAL_PAN_MIN)  panDeg  = GIMBAL_PAN_MIN;
    if (panDeg  > GIMBAL_PAN_MAX)  panDeg  = GIMBAL_PAN_MAX;
    if (tiltDeg < GIMBAL_TILT_MIN) tiltDeg = GIMBAL_TILT_MIN;
    if (tiltDeg > GIMBAL_TILT_MAX) tiltDeg = GIMBAL_TILT_MAX;

    /* 角度转绝对步数 */
    int32_t panSteps  = (int32_t)(panDeg  * GIMBAL_STEP_PER_DEG);
    int32_t tiltSteps = (int32_t)(tiltDeg * GIMBAL_STEP_PER_DEG);

    STEP_MoveTo(&gimbal->pan, panSteps);
    STEP_MoveTo(&gimbal->tilt, tiltSteps);

    gimbal->panAngle  = panDeg;
    gimbal->tiltAngle = tiltDeg;
}

/**
  * @brief  扫描搜索
  */
void GIMBAL_Sweep(Gimbal_t *gimbal, float panStart, float panEnd,
                  float tiltAngle, float stepDeg)
{
    static float sweepAngle = 0.0f;
    static uint8_t sweepDir = 0;  /* 0=正向, 1=反向 */

    if (!GIMBAL_IsBusy(gimbal))
    {
        if (sweepDir == 0)
        {
            sweepAngle += stepDeg;
            if (sweepAngle >= panEnd) sweepDir = 1;
        }
        else
        {
            sweepAngle -= stepDeg;
            if (sweepAngle <= panStart) sweepDir = 0;
        }

        GIMBAL_SetAngles(gimbal, sweepAngle, tiltAngle);
    }
}

/**
  * @brief  停止云台
  */
void GIMBAL_Stop(Gimbal_t *gimbal)
{
    STEP_Stop(&gimbal->pan);
    STEP_Stop(&gimbal->tilt);
}

/**
  * @brief  获取云台当前是否在运动中
  */
uint8_t GIMBAL_IsBusy(Gimbal_t *gimbal)
{
    return (gimbal->pan.busy || gimbal->tilt.busy) ? 1 : 0;
}
