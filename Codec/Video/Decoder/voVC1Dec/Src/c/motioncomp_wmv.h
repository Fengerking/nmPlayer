//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1998 - 1999

Module Name:

	MotionCompDec.h

Abstract:

	Decoder motion compensation routines 

Author:

	Ming-Chieh Lee (mingcl@microsoft.com) July-1998
	Joseph Wu (sjwu@microsoft.com) July-1998
    Chuang Gu (chuanggu@microsoft.com)

Revision History:

*************************************************************************/
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "typedef.h"

#ifndef _MOTIONCOMPDEC_H_
#define _MOTIONCOMPDEC_H_

struct tagWMVDecInternalMember;

#define MOTION_COMP_ARGS 	        struct tagWMVDecInternalMember *pWMVDec, \
                                    U8_WMV*              ppxlcPredMB,   /* Predicted Block               */   \
	                                const U8_WMV*        ppxlcRef,      /* Refferance Block              */   \
                                    I32_WMV                  iWidthFrm,    /* predicted block width         */   \
                                    Bool_WMV                 bInterpolateX, /* need horizantal interpolation */   \
                                    Bool_WMV                 bInterpolateY, /* need vertical interpolation   */\
                                    I32_WMV                  iMixedPelMV

#define MOTION_COMP_ARGS_MC_VEC 	U8_WMV*              ppxlcPredMB,   /* Predicted Block               */   \
	                                const U8_WMV*        ppxlcRef,      /* Refferance Block              */   \
                                    I32_WMV                  iWidthPred,    /* predicted block width         */   \
                                    I32_WMV                  iWidthRef     /* referance block width         */  


#define MOTION_COMP_ADD_ERROR_ARGS  struct tagWMVDecInternalMember *pWMVDec, \
                                    U8_WMV*       ppxlcCurrQMB,      \
                                    const UnionBuffer* ppxliErrorBuf,     \
                                    const U8_WMV* ppxlcRef,          \
                                    I32_WMV                  iWidthFrm,         \
                                    Bool_WMV                 bXEven,            \
                                    Bool_WMV                 bYEven,    \
                                    I32_WMV iMixedPelMV	


#define MOTION_COMP_ADD_ERROR_ARGS_MC_VEC  U8_WMV*       ppxlcCurrQMB,      \
                                    const U8_WMV* ppxlcRef,          \
                                    const UnionBuffer* ppxliErrorBuf,     \
                                    I32_WMV                  iWidthFrm    

#define MOTION_COMP_ZERO_ARGS       U8_WMV* ppxliCurrQYMB, \
	                                U8_WMV* ppxliCurrQUMB, \
	                                U8_WMV* ppxliCurrQVMB, \
	                                const U8_WMV* ppxliRefYMB, \
	                                const U8_WMV* ppxliRefUMB, \
	                                const U8_WMV* ppxliRefVMB, \
	                                I32_WMV iWidthY, \
	                                I32_WMV iWidthUV

////////////////// External (Exported) //////////

extern  Void_WMV ARMV7_g_MotionCompZeroMotion_WMV (MOTION_COMP_ZERO_ARGS);
extern  Void_WMV ARMV6_g_MotionCompZeroMotion_WMV (MOTION_COMP_ZERO_ARGS);
extern  Void_WMV g_MotionCompZeroMotion_WMV_ARMV4 (MOTION_COMP_ZERO_ARGS);
extern  Void_WMV g_MotionCompZeroMotion_WMV_C (
    U8_WMV* ppxliCurrQYMB, 
    U8_WMV* ppxliCurrQUMB, 
    U8_WMV* ppxliCurrQVMB,
    const U8_WMV* ppxliRefYMB, 
    const U8_WMV* ppxliRefUMB, 
    const U8_WMV* ppxliRefVMB,
    I32_WMV iWidthY,
    I32_WMV iWidthUV,
	I32_WMV stride_preY,
	I32_WMV stride_preUV);


extern Void_WMV g_MotionCompAndAddError (MOTION_COMP_ADD_ERROR_ARGS);
extern Void_WMV g_MotionComp (MOTION_COMP_ARGS);
extern Void_WMV g_MotionCompAndAddErrorRndCtrl (MOTION_COMP_ADD_ERROR_ARGS);
extern Void_WMV g_MotionCompRndCtrl (MOTION_COMP_ARGS);

#endif 
