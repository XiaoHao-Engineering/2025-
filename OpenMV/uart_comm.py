"""
  *****************************************************************
  * @file     uart_comm.py
  * @author   2026电赛 E题 小组
  * @brief    OpenMV串口通信模块
  *           与STM32双向通信，协议帧格式：
  *           接收: 0xAA 0x01 (查询指令)
  *           发送: 0xBB cx(0~255) cy(0~255) cw(0~255) ch(0~255) sum
  *****************************************************************
"""

import sensor
from pyb import UART

""" 通信参数 ***************************************************"""
UART_ID  = 3        # P4(TX), P5(RX)
BAUDRATE = 115200

"""
  * @brief  初始化UART
  * @retval uart对象
"""
def uart_init():
    uart = UART(UART_ID, BAUDRATE, timeout_char=100)
    print("[UART] Initialized at %d baud" % BAUDRATE)
    return uart


"""
  * @brief  发送坐标帧到STM32
  * @param  uart : UART对象
  * @param  cx   : 靶心X坐标(0.0~1.0)
  * @param  cy   : 靶心Y坐标(0.0~1.0)
  * @param  cw   : 靶心宽度(0.0~1.0)
  * @param  ch   : 靶心高度(0.0~1.0)
"""
def send_coords(uart, cx, cy, cw, ch):
    # 归一化坐标转0~255
    b_cx = max(0, min(255, int(cx * 255)))
    b_cy = max(0, min(255, int(cy * 255)))
    b_cw = max(0, min(255, int(cw * 255)))
    b_ch = max(0, min(255, int(ch * 255)))

    # 组帧
    frame = bytearray([0xBB, b_cx, b_cy, b_cw, b_ch])
    checksum = sum(frame) & 0xFF
    frame.append(checksum)

    uart.write(frame)


"""
  * @brief  接收STM32的查询指令
  * @param  uart : UART对象
  * @retval True 有查询指令, False 无
"""
def recv_cmd(uart):
    if uart.any() >= 2:
        data = uart.read(2)
        if data is not None and len(data) == 2:
            if data[0] == 0xAA and data[1] == 0x01:
                return True
    return False
