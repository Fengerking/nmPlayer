//*@@@+++@@@@*******************************************************************
//
// Microsoft Windows Media
// Copyright (C) 2003 Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@*******************************************************************
//
// File:    Effects_wmv.h
//
// Desc:    Declarations/definitions for effects.
//
// Author:  Peter X. Zuo (peterzuo@microsoft.com)
//			Port from effects.hpp
//
//******************************************************************************

#ifndef _WMVPEFFECTS_HPP
#define _WMVPEFFECTS_HPP


#include "EffectTypes_wmv.h"

#ifdef WMVIMAGE_V2
//
//Init Effects Members
//
tWMVDecodeStatus Effects_Init(tWMVDecInternalMember *pWMVDec);


#ifndef WMV9_SIMPLE_ONLY
//
//Apply Sprite Effects
//
tWMVDecodeStatus WMSpriteApplyEffectsV2(HWMVDecoder hWMVDecoder, I32_WMV iPrevNumMotionVectorSets);

//
//Decode Effect Parameters
//
tWMVDecodeStatus DecodeEffectParameters
(
    HWMVDecoder hWMVDecoder,
    U32_WMV* uiEffectType,
    U32_WMV* uiNumAffineCoefficients,
    Float_WMV* pAffineCoefficients,  
    U32_WMV* uiNumEffectParameters, 
    Float_WMV* pEffectParameters,
    Bool_WMV* bKeepPrevImage
);
#endif

//
//Create WMVImage Effects Structure
//
WMVIMAGE_EFFECTS *WMVImage_EffectNew( tWMVDecInternalMember *pWMVDec );

//
//Release WMVImage Effects Structure
//
void WMVImage_EffectRelease(tWMVDecInternalMember *pWMVDec,WMVIMAGE_EFFECTS *pEffects);

//
//Generate WMVImage Effects
//
tWMVDecodeStatus WMVImage_EffectGenerate(
							tWMVDecInternalMember *pWMVDec,
							WMVIMAGE_EFFECTS	*pEffects,
							const I32_WMV iImageWidth,
							const I32_WMV iImageHeight,
							const I32_WMV iEffectType,
							const I32_WMV iNumEffectParameters,
							const Float_WMV *pEffectParameters,
							U8_WMV *pInImg1Y,
							U8_WMV *pInImg1U,
							U8_WMV *pInImg1V,
							U8_WMV *pInImg2Y,
							U8_WMV *pInImg2U,
							U8_WMV *pInImg2V,
							U8_WMV *pOutImgY,
							U8_WMV *pOutImgU,
							U8_WMV *pOutImgV
							);
#endif //WMVIMAGE_V2

//
//Include other effects as well
//
#include "pageroll_wmv.h"
#include "sideroll_wmv.h"


#endif  // _WMVPEFFECTS_HPP
