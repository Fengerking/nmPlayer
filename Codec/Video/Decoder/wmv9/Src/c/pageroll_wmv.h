//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 2002

Module Name:

       directcctest.cpp

Abstract:

       Main program for frame-based color conversion with audio copy

*************************************************************************/
#include "voWmvPort.h"

//#define _loadds

// R: radius of ball
// T: vertical distance between ball center to right left end. 
void ImageRoll(I32_WMV     iWidth,
               I32_WMV     iHeight,
               U8_WMV*     pImgIn1,
               U8_WMV*     pImgIn2,
               U8_WMV*     pImgOut,
               double  R,
               double  T);

void ImageRoll_RightBottom(I32_WMV     iWidth,
                           I32_WMV     iHeight,
                           U8_WMV*     pImgIn1Y,
                           U8_WMV*     pImgIn1U,
                           U8_WMV*     pImgIn1V,
                           U8_WMV*     pImgIn2Y,
                           U8_WMV*     pImgIn2U,
                           U8_WMV*     pImgIn2V,
                           U8_WMV*     pImgOutY,
                           U8_WMV*     pImgOutU,
                           U8_WMV*     pImgOutV,
                           int     iR,
                           int     iT);

void ImageRoll_RightTop(I32_WMV     iWidth,
                        I32_WMV     iHeight,
                        U8_WMV*     pImgIn1Y,
                        U8_WMV*     pImgIn1U,
                        U8_WMV*     pImgIn1V,
                        U8_WMV*     pImgIn2Y,
                        U8_WMV*     pImgIn2U,
                        U8_WMV*     pImgIn2V,
                        U8_WMV*     pImgOutY,
                        U8_WMV*     pImgOutU,
                        U8_WMV*     pImgOutV,
                        int     iR,
                        int     iT);

void ImageRoll_LeftBottom(I32_WMV     iWidth,
                          I32_WMV     iHeight,
                          U8_WMV*     pImgIn1Y,
                          U8_WMV*     pImgIn1U,
                          U8_WMV*     pImgIn1V,
                          U8_WMV*     pImgIn2Y,
                          U8_WMV*     pImgIn2U,
                          U8_WMV*     pImgIn2V,
                          U8_WMV*     pImgOutY,
                          U8_WMV*     pImgOutU,
                          U8_WMV*     pImgOutV,
                          int     iR,
                          int     iT);

void ImageRoll_LeftTop(I32_WMV     iWidth,
                       I32_WMV     iHeight,
                       U8_WMV*     pImgIn1Y,
                       U8_WMV*     pImgIn1U,
                       U8_WMV*     pImgIn1V,
                       U8_WMV*     pImgIn2Y,
                       U8_WMV*     pImgIn2U,
                       U8_WMV*     pImgIn2V,
                       U8_WMV*     pImgOutY,
                       U8_WMV*     pImgOutU,
                       U8_WMV*     pImgOutV,
                       int     iR,
                       int     iT);

