# 2026电赛 E题 — 基础要求 实现计划

> **For agentic workers:** 嵌入式C固件项目，无自动化测试环境。采用"先写模块骨架 → 填核心逻辑 → 集成"的迭代策略。

**Goal:** 完成基础要求1（循迹跑N圈）+ 基础要求2（中心静止瞄准）+ 基础要求3（定点任意角度瞄准）的全部代码

**Architecture:** 三套独立代码（MSPM0G3507、STM32F103、OpenMV），UART通信连接MSPM0↔STM32，STM32↔OpenMV另一路UART

**Tech Stack:** MSPM0G3507 (TI DriverLib/SysConfig), STM32F103 (HAL/StdPeriph), OpenMV (MicroPython)

---

## 文件清单

### MSPM0G3507/ （8对文件，共15个源文件）
- grayscale.h / grayscale.c  — 8路灰度ADC
- pid.h / pid.c              — PD控制器
- motor.h / motor.c          — TB6612驱动
- lap_detect.h / lap_detect.c — 圈数检测
- key.h / key.c              — 按键
- oled.h / oled.c            — SSD1306 OLED
- uart_comm.h / uart_comm.c  — UART通信
- main.c                     — 主状态机

### STM32F103/ （6对文件，共11个源文件）
- stepper.h / stepper.c      — 42步进电机驱动
- gimbal.h / gimbal.c        — 二维云台控制
- cam_comm.h / cam_comm.c    — OpenMV通信
- laser.h / laser.c          — 激光开关
- geometry.h / geometry.c    — 靶心→云台角度解算
- main.c                     — 主状态机

### OpenMV/ （3个.py文件）
- target_detect.py           — 红色靶心识别
- uart_comm.py               — 串口通信
- main.py                    — 主循环

### 项目根目录
- README.md                  — 完整项目说明

---

## Task 1: MSPM0G3507 - 灰度模块
**文件:** MSPM0G3507/grayscale.h, MSPM0G3507/grayscale.c

**功能:** 初始化8路ADC通道 → 读取灰度值 → 加权偏差 → 直角判断

**接口:**
- void GRAY_Init(void)
- void GRAY_ReadAll(uint16_t *buf)
- int16_t GRAY_GetDeviation(const uint16_t *buf) 范围-100~+100
- uint8_t GRAY_IsCorner(const uint16_t *buf)  >=5路全黑返回1

**偏差算法:** sum((i-3.5) * val[i]) / sum(val[i]) * 100, i=0~7

---

## Task 2: MSPM0G3507 - PID控制器
**文件:** MSPM0G3507/pid.h, MSPM0G3507/pid.c

**功能:** 位置式PD控制器

**接口:**
- PID_t结构体: Kp, Kd, lastError, outMax, outMin
- void PID_Init(PID_t *pid, float Kp, float Kd, float outMax)
- float PID_Calc(PID_t *pid, float err, float dt)
- void PID_Reset(PID_t *pid)

---

## Task 3: MSPM0G3507 - 电机驱动
**文件:** MSPM0G3507/motor.h, MSPM0G3507/motor.c

**功能:** TB6612两路PWM+方向控制

**接口:**
- void MOTOR_Init(void)
- void MOTOR_SetSpeed(int16_t left, int16_t right)  -1000~+1000
- void MOTOR_Stop(void)

**引脚宏定义:** PWMA(PB0), PWMB(PB1), AIN1(PB2), AIN2(PB3), BIN1(PB4), BIN2(PB5), STBY(PB6)

---

## Task 4: MSPM0G3507 - 圈数检测
**文件:** MSPM0G3507/lap_detect.h, MSPM0G3507/lap_detect.c

**功能:** 直角计数→4直角=1圈→停顿2s→恢复

**接口:**
- LapState_t结构体: cornerCount, currentLap, targetLaps, isPausing, pauseTimer
- void LAP_Init(LapState_t *lap, uint8_t target)
- uint8_t LAP_Update(LapState_t *lap, const uint16_t *grayBuf)
- uint8_t LAP_GetStatus(const LapState_t *lap)

---

## Task 5: MSPM0G3507 - 按键输入
**文件:** MSPM0G3507/key.h, MSPM0G3507/key.c

**功能:** 两按键增减N值(1~5)，10ms去抖

**接口:**
- typedef enum { KEY_NONE, KEY_INC, KEY_DEC } KeyCode_t
- void KEY_Init(void)
- KeyCode_t KEY_Scan(void)

---

## Task 6: MSPM0G3507 - OLED显示
**文件:** MSPM0G3507/oled.h, MSPM0G3507/oled.c

**功能:** I2C驱动SSD1306 (128x64)，显示圈数/状态

**接口:**
- void OLED_Init(void), OLED_Clear(void)
- void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
- void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
- void OLED_ShowLapInfo(uint8_t target, uint8_t current, const char *state)

---

## Task 7: MSPM0G3507 - UART通信
**文件:** MSPM0G3507/uart_comm.h, MSPM0G3507/uart_comm.c

**功能:** MSPM0→STM32指令，115200 8N1，帧头0xAA+命令+参数+校验

**接口:**
- typedef enum { CMD_LASER_ON=0x01, CMD_AIM_START=0x02, CMD_AIM_DONE=0x03, CMD_ACK=0xAA } CmdCode_t
- void UART_Init(void), void UART_SendCmd(CmdCode_t cmd, uint8_t param)

---

## Task 8: MSPM0G3507 - 主状态机
**文件:** MSPM0G3507/main.c

**功能:** 200Hz主循环状态机

**状态:** STATE_SETUP → STATE_LINE_RUN → STATE_LAP_PAUSE → STATE_AIM_CENTER → STATE_AIM_ARBITRARY → STATE_DONE

---

## Task 9: STM32F103 - 步进电机驱动
**文件:** STM32F103/stepper.h, STM32F103/stepper.c

**功能:** 42步进电机PWM脉冲+方向驱动

**接口:**
- void STEP_Init(TIM_HandleTypeDef *htim, uint32_t ch, GPIO_TypeDef *dirPort, uint16_t dirPin)
- void STEP_SetSpeed(int32_t speed)  步/s
- void STEP_MoveSteps(int32_t steps)
- void STEP_Stop(void)

---

## Task 10: STM32F103 - 云台控制
**文件:** STM32F103/gimbal.h, STM32F103/gimbal.c

**功能:** 二维角度控制+扫描

**接口:**
- void GIMBAL_Init(void)
- void GIMBAL_SetAngles(float panDeg, float tiltDeg)
- void GIMBAL_Sweep(float panStart, float panEnd, float stepDeg)
- void GIMBAL_Stop(void)

---

## Task 11: STM32F103 - OpenMV通信
**文件:** STM32F103/cam_comm.h, STM32F103/cam_comm.c

**功能:** STM32↔OpenMV USART2通信，查询靶心坐标

**接口:**
- void CAM_Init(UART_HandleTypeDef *huart)
- uint8_t CAM_GetTarget(float *x, float *y, float *w, float *h)

**协议:** STM32发0xAA 0x01 → OpenMV回0xBB x y w h sum

---

## Task 12: STM32F103 - 激光控制
**文件:** STM32F103/laser.h, STM32F103/laser.c

**接口:**
- void LASER_Init(void), LASER_On(void), LASER_Off(void)

---

## Task 13: STM32F103 - 几何解算
**文件:** STM32F103/geometry.h, STM32F103/geometry.c

**功能:** 像素坐标→云台角度

**接口:**
- GeoParams_t结构体: fx, fy, cx, cy, camHeight, laserOffset
- void GEO_Init(const GeoParams_t *params)
- void GEO_PixelToAngle(float px, float py, float *pan, float *tilt)

---

## Task 14: STM32F103 - 主状态机
**文件:** STM32F103/main.c

**功能:** 接收MSPM0指令→云台瞄准→激光控制

**状态:** GIMBAL_IDLE → GIMBAL_SCAN → GIMBAL_AIMING → GIMBAL_LOCKED

---

## Task 15: OpenMV - 靶心检测
**文件:** OpenMV/target_detect.py

**功能:** 红色LAB阈值+最大联通域+圆形度过滤

**接口:** def find_target(img) → (cx, cy, w, h)归一化 或 None

---

## Task 16: OpenMV - 串口通信
**文件:** OpenMV/uart_comm.py

**接口:**
- def uart_init(): 返回UART对象
- def send_coords(uart, x, y, w, h)
- def recv_cmd(uart): 返回指令或None

---

## Task 17: OpenMV - 主循环
**文件:** OpenMV/main.py

**功能:** 采集→识别→响应STM32查询

---

## Task 18: README.md
**文件:** README.md

**内容:** 项目概述+硬件架构+接线表+通信协议+各模块说明+IDE使用指南
