/**
  *************************************************************
  * @file     geometry.c
  * @author   2026电赛 E题 小组
  * @brief    几何解算 模块实现
  *           小孔成像 + 三角测量
  *************************************************************
  */

#include "geometry.h"
#include <math.h>

static GeoParams_t g_params;

/* OpenMV H7Plus 图像分辨率参数 ********************************/
#define CAM_IMG_WIDTH           320     /* QVGA宽度 */
#define CAM_IMG_HEIGHT          240     /* QVGA高度 */

/**
  * @brief  初始化几何解算参数
  */
void GEO_Init(const GeoParams_t *params)
{
    g_params.fx          = params->fx;
    g_params.fy          = params->fy;
    g_params.cx          = params->cx;
    g_params.cy          = params->cy;
    g_params.camHeight   = params->camHeight;
    g_params.laserOffset = params->laserOffset;
    g_params.targetDist  = params->targetDist;
}

/**
  * @brief  将像素坐标转换为云台角度
  *
  * 算法说明：
  *   1. 归一化像素坐标(0~1)转像素坐标(0~320, 0~240)
  *   2. 像素坐标转相机坐标系偏移：(u - cx) / fx, (v - cy) / fy
  *   3. 用atan2计算水平角和俯仰角
  */
void GEO_PixelToAngle(float px, float py, float *pan, float *tilt)
{
    float imgX, imgY;
    float dx, dy;
    float panRad, tiltRad;

    /* 归一化坐标转图像像素坐标 */
    imgX = px * CAM_IMG_WIDTH;
    imgY = py * CAM_IMG_HEIGHT;

    /* 计算相对于图像中心的偏移量 */
    dx = (imgX - g_params.cx) / g_params.fx;
    dy = (imgY - g_params.cy) / g_params.fy;

    /* 计算角度（弧度）*/
    panRad  = atan2f(dx, 1.0f);
    tiltRad = atan2f(dy, 1.0f);

    /* 弧度转角度 */
    *pan  = panRad  * 180.0f / 3.14159f;
    *tilt = tiltRad * 180.0f / 3.14159f;

    /* 限幅到云台机械范围 */
    if (*pan  > 90.0f) *pan  = 90.0f;
    if (*pan  < -90.0f) *pan = -90.0f;
    if (*tilt > 60.0f) *tilt  = 60.0f;
    if (*tilt < -30.0f) *tilt = -30.0f;
}
