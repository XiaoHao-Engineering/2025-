/**
  *************************************************************
  * @file     geometry.h
  * @author   2026电赛 E题 小组
  * @brief    几何解算 模块头文件
  *           将OpenMV图像坐标转换为云台角度
  *************************************************************
  */
#ifndef __GEOMETRY_H
#define __GEOMETRY_H

#include <stdint.h>

/**
  * @brief  几何参数结构体
  */
typedef struct
{
    float fx;           /* 相机X方向焦距(像素) */
    float fy;           /* 相机Y方向焦距(像素) */
    float cx;           /* 相机主点X坐标(像素) */
    float cy;           /* 相机主点Y坐标(像素) */
    float camHeight;    /* 摄像头离地高度(mm) */
    float laserOffset;  /* 激光器与摄像头水平偏移(mm) */
    float targetDist;   /* 靶纸到车中心的距离(mm) */
} GeoParams_t;

/**
  * @brief  初始化几何解算参数
  * @param  params : 几何参数结构体指针
  * @note   根据实际安装尺寸标定参数
  * @retval 无
  */
void GEO_Init(const GeoParams_t *params);

/**
  * @brief  将像素坐标转换为云台角度
  * @param  px    : 靶心X坐标(归一化0.0~1.0)
  * @param  py    : 靶心Y坐标(归一化0.0~1.0)
  * @param  pan   : 输出水平角度(度)
  * @param  tilt  : 输出俯仰角度(度)
  * @note   采用小孔成像模型：tan(angle) = pixelOffset * pixelSize / focalLength
  * @retval 无
  */
void GEO_PixelToAngle(float px, float py, float *pan, float *tilt);

#endif
