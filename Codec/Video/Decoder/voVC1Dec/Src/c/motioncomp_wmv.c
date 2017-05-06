//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MB.h

Abstract:

    MacroBlock base class

Author:
    Bruce Lin (blin@microsoft.com) 25-Jan-1997
        Compute curr (Error + Pred) in MC routine.
    Bruce Lin (blin@microsoft.com) 23-Jan-1997
        Optimize MC routines by expanding inner loop
    Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
    Bruce Lin (blin@microsoft.com) 20-March-1996
    Chuang Gu (chuanggu@microsoft.com) 2-Feb-2000

Revision History:

*************************************************************************/


#include "xplatform_wmv.h"
#include <limits.h>
#include "typedef.h"
#include "motioncomp_wmv.h"
#include "opcodes.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
////#pragma warning(disable:4244) 

Void_WMV g_MotionCompZeroMotion_WMV_C (
    U8_WMV* ppxliCurrQYMB, 
    U8_WMV* ppxliCurrQUMB, 
    U8_WMV* ppxliCurrQVMB,
    const U8_WMV* ppxliRefYMB, 
    const U8_WMV* ppxliRefUMB, 
    const U8_WMV* ppxliRefVMB,
    I32_WMV iWidthY,
    I32_WMV iWidthUV,
	I32_WMV stride_preY,
	I32_WMV stride_preUV
)
{
    I32_WMV i; 

    for (i = 0; i < BLOCK_SIZE; i++) {
        *(U32_WMV *) ppxliCurrQYMB = *(U32_WMV *) ppxliRefYMB;
        *(U32_WMV *) (ppxliCurrQYMB + 4) = *(U32_WMV *) (ppxliRefYMB + 4);
        *(U32_WMV *) (ppxliCurrQYMB + 8) = *(U32_WMV *) (ppxliRefYMB + 8);
        *(U32_WMV *) (ppxliCurrQYMB + 12) = *(U32_WMV *) (ppxliRefYMB + 12);
        ppxliCurrQYMB += iWidthY;
        ppxliRefYMB += stride_preY;
        *(U32_WMV *) ppxliCurrQYMB = *(U32_WMV *) ppxliRefYMB;
        *(U32_WMV *) (ppxliCurrQYMB + 4) = *(U32_WMV *) (ppxliRefYMB + 4);
        *(U32_WMV *) (ppxliCurrQYMB + 8) = *(U32_WMV *) (ppxliRefYMB + 8);
        *(U32_WMV *) (ppxliCurrQYMB + 12) = *(U32_WMV *) (ppxliRefYMB + 12);
        ppxliCurrQYMB += iWidthY;  
        ppxliRefYMB += stride_preY;
        *(U32_WMV *) ppxliCurrQUMB = *(U32_WMV *) ppxliRefUMB;
        *(U32_WMV *) (ppxliCurrQUMB + 4) = *(U32_WMV *) (ppxliRefUMB + 4);
        ppxliCurrQUMB += iWidthUV;
        ppxliRefUMB += stride_preUV;
        *(U32_WMV *) ppxliCurrQVMB = *(U32_WMV *) ppxliRefVMB;
        *(U32_WMV *) (ppxliCurrQVMB + 4) = *(U32_WMV *) (ppxliRefVMB + 4);
        ppxliCurrQVMB += iWidthUV;
        ppxliRefVMB += stride_preUV;
        
    }
}