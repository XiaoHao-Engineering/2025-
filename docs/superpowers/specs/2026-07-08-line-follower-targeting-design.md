# Mite：2026电赛 E题 — 简易自行瞄准装置 设计文档

## 1. 系统概览

**赛题来源**：2025全国大学生电子设计竞赛 本科E题《简易自行瞄准装置》

**硬件平台**：

| 子系统 | 主控 | 关键外设 |
|--------|------|---------|
| 小车底盘 | MSPM0G3507 | 8路灰度(模拟量)、TB6612、按键、OLED(SSD1306/I2C) |
| 云台系统 | STM32F103C8T6 | 42步进电机x2、OpenMV H7Plus、405nm蓝紫激光 |

**电源规则**：两套完全独立电源，分开开关控制。

### 赛道规格

- 100cm x 100cm 正方形闭环黑线赛道，黑线宽 1.8cm +- 0.2cm
- 四个顶点 A/B/C/D 为 90度 垂直转角
- 逆时针行驶
- 靶纸位于 AB 外侧 50cm，A4 紫外感光纸，红色靶心 <=0.1cm

---
## 3. UART 通信协议

**物理层**：MSPM0 UART0 <-> STM32 USART1, 115200bps, 8N1

**帧格式：**

| 帧头(1B) | 命令(1B) | 参数(1B) | 校验(1B) |
|----------|----------|----------|----------|
| 0xAA     | cmd      | param    | sum      |

**命令定义：**

| 命令码 | 名称 | 说明 |
|--------|------|------|
| 0x01 | CMD_LASER_ON | 激光常开（基础1用） |
| 0x02 | CMD_AIM_START | 启动瞄准（基础2/3用） |
| 0x03 | CMD_AIM_DONE | 瞄准完成通知 |
| 0xAA | CMD_ACK | 应答帧 |


## 3. UART Communication Protocol

**Physical layer**: MSPM0 UART0 <-> STM32 USART1, 115200bps, 8N1

**Frame format**:

| Header(1B) | Cmd(1B) | Param(1B) | Checksum(1B) |
|------------|---------|-----------|--------------|
| 0xAA       | cmd     | param     | sum          |

---

## 4. Module Division and Interfaces

### 4.1 MSPM0G3507 Side (8 modules)

Functions per module:
- GRAY_Init/ReadAll/GetDeviation/IsCorner
- PID_Init/Calc/Reset (PD controller with Kp,Kd)
- MOTOR_Init/SetSpeed/Stop
- LAP_Init/Update/GetStatus (lap counting via 4 corners)
- KEY_Init/Scan (button handling)
- OLED_Init/Clear/ShowString/ShowNum/ShowLapInfo
- UART_Init/SendCmd
- main.c: STATE_SETUP/LINE_RUN/LAP_PAUSE/AIM_CENTER/AIM_ARBITRARY/DONE

### 4.2 STM32F103C8T6 Side (6 modules)

- STEP_Init/MoveTo/SetSpeed/Stop (stepper motors)
- GIMBAL_Init/SetAngles/Sweep (gimbal control)
- CAM_Init/GetTarget (OpenMV communication)
- LASER_Init/On/Off/Pulse
- GEO_CalcAngles (pixel to angle conversion)
- main.c: GIMBAL_IDLE/SCAN/AIMING/LOCKED

### 4.3 OpenMV Side (3 modules)

- target_detect.py: Red target detection via LAB threshold
- uart_comm.py: Coordinate send/recv
- main.py: Main loop

---

## 5. Key Pin Assignments

MSPM0G3507: Grayscale PA0-PA7(ADC), PWM PB0-PB1(TIMA0), OLED PC0-PC1(I2C1), UART PA8-PA9
STM32F103: Stepper PAN PA0-PA1(TIM2), TILT PA2-PA3, Laser PA4, UART1 PA9-PA10(MSPM0), UART2 PA2-PA3(OpenMV)

---

## 6. Code Style (JiangKeDa)

1. Module_Function naming, ALL_CAPS macros
2. Block comments before each function
3. Independent .h/.c pairs per peripheral
4. Hardware abstracted as Init/Control layers
5. Pins as macros in .h files
6. State machine with typedef enum + switch-case
7. main.c: init + scheduler only

---

## 7. Directory Structure

```
MSPM0G3507/   - main.c, grayscale, pid, motor, lap_detect, key, oled, uart_comm
STM32F103/    - main.c, stepper, gimbal, cam_comm, laser, geometry
OpenMV/       - main.py, target_detect.py, uart_comm.py
README.md     - System overview, wiring, protocol, usage
```

End of design document.
