//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 1996 -- 2000  Microsoft Corporation

Module Name:

	spatialPredictor.h

Abstract:

	Include file for spatial prediction

Revision History:

    Sridhar Srinivasan: 7/20/00

*************************************************************************/

#include "wmvdec_member.h"

#ifndef WMV9_SIMPLE_ONLY
t_CSpatialPredictor *t_SpatialPredictorConstruct (tWMVDecInternalMember * pWMVDec);
#endif
Void_WMV    t_SpatialPredictorDestruct (tWMVDecInternalMember *pWMVDec,t_CSpatialPredictor *pSp);
Void_WMV    predictDec (t_CSpatialPredictor *pSp, const I32_WMV iOrient,
                 U8_WMV *pRef, const I32_WMV iRefStride, I16_WMV *pDelta,
                 Bool_WMV bClear);
Void_WMV    predict_0 (t_CSpatialPredictor *pSp, const I32_WMV iOrient,
                 U8_WMV *pRef, const I32_WMV iRefStride);

#ifdef _WMV_TARGET_X86_
Void_WMV    predictDec_MMX (t_CSpatialPredictor *pSp, const I32_WMV iOrient,
                 U8_WMV *pRef, const I32_WMV iRefStride, I16_WMV *pDelta,
                 Bool_WMV bClear);
Void_WMV    predict_0_MMX (U16_WMV *pTSum, U16_WMV *pLSum, U16_WMV *pWts,
                        U8_WMV *pRef, I32_WMV iRefStride);
Void_WMV    SetupZeroPredictor_MMX (t_CSpatialPredictor *pSp);
Void_WMV    GetRange (U8_WMV *pTop, U8_WMV *pLeft, I32_WMV* piRange,
                      I32_WMV* piDC);
#endif // _WMV_TARGET_X86_

/****************************************************************
  Class CContext: local store of contextual information
  Used by encoder and decoder for consistent context generation
****************************************************************/

#ifndef WMV9_SIMPLE_ONLY
t_CContextWMV *t_ContextWMVConstruct (tWMVDecInternalMember *pWMVDec, I32_WMV iCol, I32_WMV iRow);
Void_WMV		t_ContextWMVDestruct (tWMVDecInternalMember *pWMVDec,t_CContextWMV *pContext);
#endif
Void_WMV    t_ContextGetDec (t_CContextWMV *pContext, I32_WMV iX, I32_WMV iY,
              I32_WMV iStepSize, I32_WMV *iPredOrient, I32_WMV *iEstRun);
Void_WMV    t_ContextPutDec (t_CContextWMV *pContext, I32_WMV iX, I32_WMV iY,
                             I32_WMV iOrientContext, I32_WMV iRunContext);
I32_WMV     t_ContextGetChromaDec (t_CContextWMV *pContext, I32_WMV blockX,
                                   I32_WMV blockY);// chroma context
