/**
  *************************************************************
  * @file     pid.h
  * @author   2026电赛 E题 小组
  * @brief    PD控制器 模块头文件
  *           位置式PD控制，用于循迹纠偏
  *************************************************************
  */
#ifndef __PID_H
#define __PID_H

#include <stdint.h>

/**
  * @brief  PID控制器结构体
  */
typedef struct
+{
    float Kp;           /* 比例系数 */
    float Kd;           /* 微分系数 */
    float lastError;    /* 上一次偏差，用于微分计算 */
    float outMax;       /* 输出上限 */
    float outMin;       /* 输出下限 */
} PID_t;

/**
  * @brief  初始化PID控制器
  * @param  pid    : PID控制器结构体指针
  * @param  Kp     : 比例系数
  * @param  Kd     : 微分系数
  * @param  outMax : 输出上限（输出下限自动取负值）
  * @retval 无
  */
void PID_Init(PID_t *pid, float Kp, float Kd, float outMax);

/**
  * @brief  PID计算函数
  * @param  pid : PID控制器结构体指针
  * @param  err : 当前偏差值
  * @param  dt  : 控制周期（秒）
  * @retval float 控制输出值
  */
float PID_Calc(PID_t *pid, float err, float dt);

/**
  * @brief  重置PID内部状态
  * @param  pid : PID控制器结构体指针
  * @retval 无
  */
void PID_Reset(PID_t *pid);

#endif
