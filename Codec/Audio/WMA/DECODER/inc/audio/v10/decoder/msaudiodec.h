//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
    /*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudioDec.h

Abstract:

    Header file for CAudioObjectDecoder.

Author:

    Wei-ge Chen (wchen) 14-July-1998

Revision History:


*************************************************************************/

#ifndef __MSAUDIODEC_H_
#define __MSAUDIODEC_H_

#include <limits.h>
#include "stdio.h"
#ifndef BUILD_WITHOUT_C_LIB
#include <math.h>
#endif	//BUILD_WITHOUT_C_LIB

#if defined(ENABLE_ARITHCODER)
    #include "arithcoder_c.h"
#endif //ENABLE_ARITHCODER

//#include "macros.h"
#include "msaudio.h"
#include "strmdec_wma.h"
#include "wmaltrt.h"

#ifdef ENABLE_ARITHCODER
#include "arithdecoder_c.h"
#endif //ENABLE_ARITHCODER

//typedef struct LtRtDownmix LtRtDownmix;

#include "wma10dec.h"
#include "wmaprodecS_api.h"

#ifdef WMA_COMPRESSION_TOOL
#include "wmacmpt.h"
#endif //WMA_COMPRESSION_TOOL
#ifdef BUILD_UES
#include "uesdec.h"
#endif
//
//ARM specific optimization
//
//#ifdef WMA_TARGET_ARM
//#include <arm_flags.h>
//#endif //WMA_TARGET_ARM

#ifdef WMA_TARGET_ARM
#ifdef _IOS
//ARM specific optimization
//define as 1 to turn on asm optimization, 0 to turn it off
#define WMA_OPT_STRMDEC_ARM               1        //strmdec_arm.s   --- iOS platform disable(0)
#define WMA_OPT_LOSSLESSDECLSL_ARM        0        //losslessdeclsl_arm.s
#define WMA_OPT_LOSSLESSDECPROLSL_ARM     1        //losslessdecprolsl_arm.s
#define WMA_OPT_HUFFDEC_ARM               1        //huffdec_arm.s --- iOS platform disable(0)
#define WMA_OPT_CHANNEL_DOWNMIX_ARM       1        //msaudiodec_arm.s   20111010
#define WMA_OPT_REQUANTO16_ARM            1        //msaudiodec_arm.s   20111010
#define WMA_OPT_VOMEMORY_ARM              1        //voMemory_arm.s --- iOS platform disable(0)
#else
//ARM specific optimization
//define as 1 to turn on asm optimization, 0 to turn it off
#define WMA_OPT_STRMDEC_ARM               1        //strmdec_arm.s   --- iOS platform disable(0)
#define WMA_OPT_LOSSLESSDECLSL_ARM        1        //losslessdeclsl_arm.s
#define WMA_OPT_LOSSLESSDECPROLSL_ARM     1        //losslessdecprolsl_arm.s
#define WMA_OPT_HUFFDEC_ARM               1        //huffdec_arm.s --- iOS platform disable(0)
#define WMA_OPT_CHANNEL_DOWNMIX_ARM       1        //msaudiodec_arm.s
#define WMA_OPT_REQUANTO16_ARM            1        //msaudiodec_arm.s
#define WMA_OPT_VOMEMORY_ARM              1        //voMemory_arm.s --- iOS platform disable(0)
#endif //_IOS

#endif //WMA_TARGET_ARM

#if defined (BUILD_WMAPRO) && defined (BUILD_WMAPROLBRV2)
#include "baseplusdecpro.h"
#endif

#endif  //__MSAUDIODEC_H_
