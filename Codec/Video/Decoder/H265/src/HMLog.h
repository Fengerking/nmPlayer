/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
 * Copyright (c) 2010-2013, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     HMLog.h
    \brief    trying to dump log  same as HM
    \Caution  1. don't use struct, so can be reused by HM
              
*/

#ifndef __COMHMLOG__
#define __COMHMLOG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "voType.h"



#define H265_LOG 0
//1.how to set these Macros:
//#define SLICE_LOG 1 (must enable)
//#define LCU_LOG 1 (should enable)

//case 1: check parser, only one can be enabled
//#define CABAC_LOG 1
//#define MOTION_INFO_LOG 1

//case 2: check other modules
//#define INTRA_LOG 1
//#define INTER_LOG 1
//#define DEQUANT_LOG 1
//#define RECON_LOG 1        //only print intra

//2.how to print specified POCs, for example, only print POC=14.
//a.int g_start_print_POC = 14; //[ include  
//b.int g_end_print_POC = 15;    //) not include  

#if H265_LOG
#define SLICE_LOG 1
#define LCU_LOG 1

#define CABAC_LOG 1
#define MOTION_INFO_LOG 0
#define QP_INFO_LOG 0

#define INTRA_LOG 0
#define INTER_LOG 0
#define DEQUANT_LOG 0
#define RECON_LOG 0

#define SAO_LOG 0           //not implemented yet
#define INTRA_PRED_LOG  0   //not implemented yet

VO_VOID printCABACState(VO_U32 m_uiRange, VO_U32 m_uiValue);
VO_VOID printSliceInfo(VO_U32  sliceType, VO_S32 iPOC);
VO_VOID printLCUInfo(VO_U32 lcuAddr);
VO_VOID printQPInfo(VO_U32 *QP, VO_U32 PicWidthInMinTbsY, VO_U32 stride);
VO_VOID printIntraPredictionInput(VO_S32 predModeIntra,
  VO_S32 cIdx,
  VO_U8* top,
  VO_U8* left,
  VO_S32 nTbS,
  VO_U32 tmp_bl,
  VO_U32 tmp_l,
  VO_U32 tmp_tl,
  VO_U32 tmp_t,
  VO_U32 tmp_tr);
VO_VOID printIntraPredictionOutput(VO_S32 cIdx,
  VO_U8* p_ref,
  VO_S32 pic_stride,
  VO_U32 log2TrafoSize);
VO_VOID printxPredInterLuma(VO_U8* p_ceof_value, VO_U32 width, VO_U32 height, VO_U32 stride);
VO_VOID printxPredInterChroma(VO_U8* dstCb, VO_U8* dstCr, VO_U32 width, VO_U32 height, VO_U32 stride);
VO_VOID printMotionInfo(VO_U8 pred_mode, VO_U8 inter_dir, VO_U32 xPb,  VO_U32 yPb, VO_S8 iRefIdx0, VO_S8 iRefIdx1, VO_S32 m_acMv0, VO_S32 m_acMv1);
VO_VOID printReconInput(VO_U32 cIdx, VO_U8 *pRred, VO_U32 pred_stride, VO_U32 x0, VO_U32 y0, VO_S16 *TransCoeffLevel, VO_U32  log2TrafoSize, VO_U32 qp, VO_U32 transform_skip_flag, VO_U32 cu_transquant_bypass_flag);
VO_VOID printDeQuantOut(VO_U32 cIdx, VO_U32 qp, VO_S16 *m_plTempCoeff, VO_U32 uiWidth, VO_U32 height, VO_U32 stride, VO_U32 x, VO_U32 y);
VO_VOID printResidualOutput(VO_U32 cIdx, VO_U8 *p_recon_buf, VO_U32 log2TrafoSize, VO_U32 pred_stride);

VO_VOID printSAOParam(VO_VOID *p_void_slice, VO_U32 ctb_addr_in_rs);

#else
#define CABAC_LOG 0
#define SLICE_LOG 0
#define LCU_LOG 0
#define INTRA_PRED_LOG  0
#define INTER_LOG 0
#define SAO_LOG 0

#endif



#endif
