/**
  *************************************************************
  * @file     motor.c
  * @author   2026电赛 E题 小组
  * @brief    TB6612电机驱动 模块实现
  *           PWM通过TIMA0控制，方向通过GPIO控制
  *************************************************************
  */

#include "motor.h"
#include "ti_msp_dl_config.h"

/* 外部引用的TIMA0定时器句柄（由SysConfig生成）*/

/**
  * @brief  初始化TB6612电机驱动
  */
void MOTOR_Init(void)
{
    /* PWM定时器由SysConfig配置，此处使能PWM输出 */
    DL_TimerA_enableCaptureCompareOutput(MOTOR_TIM_INST, DL_TIMERA_CC_0_INDEX);
    DL_TimerA_enableCaptureCompareOutput(MOTOR_TIM_INST, DL_TIMERA_CC_1_INDEX);

    /* 设置STBY为高电平，使能TB6612 */
    DL_GPIO_setPins(MOTOR_STBY_PORT, MOTOR_STBY_PIN);

    /* 初始状态：电机停止 */
    MOTOR_Stop();
}

/**
  * @brief  设置一个电机的方向和PWM
  * @param  speed     : 速度值 (-1000 ~ +1000)
  * @param  in1_port  : IN1引脚PORT
  * @param  in1_pin   : IN1引脚
  * @param  in2_port  : IN2引脚PORT
  * @param  in2_pin   : IN2引脚
  * @param  pwm_inst  : PWM定时器实例
  * @param  gen       : PWM生成器索引
  */
static void MOTOR_SetOneMotor(int16_t speed,
                              GPIO_Regs* in1_port, uint32_t in1_pin,
                              GPIO_Regs* in2_port, uint32_t in2_pin,
                              TIMERA_Regs* pwm_inst, uint32_t gen)
{
    uint16_t pwmValue;

    if (speed > 0)
    {
        /* 正转：IN1=1, IN2=0 */
        DL_GPIO_setPins(in1_port, in1_pin);
        DL_GPIO_clearPins(in2_port, in2_pin);
    }
    else if (speed < 0)
    {
        /* 反转：IN1=0, IN2=1 */
        DL_GPIO_clearPins(in1_port, in1_pin);
        DL_GPIO_setPins(in2_port, in2_pin);
        speed = -speed;  /* 取绝对值 */
    }
    else
    {
        /* 停止：IN1=0, IN2=0 */
        DL_GPIO_clearPins(in1_port, in1_pin);
        DL_GPIO_clearPins(in2_port, in2_pin);
    }

    /* 限幅后设置PWM占空比 */
    if (speed > MOTOR_SPEED_MAX) speed = MOTOR_SPEED_MAX;
    pwmValue = (uint16_t)((uint32_t)speed * MOTOR_PWM_MAX / MOTOR_SPEED_MAX);

    if (gen == DL_TIMERA_MUX_GENERATOR_0)
        DL_TimerA_setCaptureCompareValue(pwm_inst, DL_TIMERA_MUX_MATCH_0, pwmValue);
    else
        DL_TimerA_setCaptureCompareValue(pwm_inst, DL_TIMERA_MUX_MATCH_1, pwmValue);
}

/**
  * @brief  设置左右电机速度
  */
void MOTOR_SetSpeed(int16_t left, int16_t right)
{
    /* 左电机 */
    MOTOR_SetOneMotor(left,
                      MOTOR_AIN1_PORT, MOTOR_AIN1_PIN,
                      MOTOR_AIN2_PORT, MOTOR_AIN2_PIN,
                      MOTOR_TIM_INST, DL_TIMERA_MUX_GENERATOR_0);

    /* 右电机 */
    MOTOR_SetOneMotor(right,
                      MOTOR_BIN1_PORT, MOTOR_BIN1_PIN,
                      MOTOR_BIN2_PORT, MOTOR_BIN2_PIN,
                      MOTOR_TIM_INST, DL_TIMERA_MUX_GENERATOR_1);
}

/**
  * @brief  紧急停止
  */
void MOTOR_Stop(void)
{
    /* 所有IN引脚置低，电机刹车 */
    DL_GPIO_clearPins(MOTOR_AIN1_PORT, MOTOR_AIN1_PIN);
    DL_GPIO_clearPins(MOTOR_AIN2_PORT, MOTOR_AIN2_PIN);
    DL_GPIO_clearPins(MOTOR_BIN1_PORT, MOTOR_BIN1_PIN);
    DL_GPIO_clearPins(MOTOR_BIN2_PORT, MOTOR_BIN2_PIN);

    /* PWM归零 */
    DL_TimerA_setCaptureCompareValue(MOTOR_TIM_INST, DL_TIMERA_MUX_MATCH_0, 0);
    DL_TimerA_setCaptureCompareValue(MOTOR_TIM_INST, DL_TIMERA_MUX_MATCH_1, 0);
}
