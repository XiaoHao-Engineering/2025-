"""
  *****************************************************************
  * @file     target_detect.py
  * @author   2026电赛 E题 小组
  * @brief    靶心识别模块
  *           识别A4靶纸上的红色靶心，返回归一化坐标
  *****************************************************************
"""

import sensor
import image
import math

""" 红色LAB阈值（需根据实际光照环境标定）*************************"""
# 红色在LAB空间：L低，A正大，B适中
RED_THRESHOLD = (0, 100, 20, 80, -10, 50)

""" 图像参数 ***************************************************"""
IMG_WIDTH  = 320     # QVGA
IMG_HEIGHT = 240

"""
  * @brief  寻找图像中的红色靶心
  * @param  img : sensor.snapshot() 捕获的图像对象
  * @retval (cx, cy, w, h) 归一化坐标(0.0~1.0)和归一化尺寸
  *         如果未检测到有效目标，返回 None
"""
def find_target(img):
    # 在图像中分割红色区域
    blobs = img.find_blobs([RED_THRESHOLD], pixels_threshold=20,
                           area_threshold=20, merge=True)

    if not blobs:
        return None

    # 筛选最大面积的红色区域
    max_blob = None
    max_area = 0

    for blob in blobs:
        # 过滤太小的区域
        if blob.area() < 50:
            continue

        # 过滤过于细长的区域（圆形度筛选）
        roundness = blob.roundness()
        if roundness < 0.3:
            continue

        if blob.area() > max_area:
            max_area = blob.area()
            max_blob = blob

    if max_blob is None:
        return None

    # 归一化坐标
    cx = max_blob.cx() / IMG_WIDTH
    cy = max_blob.cy() / IMG_HEIGHT
    w  = max_blob.w() / IMG_WIDTH
    h  = max_blob.h() / IMG_HEIGHT

    return (cx, cy, w, h)


"""
  * @brief  初始化摄像头传感器
"""
def sensor_init():
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)  # 彩色
    sensor.set_framesize(sensor.QVGA)    # 320x240
    sensor.set_windowing((320, 240))
    sensor.skip_frames(time=2000)        # 等待图像稳定
    sensor.set_auto_gain(False)          # 关闭自动增益
    sensor.set_auto_whitebal(False)      # 关闭自动白平衡
    print("[CAM] Sensor initialized")
