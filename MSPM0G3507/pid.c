/**
  *************************************************************
  * @file     pid.c
  * @author   2026电赛 E题 小组
  * @brief    PD控制器 模块实现
  *           位置式PD: output = Kp*err + Kd*(err - lastErr)/dt
  *************************************************************
  */

#include "pid.h"

/**
  * @brief  初始化PID控制器
  * @param  pid    : PID控制器结构体指针
  * @param  Kp     : 比例系数
  * @param  Kd     : 微分系数
  * @param  outMax : 输出上限
  */
void PID_Init(PID_t *pid, float Kp, float Kd, float outMax)
{
    pid->Kp       = Kp;
    pid->Kd       = Kd;
    pid->lastError = 0.0f;
    pid->outMax   = outMax;
    pid->outMin   = -outMax;
}

/**
  * @brief  PID计算函数
  * @param  pid : PID控制器结构体指针
  * @param  err : 当前偏差值
  * @param  dt  : 控制周期（秒）
  * @retval float 控制输出值
  */
float PID_Calc(PID_t *pid, float err, float dt)
{
    float output;

    /* P项 */
    float pOut = pid->Kp * err;

    /* D项 */
    float dOut = 0.0f;
    if (dt > 0.0001f)
    {
        dOut = pid->Kd * (err - pid->lastError) / dt;
    }

    /* 保存当前误差供下次微分计算 */
    pid->lastError = err;

    /* 总输出 */
    output = pOut + dOut;

    /* 限幅 */
    if (output > pid->outMax)  output = pid->outMax;
    if (output < pid->outMin)  output = pid->outMin;

    return output;
}

/**
  * @brief  重置PID内部状态
  * @param  pid : PID控制器结构体指针
  */
void PID_Reset(PID_t *pid)
{
    pid->lastError = 0.0f;
}
