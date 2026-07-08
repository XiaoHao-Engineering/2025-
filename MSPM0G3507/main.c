/**
  *************************************************************
  * @file     main.c
  * @author   2026电赛 E题 小组
  * @brief    主状态机
  *           基础要求1: 循迹跑N圈
  *           基础要求2: 中心静止瞄准
  *           基础要求3: 定点任意角度瞄准
  *
  * @note     控制周期5ms (200Hz)，由TIMB定时器中断驱动
  *************************************************************
  */

#include "ti_msp_dl_config.h"
#include "grayscale.h"
#include "pid.h"
#include "motor.h"
#include "lap_detect.h"
#include "key.h"
#include "oled.h"
#include "uart_comm.h"

/* 系统状态枚举 **********************************************/
typedef enum
{
    STATE_SETUP,            /* 上电初始化，等待圈数设定 */
    STATE_LINE_RUN,         /* 基础1：循迹跑圈 */
    STATE_LAP_PAUSE,        /* 每圈结束停顿2s */
    STATE_AIM_CENTER,       /* 基础2：中心静止瞄准 */
    STATE_AIM_ARBITRARY,    /* 基础3：定点任意角度瞄准 */
    STATE_DONE              /* 全部任务完成 */
} SystemState_t;

/* 全局变量 **************************************************/
static SystemState_t g_state = STATE_SETUP;
static LapState_t    g_lap;
static PID_t         g_pid;
static uint16_t      g_grayBuf[8];
static uint8_t       g_targetLaps = 3;   /* 默认3圈 */
static uint32_t      g_tickCount  = 0;   /* 系统滴答计数(5ms单位) */

/* 基础1控制参数 *********************************************/
#define BASE_SPEED      500     /* 基础速度 (0~1000) */
#define PID_KP          0.8f    /* 比例系数（需实车调试）*/
#define PID_KD          0.3f    /* 微分系数（需实车调试）*/
#define PID_OUT_MAX     400     /* 最大纠偏输出 */
#define CONTROL_PERIOD  0.005f  /* 控制周期5ms */

/**
  * @brief  系统初始化
  */
static void System_Init(void)
{
    /* SysConfig生成的系统初始化（时钟、GPIO等）*/
    SYSCFG_DL_init();

    /* 模块初始化 */
    GRAY_Init();
    MOTOR_Init();
    KEY_Init();
    OLED_Init();
    UART_Init();
    PID_Init(&g_pid, PID_KP, PID_KD, PID_OUT_MAX);
    LAP_Init(&g_lap, g_targetLaps);

    /* 显示初始信息 */
    OLED_ShowLapInfo(g_targetLaps, 0, "SETUP");
}

/**
  * @brief  STATE_SETUP: 等待圈数设定
  */
static void State_Setup(void)
{
    KeyCode_t key;

    key = KEY_Scan();
    if (key == KEY_INC)
    {
        if (g_targetLaps < LAP_MAX_LAPS)
            g_targetLaps++;
    }
    else if (key == KEY_DEC)
    {
        if (g_targetLaps > LAP_MIN_LAPS)
            g_targetLaps--;
    }

    LAP_Init(&g_lap, g_targetLaps);
    OLED_ShowLapInfo(g_targetLaps, 0, "SETUP");

    /* TODO: 添加一个确认键进入运行状态。此处用系统滴答超过3秒后自动进入 */
    if (g_tickCount > 600)  /* 3秒无操作后自动开始 */
    {
        g_state = STATE_LINE_RUN;
        g_tickCount = 0;
        OLED_ShowLapInfo(g_targetLaps, 0, "RUN");

        /* 发送激光常开指令 */
        UART_SendCmd(CMD_LASER_ON, 0x00);
    }
}

/**
  * @brief  STATE_LINE_RUN: 循迹跑圈
  *         控制律: 灰度偏差→PD→PWM，同时检测圈数
  */
static void State_LineRun(void)
{
    int16_t deviation;
    float   correction;
    int16_t leftSpeed, rightSpeed;
    uint8_t lapResult;

    /* 1. 读取灰度值 */
    GRAY_ReadAll(g_grayBuf);

    /* 2. 计算偏差 */
    deviation = GRAY_GetDeviation(g_grayBuf);

    /* 3. PD纠偏 */
    correction = PID_Calc(&g_pid, (float)deviation, CONTROL_PERIOD);

    /* 4. 计算电机速度 */
    leftSpeed  = (int16_t)(BASE_SPEED + correction);
    rightSpeed = (int16_t)(BASE_SPEED - correction);

    /* 5. 驱动电机 */
    MOTOR_SetSpeed(leftSpeed, rightSpeed);

    /* 6. 圈数检测 */
    lapResult = LAP_Update(&g_lap, g_grayBuf);

    if (lapResult == 1)
    {
        /* 全部圈数完成，进入瞄准状态 */
        MOTOR_Stop();
        g_state = STATE_AIM_CENTER;
        OLED_ShowLapInfo(g_targetLaps, g_lap.currentLap, "AIM");

        /* 发送瞄准指令 */
        UART_SendCmd(CMD_AIM_START, 0x00);
    }
    else if (lapResult == 2)
    {
        /* 圈间停顿 */
        MOTOR_Stop();
        g_state = STATE_LAP_PAUSE;
        g_tickCount = 0;
        OLED_ShowLapInfo(g_targetLaps, g_lap.currentLap, "PAUSE");
    }

    /* 更新OLED */
    OLED_ShowLapInfo(g_targetLaps, g_lap.currentLap, "RUN");
}

/**
  * @brief  STATE_LAP_PAUSE: 圈间停顿2s
  */
static void State_LapPause(void)
{
    /* 等待2秒(400个5ms周期) */
    if (g_tickCount >= 400)
    {
        g_state = STATE_LINE_RUN;
        g_tickCount = 0;
        OLED_ShowLapInfo(g_targetLaps, g_lap.currentLap, "RUN");
    }
}

/**
  * @brief  STATE_AIM_CENTER: 中心静止瞄准
  * @note   基础2：赛车停在场地中心，STM32云台自主瞄准
  *         MSPM0只需等待STM32回复完成
  */
static void State_AimCenter(void)
{
    /* 保持停车状态，等待STM32完成瞄准后自动进入下一个状态 */
    /* 实际可通过UART接收STM32完成通知，此处简化：2秒后进入基础3 */
    if (g_tickCount > 400)  /* 2s */
    {
        g_state = STATE_AIM_ARBITRARY;
        g_tickCount = 0;

        /* 再次发送瞄准指令（基础3）*/
        UART_SendCmd(CMD_AIM_START, 0x00);
    }
}

/**
  * @brief  STATE_AIM_ARBITRARY: 定点任意角度瞄准
  * @note   基础3：小车停在赛道任意点位，STM32云台瞄准
  */
static void State_AimArbitrary(void)
{
    /* 保持停车，等待STM32完成 */
    if (g_tickCount > 600)  /* 3s超时后进入完成状态 */
    {
        g_state = STATE_DONE;
        OLED_ShowLapInfo(g_targetLaps, g_lap.currentLap, "DONE");
    }
}

/**
  * @brief  STATE_DONE: 全部完成
  */
static void State_Done(void)
{
    MOTOR_Stop();
    /* 保持停机，不做任何操作 */
}

/**
  * @brief  主函数
  * @note   系统初始化后进入大循环，由TIMB定时器中断同步控制周期
  */
int main(void)
{
    System_Init();

    while (1)
    {
        /* 状态机调度 */
        switch (g_state)
        {
            case STATE_SETUP:
                State_Setup();
                break;

            case STATE_LINE_RUN:
                State_LineRun();
                break;

            case STATE_LAP_PAUSE:
                State_LapPause();
                break;

            case STATE_AIM_CENTER:
                State_AimCenter();
                break;

            case STATE_AIM_ARBITRARY:
                State_AimArbitrary();
                break;

            case STATE_DONE:
                State_Done();
                break;

            default:
                break;
        }

        /* 系统滴答更新（假设主循环每次执行约5ms，
           实际需在定时器中断中递增g_tickCount）*/
        g_tickCount++;

        /* 简单延时模拟控制周期 */
        for (volatile uint32_t i = 0; i < 5000; i++);
    }
}
