//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       postfilter.h
//
//--------------------------------------------------------------------------

#ifndef __POSTFILTER_H__
#define __POSTFILTER_H__

#include "wmvdec_api.h"

typedef enum DeblockCPUType {ANSI = 0, MMX, SSE1, SSE2, OTHERS} DeblockCPUType;
#define DEBLOCKMB_ARGS      tWMVDecInternalMember *pWMVDec, \
                            U8_WMV __huge        *ppxliY,           \
                            U8_WMV __huge        *ppxliU,           \
                            U8_WMV __huge        *ppxliV,           \
                            Bool_WMV                  bDoLeft,          \
                            Bool_WMV                  bDoRightHalfEdge, \
                            Bool_WMV                  bDoTop,           \
                            Bool_WMV                  bDoBottomHalfEdge,\
                            Bool_WMV                  bDoMiddle,        \
                            I32_WMV                   iStepSize,        \
                            I32_WMV                   iWidthPrevY,      \
                            I32_WMV                   iWidthPrevUV

#define DEBLOCKYMB_ARGS     U8_WMV *ppxliY,           \
                            U32_WMV                  iMBStartY,        \
                            U32_WMV                  iMBEndY,          \
                            I32_WMV                   iStepSize,        \
                            U32_WMV                  NumMBX,           \
                            I32_WMV                   iWidthPrevY,      \
                            U32_WMV                  MBSizeXWidthPrevY,    \
                            DeblockCPUType        cpuType
#define DEBLOCKUVMB_ARGS    U8_WMV *ppxliUV,          \
                            U32_WMV                  iMBStartY,        \
                            U32_WMV                  iMBEndY,          \
                            I32_WMV                   iStepSize,        \
                            U32_WMV                  NumMBX,           \
                            I32_WMV                   iWidthPrevUV,     \
                            U32_WMV                  BlkSizeXWidthPrevUV,  \
                            DeblockCPUType        cpuType

#define DEBLOCKPMB_ARGS     U8_WMV *ppxliY,           \
                            U8_WMV *ppxliU,           \
                            U8_WMV *ppxliV,           \
                            const CWMVMBMode*     pmbmd,            \
                            U32_WMV                  iMBStartY,        \
                            U32_WMV                  iMBEndY,          \
                            U32_WMV                  NumMBX,           \
                            U32_WMV                  NumMBY,           \
                            I32_WMV                   iStepSize,        \
                            I32_WMV                   iWidthPrevY,      \
                            I32_WMV                   iWidthPrevUV,     \
                            U32_WMV                  MBSizeXWidthPrevY,\
                            U32_WMV                  BlkSizeXWidthPrevUV,  \
                            DeblockCPUType        cpuType

#define FILTERHEDGE_ARGS    U8_WMV* ppxlcCenter, \
                            I32_WMV iPixelDistance, \
                            I32_WMV iPixelIncrement, \
                            I32_WMV iEdgeLength, \
                            I32_WMV iStepSize

#define FILTERVEDGE_ARGS    U8_WMV               *ppxlcCenter,      \
                            I32_WMV                   iPixelDistance,   \
                            I32_WMV                   iPixelIncrement,  \
                            I32_WMV                   NumMBX,           \
                            I32_WMV                   iEdgeLength,      \
                            I32_WMV                   iStepSize,        \
                            Void_WMV   (*pFilterHEdge)(FILTERHEDGE_ARGS)

#define APPLYSMOOTHING_ARGS U8_WMV                *pixel,           \
                            I32_WMV                   width,            \
                            I32_WMV                   max_diff,         \
                            I32_WMV                   thr

#define DETERMTHR_ARGS		U8_WMV                *ptr,             \
                            I32_WMV                   *thr,             \
                            I32_WMV                   *range,           \
                            I32_WMV                   width

#define DETERMTHRSSE_ARGS	U8_WMV                *ptr,             \
                            I32_WMV                   *thr,             \
                            I32_WMV                   width

#define DERINGMB_ARGS       U8_WMV                *ppxlcY,           \
                            U8_WMV                *ppxlcU,           \
                            U8_WMV                *ppxlcV,           \
                            I32_WMV                  iStepSize,         \
                            I32_WMV                  iWidthPrevY,       \
                            I32_WMV                  iWidthPrevUV

#define DERINGIMB_ARGS		U8_WMV *ppxlcY,	\
							U8_WMV *ppxlcU, \
							U8_WMV *ppxlcV, \
							I32_WMV              iStepSize, \
							U32_WMV             iMBStartY, \
							U32_WMV             iMBEndY, \
							U32_WMV             NumMBX, \
							U32_WMV             NumMBY, \
							I32_WMV              iWidthPrevY, \
							I32_WMV              iWidthPrevUV, \
							U32_WMV             MBSizeXWidthPrevY, \
							U32_WMV             BlkSizeXWidthPrevUV, \
							DeblockCPUType   cpuType

#if defined(_IOS) || defined(_MAC_OS)
Void_WMV (*g_pDeblockMB)(DEBLOCKMB_ARGS);
Void_WMV (*g_pApplySmoothing)(APPLYSMOOTHING_ARGS);
Void_WMV (*g_pDetermineThreshold)(DETERMTHR_ARGS);
Void_WMV (*g_pDeringMB)(DERINGMB_ARGS);
Void_WMV (*pFilterHEdge) (FILTERHEDGE_ARGS);
#else
extern Void_WMV (*g_pDeblockMB)(DEBLOCKMB_ARGS);
extern Void_WMV (*g_pApplySmoothing)(APPLYSMOOTHING_ARGS);
extern Void_WMV (*g_pDetermineThreshold)(DETERMTHR_ARGS);
extern Void_WMV (*g_pDeringMB)(DERINGMB_ARGS);
extern Void_WMV (*pFilterHEdge) (FILTERHEDGE_ARGS);
#endif

Void_WMV    DeblockMB(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_Short(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_Improved(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_MMX(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_Short_MMX(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_MMX_Improved(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_KNI(DEBLOCKMB_ARGS);
Void_WMV    DeblockMB_Short_KNI(DEBLOCKMB_ARGS);

Void_WMV    DeblockMBInterlace411(DEBLOCKMB_ARGS);
Void_WMV    DeblockMBInterlace411_Short(DEBLOCKMB_ARGS);
Void_WMV    DeblockMBInterlace411_MMX(DEBLOCKMB_ARGS);
Void_WMV    DeblockMBInterlace411_Short_MMX(DEBLOCKMB_ARGS);
Void_WMV    DeblockMBInterlace411_KNI(DEBLOCKMB_ARGS);

Void_WMV DeringMB(DERINGMB_ARGS);

Void_WMV ApplySmoothing(APPLYSMOOTHING_ARGS);
Void_WMV ApplySmoothing_Improved(APPLYSMOOTHING_ARGS);
Void_WMV ApplySmoothing_MMX(APPLYSMOOTHING_ARGS);
Void_WMV ApplySmoothing_MMX_Improved(APPLYSMOOTHING_ARGS);
Void_WMV ApplySmoothing_KNI(APPLYSMOOTHING_ARGS);

Void_WMV    DetermineThreshold(DETERMTHR_ARGS);
Void_WMV    DetermineThreshold_MMX(DETERMTHR_ARGS);
Void_WMV    DetermineThreshold_MMX_Improved(DETERMTHR_ARGS);
Void_WMV    DetermineThreshold_KNI(DETERMTHR_ARGS);
Void_WMV    DetermineThreshold_KNI_Improved(DETERMTHR_ARGS);

Void_WMV g_InitPostFilter (Bool_WMV bFastDeblock, Bool_WMV bYUV411);

Void_WMV    FilterHorizontalEdge_MMX(FILTERHEDGE_ARGS);
Void_WMV    FilterHorizontalEdge_KNI(FILTERHEDGE_ARGS);
Void_WMV    FilterHorizontalHalfEdge_MMX(FILTERHEDGE_ARGS);
Void_WMV    FilterHorizontalHalfEdge_Short_MMX(FILTERHEDGE_ARGS);
Void_WMV    FilterHorizontalEdge_Short_MMX(FILTERHEDGE_ARGS);
Void_WMV    FilterVerticalUVHalfEdge_MMX(U8_WMV *ppxlcCenter, I32_WMV iPixelDistance,
                                     I32_WMV iPixelIncrement, I32_WMV NumMBX,
                                     I32_WMV iEdgeLength, I32_WMV iStepSize);
Void_WMV    FilterVerticalYEdge_MSSE(FILTERVEDGE_ARGS);
Void_WMV    FilterVerticalYEdge_SSE2(FILTERVEDGE_ARGS);
Void_WMV    FilterHorizontalEdge_Short_MMX(FILTERHEDGE_ARGS);
Void_WMV    FilterHorizontalEdge_Short_KNI(FILTERHEDGE_ARGS);
Void_WMV    DeblockYMB_MSSE(DEBLOCKYMB_ARGS);
Void_WMV    DeblockYMB_FASTEST_MSSE(DEBLOCKYMB_ARGS);
Void_WMV    DeblockUVMB_MSSE(DEBLOCKUVMB_ARGS);
Void_WMV    DeblockUVMB_FASTEST_MSSE(DEBLOCKUVMB_ARGS);
Void_WMV    DeringIMB_MSSE(DERINGIMB_ARGS);
Void_WMV    DetermineYThreshold_MMX(DETERMTHRSSE_ARGS);
Void_WMV    DetermineYThreshold_KNI(DETERMTHRSSE_ARGS);
Void_WMV    DetermineUVThreshold_MMX(DETERMTHRSSE_ARGS);
Void_WMV    DetermineUVThreshold_KNI(DETERMTHRSSE_ARGS);
Void_WMV    FilterVerticalUVEdge_MSSE(FILTERVEDGE_ARGS);
Void_WMV    FilterVerticalUVEdge_SSE2(FILTERVEDGE_ARGS);
Void_WMV    DeblockPMB_MSSE(DEBLOCKPMB_ARGS);
Void_WMV    DeblockPMB_FASTEST_MSSE(DEBLOCKPMB_ARGS);
Void_WMV    DeringPMB_MSSE(DERINGMB_ARGS);

#endif

