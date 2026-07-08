/**
  *************************************************************
  * @file     main.c
  * @author   2026电赛 E题 小组
  * @brief    STM32F103C8T6 主控程序
  *           云台瞄准系统：接收MSPM0指令 → 驱动云台瞄准 → 激光控制
  *************************************************************
  */

#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "stepper.h"
#include "gimbal.h"
#include "cam_comm.h"
#include "laser.h"
#include "geometry.h"

/* 状态枚举 **************************************************/
typedef enum
{
    GIMBAL_IDLE,        /* 空闲，等待MSPM0指令 */
    GIMBAL_SCAN,        /* 大范围扫描搜索靶心 */
    GIMBAL_AIMING,      /* 精细闭环瞄准 */
    GIMBAL_LOCKED       /* 瞄准锁定 */
} GimbalState_t;

/* 全局变量 **************************************************/
static GimbalState_t g_state = GIMBAL_IDLE;
static Gimbal_t      g_gimbal;
static GeoParams_t   g_geoParams;

/* 瞄准参数 **************************************************/
#define SCAN_TIMEOUT_MS      8000    /* 扫描超时(ms) */
#define AIM_PRECISION         0.02f  /* 瞄准精度(归一化坐标) */
#define AIM_CENTER_TIMEOUT_MS 25000  /* 基础2超时25s */
#define AIM_ARBITRARY_TIMEOUT_MS 45000 /* 基础3超时45s */

static uint32_t g_aimTimeout = 0;    /* 瞄准超时计数 */

/* USART句柄声明（由HAL生成在usart.h中）*/
extern UART_HandleTypeDef huart1;   /* MSPM0通信 */
extern UART_HandleTypeDef huart2;   /* OpenMV通信 */

/**
  * @brief  接收MSPM0命令
  * @retval 命令码(0=无命令)
  */
static uint8_t RecvMSPM0Cmd(void)
{
    uint8_t buf[4];
    uint8_t sum;

    if (HAL_UART_Receive(&huart1, buf, 4, 10) == HAL_OK)
    {
        if (buf[0] == 0xAA)
        {
            sum = buf[0] + buf[1] + buf[2];
            if (sum == buf[3])
            {
                return buf[1];  /* 返回命令码 */
            }
        }
    }
    return 0;
}

/**
  * @brief  初始化系统
  */
static void System_Init(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();

    /* 模块初始化 */
    LASER_Init();
    GIMBAL_Init(&g_gimbal);
    CAM_Init(&huart2);

    /* 几何参数初始化（需根据实际安装标定）*/
    g_geoParams.fx          = 320.0f;
    g_geoParams.fy          = 240.0f;
    g_geoParams.cx          = 160.0f;
    g_geoParams.cy          = 120.0f;
    g_geoParams.camHeight   = 150.0f;   /* 摄像头高度~15cm */
    g_geoParams.laserOffset = 20.0f;    /* 激光与摄像头偏移~2cm */
    g_geoParams.targetDist  = 500.0f;   /* 靶纸距离~50cm */
    GEO_Init(&g_geoParams);

    g_state = GIMBAL_IDLE;
}

/**
  * @brief  空闲状态：等待MSPM0指令
  */
static void State_Idle(void)
{
    uint8_t cmd = RecvMSPM0Cmd();

    if (cmd == 0x01)  /* CMD_LASER_ON */
    {
        LASER_On();
    }
    else if (cmd == 0x02)  /* CMD_AIM_START */
    {
        LASER_On();
        g_state = GIMBAL_SCAN;
        g_aimTimeout = 0;
    }
}

/**
  * @brief  扫描状态：大范围扫描搜索靶心
  */
static void State_Scan(void)
{
    float tx, ty, tw, th;
    float pan, tilt;

    /* 云台扫描运动 */
    GIMBAL_Sweep(&g_gimbal, -60.0f, 60.0f, 15.0f, 2.0f);

    /* 查询OpenMV是否识别到靶心 */
    if (CAM_GetTarget(&tx, &ty, &tw, &th))
    {
        /* 找到目标，进入瞄准状态 */
        g_state = GIMBAL_AIMING;
    }

    /* 超时处理 */
    g_aimTimeout += 10;  /* 假设10ms周期 */
    if (g_aimTimeout > SCAN_TIMEOUT_MS)
    {
        /* 扫描超时，回到空闲 */
        g_state = GIMBAL_IDLE;
        g_aimTimeout = 0;
    }
}

/**
  * @brief  瞄准状态：闭环精细瞄准
  */
static void State_Aiming(void)
{
    float tx, ty, tw, th;
    float pan, tilt;

    /* 获取当前靶心坐标 */
    if (CAM_GetTarget(&tx, &ty, &tw, &th))
    {
        /* 像素坐标转云台角度 */
        GEO_PixelToAngle(tx, ty, &pan, &tilt);

        /* 驱动云台 */
        GIMBAL_SetAngles(&g_gimbal, pan, tilt);

        /* 检查是否已瞄准到靶心（误差小于精度阈值）*/
        float errX = tx - 0.5f;  /* 假设靶心在图像中央 */
        float errY = ty - 0.5f;
        float dist = errX * errX + errY * errY;

        if (dist < AIM_PRECISION * AIM_PRECISION)
        {
            g_state = GIMBAL_LOCKED;
        }
    }

    g_aimTimeout += 10;
    if (g_aimTimeout > AIM_CENTER_TIMEOUT_MS)
    {
        /* 超时，停止瞄准 */
        g_state = GIMBAL_IDLE;
        g_aimTimeout = 0;
    }
}

/**
  * @brief  锁定状态：瞄准完成，保持激光
  */
static void State_Locked(void)
{
    LASER_On();
    /* 保持云台不动，激光持续照射 */
}

/**
  * @brief  主函数
  */
int main(void)
{
    System_Init();

    while (1)
    {
        switch (g_state)
        {
            case GIMBAL_IDLE:
                State_Idle();
                break;

            case GIMBAL_SCAN:
                State_Scan();
                break;

            case GIMBAL_AIMING:
                State_Aiming();
                break;

            case GIMBAL_LOCKED:
                State_Locked();
                break;
        }

        HAL_Delay(10);  /* 10ms控制周期 */
    }
}

/**
  * @brief  系统滴答定时器回调（HAL库用）
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}
