/**
  *************************************************************
  * @file     gimbal.h
  * @author   2026电赛 E题 小组
  * @brief    二维云台控制 模块头文件
  *           封装水平和俯仰两个步进电机的协同控制
  *************************************************************
  */
#ifndef __GIMBAL_H
#define __GIMBAL_H

#include <stdint.h>
#include "stepper.h"

/* 云台角度限值（度）*****************************************/
#define GIMBAL_PAN_MIN      -90.0f  /* 水平最小角度 */
#define GIMBAL_PAN_MAX       90.0f  /* 水平最大角度 */
#define GIMBAL_TILT_MIN     -30.0f  /* 俯仰最小角度 */
#define GIMBAL_TILT_MAX      60.0f  /* 俯仰最大角度 */

#define GIMBAL_STEP_PER_DEG 200.0f  /* 每度步数(假设1.8度步进角，无细分) */

/**
  * @brief  云台状态结构体
  */
typedef struct
{
    Stepper_t pan;          /* 水平步进电机 */
    Stepper_t tilt;         /* 俯仰步进电机 */
    float     panAngle;     /* 当前水平角度 */
    float     tiltAngle;    /* 当前俯仰角度 */
} Gimbal_t;

/**
  * @brief  初始化云台
  * @param  gimbal : 云台结构体指针
  * @retval 无
  */
void GIMBAL_Init(Gimbal_t *gimbal);

/**
  * @brief  设定云台角度
  * @param  gimbal   : 云台结构体指针
  * @param  panDeg   : 水平角度（度）
  * @param  tiltDeg  : 俯仰角度（度）
  * @retval 无
  */
void GIMBAL_SetAngles(Gimbal_t *gimbal, float panDeg, float tiltDeg);

/**
  * @brief  扫描搜索
  * @param  gimbal    : 云台结构体指针
  * @param  panStart  : 水平起始角度
  * @param  panEnd    : 水平结束角度
  * @param  tiltAngle : 俯仰固定角度
  * @param  stepDeg   : 每步扫描角度
  * @retval 无
  */
void GIMBAL_Sweep(Gimbal_t *gimbal, float panStart, float panEnd,
                  float tiltAngle, float stepDeg);

/**
  * @brief  停止云台
  * @param  gimbal : 云台结构体指针
  * @retval 无
  */
void GIMBAL_Stop(Gimbal_t *gimbal);

/**
  * @brief  获取云台当前是否在运动中
  * @param  gimbal : 云台结构体指针
  * @retval 0=空闲, 1=运动中
  */
uint8_t GIMBAL_IsBusy(Gimbal_t *gimbal);

#endif
