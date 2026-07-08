# 2026 全国大学生电子设计竞赛 — 本科E题《简易自行瞄准装置》

## 项目概述

基于 **MSPM0G3507 + STM32F103C8T6 + OpenMV H7Plus** 双主控架构的自动循迹激光瞄准装置。

**关键能力：**
- 八路灰度传感器PID循迹，100cm正方形黑色闭环赛道自动跑N圈（1~5圈）
- 每圈自动识别直角弯（全压黑线检测法）并完成圈数计数
- 圈间2s停顿，跑完N圈后自动返回出发位置
- 搭载42步进电机二维云台 + OpenMV视觉闭环 + 405nm蓝紫激光自动瞄准

**赛题来源：** 2025年全国大学生电子设计竞赛 本科E题《简易自行瞄准装置》

---

## 硬件架构

```
+============================+        UART        +==============================+
|   MSPM0G3507 (底盘主控)     |  <-------------->  |   STM32F103C8T6 (云台主控)   |
|                            |  115200 8N1        |                              |
|   - 8路灰度(ADC0_IN0~7)    |                    |   - 42步进电机 x2 (PWM+DIR)   |
|   - TB6612 电机驱动         |                    |   - 405nm激光 (GPIO)          |
|   - 按键 x2 (PC2, PC3)     |                    |                              |
|   - OLED SSD1306 (I2C1)    |                    |   +------- UART --------+     |
|   - UART1 (PA8, PA9)       |                    |   |  OpenMV H7Plus      |     |
|                            |                    |   |  - 红色靶心视觉识别    |     |
+============================+                    |   |  - 归一化坐标反馈      |     |
                                                  |   +-----------------------+     |
                                                  +==============================+
```

**电源规则：** 两套完全独立电源，分开开关控制（赛题硬性规定）

---

## 接线表

### MSPM0G3507

| 功能 | 引脚 | 外设 | 备注 |
|------|------|------|------|
| 灰度 通道0~7 | PA0 ~ PA7 | ADC0_IN0~7 | 12位ADC，3.3V参考 |
| 左电机PWM | PB0 | TIMA0_C0 | TB6612 PWMA |
| 右电机PWM | PB1 | TIMA0_C1 | TB6612 PWMB |
| 左电机IN1 | PB2 | GPIO | TB6612 AIN1 |
| 左电机IN2 | PB3 | GPIO | TB6612 AIN2 |
| 右电机IN1 | PB4 | GPIO | TB6612 BIN1 |
| 右电机IN2 | PB5 | GPIO | TB6612 BIN2 |
| 电机待机 | PB6 | GPIO | TB6612 STBY(高有效) |
| OLED SCL | PC0 | I2C1_SCL | SSD1306 |
| OLED SDA | PC1 | I2C1_SDA | SSD1306 |
| 按键INC | PC2 | GPIO_输入 | 上拉，按下低电平 |
| 按键DEC | PC3 | GPIO_输入 | 上拉，按下低电平 |
| UART1 TX | PA8 | UART1_TX | → STM32 RX(PA10) |
| UART1 RX | PA9 | UART1_RX | ← STM32 TX(PA9) |

### STM32F103C8T6

| 功能 | 引脚 | 外设 | 备注 |
|------|------|------|------|
| 水平步进PUL | PA0 | TIM2_CH1 | 42步进电机脉冲 |
| 水平步进DIR | PA1 | GPIO | 方向控制 |
| 俯仰步进PUL | PA2 | TIM2_CH2 | 42步进电机脉冲 |
| 俯仰步进DIR | PA3 | GPIO | 方向控制 |
| 激光控制 | PA4 | GPIO | 高电平开启laser |
| USART1 TX | PA9 | USART1_TX | → MSPM0 RX(PA9) |
| USART1 RX | PA10 | USART1_RX | ← MSPM0 TX(PA8) |
| USART2 TX | PA2 | USART2_TX | → OpenMV RX(P5) |
| USART2 RX | PA3 | USART2_RX | ← OpenMV TX(P4) |

### OpenMV H7Plus

| 功能 | 引脚 | 备注 |
|------|------|------|
| UART3 TX | P4 | → STM32 RX(PA3) |
| UART3 RX | P5 | ← STM32 TX(PA2) |

---

## UART 通信协议

### MSPM0 ↔ STM32（UART1, 115200, 8N1）

**帧格式：**

| 帧头(0xAA) | 命令(1B) | 参数(1B) | 和校验(1B) |
|-------------|----------|----------|------------|

校验 = 帧头 + 命令 + 参数

**命令表：**

| 命令码 | 名称 | 触发时机 | 说明 |
|--------|------|----------|------|
| 0x01 | CMD_LASER_ON | 小车开始循迹 | 激光常开，不要求瞄准 |
| 0x02 | CMD_AIM_START | 小车停止后 | 启动云台瞄准闭环 |
| 0x03 | CMD_AIM_DONE | 瞄准完成 | STM32发回确认 |
| 0xAA | CMD_ACK | 任意 | 应答帧 |

### STM32 ↔ OpenMV（USART2, 115200, 8N1）

**STM32 查询帧：** `0xAA 0x01`（2字节）

**OpenMV 应答帧：**

| 帧头(0xBB) | X(1B) | Y(1B) | W(1B) | H(1B) | 校验(1B) |
|-------------|-------|-------|-------|-------|----------|

- X, Y, W, H 为归一化坐标(0~255 映射到 0.0~1.0)
- X=0 && Y=0 表示未检测到目标
- 校验 = 0xBB + X + Y + W + H

---

## 圈数检测算法

```
1. 8路灰度探头正常循迹时，最多3~4路压黑线
2. 进入直角弯区 → ≥5路同时检测到黑线
3. 防抖处理：连续检测到≥3个采样周期才确认
4. 一次直角计数+1
5. 累计4次直角(一圈完整的4个顶点) → 圈数+1
6. 每圈结束停顿2秒
7. 达到目标圈数N → 停车
```

> 起点线检测：赛道为纯闭环黑线，无起跑线标记。圈数完全依赖"4个直角=1圈"算法。

---

## 项目目录结构

```
2026电赛/
├── README.md
├── docs/
│   └── superpowers/
│       ├── specs/
│       │   └── 2026-07-08-line-follower-targeting-design.md
│       └── plans/
│           └── 2026-07-08-base-requirements-plan.md
│
├── MSPM0G3507/              ← CCS 项目（移植到现有CCS工程）
│   ├── main.c               → 主状态机（6种状态流转）
│   ├── grayscale.h/.c       → 8路灰度ADC采集 + 偏差计算 + 直角检测
│   ├── pid.h/.c             → PD控制器
│   ├── motor.h/.c           → TB6612电机驱动
│   ├── lap_detect.h/.c      → 圈数检测（4直角=1圈）
│   ├── key.h/.c             → 按键（圈数设定）
│   ├── oled.h/.c            → SSD1306 OLED显示
│   └── uart_comm.h/.c       → UART通信
│
├── STM32F103/               ← Keil/STM32CubeIDE 项目
│   ├── main.c               → 主状态机（IDLE→SCAN→AIMING→LOCKED）
│   ├── stepper.h/.c         → 42步进电机驱动
│   ├── gimbal.h/.c          → 二维云台控制
│   ├── cam_comm.h/.c        → OpenMV通信
│   ├── laser.h/.c           → 激光控制
│   └── geometry.h/.c        → 像素坐标→云台角度解算
│
└── OpenMV/                  ← OpenMV IDE 项目
    ├── main.py              → 主循环
    ├── target_detect.py     → 红色靶心识别
    └── uart_comm.py         → 串口通信
```

---

## 如何使用

### CCS（MSPM0G3507）

1. 在CCS中新建 MSPM0G3507 项目（使用SysConfig）
2. 在 SysConfig 中配置：
   - ADC0_IN0~7（8路灰度）
   - TIMA0（两路PWM，频率10kHz）
   - I2C1（OLED用）
   - UART1（115200）
   - GPIO引脚（按键、电机方向、STBY）
3. 将 `MSPM0G3507/` 下的 .h/.c 文件复制到CCS项目的 `src/` 目录
4. 在 `ti_msp_dl_config.h` 中补充外设句柄的 extern 声明
5. 修改 SysConfig 生成的引脚分配与本代码宏定义一致
6. 编译下载

### STM32CubeIDE / Keil（STM32F103C8T6）

1. 在 STM32CubeMX 中新建项目，选择 STM32F103C8T6
2. 配置：
   - USART1（115200，MSPM0通信）
   - USART2（115200，OpenMV通信）
   - TIM2（PWM输出，步进电机脉冲）
   - GPIO（激光IO、步进方向）
3. 生成代码后，将 `STM32F103/` 下的 .h/.c 复制到 `Src/ Inc/` 目录
4. 在 `main.h` 中添加 `#include` 引用
5. 编译下载

### OpenMV IDE（OpenMV H7Plus）

1. 打开 OpenMV IDE
2. 将 `OpenMV/` 目录下的3个 .py 文件复制到项目目录
3. 调整 `target_detect.py` 中的 `RED_THRESHOLD` 以适应实际光照
4. 连接H7Plus，运行

---

## 模块接口速查

### MSPM0G3507 模块

| 函数 | 说明 | 参数 | 返回值 |
|------|------|------|--------|
| GRAY_Init() | 初始化灰度ADC | 无 | void |
| GRAY_ReadAll(buf) | 读取8路灰度 | uint16_t * | void |
| GRAY_GetDeviation(buf) | 加权偏差 | const uint16_t * | -100~+100 |
| GRAY_IsCorner(buf) | 直角检测 | const uint16_t * | 0/1 |
| PID_Init(pid, Kp, Kd, max) | 初始化PID | PID_t*, float x 3 | void |
| PID_Calc(pid, err, dt) | PD计算 | PID_t*, float x 2 | float |
| MOTOR_Init() | 初始化TB6612 | 无 | void |
| MOTOR_SetSpeed(l, r) | 设速度 | int16_t, int16_t | void |
| LAP_Init(lap, N) | 初始化圈数 | LapState_t*, uint8_t | void |
| LAP_Update(lap, gray) | 圈数更新 | LapState_t*, uint16_t* | 0/1/2 |
| KEY_Scan() | 按键扫描 | void | KEY_NONE/INC/DEC |
| OLED_ShowLapInfo() | 显示圈数 | uint8_t, uint8_t, char* | void |
| UART_SendCmd(cmd, param) | 发送命令 | CmdCode_t, uint8_t | void |

### STM32F103 模块

| 函数 | 说明 | 参数 | 返回值 |
|------|------|------|--------|
| GIMBAL_Init(g) | 初始化云台 | Gimbal_t* | void |
| GIMBAL_SetAngles(g, pan, tilt) | 设角度 | Gimbal_t*, float x 2 | void |
| GIMBAL_Sweep(g, start, end, tilt, step) | 扫描 | Gimbal_t*, float x 4 | void |
| CAM_GetTarget(x,y,w,h) | 获取靶心 | float* x 4 | 0/1 |
| LASER_On/Off() | 激光控制 | void | void |
| GEO_PixelToAngle(px, py, pan, tilt) | 坐标转角度 | float x 2, float* x 2 | void |

### OpenMV 模块

| 函数 | 说明 | 参数 | 返回值 |
|------|------|------|--------|
| find_target(img) | 找红色靶心 | image对象 | (cx,cy,w,h)或None |
| uart_init() | 初始化UART | void | UART对象 |
| send_coords(uart, x, y, w, h) | 发送坐标 | UART, float x 4 | void |
| recv_cmd(uart) | 接收查询 | UART | True/False |

---

## 调试建议

1. **灰度阈值标定：** 首次上电后查看各通道ADC值，白底约100~200，黑线约1500~3000+
2. **PID参数整定：** 先只调P（让小车能沿黑线走），再加大D抑制过冲
3. **直角防抖：** 赛道顶点处全压黑线时间约50~100ms（视车速），防抖计数设3~5个采样周期
4. **瞄准精度：** OpenMV靶心阈值需根据赛场实际光照重新标定
5. **电源注意：** 步进电机启停瞬间电流较大，建议独立供电并加大电容

---

## 后续扩展（发挥部分）

| 要求 | 说明 | 状态 |
|------|------|------|
| 发挥1 | 单圈≤20s，全程光斑≤2cm | ⏳ 待实现 |
| 发挥2 | N=2连续跑两圈，总时≤40s | ⏳ 待实现 |
| 发挥3 | 跑圈同时画6cm同心圆 | ⏳ 待实现（坐标同步协议需扩展） |

---

**2026年7月 于电子设计竞赛备战**
