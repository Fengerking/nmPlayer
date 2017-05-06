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
#include <math.h>

void SideRoll_Right(I32_WMV     iWidth,
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

void SideRoll_Left(I32_WMV     iWidth,
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

void SideRoll_Bottom(I32_WMV     iWidth,
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

void SideRoll_Top(I32_WMV     iWidth,
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

#ifdef WMV_C_OPT_WVP2

extern long ImageRoll_LineQuickFill(U8_WMV* pOutY, U8_WMV* pInY, long iDOffset_new, long loops);

#define ImageRoll_LineYQuickFill(pOutY, pInY, iDOffset_new, loops)	\
	ImageRoll_LineQuickFill(pOutY, pInY, iDOffset_new, loops)


#define ImageRoll_LineUVQuickFill(pOutU, pIn1U, pOutV, pIn1V, iDUVOffset_new, loops)	\
	ImageRoll_LineQuickFill(pOutU, pIn1U, iDUVOffset_new, loops);						\
	ImageRoll_LineQuickFill(pOutV, pIn1V, iDUVOffset_new, loops);

#endif
