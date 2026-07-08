"""
  *****************************************************************
  * @file     main.py
  * @author   2026电赛 E题 小组
  * @brief    OpenMV H7Plus 主程序
  *           流程: 初始化 → 采集图像 → 识别靶心 → 等待查询 → 发送坐标
  *****************************************************************
"""

import sensor
import image
import pyb

from target_detect import find_target, sensor_init
from uart_comm import uart_init, send_coords, recv_cmd

# LED指示
led_red   = pyb.LED(1)
led_green = pyb.LED(2)

"""
  * @brief  主函数
"""
def main():
    print("[SYS] OpenMV Targeting System starting...")

    # 初始化摄像头
    sensor_init()

    # 初始化串口
    uart = uart_init()

    # LED指示系统就绪
    led_green.on()
    pyb.delay(500)
    led_green.off()

    print("[SYS] System ready, waiting for STM32 query...")

    # 主循环
    while True:
        # 捕获图像
        img = sensor.snapshot()

        # 识别靶心
        result = find_target(img)

        # 接收STM32查询
        if recv_cmd(uart):
            if result is not None:
                cx, cy, cw, ch = result
                send_coords(uart, cx, cy, cw, ch)

                # 点亮红色LED表示检测到目标
                led_red.on()
            else:
                # 未检测到目标，发送(0,0,0,0)
                send_coords(uart, 0, 0, 0, 0)
                led_red.off()

        # 帧率控制
        pyb.delay(10)


""" 程序入口 *************************************************"""
if __name__ == "__main__":
    main()
