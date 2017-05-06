//*@@@+++@@@@*******************************************************************
//
// Microsoft Windows Media
// Copyright (C) 2003 Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@*******************************************************************
//
// File:    Effects_wmv.c
//
// Desc:    Implementation of special effects in PhotoMotion Mode
//
// Author:  Peter X. Zuo (peterzuo@microsoft.com)
//          Port from Effects.cpp desktop code.
// Date:    2/12/2004
//
//******************************************************************************
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "Affine.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "effects_wmv.h"


#ifdef WMVIMAGE_V2

extern tWMVDecodeStatus DecodeAffineCoefficients (
    tWMVDecInternalMember *pWMVDec,
    float* pfltA, 
    float* pfltB, 
    float* pfltC, 
    float* pfltD, 
    float* pfltE, 
    float* pfltF, 
    float* pfltFading
);

tWMVDecodeStatus Effects_Init(tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_pEffects==NULL)
    {
        pWMVDec->m_pEffects = WMVImage_EffectNew( pWMVDec );
        if (pWMVDec->m_pEffects == NULL)
        {
            return WMV_Failed;
        }
    }

    if (pWMVDec->m_pAffineCoefficients == NULL)
    {
#ifdef XDM
		pWMVDec->m_pAffineCoefficients  = (Float_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(Float_WMV)*14;
#else
        pWMVDec->m_pAffineCoefficients = (Float_WMV *)wmvMalloc(pWMVDec, sizeof(Float_WMV)*WMVP_MAX_NUM_AFFINE_COFFICIENTS, DHEAP_STRUCT);
        if (pWMVDec->m_pAffineCoefficients == NULL)
        {
            return WMV_Failed;
        }
#endif
    }
    if (pWMVDec->m_pEffectParameters == NULL)
    {
#ifdef XDM
        pWMVDec->m_pEffectParameters    = (Float_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
        pWMVDec->m_MemXDM.pPrivateBuff += sizeof(Float_WMV)*WMVP_MAX_NUM_EFFECT_PARAMETERS;
#else
        pWMVDec->m_pEffectParameters = (Float_WMV *)wmvMalloc(pWMVDec, sizeof(Float_WMV)*WMVP_MAX_NUM_EFFECT_PARAMETERS, DHEAP_STRUCT);
        if (pWMVDec->m_pEffectParameters == NULL)
        {
            return WMV_Failed;
        }
#endif
    }
    return WMV_Succeeded;
}

#ifdef WMV_C_OPT_WVP2
extern void AffineAddCrossFadeYFrame(U8_WMV *pTmp1, U8_WMV *pTmp2, U8_WMV *pTmp3, long iSize);
extern void AffineAddCrossFadeUVFrame(U8_WMV *pTmp1, U8_WMV *pTmp2, U8_WMV *pTmp3, long iSize);
extern void WMVImageFadeColorQuickFillY(U8_WMV *pOutY, U8_WMV *pIn1Y, U8_WMV *pIn2Y, I32_WMV iSize, I32_WMV iBackgroundY);
extern void WMVImageFadeColorQuickFillUV(U8_WMV *pOutU, U8_WMV *pIn1U, U8_WMV *pIn2U, I32_WMV iSize, I32_WMV iBackgroundU);
#endif

#if !defined(WMV_OPT_WVP2_ARM)
void AffineAddCrossFadeYFrame(U8_WMV *pTmp1, U8_WMV *pTmp2, U8_WMV *pTmp3, long iSize)
{
    register long i;
    register U32_WMV tmp;

    for (i=0; i<iSize; i++) {
        tmp = pTmp1[i] + pTmp2[i];
        if (tmp > 255)  
            tmp = 255;
        //tmp = tmp & 0xff;
        pTmp3[i] = (U8_WMV)tmp;
    }
}

void AffineAddCrossFadeUVFrame(U8_WMV *pTmp1, U8_WMV *pTmp2, U8_WMV *pTmp3, long iSize)
{
    register long i;
    register U32_WMV tmp;

    for (i=0; i<iSize; i++) {
        tmp = pTmp1[i] + pTmp2[i] - 128;

        if (tmp < 0)
            tmp = 0;
        else
        if (tmp > 255)  
            tmp = 255;

        pTmp3[i] = (U8_WMV)tmp;
    }
}
#endif

#ifndef WMV9_SIMPLE_ONLY

tWMVDecodeStatus WMSpriteApplyEffectsV2
(
    HWMVDecoder hWMVDecoder,
    I32_WMV iPrevNumMotionVectorSets
)
{
    float fltA1, fltB1, fltC1, fltD1, fltE1, fltF1, fltFading1;
    float fltA2, fltB2, fltC2, fltD2, fltE2, fltF2, fltFading2;
    long iAffinePattern1, iAffinePattern2;

    unsigned char *pY;
    unsigned char *pU;
    unsigned char *pV;

    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus result;

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);



    iAffinePattern1 = pWMVDec->m_iAffinePattern;
    iAffinePattern2 = pWMVDec->m_iAffinePattern;

    result = DecodeAffineCoefficients ((tWMVDecInternalMember*)hWMVDecoder, &fltA1, &fltB1, &fltC1, &fltD1, &fltE1, &fltF1, &fltFading1);
    if (WMV_Succeeded != result) {
        return result;
    }

    iAffinePattern1 = pWMVDec->m_iAffinePattern;

    if (pWMVDec->m_iNumMotionVectorSets == 2) {

        result = DecodeAffineCoefficients ((tWMVDecInternalMember*)hWMVDecoder, &fltA2, &fltB2, &fltC2, &fltD2, &fltE2, &fltF2, &fltFading2);
        if (WMV_Succeeded != result) {
            return result;
        }
        iAffinePattern2 = pWMVDec->m_iAffinePattern;
    }

    result = DecodeEffectParameters((tWMVDecInternalMember*)hWMVDecoder, (U32_WMV*)&pWMVDec->m_iEffectType, (U32_WMV*)&pWMVDec->m_iNumAffineCoefficients, pWMVDec->m_pAffineCoefficients, (U32_WMV*)&pWMVDec->m_iNumEffectParameters, pWMVDec->m_pEffectParameters, &pWMVDec->m_bKeepPrevImage);
    if (result != WMV_Succeeded) {
        return result;
    }


    if (pWMVDec->m_iEffectType > 0)
    {
        switch(pWMVDec->m_iNumAffineCoefficients)
        {
        case 2:
            fltFading1 = pWMVDec->m_pAffineCoefficients[0];
            fltFading2 = pWMVDec->m_pAffineCoefficients[1];
            break;
        case 7:
            pWMVDec->m_iNumMotionVectorSets = 1;
            fltA1 = pWMVDec->m_pAffineCoefficients[0];
            fltB1 = pWMVDec->m_pAffineCoefficients[1];
            fltC1 = pWMVDec->m_pAffineCoefficients[2];
            fltD1 = pWMVDec->m_pAffineCoefficients[3];
            fltE1 = pWMVDec->m_pAffineCoefficients[4];
            fltF1 = pWMVDec->m_pAffineCoefficients[5];
            fltFading1 = pWMVDec->m_pAffineCoefficients[6];
            iAffinePattern1 = pWMVDec->m_iAffinePattern1;
            break;
        case 14:
            pWMVDec->m_iNumMotionVectorSets = 2;
            fltA1 = pWMVDec->m_pAffineCoefficients[0];
            fltB1 = pWMVDec->m_pAffineCoefficients[1];
            fltC1 = pWMVDec->m_pAffineCoefficients[2];
            fltD1 = pWMVDec->m_pAffineCoefficients[3];
            fltE1 = pWMVDec->m_pAffineCoefficients[4];
            fltF1 = pWMVDec->m_pAffineCoefficients[5];
            fltFading1 = pWMVDec->m_pAffineCoefficients[6];
            iAffinePattern1 = pWMVDec->m_iAffinePattern1;

            fltA2 = pWMVDec->m_pAffineCoefficients[7];
            fltB2 = pWMVDec->m_pAffineCoefficients[8];
            fltC2 = pWMVDec->m_pAffineCoefficients[9];
            fltD2 = pWMVDec->m_pAffineCoefficients[10];
            fltE2 = pWMVDec->m_pAffineCoefficients[11];
            fltF2 = pWMVDec->m_pAffineCoefficients[12];
            fltFading2 = pWMVDec->m_pAffineCoefficients[13];
            iAffinePattern1 = pWMVDec->m_iAffinePattern2;
            break;
        default:
            break;
        }
    }


    // Start transform.

    pY = pWMVDec->m_ppxliPostQY;
    pU = pWMVDec->m_ppxliPostQU;
    pV = pWMVDec->m_ppxliPostQV;

    if (pWMVDec->m_iNumMotionVectorSets == 2) {
        pY = pWMVDec->m_ppxliMultiresY;
        pU = pWMVDec->m_ppxliMultiresU;
        pV = pWMVDec->m_ppxliMultiresV;
    }


    if ((fltA1 != pWMVDec->m_fltA1)||(fltB1 != pWMVDec->m_fltB1)||(fltC1 != pWMVDec->m_fltC1)||
        (fltD1 != pWMVDec->m_fltD1)||(fltE1 != pWMVDec->m_fltE1)||(fltF1 != pWMVDec->m_fltF1)||
        (fltFading1 != pWMVDec->m_fltFading1)||(pWMVDec->m_tFrmPredType == IVOP)||
        (iPrevNumMotionVectorSets != pWMVDec->m_iNumMotionVectorSets)) {

        if (iAffinePattern1 == 0) {
            Affine_Transform (
                (HWMVDecoder*)hWMVDecoder,
                pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                pY, pU, pV,
                1.0f, 0.0, fltC1, 0.0f, 1.0f, fltF1, fltFading1
                );
        }
        else if (iAffinePattern1 == 1) {
            Affine_Transform (
                (HWMVDecoder*)hWMVDecoder,
                pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                pY, pU, pV,
                fltA1, 0.0f, fltC1, 0.0f, fltA1, fltF1, fltFading1
                );
        }
        else {
            Affine_Transform (
                (HWMVDecoder*)hWMVDecoder,
                pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                pY, pU, pV,
                fltA1, fltB1, fltC1, fltD1, fltE1, fltF1, fltFading1
                );
        }
    }

    pWMVDec->m_fltA1 = fltA1;
    pWMVDec->m_fltB1 = fltB1;
    pWMVDec->m_fltC1 = fltC1;
    pWMVDec->m_fltD1 = fltD1;
    pWMVDec->m_fltE1 = fltE1;
    pWMVDec->m_fltF1 = fltF1;
    pWMVDec->m_fltFading1 = fltFading1;


    if (pWMVDec->m_iNumMotionVectorSets == 2) {
        if ((fltA2 != pWMVDec->m_fltA2)||(fltB2 != pWMVDec->m_fltB2)||(fltC2 != pWMVDec->m_fltC2)||
            (fltD2 != pWMVDec->m_fltD2)||(fltE2 != pWMVDec->m_fltE2)||(fltF2 != pWMVDec->m_fltF2)||
            (fltFading2 != pWMVDec->m_fltFading2)||(pWMVDec->m_tFrmPredType == IVOP)||
            (iPrevNumMotionVectorSets != pWMVDec->m_iNumMotionVectorSets)) {

            if (pWMVDec->m_iAffinePattern == 0) {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                    pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                    1.0f, 0.0f, fltC2, 0.0f, 1.0f, fltF2, fltFading2
                    );
            }
            else if (pWMVDec->m_iAffinePattern == 1) {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                    pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                    fltA2, 0.0f, fltC2, 0.0f, fltA2, fltF2, fltFading2
                    );
            }
            else {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                    pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                    fltA2, fltB2, fltC2, fltD2, fltE2, fltF2, fltFading2
                    );
            }
        }

        pWMVDec->m_fltA2 = fltA2;
        pWMVDec->m_fltB2 = fltB2;
        pWMVDec->m_fltC2 = fltC2;
        pWMVDec->m_fltD2 = fltD2;
        pWMVDec->m_fltE2 = fltE2;
        pWMVDec->m_fltF2 = fltF2;
        pWMVDec->m_fltFading2 = fltFading2;
    }

    // Generate effects.

    switch(pWMVDec->m_iEffectType)
    {
    case WMVP_TRANSITION_BOW_TIE:
    case WMVP_TRANSITION_CIRCLE:
    case WMVP_TRANSITION_DIAMOND:
    case WMVP_TRANSITION_DIAGONAL:
    case WMVP_TRANSITION_INSET:
    case WMVP_TRANSITION_IRIS:
    case WMVP_TRANSITION_FADE_TO_COLOR:
    case WMVP_TRANSITION_FILLED_V:
    case WMVP_TRANSITION_FLIP:
    case WMVP_TRANSITION_RECTANGLE:
    case WMVP_TRANSITION_REVEAL:
    case WMVP_TRANSITION_SLIDE:
    case WMVP_TRANSITION_SPLIT:
    case WMVP_TRANSITION_STAR:
    case WMVP_TRANSITION_WHEEL:
        WMVImage_EffectGenerate(
			pWMVDec,
            pWMVDec->m_pEffects,
            pWMVDec->m_iSpriteWidthDsiplay,
            pWMVDec->m_iSpriteHeightDisplay,
            pWMVDec->m_iEffectType,
            pWMVDec->m_iNumEffectParameters,
            pWMVDec->m_pEffectParameters,
            pWMVDec->m_ppxliPostPrevQY,
            pWMVDec->m_ppxliPostPrevQU,
            pWMVDec->m_ppxliPostPrevQV,
            pWMVDec->m_ppxliMultiresY,
            pWMVDec->m_ppxliMultiresU,
            pWMVDec->m_ppxliMultiresV,
            pWMVDec->m_ppxliPostQY,
            pWMVDec->m_ppxliPostQU,
            pWMVDec->m_ppxliPostQV);
        break;
//
//Shatter effect is not revealed in 9.1 in PhotoStory. It suffers with bugs and performances. 2/12/2004
//
#if 0
    case WMVP_TRANSITION_SHATTER:
        Int iStatus, iDistance, iRand, iBlockSize;

        iStatus    = Int(m_pEffectParameters[0]);
        iDistance  = Int(m_pEffectParameters[1]);
        iRand      = Int(m_pEffectParameters[2]);
        iBlockSize = Int(m_pEffectParameters[3]);

        if (iStatus == 0)
            NewShatterEffect(m_iSpriteWidthDsiplay,
            m_iSpriteHeightDisplay,
            m_ppxliPostPrevQY,
            m_ppxliPostPrevQU,
            m_ppxliPostPrevQV,
            m_ppxliMultiresY,
            m_ppxliMultiresU,
            m_ppxliMultiresV,
            m_ppxliPostQY,
            m_ppxliPostQU,
            m_ppxliPostQV,
            iBlockSize
            );
        else 
            ShatterEffect(iDistance, iRand);

        break;
#endif

    case WMVP_TRANSITION_PAGE_ROLL:
        {
            I32_WMV iType, iR, iT;
            U8_WMV *ppxliImg1Y, *ppxliImg1U, *ppxliImg1V;
            U8_WMV *ppxliImg2Y, *ppxliImg2U, *ppxliImg2V;


            iType = (I32_WMV)(pWMVDec->m_pEffectParameters[0]);
            iR = (I32_WMV)(pWMVDec->m_pEffectParameters[1]);
            iT = (I32_WMV)(pWMVDec->m_pEffectParameters[2]);

            if (pWMVDec->m_pEffectParameters[3] == WMVP_NORMAL)
            {
                ppxliImg1Y = pWMVDec->m_ppxliPostPrevQY;
                ppxliImg1U = pWMVDec->m_ppxliPostPrevQU;
                ppxliImg1V = pWMVDec->m_ppxliPostPrevQV;
                ppxliImg2Y = pWMVDec->m_ppxliMultiresY;
                ppxliImg2U = pWMVDec->m_ppxliMultiresU;
                ppxliImg2V = pWMVDec->m_ppxliMultiresV;
            }
            else
            {
                ppxliImg1Y = pWMVDec->m_ppxliMultiresY;
                ppxliImg1U = pWMVDec->m_ppxliMultiresU;
                ppxliImg1V = pWMVDec->m_ppxliMultiresV;
                ppxliImg2Y = pWMVDec->m_ppxliPostPrevQY;
                ppxliImg2U = pWMVDec->m_ppxliPostPrevQU;
                ppxliImg2V = pWMVDec->m_ppxliPostPrevQV;
            }


            if (iType == 1)
                ImageRoll_RightTop(
                pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 2)
                ImageRoll_LeftBottom(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 3)
                ImageRoll_LeftTop(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 4)
                ImageRoll_RightBottom(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 5)
                SideRoll_Top(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 6)
                SideRoll_Bottom(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 7)
                SideRoll_Left(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            else if (iType == 8)
                SideRoll_Right(pWMVDec->m_iSpriteWidthDsiplay,
                pWMVDec->m_iSpriteHeightDisplay, 
                ppxliImg1Y,
                ppxliImg1U,
                ppxliImg1V,
                ppxliImg2Y,
                ppxliImg2U,
                ppxliImg2V,
                pWMVDec->m_ppxliPostQY,
                pWMVDec->m_ppxliPostQU,
                pWMVDec->m_ppxliPostQV,
                iR,
                iT);
            }
        break;
    case WMVP_TRANSITION_CROSS_FADE:
    default:
        if (pWMVDec->m_iNumMotionVectorSets == 2) {
            long iSize = pWMVDec->m_iNewWidth*pWMVDec->m_iNewHeight;

#ifdef WMV_C_OPT_WVP2
            AffineAddCrossFadeYFrame(pY, pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostQY, iSize);
            iSize = iSize>>2;
            AffineAddCrossFadeUVFrame(pU, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostQU, iSize);
            AffineAddCrossFadeUVFrame(pV, pWMVDec->m_ppxliPostPrevQV, pWMVDec->m_ppxliPostQV, iSize);
#else

            U8_WMV * pTmp1 = pY;
            U8_WMV * pTmp2 = pWMVDec->m_ppxliPostPrevQY;
            U8_WMV * pTmp3 = pWMVDec->m_ppxliPostQY;
            long i;
            for (i=0; i<iSize; i++) {
                I32_WMV tmp = *pTmp1 + *pTmp2;
                if (tmp > 255) 
                    tmp = 255;
                if (tmp < 0) 
                    tmp = 0;
                *pTmp3 = (U8_WMV)tmp;
                pTmp1++;
                pTmp2++;
                pTmp3++;
            }

            pTmp1 = pU;
            pTmp2 = pWMVDec->m_ppxliPostPrevQU;
            pTmp3 = pWMVDec->m_ppxliPostQU;
            iSize = pWMVDec->m_iNewWidth*pWMVDec->m_iNewHeight/4;
            for (i=0; i<iSize; i++) {
                I32_WMV tmp = *pTmp1 + *pTmp2 - 128;
                if (tmp > 255) 
                    tmp = 255;
                if (tmp < 0) 
                    tmp = 0;
                *pTmp3 = (U8_WMV)tmp;//*pTmp1 + *pTmp2 - 128;
                pTmp1++;
                pTmp2++;
                pTmp3++;
            }

            pTmp1 = pV;
            pTmp2 = pWMVDec->m_ppxliPostPrevQV;
            pTmp3 = pWMVDec->m_ppxliPostQV;
            iSize = pWMVDec->m_iNewWidth*pWMVDec->m_iNewHeight/4;
            for (i=0; i<iSize; i++) {
                I32_WMV tmp = *pTmp1 + *pTmp2 - 128;
                if (tmp > 255) 
                    tmp = 255;
                if (tmp < 0) 
                    tmp = 0;
                *pTmp3 = (U8_WMV)tmp;//*pTmp1 + *pTmp2 - 128;
                pTmp1++;
                pTmp2++;
                pTmp3++;
            }
#endif
            DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliPostQY,pWMVDec->m_ppxliCurrQY,pWMVDec->m_ppxliRef0Y);

        }
    }
    return WMV_Succeeded;
}

//
//PhotoStory2: DecodeEffectParameters
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
)
{
    tWMVDecodeStatus result;
    U32_WMV uiK;
    U32_WMV n;

    tWMVDecInternalMember *pWMVDec; 

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeEffectParameters);

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    *uiEffectType = (U32_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 32);

    if (*uiEffectType > 0)
    {
        *uiNumAffineCoefficients = (U32_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 4);
        if (*uiNumAffineCoefficients == 7)
        {
            result = DecodeAffineCoefficients ((tWMVDecInternalMember*)hWMVDecoder, &pAffineCoefficients[0], &pAffineCoefficients[1], &pAffineCoefficients[2], &pAffineCoefficients[3], &pAffineCoefficients[4], &pAffineCoefficients[5], &pAffineCoefficients[6]);
            if (result != WMV_Succeeded)
            {
                return result;
            }
            pWMVDec->m_iAffinePattern1 = pWMVDec->m_iAffinePattern;
        }
        else if (*uiNumAffineCoefficients == 14)
        {
            result = DecodeAffineCoefficients ((tWMVDecInternalMember*)hWMVDecoder, &pAffineCoefficients[0], &pAffineCoefficients[1], &pAffineCoefficients[2], &pAffineCoefficients[3], &pAffineCoefficients[4], &pAffineCoefficients[5], &pAffineCoefficients[6]);
            if (result != WMV_Succeeded)
            {
                return result;
            }
            pWMVDec->m_iAffinePattern1 = pWMVDec->m_iAffinePattern;

            result = DecodeAffineCoefficients ((tWMVDecInternalMember*)hWMVDecoder, &pAffineCoefficients[7], &pAffineCoefficients[8], &pAffineCoefficients[9], &pAffineCoefficients[10], &pAffineCoefficients[11], &pAffineCoefficients[12], &pAffineCoefficients[13]);
            if (result != WMV_Succeeded)
            {
                return result;
            }
            pWMVDec->m_iAffinePattern2 = pWMVDec->m_iAffinePattern;
        }
        else
        {
			if(*uiNumAffineCoefficients > WMVP_MAX_NUM_AFFINE_COFFICIENTS )
				return WMV_Failed;

            for (n = 0; n < *uiNumAffineCoefficients; n++)
            {
                U32_WMV uiP = (((U32_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15) + (U32_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
                if (BS_invalid(pWMVDec->m_pbitstrmIn))
                {
                    return WMV_Failed;
                }
                pAffineCoefficients[n] = (Float_WMV) uiP / (Float_WMV) (0x8000) - (Float_WMV) (0x4000);
            }
        }

        *uiNumEffectParameters = (U32_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 16);
		if(*uiNumEffectParameters > WMVP_MAX_NUM_EFFECT_PARAMETERS )
			return WMV_Failed;

        for (n = 0; n < *uiNumEffectParameters; n++)
        {
            U32_WMV uiP = (((U32_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15) + (U32_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
            if (BS_invalid(pWMVDec->m_pbitstrmIn))
            {
                return WMV_Failed;
            }
            pEffectParameters[n] = (Float_WMV) uiP / (Float_WMV) (0x8000) - (Float_WMV) (0x4000);
        }
    }

    uiK = (U32_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    *bKeepPrevImage = (uiK == 0)? FALSE : TRUE;

    return WMV_Succeeded;
}

#endif

////////////////////////////////////////////////////////////////////////////////
// WMVPEffects class constructor
//
WMVIMAGE_EFFECTS *WMVImage_EffectNew( tWMVDecInternalMember *pWMVDec )
{
    WMVIMAGE_EFFECTS *new_effect;
#ifdef XDM
    new_effect    = (WMVIMAGE_EFFECTS *)pWMVDec->m_MemXDM.pPrivateBuff;
    pWMVDec->m_MemXDM.pPrivateBuff += sizeof(WMVIMAGE_EFFECTS);
#else
    new_effect = (WMVIMAGE_EFFECTS *)wmvMalloc(pWMVDec, sizeof(WMVIMAGE_EFFECTS), DHEAP_STRUCT);
#endif
    if (new_effect)
    {
        memset(new_effect, 0, sizeof(WMVIMAGE_EFFECTS));
    }

    return new_effect;
}

////////////////////////////////////////////////////////////////////////////////
// WMVPEffects class destructor
//
void WMVImage_EffectRelease(tWMVDecInternalMember *pWMVDec, WMVIMAGE_EFFECTS *pEffects)
{    
    if (pEffects)
    {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO WMVImage_EffectRelease 1" );
#endif
        // Release memory for boundary locations. 
        wmvFree(pWMVDec, pEffects->m_pOuterLeftX);
        wmvFree(pWMVDec, pEffects->m_pInnerLeftX);
        wmvFree(pWMVDec, pEffects->m_pOuterRightX);
        wmvFree(pWMVDec, pEffects->m_pInnerRightX);
        wmvFree(pWMVDec, pEffects->m_pFlipFlag);
        wmvFree(pWMVDec, pEffects->m_pFlipX);
        wmvFree(pWMVDec, pEffects->m_pFlipYRatio);

        wmvFree(pWMVDec, pEffects);

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO WMVImage_EffectRelease 2" );
#endif
    }
}


////////////////////////////////////////////////////////////////////////////////
// AllocateMemory()
//
// Allocate memory if necessary for storing boundar locations .
//
tWMVDecodeStatus WMVImage_AllocateMemory(
	tWMVDecInternalMember *pWMVDec,
    WMVIMAGE_EFFECTS    *pEffects,
    const I32_WMV iImageWidth,
    const I32_WMV iImageHeight
)
{
    tWMVDecodeStatus hr = WMV_Succeeded;

    if (iImageWidth <= 0 || iImageHeight <= 0)
    {
        return WMV_InValidArguments;
    }

    do
    {
        if (pEffects->m_pOuterLeftX == NULL || pEffects->m_iImageHeight < iImageHeight)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pOuterLeftX  :%x",(U32_WMV)pEffects->m_pOuterLeftX );
#endif
            wmvFree(pWMVDec, pEffects->m_pOuterLeftX);
#ifdef XDM
			pEffects->m_pOuterLeftX    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageHeight;
#else
            pEffects->m_pOuterLeftX = (I32_WMV *) wmvMalloc(pWMVDec, sizeof(I32_WMV)*iImageHeight, DHEAP_STRUCT);
            if (pEffects->m_pOuterLeftX == NULL)
            {
                hr = WMV_BadMemory;
                break;
            }
#endif
        }
        if (pEffects->m_pInnerLeftX == NULL || pEffects->m_iImageHeight < iImageHeight)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pInnerLeftX  :%x",(U32_WMV)pEffects->m_pInnerLeftX );
#endif
            wmvFree(pWMVDec, pEffects->m_pInnerLeftX);
#ifdef XDM
			pEffects->m_pInnerLeftX    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageHeight;
#else
            pEffects->m_pInnerLeftX = (I32_WMV *) wmvMalloc(pWMVDec, sizeof(I32_WMV)*iImageHeight, DHEAP_STRUCT);
            if (pEffects->m_pInnerLeftX == NULL) 
            {
                hr = WMV_BadMemory;
                break;
            }
#endif
        }
        if (pEffects->m_pOuterRightX == NULL || pEffects->m_iImageHeight < iImageHeight)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pOuterRightX  :%x",(U32_WMV)pEffects->m_pOuterRightX );
#endif
            wmvFree(pWMVDec, pEffects->m_pOuterRightX);
#ifdef XDM
			pEffects->m_pOuterRightX    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageHeight;
#else
            pEffects->m_pOuterRightX = (I32_WMV *) wmvMalloc(pWMVDec, sizeof(I32_WMV)*iImageHeight, DHEAP_STRUCT);
            if (pEffects->m_pOuterRightX == NULL) 
            {
                hr = WMV_BadMemory;
                break;
            }
#endif
        }
        if (pEffects->m_pInnerRightX == NULL || pEffects->m_iImageHeight < iImageHeight)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pInnerRightX  :%x",(U32_WMV)pEffects->m_pInnerRightX );
#endif
            wmvFree(pWMVDec, pEffects->m_pInnerRightX);
#ifdef XDM
			pEffects->m_pInnerRightX    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageHeight;
#else
            pEffects->m_pInnerRightX = (I32_WMV *) wmvMalloc(pWMVDec, sizeof(I32_WMV)*iImageHeight, DHEAP_STRUCT);
            if (pEffects->m_pInnerRightX == NULL) 
            {
                hr = WMV_BadMemory;
                break;
            }
#endif
        }

        if (pEffects->m_pFlipFlag == NULL || pEffects->m_iImageWidth < iImageWidth)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pFlipFlag  :%x",(U32_WMV)pEffects->m_pFlipFlag );
#endif
            wmvFree(pWMVDec, pEffects->m_pFlipFlag);
#ifdef XDM
			pEffects->m_pFlipFlag    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageWidth;
#else
            pEffects->m_pFlipFlag = (Bool_WMV *) wmvMalloc(pWMVDec, sizeof(Bool_WMV)*iImageWidth, DHEAP_STRUCT);
            if (pEffects->m_pFlipFlag == NULL) 
            {
                hr = WMV_BadMemory;
                break;
            }
#endif 
        }
        if (pEffects->m_pFlipX == NULL || pEffects->m_iImageWidth < iImageWidth)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pFlipX  :%x",(U32_WMV)pEffects->m_pFlipX );
#endif
            wmvFree(pWMVDec, pEffects->m_pFlipX);
#ifdef XDM
			pEffects->m_pFlipX    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageWidth;
#else
            pEffects->m_pFlipX = (I32_WMV *) wmvMalloc(pWMVDec, sizeof(I32_WMV)*iImageWidth, DHEAP_STRUCT);
            if (pEffects->m_pFlipX == NULL) 
            {
                hr = WMV_BadMemory;
                break;
            }
#endif
        }
        if (pEffects->m_pFlipYRatio == NULL || pEffects->m_iImageWidth < iImageWidth)
        {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pFlipYRatio  :%x",(U32_WMV)pEffects->m_pFlipYRatio );
#endif
            wmvFree(pWMVDec, pEffects->m_pFlipYRatio);
#ifdef XDM
			pEffects->m_pFlipYRatio    = (I32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I32_WMV)*iImageWidth;
#else
            pEffects->m_pFlipYRatio = (I32_WMV *) wmvMalloc(pWMVDec, sizeof(I32_WMV)*iImageWidth, DHEAP_STRUCT);
            if (pEffects->m_pFlipYRatio == NULL) 
            {
                hr = WMV_BadMemory;
                break;
            }
#endif
        }
    }while(0);

    if (hr != WMV_Succeeded)
    {
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pOuterLeftX: %x", pEffects->m_pOuterLeftX);
#endif
        FREE_PTR(pWMVDec, pEffects->m_pOuterLeftX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pInnerLeftX: %x", pEffects->m_pInnerLeftX);
#endif
		FREE_PTR(pWMVDec, pEffects->m_pInnerLeftX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pOuterRightX: %x", pEffects->m_pOuterRightX);
#endif
		FREE_PTR(pWMVDec, pEffects->m_pOuterRightX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pInnerRightX: %x", pEffects->m_pInnerRightX);
#endif
		FREE_PTR(pWMVDec, pEffects->m_pInnerRightX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFlipFlag: %x", pEffects->m_pFlipFlag);
#endif
		FREE_PTR(pWMVDec, pEffects->m_pFlipFlag);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFlipX: %x", pEffects->m_pFlipX);
#endif
		FREE_PTR(pWMVDec, pEffects->m_pFlipX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFlipYRatio: %x", pEffects->m_pFlipYRatio);
#endif
		FREE_PTR(pWMVDec, pEffects->m_pFlipYRatio);
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// GenerateOutput()
//
// Transforms input images into output image.
//
tWMVDecodeStatus WMVImage_GenerateOutput
(
    WMVIMAGE_EFFECTS    *pEffects,
    U8_WMV *pInImg1Y,
    U8_WMV *pInImg1U,
    U8_WMV *pInImg1V,
    U8_WMV *pInImg2Y,
    U8_WMV *pInImg2U,
    U8_WMV *pInImg2V,
    U8_WMV *pOutImgY,
    U8_WMV *pOutImgU,
    U8_WMV *pOutImgV
)
{
    U8_WMV *pIn1Y = pInImg1Y;
    U8_WMV *pIn1U = pInImg1U;
    U8_WMV *pIn1V = pInImg1V;

    U8_WMV *pIn2Y = pInImg2Y;
    U8_WMV *pIn2U = pInImg2U;
    U8_WMV *pIn2V = pInImg2V;

    U8_WMV *pOutY = pOutImgY;
    U8_WMV *pOutU = pOutImgU;
    U8_WMV *pOutV = pOutImgV;

    I32_WMV x, y, n, xStart, xEnd, offset;
    I32_WMV outerLeftX, innerLeftX, outerRightX, innerRightX;

    // Blend input images into output image: Y component.
    //I32_WMV xCenter = pEffects->m_iCenterX;

    if (pInImg1Y == NULL || pInImg1U == NULL || pInImg1V == NULL || 
        pInImg2Y == NULL || pInImg2U == NULL || pInImg2V == NULL || 
        pOutImgY == NULL || pOutImgU == NULL || pOutImgV == NULL)
    {
        return WMV_InValidArguments;
    }

#ifdef WMV_C_OPT_WVP2

    offset = 0;
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        outerLeftX = pEffects->m_pOuterLeftX[y];
        innerLeftX = pEffects->m_pInnerLeftX[y];
        outerRightX = pEffects->m_pOuterRightX[y];
        innerRightX = pEffects->m_pInnerRightX[y];

        // Image 1 from outside to outer left.
        xEnd = (outerLeftX < pEffects->m_iImageWidth)? outerLeftX : pEffects->m_iImageWidth;

        if (xEnd > 0)
            memcpy(pOutY+offset, pIn1Y+offset, xEnd);

        // Image 1 from outside to outer right.
        xEnd = (outerRightX > 0)? outerRightX : 0;

        n = pEffects->m_iImageWidth-1 - xEnd;
        if (n > 0)
            memcpy(pOutY+offset+xEnd+1, pIn1Y+offset+xEnd+1, n);

        // Image 1 from inner left to inner right.
        xStart = (innerLeftX > -1)? innerLeftX + 1 : 0;
        xEnd = (innerRightX < pEffects->m_iImageWidth)? innerRightX : pEffects->m_iImageWidth;

        n = xEnd - xStart;
        if (n>0)
            memcpy(pOutY+offset+xStart, pIn1Y+offset+xStart, n);

        // Image 2 from outer left to inner left.
        xStart = (outerLeftX > 0)? outerLeftX : 0;
        xEnd = (innerLeftX < pEffects->m_iImageWidth)? innerLeftX : pEffects->m_iImageWidth-1;

        n = xEnd - xStart+1;
        if (n>0)
            memcpy(pOutY+offset+xStart, pIn2Y+offset+xStart, n);

        // Image 2 from outer right to inner right.
        xStart = (outerRightX < pEffects->m_iImageWidth)? outerRightX : pEffects->m_iImageWidth-1;
        xEnd = (innerRightX > 0)? innerRightX : 0;

        n = xStart - xEnd + 1;
        if (n>0)
            memcpy(pOutY+offset+xEnd, pIn2Y+offset+xEnd, n);

        offset += pEffects->m_iImageWidth;
    }

    offset = 0;
    // Blend input images into output image: UV components.
    for (y = 0; y < pEffects->m_iImageHeight; y += 2)
    {
        outerLeftX = pEffects->m_pOuterLeftX[y];
        innerLeftX = pEffects->m_pInnerLeftX[y];
        outerRightX = pEffects->m_pOuterRightX[y];
        innerRightX = pEffects->m_pInnerRightX[y];

        // Image 1 from outside to outer left.
        xEnd = (outerLeftX < pEffects->m_iImageWidth)? outerLeftX : pEffects->m_iImageWidth;

        xEnd=(xEnd+1)>>1;
        if (xEnd>0)
        {
            memcpy(pOutU+offset, pIn1U+offset, xEnd);
            memcpy(pOutV+offset, pIn1V+offset, xEnd);
        }

        // Image 1 from outside to outer right.
        xStart = (pEffects->m_iImageWidth&1)? pEffects->m_iImageWidth-1 : pEffects->m_iImageWidth-2;
        xEnd = (outerRightX > 0)? outerRightX : 0;
        n = (xStart-xEnd+1)>>1;
        if (n > 0)
        {
            x = (xStart>>1) - n + 1;
            memcpy(pOutU+offset+x, pIn1U+offset+x, n);
            memcpy(pOutV+offset+x, pIn1V+offset+x, n);
        }

        // Image 1 from inner left to inner right.
        xStart = (innerLeftX > -1)? innerLeftX + 1 : 0;
        if (xStart&1) xStart++;
        xEnd = (innerRightX < pEffects->m_iImageWidth)? innerRightX : pEffects->m_iImageWidth;

        n = (xEnd-xStart+1)>>1;
        if (n > 0)
        {
            x = xStart>>1;
            memcpy(pOutU+offset+x, pIn1U+offset+x, n);
            memcpy(pOutV+offset+x, pIn1V+offset+x, n);
        }

        // Image 2 from outer left to inner left.
        xStart = (outerLeftX > 0)? outerLeftX : 0;
        if (xStart&1) xStart++;
        xEnd = (innerLeftX < pEffects->m_iImageWidth)? innerLeftX : pEffects->m_iImageWidth-1;

        n = (xEnd-xStart+2)>>1;
        if (n > 0)
        {
            x = xStart>>1;
            memcpy(pOutU+offset+x, pIn2U+offset+x, n);
            memcpy(pOutV+offset+x, pIn2V+offset+x, n);
        }

        // Image 2 from outer right to inner right.
        xStart = (outerRightX < pEffects->m_iImageWidth)? outerRightX : pEffects->m_iImageWidth-1;
        if (xStart&1) xStart--;
        xEnd = (innerRightX > 0)? innerRightX : 0;

        n = (xStart-xEnd+2)>>1;
        if (n > 0)
        {
            x = (xStart>>1) - n + 1;
            memcpy(pOutU+offset+x, pIn2U+offset+x, n);
            memcpy(pOutV+offset+x, pIn2V+offset+x, n);
        }

        offset += pEffects->m_iImageWidth >> 1;
    }


#else
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        offset = y * pEffects->m_iImageWidth;

        outerLeftX = pEffects->m_pOuterLeftX[y];
        innerLeftX = pEffects->m_pInnerLeftX[y];
        outerRightX = pEffects->m_pOuterRightX[y];
        innerRightX = pEffects->m_pInnerRightX[y];

        // Image 1 from outside to outer left.
        xEnd = (outerLeftX < pEffects->m_iImageWidth)? outerLeftX : pEffects->m_iImageWidth;

        for (x = 0; x < xEnd; x++)
        {
            pOutY[offset+x] = pIn1Y[offset+x];
        }

        // Image 1 from outside to outer right.
        xEnd = (outerRightX > 0)? outerRightX : 0;
        for (x = pEffects->m_iImageWidth-1; x > xEnd; x--)
        {
            pOutY[offset+x] = pIn1Y[offset+x];
        }

        // Image 1 from inner left to inner right.
        xStart = (innerLeftX > -1)? innerLeftX + 1 : 0;
        xEnd = (innerRightX < pEffects->m_iImageWidth)? innerRightX : pEffects->m_iImageWidth;
        for (x = xStart; x < xEnd; x++)
        {
            pOutY[offset+x] = pIn1Y[offset+x];
        }

        // Image 2 from outer left to inner left.
        xStart = (outerLeftX > 0)? outerLeftX : 0;
        xEnd = (innerLeftX < pEffects->m_iImageWidth)? innerLeftX : pEffects->m_iImageWidth-1;
        for (x = xStart; x <= xEnd; x++)
        {
            pOutY[offset+x] = pIn2Y[offset+x];
        }

        // Image 2 from outer right to inner right.
        xStart = (outerRightX < pEffects->m_iImageWidth)? outerRightX : pEffects->m_iImageWidth-1;
        xEnd = (innerRightX > 0)? innerRightX : 0;
        for (x = xStart; x >= xEnd; x--)
        {
            pOutY[offset+x] = pIn2Y[offset+x];
        }
    }

    // Blend input images into output image: UV components.
    for (y = 0; y < pEffects->m_iImageHeight; y += 2)
    {
        offset = y * pEffects->m_iImageWidth / 4;

        outerLeftX = pEffects->m_pOuterLeftX[y];
        innerLeftX = pEffects->m_pInnerLeftX[y];
        outerRightX = pEffects->m_pOuterRightX[y];
        innerRightX = pEffects->m_pInnerRightX[y];

        // Image 1 from outside to outer left.
        xEnd = (outerLeftX < pEffects->m_iImageWidth)? outerLeftX : pEffects->m_iImageWidth;
        for (x = 0; x < xEnd; x+=2)
        {
            n = offset + ((x >> 1));
            pOutU[n] = pIn1U[n];
            pOutV[n] = pIn1V[n];
        }

        // Image 1 from outside to outer right.
        xStart = (pEffects->m_iImageWidth&1)? pEffects->m_iImageWidth-1 : pEffects->m_iImageWidth-2;
        xEnd = (outerRightX > 0)? outerRightX : 0;
        for (x = xStart; x > xEnd; x -= 2)
        {
            n = offset + (x >> 1);
            pOutU[n] = pIn1U[n];
            pOutV[n] = pIn1V[n];
        }

        // Image 1 from inner left to inner right.
        xStart = (innerLeftX > -1)? innerLeftX + 1 : 0;
        if (xStart&1) xStart++;
        xEnd = (innerRightX < pEffects->m_iImageWidth)? innerRightX : pEffects->m_iImageWidth;
        for (x = xStart; x < xEnd; x += 2)
        {
            n = offset + (x >> 1);
            pOutU[n] = pIn1U[n];
            pOutV[n] = pIn1V[n];
        }

        // Image 2 from outer left to inner left.
        xStart = (outerLeftX > 0)? outerLeftX : 0;
        if (xStart&1) xStart++;
        xEnd = (innerLeftX < pEffects->m_iImageWidth)? innerLeftX : pEffects->m_iImageWidth-1;
        for (x = xStart; x <= xEnd; x += 2)
        {
            n = offset + (x >> 1);
            pOutU[n] = pIn2U[n];
            pOutV[n] = pIn2V[n];
        }

        // Image 2 from outer right to inner right.
        xStart = (outerRightX < pEffects->m_iImageWidth)? outerRightX : pEffects->m_iImageWidth-1;
        if (xStart&1) xStart--;
        xEnd = (innerRightX > 0)? innerRightX : 0;
        for (x = xStart; x >= xEnd; x -= 2)
        {
            n = offset + (x >> 1);
            pOutU[n] = pIn2U[n];
            pOutV[n] = pIn2V[n];
        }
    }
#endif

    return WMV_Succeeded;
}

#if defined(WMV_C_OPT_WVP2)
extern U8_WMV * WMVImageGenerateFlipOutputYLine(U8_WMV *pOutY, U8_WMV *pInY, WMVIMAGE_EFFECTS *pEffects, int y);
extern U8_WMV *WMVImageGenerateFlipOutputUVLine(U8_WMV *pOutU, U8_WMV *pInU, U8_WMV *pOutV, U8_WMV *pInV, WMVIMAGE_EFFECTS *pEffects, int y);
#endif

#if defined(WMV_C_OPT_WVP2) && !defined(WMV_OPT_WVP2_ARM)

U8_WMV * WMVImageGenerateFlipOutputYLine(U8_WMV *pOutY, U8_WMV *pInY, WMVIMAGE_EFFECTS *pEffects, int y)
{
    int x, xx, yy, yc;

    yc = y - pEffects->m_iCenterY;

    for (x = 0; x < pEffects->m_iImageWidth; x++)
    {
        if (!pEffects->m_pFlipFlag[x])
        {
            *pOutY++ = 0;
        }
        else
        {
            yy = ((yc * pEffects->m_pFlipYRatio[x]) >> 20) + pEffects->m_iCenterY;
    
            if (yy < 0 || yy >= pEffects->m_iImageHeight)
            {
                // Y
                *pOutY++ = 0;
            }
            else 
            {
                // Y
                xx = pEffects->m_pFlipX[x];
                *pOutY++ = pInY[yy*pEffects->m_iImageWidth+xx];

            }
        }
    }

    return pOutY;
}

U8_WMV *WMVImageGenerateFlipOutputUVLine(U8_WMV *pOutU, U8_WMV *pInU, U8_WMV *pOutV, U8_WMV *pInV, WMVIMAGE_EFFECTS *pEffects, int y)
{
    int x, xx, yy, yc, n, iImageWidthUV;

    iImageWidthUV = pEffects->m_iImageWidth >> 1;

    yc = y - pEffects->m_iCenterY;
    for (x = 0; x < pEffects->m_iImageWidth; x+=2)
    {
        if (!pEffects->m_pFlipFlag[x])
        {
            *pOutU++ = 128;
            *pOutV++ = 128;
        }
        else
        {
            yy = ((yc * pEffects->m_pFlipYRatio[x]) >> 20) + pEffects->m_iCenterY;
    
            if (yy < 0 || yy >= pEffects->m_iImageHeight)
            {
                *pOutU++ = 128;
                *pOutV++ = 128;
            }
            else 
            {
                xx = pEffects->m_pFlipX[x];

                n = (yy >> 1) * iImageWidthUV + (xx >> 1);

                *pOutU++ = pInU[n];
                *pOutV++ = pInV[n];
            }
        }
    }

    return pOutU;
}

#endif //WMV_C_OPT_WVP2

////////////////////////////////////////////////////////////////////////////////
// GenerateFlipOutput()
//
// Transforms input images into flipped output image.
//
tWMVDecodeStatus WMVImage_GenerateFlipOutput
(
    WMVIMAGE_EFFECTS    *pEffects,
    U8_WMV *pInImg1Y,
    U8_WMV *pInImg1U,
    U8_WMV *pInImg1V,
    U8_WMV *pInImg2Y,
    U8_WMV *pInImg2U,
    U8_WMV *pInImg2V,
    U8_WMV *pOutImgY,
    U8_WMV *pOutImgU,
    U8_WMV *pOutImgV                                      
)
{
    U8_WMV *pInY;
    U8_WMV *pInU;
    U8_WMV *pInV;

    if (pInImg1Y == NULL || pInImg1U == NULL || pInImg1V == NULL || 
        pInImg2Y == NULL || pInImg2U == NULL || pInImg2V == NULL || 
        pOutImgY == NULL || pOutImgU == NULL || pOutImgV == NULL)
    {
        return WMV_InValidArguments;
    }

    if (pEffects->m_fFlipAngle == 90)
    {
        // Special case: output is black.
        memset(pOutImgY, 0, pEffects->m_iImageWidth*pEffects->m_iImageHeight);
        memset(pOutImgU, 128, pEffects->m_iImageWidth*pEffects->m_iImageHeight/4);
        memset(pOutImgV, 128, pEffects->m_iImageWidth*pEffects->m_iImageHeight/4);
        return WMV_Succeeded;
    }
    else if (pEffects->m_fFlipAngle < 90)
    {
        // Use 1st image as input.
        pInY = pInImg1Y;
        pInU = pInImg1U;
        pInV = pInImg1V;
    }
    else
    {
        // Use 2nd image as input.
        pEffects->m_fFlipAngle -= 180;
        pInY = pInImg2Y;
        pInU = pInImg2U;
        pInV = pInImg2V;
    }

    {
        U8_WMV *pOutY = pOutImgY;
        U8_WMV *pOutU = pOutImgU;
        U8_WMV *pOutV = pOutImgV;

        I32_WMV iImageWidthUV = pEffects->m_iImageWidth >> 1;
        I32_WMV x, y, xc;
        Float_WMV f = pEffects->m_fFocalLength, tmp;
        Float_WMV sinA = (Float_WMV)(sin(pEffects->m_fFlipAngle * M_PI / 180));
        Float_WMV cosA = (Float_WMV)(cos(pEffects->m_fFlipAngle * M_PI / 180));
        Float_WMV fcosA = f * cosA;

#ifndef WMV_C_OPT_WVP2
        Bool_WMV doUVRow, doUVCol;
        I32_WMV yc, xx, yy, n;
#endif

        // Pre-calculate tranform parameters.
        for (x = 0; x < pEffects->m_iImageWidth; x++)
        {
            xc = x - pEffects->m_iCenterX;
            tmp = fcosA + xc * sinA;
            if (tmp <= 0)
            {
                pEffects->m_pFlipFlag[x] = FALSE_WMV;
            }
            else
            {
                pEffects->m_pFlipX[x] = (I32_WMV)(xc * f / tmp + pEffects->m_iCenterX + 0.5);

#ifdef WMV_C_OPT_WVP2
                if (pEffects->m_pFlipX[x] < 0 || pEffects->m_pFlipX[x]>= pEffects->m_iImageWidth)
                    pEffects->m_pFlipFlag[x] = FALSE_WMV;
                else
#endif              
                    pEffects->m_pFlipFlag[x] = TRUE_WMV;

                tmp = fcosA / tmp;
                if (tmp > 2047.0f)
                {
                    pEffects->m_pFlipYRatio[x] = 2147483647;
                }
                else if (tmp < -2047.0f) 
                {
                    pEffects->m_pFlipYRatio[x] = -2147483647;
                }
                else
                {
                    pEffects->m_pFlipYRatio[x] = (I32_WMV)(tmp * 0x100000);
                }
            }
        }

        // Transform input images into output image.

#ifdef WMV_C_OPT_WVP2

        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pOutY = WMVImageGenerateFlipOutputYLine(pOutY, pInY, pEffects, y);
        }

        for (y = 0; y < pEffects->m_iImageHeight; y+=2)
        {
            pOutU = WMVImageGenerateFlipOutputUVLine(pOutU, pInU, pOutV, pInV, pEffects, y);
            pOutV += iImageWidthUV;
        }

#else
        doUVRow = TRUE_WMV;
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            yc = y - pEffects->m_iCenterY;

            doUVCol = TRUE_WMV;
            for (x = 0; x < pEffects->m_iImageWidth; x++)
            {
                if (!pEffects->m_pFlipFlag[x])
                {
                    xx = -1;
                    yy = -1;
                }
                else
                {
                    xx = pEffects->m_pFlipX[x];
                    yy = ((yc * pEffects->m_pFlipYRatio[x]) >> 20) + pEffects->m_iCenterY;
                }
            
                if (xx < 0 || xx >= pEffects->m_iImageWidth || yy < 0 || yy >= pEffects->m_iImageHeight)
                {
                    // Y
                    *pOutY++ = 0;

                    // U, V
                    if (doUVRow && doUVCol)
                    {
                        *pOutU++ = 128;
                        *pOutV++ = 128;
                    }
                }
                else 
                {
                    // Y
                    *pOutY++ = pInY[yy*pEffects->m_iImageWidth+xx];

                    // U, V
                    if (doUVRow && doUVCol)
                    {
                        n = (yy >> 1) * iImageWidthUV + (xx >> 1);
                        *pOutU++ = pInU[n];
                        *pOutV++ = pInV[n];
                    }
                }

                doUVCol = !doUVCol;
            }

            doUVRow = !doUVRow;
        }
#endif
    }

    return WMV_Succeeded;
}

#if defined(WMV_C_OPT_WVP2) && !defined(WMV_OPT_WVP2_ARM)

void WMVImageFadeColorQuickFillY(U8_WMV *pOutY, U8_WMV *pIn1Y, U8_WMV *pIn2Y, I32_WMV iSize, I32_WMV iBackgroundY)
{
    int x;
    I32_WMV iY;

    for (x = 0; x < iSize; x++)
    {
        iY = pIn1Y[x] + pIn2Y[x] + iBackgroundY;
        assert(iY>=0);

        if (iY > 255)
            iY = 255;
        //iY &= 0xff;

        pOutY[x] = (U8_WMV)(iY);
    }
}

void WMVImageFadeColorQuickFillUV(U8_WMV *pOutU, U8_WMV *pIn1U, U8_WMV *pIn2U, I32_WMV iSize, I32_WMV iBackgroundU)
{
    int x;
    I32_WMV iU;

    for (x = 0; x < iSize; x++)
    {
        iU = pIn1U[x] + pIn2U[x] + iBackgroundU - 128;

        if (iU<0)   iU=0;
        else
        if (iU > 255)
            iU = 255;

        pOutU[x] = (U8_WMV)(iU);
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////
// GenerateFadeToColorOutput()
//
// Blends input images and background color into output image.
//
tWMVDecodeStatus WMVImage_GenerateFadeToColorOutput
(
    WMVIMAGE_EFFECTS    *pEffects,
    U8_WMV *pInImg1Y,
    U8_WMV *pInImg1U,
    U8_WMV *pInImg1V,
    U8_WMV *pInImg2Y,
    U8_WMV *pInImg2U,
    U8_WMV *pInImg2V,
    U8_WMV *pOutImgY,
    U8_WMV *pOutImgU,
    U8_WMV *pOutImgV
)
{
    U8_WMV *pIn1Y = pInImg1Y;
    U8_WMV *pIn1U = pInImg1U;
    U8_WMV *pIn1V = pInImg1V;

    U8_WMV *pIn2Y = pInImg2Y;
    U8_WMV *pIn2U = pInImg2U;
    U8_WMV *pIn2V = pInImg2V;

    U8_WMV *pOutY = pOutImgY;
    U8_WMV *pOutU = pOutImgU;
    U8_WMV *pOutV = pOutImgV;

    I32_WMV iBackgroundY = (I32_WMV)(( 0.257 * pEffects->m_iBackgroundRed + 0.504 * pEffects->m_iBackgroundGreen + 0.098 * pEffects->m_iBackgroundBlue) * pEffects->m_fBackgroundWeight);
    I32_WMV iBackgroundU = (I32_WMV)((-0.148 * pEffects->m_iBackgroundRed - 0.291 * pEffects->m_iBackgroundGreen + 0.439 * pEffects->m_iBackgroundBlue) * pEffects->m_fBackgroundWeight);
    I32_WMV iBackgroundV = (I32_WMV)(( 0.439 * pEffects->m_iBackgroundRed - 0.368 * pEffects->m_iBackgroundGreen - 0.071 * pEffects->m_iBackgroundBlue) * pEffects->m_fBackgroundWeight);

    if (pInImg1Y == NULL || pInImg1U == NULL || pInImg1V == NULL || 
        pInImg2Y == NULL || pInImg2U == NULL || pInImg2V == NULL || 
        pOutImgY == NULL || pOutImgU == NULL || pOutImgV == NULL)
    {
        return WMV_InValidArguments;
    }

    {
#ifdef WMV_C_OPT_WVP2

        I32_WMV iSize = pEffects->m_iImageHeight*pEffects->m_iImageWidth;

        if (iBackgroundY > 255)
            iBackgroundY = 255;
        else if (iBackgroundY < 0)
            iBackgroundY = 0;

        if (iBackgroundU > 384)
            iBackgroundU = 384;
        else if (iBackgroundU < -384)
            iBackgroundU = -384;

        if (iBackgroundV > 384)
            iBackgroundV = 384;
        else if (iBackgroundV < -384)
            iBackgroundV = -384;

        WMVImageFadeColorQuickFillY(pOutY, pIn1Y, pIn2Y, iSize, iBackgroundY);
        WMVImageFadeColorQuickFillUV(pOutU, pIn1U, pIn2U, iSize>>2, iBackgroundU);
        WMVImageFadeColorQuickFillUV(pOutV, pIn1V, pIn2V, iSize>>2, iBackgroundV);
#else

        I32_WMV x, y, n, offset;

        // Blend input images into output image: Y component.
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            offset = y * pEffects->m_iImageWidth;

            for (x = 0; x < pEffects->m_iImageWidth; x++)
            {
                I32_WMV iY = pIn1Y[offset+x] + pIn2Y[offset+x] + iBackgroundY;
                if (iY > 255)
                {
                    iY = 255;
                }
                else if (iY < 0)
                {
                    iY = 0;
                }
                pOutY[offset+x] = (U8_WMV)(iY);
            }
        }

        // Blend input images into output image: UV components.
        for (y = 0; y < pEffects->m_iImageHeight; y += 2)
        {
            offset = y * pEffects->m_iImageWidth / 4;

            for (x = 0; x < pEffects->m_iImageWidth; x+=2)
            {
                I32_WMV iU, iV;
                n = offset + ((x >> 1));

                iU = pIn1U[n] + pIn2U[n] + iBackgroundU - 128;
                if (iU > 255)
                {
                    iU = 255;
                }
                else if (iU < 0)
                {
                    iU = 0;
                }
                pOutU[n] = (U8_WMV)(iU);

                iV = pIn1V[n] + pIn2V[n] + iBackgroundV - 128;
                if (iV > 255)
                {
                    iV = 255;
                }
                else if (iV < 0)
                {
                    iV = 0;
                }
                pOutV[n] = (U8_WMV)(iV);
            }
        }
#endif
    }

    return WMV_Succeeded;
}

#define NUMVERT_GETBOWTIE   4
#define NUMVERT_GETCIRCLE   2
#define NUMVERT_GETDIAMOND  3
#define NUMVERT_GETIRIS     6
#define NUMVERT_GETRECT     2
#define NUMVERT_GETSTAR     6
#define NUMVERT_GETWHEEL    6

////////////////////////////////////////////////////////////////////////////////
// GetBowTieBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetBowTieBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fWidth,
    const Float_WMV fHeight
)
{
//    const I32_WMV iNumVertices = 4;
    Float_WMV X[NUMVERT_GETBOWTIE], Y[NUMVERT_GETBOWTIE];
    I32_WMV y, n, yEnd;

    // Specifial case: fWidth <= 0 || fHeight <= 0
    if (fWidth <= 0 || fHeight <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }

        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    X[0] = -fWidth / 2;
    Y[0] = pEffects->m_iImageHeight / 2.0F;
    X[1] = 0;
    Y[1] = Y[0] - fHeight;
    X[2] = 0;
    Y[2] = -Y[1];
    X[3] = X[0];
    Y[3] = -Y[0];

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETBOWTIE; n++)
    {
        X[n] += fCenterX;
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[1] <= fCenterY)? Y[1] : fCenterY);
    if (yEnd > pEffects->m_iImageHeight) yEnd = pEffects->m_iImageHeight;

    {
        Float_WMV tmp = (X[1] - X[0]) / (Y[1] - Y[0]);
        for (y = 0; y < yEnd; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[0]) * tmp + X[0] + 0.5);
        }

        // 1st segment
        yEnd = (I32_WMV)((Y[2] <= pEffects->m_iImageHeight)? Y[2] : pEffects->m_iImageHeight);
        for (y = y; y < yEnd; y++) 
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
        }

        // 2nd segment
        yEnd = (I32_WMV)((Y[3] <= pEffects->m_iImageHeight)? Y[3] : pEffects->m_iImageHeight);
        tmp = (X[3] - X[2]) / (Y[3] - Y[2]);
        for (y = y; y < yEnd; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[2]) * tmp + X[2] + 0.5);
        }
    }

    // Compute inner boundary locations.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    // Mirror the left side.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pOuterLeftX[y] + 0.5);
        pEffects->m_pInnerRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pInnerLeftX[y] + 0.5);
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// GetCircleBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetCircleBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fRadius
)
{
//    const I32_WMV iNumVertices = 2;
    Float_WMV X[NUMVERT_GETCIRCLE], Y[NUMVERT_GETCIRCLE];
    I32_WMV y, n, yEnd;

    // Specifial case: fRadius <= 0
    if (fRadius <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }

        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    X[0] = 0;
    Y[0] = fRadius;
    X[1] = 0;
    Y[1] = -fRadius;

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETCIRCLE; n++)
    {
        X[n] += fCenterX;
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[0] <= pEffects->m_iImageHeight)? Y[0] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }


    {
        Float_WMV fRadiusSq = fRadius * fRadius;

        // 1st segment
        yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);
        for (y = y; y < yEnd; y++) 
        {
            double tmp = (double)(fRadiusSq - (y - fCenterY) * (y - fCenterY));
            if (tmp <= 0)
            {
                pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            }
            else
            {
                pEffects->m_pOuterLeftX[y] = (I32_WMV)(-sqrt(tmp) + fCenterX + 0.5);
            }
        }
    }

    // 3rd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // Compute inner boundary locations.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    // Mirror the left side.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pOuterLeftX[y] + 0.5);
        pEffects->m_pInnerRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pInnerLeftX[y] + 0.5);
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// GetDiamondBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetDiamondBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fWidth,
    const Float_WMV fHeight
)
{
//    const I32_WMV iNumVertices = 3;
    Float_WMV X[NUMVERT_GETDIAMOND], Y[NUMVERT_GETDIAMOND];
    I32_WMV y, n, yEnd;

    // Specifial case: fWidth <= 0 || fHeight <= 0
    if (fWidth <= 0 || fHeight <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }

        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    X[0] = 0;
    Y[0] = fHeight / 2;
    X[1] = -fWidth / 2;
    Y[1] = 0;
    X[2] = 0;
    Y[2] = -Y[0];

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETDIAMOND; n++)
    {
        X[n] += fCenterX;
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[0] <= pEffects->m_iImageHeight)? Y[0] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);

    {
        Float_WMV tmp = (X[1] - X[0]) / (Y[1] - Y[0]);
        for (y = y; y < yEnd; y++) 
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[0]) * tmp + X[0] + 0.5);
        }

        // 2nd segment
        yEnd = (I32_WMV)((Y[2] <= pEffects->m_iImageHeight)? Y[2] : pEffects->m_iImageHeight);
        tmp = (X[2] - X[1]) / (Y[2] - Y[1]);
        for (y = y; y < yEnd; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[1]) * tmp + X[1] + 0.5);
        }
    }

    // 3rd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // Compute inner boundary locations.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    // Mirror the left side.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pOuterLeftX[y] + 0.5);
        pEffects->m_pInnerRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pInnerLeftX[y] + 0.5);
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// GetIrisBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetIrisBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fWidth,
    const Float_WMV fHeight
)
{
//    const I32_WMV iNumVertices = 6;
    Float_WMV X[NUMVERT_GETIRIS], Y[NUMVERT_GETIRIS];
    I32_WMV y, n, yEnd;

    // Specifial case: fWidth <= 0 || fHeight <= 0
    if (fWidth <= 0 || fHeight <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }

        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    X[0] = -fWidth / 2;
    Y[0] = pEffects->m_iImageHeight / 2.0F;
    X[1] = X[0];
    Y[1] = fHeight / 2;
    X[2] = -pEffects->m_iImageWidth / 2.0F;
    Y[2] = Y[1];
    X[3] = X[2];
    Y[3] = -Y[2];
    X[4] = X[1];
    Y[4] = -Y[1];
    X[5] = X[0];
    Y[5] = -Y[0];

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETIRIS; n++)
    {
        X[n] += (I32_WMV)(fCenterX + 0.5);
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(X[0] + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[3] <= pEffects->m_iImageHeight)? Y[3] : pEffects->m_iImageHeight);
    for (y = y; y < yEnd; y++) 
    {
        pEffects->m_pOuterLeftX[y] = 0;
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(X[0] + 0.5);
    }

    // Compute inner boundary locations.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    // Mirror the left side.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pOuterLeftX[y] + 0.5);
        pEffects->m_pInnerRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pInnerLeftX[y] + 0.5);
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// GetRectangleBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetRectangleBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fWidth,
    const Float_WMV fHeight
)
{
//    const I32_WMV iNumVertices = 2;
    Float_WMV X[NUMVERT_GETRECT], Y[NUMVERT_GETRECT];
    I32_WMV y, n, yEnd;

    // Specifial case: fWidth <= 0 || fHeight <= 0
    if (fWidth <= 0 || fHeight <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }

        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    X[0] = -fWidth / 2;
    Y[0] = fHeight / 2;
    X[1] = X[0];
    Y[1] = -Y[0];

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETRECT; n++)
    {
        X[n] += (I32_WMV)(fCenterX + 0.5);
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[0] <= pEffects->m_iImageHeight)? Y[0] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);
    for (y = y; y < yEnd; y++) 
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(X[0] + 0.5);
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // Compute inner boundary locations.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    // Mirror the left side.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pOuterLeftX[y] + 0.5);
        pEffects->m_pInnerRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pInnerLeftX[y] + 0.5);
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// GetStarBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetStarBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fRadius
)
{
//    const I32_WMV iNumVertices = 6;
    Float_WMV X[NUMVERT_GETSTAR], Y[NUMVERT_GETSTAR];
    I32_WMV y, n, yEnd;
    Float_WMV fCenterYOffset;

    // Specifial case: fRadius <= 0
    if (fRadius <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }
        
        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    X[0] = 0;
    Y[0] = fRadius;
    X[1] = (Float_WMV)(-fRadius * 0.22451398828979268622097257589876);
    Y[1] = (Float_WMV)( fRadius * 0.30901699437494742410229341718282);
    X[2] = (Float_WMV)(-fRadius * 0.95105651629515357211643933337938);
    Y[2] = Y[1];
    X[3] = (Float_WMV)(-fRadius * 0.36327126400268044294773337874031);
    Y[3] = (Float_WMV)(-fRadius * 0.11803398874989484820458683436564);
    X[4] = (Float_WMV)(-fRadius * 0.58778525229247312916870595463907);
    Y[4] = (Float_WMV)(-fRadius * 0.80901699437494742410229341718282);
    X[5] = 0;
    Y[5] = (Float_WMV)(-fRadius * 0.38196601125010515179541316563436);

    // Transform coordinates to image coordinate system.
    fCenterYOffset = (Y[0] + Y[4]) / 2;
    for (n = 0; n < NUMVERT_GETSTAR; n++)
    {
        X[n] += (I32_WMV)(fCenterX + 0.5);
        Y[n] = -Y[n] + fCenterY + fCenterYOffset;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[0] <= pEffects->m_iImageHeight)? Y[0] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);

    {
        Float_WMV tmp = (X[1] - X[0]) / (Y[1] - Y[0]);
        for (y = y; y < yEnd; y++) 
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[0]) * tmp + X[0] + 0.5);
        }

        // 2nd segment
        yEnd = (I32_WMV)((Y[3] <= pEffects->m_iImageHeight)? Y[3] : pEffects->m_iImageHeight);
        tmp = (X[3] - X[2]) / (Y[3] - Y[2]);
        for (y = y; y < yEnd; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[2]) * tmp + X[2] + 0.5);
        }

        // 3rd segment
        tmp = (X[4] - X[3]) / (Y[4] - Y[3]);
        yEnd = (I32_WMV)((Y[4] <= pEffects->m_iImageHeight)? Y[4] : pEffects->m_iImageHeight);
        for (y = y; y < yEnd; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - Y[3]) * tmp + X[3] + 0.5);
        }

        // 4th segment
        for (y = y; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
        }

        // Compute inner boundary locations.
        // 0th segment
        yEnd = (I32_WMV)((Y[5] <= pEffects->m_iImageHeight)? Y[5] : pEffects->m_iImageHeight);
        for (y = 0; y < yEnd; y++)
        {
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
        }

        // 1st segment
        tmp = (X[5] - X[4]) / (Y[5] - Y[4]);
        yEnd = (I32_WMV)((Y[4] <= pEffects->m_iImageHeight)? Y[4] : pEffects->m_iImageHeight);
        for (y = y; y < yEnd; y++) 
        {
            pEffects->m_pInnerLeftX[y] = (I32_WMV)((y - Y[4]) * tmp + X[4] + 0.5);
        }
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    // Mirror the left side.
    for (y = 0; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pOuterLeftX[y] + 0.5);
        pEffects->m_pInnerRightX[y] = (I32_WMV)(2 * fCenterX - pEffects->m_pInnerLeftX[y] + 0.5);
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// GetWheelBoundary()
//
// Computes the boundary locations.
//
tWMVDecodeStatus WMVImage_GetWheelBoundary
(
    WMVIMAGE_EFFECTS    *pEffects,
    const Float_WMV fCenterX,
    const Float_WMV fCenterY,
    const Float_WMV fAngle
)
{
//    const I32_WMV iNumVertices = 6;
    Float_WMV X[NUMVERT_GETWHEEL], Y[NUMVERT_GETWHEEL];
    I32_WMV y, n, yEnd;
    Float_WMV tanH, tanV, tanA;
         
    // Specifial cases: iAngle <= 0 || iAngle >= 90
    if (fAngle <= 0)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }
        
        return WMV_Succeeded;
    }
    else if (fAngle >= 90)
    {
        for (y = 0; y < pEffects->m_iImageHeight; y++)
        {
            pEffects->m_pOuterLeftX[y] = 0;
            pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
            pEffects->m_pOuterRightX[y] = pEffects->m_iImageWidth - 1;
            pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
        }
        
        return WMV_Succeeded;
    }

    // LEFT SIDE
    // Compute vertex coordinates.
    tanH = (Float_WMV)(pEffects->m_iImageHeight) / pEffects->m_iImageWidth;
    tanV = (Float_WMV)(pEffects->m_iImageWidth) / pEffects->m_iImageHeight;
    tanA = (Float_WMV)(tan(fAngle * M_PI / 180));
    X[0] = 0;
    Y[0] = (Float_WMV)(pEffects->m_iImageHeight);
    X[1] = (tanA < tanV)? -Y[0] * tanA : -(Float_WMV)(pEffects->m_iImageWidth);
    Y[1] = (tanA < tanV)? Y[0] : -X[1] / tanA;
    X[2] = 0;
    Y[2] = 0;
    X[3] = -(Float_WMV)(pEffects->m_iImageWidth);
    Y[3] = 0;
    Y[4] = (tanA < tanH)? X[3] * tanA : -(Float_WMV)(pEffects->m_iImageHeight);
    X[4] = (tanA < tanH)? X[3] : Y[4] / tanA;
    X[5] = 0;
    Y[5] = 0;

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETWHEEL; n++)
    {
        X[n] += (I32_WMV)(fCenterX + 0.5);
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((fCenterY <= pEffects->m_iImageHeight)? fCenterY : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++) 
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)((y - fCenterY) * tanA + fCenterX + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[4] <= pEffects->m_iImageHeight)? Y[4] : pEffects->m_iImageHeight);
    for (y = y; y < yEnd; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(X[3] + 0.5);
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pOuterLeftX[y] = (I32_WMV)(fCenterX + 1 + 0.5);
    }

    // Compute inner boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((fCenterY <= pEffects->m_iImageHeight)? fCenterY : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++) 
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[4] <= pEffects->m_iImageHeight)? Y[4] : pEffects->m_iImageHeight);
    for (y = y; y < yEnd; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(-(y - fCenterY) / tanA + fCenterX + 0.5);
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++)
    {
        pEffects->m_pInnerLeftX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // RIGHT SIDE
    X[0] = 0;
    Y[0] = 0;
    X[1] = (tanA < tanH)? (Float_WMV)(pEffects->m_iImageWidth) : (Float_WMV)(pEffects->m_iImageHeight) / tanA;
    Y[1] = X[1] * tanA;
    X[2] = (Float_WMV)(pEffects->m_iImageWidth);
    Y[2] = 0;
    X[3] = 0;
    Y[3] = 0;
    X[4] = (tanA < tanV)? (Float_WMV)(pEffects->m_iImageHeight) * tanA : (Float_WMV)(pEffects->m_iImageWidth);
    Y[4] = -X[4] / tanA;
    X[5] = 0;
    Y[5] = 0;

    // Transform coordinates to image coordinate system.
    for (n = 0; n < NUMVERT_GETWHEEL; n++)
    {
        X[n] += (I32_WMV)(fCenterX + 0.5);
        Y[n] = -Y[n] + fCenterY;
    }

    // Compute outer boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(fCenterX - 1 + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[3] <= pEffects->m_iImageHeight)? Y[3] : pEffects->m_iImageHeight);
    for (y = y; y < yEnd; y++)
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)(X[2] + 0.5);
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++) 
    {
        pEffects->m_pOuterRightX[y] = (I32_WMV)((y - fCenterY) * tanA + fCenterX + 0.5);
    }

    // Compute inner boundary locations.
    // 0th segment
    yEnd = (I32_WMV)((Y[1] <= pEffects->m_iImageHeight)? Y[1] : pEffects->m_iImageHeight);
    for (y = 0; y < yEnd; y++)
    {
        pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);
    }

    // 1st segment
    yEnd = (I32_WMV)((Y[3] <= pEffects->m_iImageHeight)? Y[3] : pEffects->m_iImageHeight);
    for (y = y; y < yEnd; y++)
    {
        pEffects->m_pInnerRightX[y] = (I32_WMV)(-(y - fCenterY) / tanA + fCenterX + 0.5);
    }

    // 2nd segment
    for (y = y; y < pEffects->m_iImageHeight; y++) 
    {
        pEffects->m_pInnerRightX[y] = (I32_WMV)(fCenterX + 0.5);;
    }

    return WMV_Succeeded;
}

////////////////////////////////////////////////////////////////////////////////
// WMVImage_EffectGenerate()
//
// Blends two input images to create a desired effect.
//
tWMVDecodeStatus WMVImage_EffectGenerate(
	tWMVDecInternalMember *pWMVDec,
    WMVIMAGE_EFFECTS    *pEffects,
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
)
{
    tWMVDecodeStatus hr;
    if (iImageWidth <= 0 || iImageHeight <= 0)
    {
        return WMV_InValidArguments;
    }
    else if (iNumEffectParameters > 0 && pEffectParameters == NULL)
    {
        return WMV_Failed;
    }
    else if (pInImg1Y == NULL || pInImg1U == NULL || pInImg1V == NULL || 
        pInImg2Y == NULL || pInImg2U == NULL || pInImg2V == NULL || 
        pOutImgY == NULL || pOutImgU == NULL || pOutImgV == NULL)
    {
        return WMV_Failed;
    }

    // Allocate memory for storing boundary locations.
    hr = WMVImage_AllocateMemory(pWMVDec,pEffects, iImageWidth, iImageHeight);
    if (hr != WMV_Succeeded)
    {
        return hr;
    }

    // Get boundary locations.
    pEffects->m_iImageWidth = iImageWidth;
    pEffects->m_iImageHeight = iImageHeight;

    switch (iEffectType)
    {
    case WMVP_TRANSITION_BOW_TIE:
        WMVImage_GetBowTieBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2], pEffectParameters[3]);

        pEffects->m_iCenterX = (I32_WMV)pEffectParameters[0];
        pEffects->m_iCenterY = (I32_WMV)pEffectParameters[1];
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[4]);
        break;
    case WMVP_TRANSITION_CIRCLE:
        WMVImage_GetCircleBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2]);

        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[3]);
        break;
    case WMVP_TRANSITION_DIAMOND:
    case WMVP_TRANSITION_DIAGONAL:
    case WMVP_TRANSITION_FILLED_V:
        WMVImage_GetDiamondBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2], pEffectParameters[3]);

        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[4]);
        break;
    case WMVP_TRANSITION_FADE_TO_COLOR:
        pEffects->m_iBackgroundRed = (I32_WMV)((pEffectParameters[0] < 0)? 0 : ((pEffectParameters[0] > 255.0f)? 255 : (I32_WMV)(pEffectParameters[0])));
        pEffects->m_iBackgroundGreen = (I32_WMV)((pEffectParameters[1] < 0)? 0 : ((pEffectParameters[1] > 255.0f)? 255 : (I32_WMV)(pEffectParameters[1])));
        pEffects->m_iBackgroundBlue = (I32_WMV)((pEffectParameters[2] < 0)? 0 : ((pEffectParameters[2] > 255.0f)? 255 : (I32_WMV)(pEffectParameters[2])));
        pEffects->m_fBackgroundWeight = pEffectParameters[3];
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[4]);
        break;
    case WMVP_TRANSITION_FLIP:
        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_fFlipAngle = pEffectParameters[2];
        pEffects->m_fFocalLength = pEffectParameters[3];
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[4]);
        break;
    case WMVP_TRANSITION_IRIS:
        WMVImage_GetIrisBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2], pEffectParameters[3]);

        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[4]);
        break;
    case WMVP_TRANSITION_RECTANGLE:
    case WMVP_TRANSITION_INSET:
    case WMVP_TRANSITION_REVEAL:
    case WMVP_TRANSITION_SLIDE:
    case WMVP_TRANSITION_SPLIT:
        WMVImage_GetRectangleBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2], pEffectParameters[3]);

        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[4]);
        break;
    case WMVP_TRANSITION_STAR:
        WMVImage_GetStarBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2]);

        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[3]);
        break;
    case WMVP_TRANSITION_WHEEL:
        WMVImage_GetWheelBoundary(pEffects, pEffectParameters[0], pEffectParameters[1],
            pEffectParameters[2]);

        pEffects->m_iCenterX = (I32_WMV)(pEffectParameters[0]);
        pEffects->m_iCenterY = (I32_WMV)(pEffectParameters[1]);
        pEffects->m_iCompositionOrder = (I32_WMV)(pEffectParameters[3]);
        break;
    default:
        return WMV_Failed;
        break;
    }

    // Generate the effect.
    switch (iEffectType)
    {
    case WMVP_TRANSITION_BOW_TIE:
    case WMVP_TRANSITION_CIRCLE:
    case WMVP_TRANSITION_DIAMOND:
    case WMVP_TRANSITION_DIAGONAL:
    case WMVP_TRANSITION_FILLED_V:
    case WMVP_TRANSITION_IRIS:
    case WMVP_TRANSITION_RECTANGLE:
    case WMVP_TRANSITION_INSET:
    case WMVP_TRANSITION_REVEAL:
    case WMVP_TRANSITION_SPLIT:
    case WMVP_TRANSITION_STAR:
    case WMVP_TRANSITION_WHEEL:
        switch (pEffects->m_iCompositionOrder)
        {
        case WMVP_REVERSED:
            WMVImage_GenerateOutput(pEffects, pInImg2Y, pInImg2U, pInImg2V, pInImg1Y, pInImg1U, pInImg1V,
                pOutImgY, pOutImgU, pOutImgV);
                break;
        case WMVP_NORMAL:
        default:
            WMVImage_GenerateOutput(pEffects, pInImg1Y, pInImg1U, pInImg1V, pInImg2Y, pInImg2U, pInImg2V,
                pOutImgY, pOutImgU, pOutImgV);
            break;
        }
        break;
    case WMVP_TRANSITION_FLIP:
        switch (pEffects->m_iCompositionOrder)
        {
        case WMVP_REVERSED:
            WMVImage_GenerateFlipOutput(pEffects, pInImg2Y, pInImg2U, pInImg2V, pInImg1Y, pInImg1U, pInImg1V,
                pOutImgY, pOutImgU, pOutImgV);
            break;
        case WMVP_NORMAL:
        default:
            WMVImage_GenerateFlipOutput(pEffects, pInImg1Y, pInImg1U, pInImg1V, pInImg2Y, pInImg2U, pInImg2V,
                pOutImgY, pOutImgU, pOutImgV);
            break;
        }
        break;
    case WMVP_TRANSITION_FADE_TO_COLOR:
        switch (pEffects->m_iCompositionOrder)
        {
        case WMVP_REVERSED:
            WMVImage_GenerateFadeToColorOutput(pEffects, pInImg2Y, pInImg2U, pInImg2V, pInImg1Y, pInImg1U, pInImg1V,
                pOutImgY, pOutImgU, pOutImgV);
            break;
        case WMVP_NORMAL:
        default:
            WMVImage_GenerateFadeToColorOutput(pEffects, pInImg1Y, pInImg1U, pInImg1V, pInImg2Y, pInImg2U, pInImg2V,
                    pOutImgY, pOutImgU, pOutImgV);
            break;
        }
        break;
    default:
        return WMV_Failed;
        break;
    }

    return WMV_Succeeded;
}

#endif //WMVIMAGE_V2
