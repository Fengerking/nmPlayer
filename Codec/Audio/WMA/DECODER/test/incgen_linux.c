/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		incgen_linux.c
* 
* Abstact:	
*
*		Output inc file to get the offset value of stuct for Assembly.
*
* Author:
*
*		Witten Wen 18-March-2009
*
* Revision History:
*
******************************************************/
#include	<stdio.h>
//#include	<stddef.h>
//#include	"macros.h"
//#include	"msaudio.h"
#include	"msaudiodec.h"
#include	"strmdec_wma.h"
#include	"huffdec.h"

#ifdef LINUX
const char *incname = "/sdcard/wma_member_arm.inc";
#else	//LINUX
//const char *incname = "\\Storage Card\\WMA\\wma_member_arm.inc";
const char *incname = "\\ResidentFlash\\wma2\\wma_member_arm.inc";
//const char *incname = "\\SDMMC\\WMA\\wma_member_arm.inc";
#endif	//LINUX

int EnableArm	=	1;
int DisableArm	=	0;

int main( int argc, char** argv )
{
	FILE *hinc = NULL;
	int offset, StructSize;
	char introduce1[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
	char introduce2[] = "@ Filename wma_arm_version.h";
	char introduce3[] = "@";
	char introduce4[] = "@ Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.";
	char introduce5[] = "@";
	char introduce6[] = "@ VisualOn WMA decoder assembly macros define";
	char introduce7[] = "@";
	char introduce8[] = "@ Author:";
	char introduce9[] = "@";
	char introduce10[] = "@		Witten Wen 9-April-2008";
	char introduce11[] = "@";
	char introduce12[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";

	hinc = fopen(incname, "wb");
	
	//file introduce 
	fprintf(hinc, "%s\n%s\n%s\n%s\n", introduce1, introduce2, introduce3, introduce4);
	fprintf(hinc, "%s\n%s\n%s\n%s\n", introduce5, introduce6, introduce7, introduce8);
	fprintf(hinc, "%s\n%s\n%s\n%s\n\n", introduce9, introduce10, introduce11, introduce12);

	//struct CAudioObjectDecoder
	offset = offsetof(CAudioObjectDecoder, pau);
	fprintf(hinc, ".equ		CAudioObjectDecoder_pau                  ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_LLMdecsts);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_LLMdecsts          ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_Colombdecsts);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_Colombdecsts       ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_iResQ);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_iResQ              ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_iResQBits);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_iResQBits          ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_ibstrm);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_ibstrm             ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_fChannelFoldDown);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_fChannelFoldDown   ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_fReQuantizeTo16);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_fReQuantizeTo16    ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_cDstChannel);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_cDstChannel        ,	%d\n", offset);	
	offset = offsetof(CAudioObjectDecoder, m_rgrgfltChDnMixMtx);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_rgrgfltChDnMixMtx  ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_pctTemp);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_pctTemp            ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_ppcbtTemp);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_ppcbtTemp          ,	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_fLtRtDownmix);
	fprintf(hinc, ".equ		CAudioObjectDecoder_m_fLtRtDownmix       ,	%d\n", offset);
	

	//struct CAudioObject
	fprintf(hinc, "\n");
	offset = offsetof(CAudioObject, m_qstQuantStep);
	fprintf(hinc, ".equ		CAudioObject_m_qstQuantStep              ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_cChannel);
	fprintf(hinc, ".equ		CAudioObject_m_cChannel                  ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_fNoiseSub);
	fprintf(hinc, ".equ		CAudioObject_m_fNoiseSub                 ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_iMaxEscSize);
	fprintf(hinc, ".equ		CAudioObject_m_iMaxEscSize               ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_iBitDepthSelector);
	fprintf(hinc, ".equ		CAudioObject_m_iBitDepthSelector         ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_nValidBitsPerSample);
	fprintf(hinc, ".equ		CAudioObject_m_nValidBitsPerSample       ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_iPCMSampleMin);
	fprintf(hinc, ".equ		CAudioObject_m_iPCMSampleMin             ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_iPCMSampleMax);
	fprintf(hinc, ".equ		CAudioObject_m_iPCMSampleMax             ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_bUnifiedPureLLMCurrFrm);
	fprintf(hinc, ".equ		CAudioObject_m_bUnifiedPureLLMCurrFrm    ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_bFirstUnifiedPureLLMFrm);
	fprintf(hinc, ".equ		CAudioObject_m_bFirstUnifiedPureLLMFrm   ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_bPureLosslessMode);
	fprintf(hinc, ".equ		CAudioObject_m_bPureLosslessMode         ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_bDoInterChDecorr);
	fprintf(hinc, ".equ		CAudioObject_m_bDoInterChDecorr          ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_bSeekable);
	fprintf(hinc, ".equ		CAudioObject_m_bSeekable                 ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_iCurrReconCoef);
	fprintf(hinc, ".equ		CAudioObject_m_iCurrReconCoef            ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_cFrameSampleHalf);
	fprintf(hinc, ".equ		CAudioObject_m_cFrameSampleHalf          ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLowCutOff);
	fprintf(hinc, ".equ		CAudioObject_m_cLowCutOff                ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_cHighCutOff);
	fprintf(hinc, ".equ		CAudioObject_m_cHighCutOff               ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_cValidBarkBand);
	fprintf(hinc, ".equ		CAudioObject_m_cValidBarkBand            ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_rgiBarkIndex);
	fprintf(hinc, ".equ		CAudioObject_m_rgiBarkIndex              ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_rgpcinfo);
	fprintf(hinc, ".equ		CAudioObject_m_rgpcinfo                  ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_iFirstNoiseIndex);
	fprintf(hinc, ".equ		CAudioObject_m_iFirstNoiseIndex          ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_tRandState);
	fprintf(hinc, ".equ		CAudioObject_m_tRandState                ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_cChInTile);
	fprintf(hinc, ".equ		CAudioObject_m_cChInTile                 ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_rgiChInTile);
	fprintf(hinc, ".equ		CAudioObject_m_rgiChInTile               ,	%d\n", offset);	
	offset = offsetof(CAudioObject, m_iResidueMemScaling);
	fprintf(hinc, ".equ		CAudioObject_m_iResidueMemScaling        ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLastCodedIndex);
	fprintf(hinc, ".equ		CAudioObject_m_cLastCodedIndex           ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLeftShiftBitsFixedPre);
	fprintf(hinc, ".equ		CAudioObject_m_cLeftShiftBitsFixedPre    ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLeftShiftBitsFixedPost);
	fprintf(hinc, ".equ		CAudioObject_m_cLeftShiftBitsFixedPost   ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_bFreqex);
	fprintf(hinc, ".equ		CAudioObject_m_bFreqex                   ,	%d\n", offset);
	offset = offsetof(CAudioObject, m_bDoFexOnCodedChannels);
	fprintf(hinc, ".equ		CAudioObject_m_bDoFexOnCodedChannels     ,	%d\n", offset);	

	//struct PerChannelInfo
	fprintf(hinc, "\n");
	StructSize = sizeof(PerChannelInfo);
	fprintf(hinc, ".equ		PerChannelInfo_size                      ,	%d\n", StructSize);
	offset = offsetof(PerChannelInfo, m_rgiCoefQ);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgiCoefQ                ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgbBandNotCoded);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgbBandNotCoded         ,	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_rgffltSqrtBWRatio);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgffltSqrtBWRatio       ,	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_rgiNoisePower);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgiNoisePower           ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgiCoefRecon);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgiCoefRecon            ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgiPCMBuffer);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgiPCMBuffer            ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiSinRampUpStart);
	fprintf(hinc, ".equ		PerChannelInfo_m_fiSinRampUpStart        ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiCosRampUpStart);
	fprintf(hinc, ".equ		PerChannelInfo_m_fiCosRampUpStart        ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiSinRampUpPrior);
	fprintf(hinc, ".equ		PerChannelInfo_m_fiSinRampUpPrior        ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiCosRampUpPrior);
	fprintf(hinc, ".equ		PerChannelInfo_m_fiCosRampUpPrior        ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiSinRampUpStep);
	fprintf(hinc, ".equ		PerChannelInfo_m_fiSinRampUpStep         ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSubFrameSampleHalf);
	fprintf(hinc, ".equ		PerChannelInfo_m_cSubFrameSampleHalf     ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSubbandAdjusted);
	fprintf(hinc, ".equ		PerChannelInfo_m_cSubbandAdjusted        ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iSizePrev);
	fprintf(hinc, ".equ		PerChannelInfo_m_iSizePrev               ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iSizeCurr);
	fprintf(hinc, ".equ		PerChannelInfo_m_iSizeCurr               ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_wtMaxWeight);
	fprintf(hinc, ".equ		PerChannelInfo_m_wtMaxWeight             ,	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_cLeftShiftBitsQuant);
	fprintf(hinc, ".equ		PerChannelInfo_m_cLeftShiftBitsQuant     ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cLeftShiftBitsTotal);
	fprintf(hinc, ".equ		PerChannelInfo_m_cLeftShiftBitsTotal     ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_qfltMaxQuantStep);
	fprintf(hinc, ".equ		PerChannelInfo_m_qfltMaxQuantStep        ,	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_cLMSPredictors);
	fprintf(hinc, ".equ		PerChannelInfo_m_cLMSPredictors          ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iSum);
	fprintf(hinc, ".equ		PerChannelInfo_m_iSum                    ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgsubfrmconfig);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgsubfrmconfig          ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSubFrameSampleHalfWithUpdate);
	fprintf(hinc, ".equ		PerChannelInfo_m_cSubFrameSampleHalfWithUpdate ,	%d\n", offset);		
	offset = offsetof(PerChannelInfo, m_iUpdSpdUpSamples);
	fprintf(hinc, ".equ		PerChannelInfo_m_iUpdSpdUpSamples        ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iTransientPosition);
	fprintf(hinc, ".equ		PerChannelInfo_m_iTransientPosition      ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_bNoDecodeForCx);
	fprintf(hinc, ".equ		PerChannelInfo_m_bNoDecodeForCx          ,	%d\n", offset);
	//for PerChannelInfo test
/*	offset = offsetof(PerChannelInfo, m_cLastCodedIndex);
	fprintf(hinc, ".equ		PerChannelInfo_m_cLastCodedIndex         ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSamplesLeft);
	fprintf(hinc, ".equ		PerChannelInfo_m_cSamplesLeft            ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_ppcinfoShr);
	fprintf(hinc, ".equ		PerChannelInfo_m_ppcinfoShr          ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iStartPtInFile);
	fprintf(hinc, ".equ		PerChannelInfo_m_iStartPtInFile          ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgrgdblLLMLPCFilter);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgrgdblLLMLPCFilter          ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgrgfltLLMLPCFilterRec);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgrgfltLLMLPCFilterRec          ,	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgiLPCFilter);
	fprintf(hinc, ".equ		PerChannelInfo_m_rgiLPCFilter          ,	%d\n", offset);
*/
	//struct CWMAInputBitStream
	fprintf(hinc, "\n");
	offset = offsetof(CWMAInputBitStream, m_pBuffer);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_pBuffer             ,	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_cbBuflen);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_cbBuflen            ,	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_dwDot);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_dwDot               ,	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_dwBitsLeft);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_dwBitsLeft          ,	%d\n", offset);	
	offset = offsetof(CWMAInputBitStream, m_dwDotT);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_dwDotT              ,	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_cBitDotT);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_cBitDotT            ,	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_cFrmBitCnt);
	fprintf(hinc, ".equ		CWMAInputBitStream_m_cFrmBitCnt          ,	%d\n", offset);
	
	//struct MCLMSPredictor for lossless
	fprintf(hinc, "\n");
	offset = offsetof(MCLMSPredictor, m_iOrder_X_CH);
	fprintf(hinc, ".equ		MCLMSPredictor_m_iOrder_X_CH             ,	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_iScaling);
	fprintf(hinc, ".equ		MCLMSPredictor_m_iScaling                ,	%d\n", offset);
	offset = offsetof(MCLMSPredictor, m_iScalingOffset);
	fprintf(hinc, ".equ		MCLMSPredictor_m_iScalingOffset          ,	%d\n", offset);
	offset = offsetof(MCLMSPredictor, m_iRecent);
	fprintf(hinc, ".equ		MCLMSPredictor_m_iRecent                 ,	%d\n", offset);
	offset = offsetof(MCLMSPredictor, m_rgiPrevVal);
	fprintf(hinc, ".equ		MCLMSPredictor_m_rgiPrevVal              ,	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_rgiFilter);
	fprintf(hinc, ".equ		MCLMSPredictor_m_rgiFilter               ,	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_rgiFilterCurrTime);
	fprintf(hinc, ".equ		MCLMSPredictor_m_rgiFilterCurrTime       ,	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_rgiUpdate);
	fprintf(hinc, ".equ		MCLMSPredictor_m_rgiUpdate               ,	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_iUpdStepSize);
	fprintf(hinc, ".equ		MCLMSPredictor_m_iUpdStepSize            ,	%d\n", offset);
	
	//struct LMSPredictor
	fprintf(hinc, "\n");
	offset = offsetof(LMSPredictor, m_iOrder);
	fprintf(hinc, ".equ		LMSPredictor_m_iOrder                    ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iOrder_Div_8);
	fprintf(hinc, ".equ		LMSPredictor_m_iOrder_Div_8              ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iOrder_Div_16);
	fprintf(hinc, ".equ		LMSPredictor_m_iOrder_Div_16             ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iScaling);
	fprintf(hinc, ".equ		LMSPredictor_m_iScaling                  ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iScalingOffset);
	fprintf(hinc, ".equ		LMSPredictor_m_iScalingOffset            ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iUpdStepSize_X_8);
	fprintf(hinc, ".equ		LMSPredictor_m_iUpdStepSize_X_8          ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iRecent);
	fprintf(hinc, ".equ		LMSPredictor_m_iRecent                   ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_rgiPrevVal);
	fprintf(hinc, ".equ		LMSPredictor_m_rgiPrevVal                ,	%d\n", offset);
	offset = offsetof(LMSPredictor, m_rgiUpdate);
	fprintf(hinc, ".equ		LMSPredictor_m_rgiUpdate                 ,	%d\n", offset);	
	offset = offsetof(LMSPredictor, m_rgiFilter);
	fprintf(hinc, ".equ		LMSPredictor_m_rgiFilter                 ,	%d\n", offset);
	
	//struct FastFloat
	fprintf(hinc, "\n");
	offset = offsetof(FastFloat, iFracBits);
	fprintf(hinc, ".equ		FastFloat_iFracBits                      ,	%d\n", offset);
	offset = offsetof(FastFloat, iFraction);
	fprintf(hinc, ".equ		FastFloat_iFraction                      ,	%d\n", offset);

	//struct tagRandState
	fprintf(hinc, "\n");
	offset = offsetof(struct tagRandState, iPrior);
	fprintf(hinc, ".equ		tagRandState_iPrior                      ,	%d\n", offset);
	offset = offsetof(struct tagRandState, uiRand);
	fprintf(hinc, ".equ		tagRandState_uiRand                      ,	%d\n", offset);
	
	//struct huffResult
	fprintf(hinc, "\n");
	offset = offsetof(huffResult, state);
	fprintf(hinc, ".equ		HuffResult_state                         ,	%d\n", offset);
	offset = offsetof(huffResult, run);
	fprintf(hinc, ".equ		HuffResult_run                           ,	%d\n", offset);
	offset = offsetof(huffResult, level);
	fprintf(hinc, ".equ		HuffResult_level                         ,	%d\n", offset);

	//struct huffVecResult
	fprintf(hinc, "\n");
	StructSize = sizeof(huffVecResult);
	fprintf(hinc, ".equ		huffVecResult_size                       ,	%d\n", StructSize);
	offset = offsetof(huffVecResult, state);
	fprintf(hinc, ".equ		HuffVecResult_state                      ,	%d\n", offset);
	offset = offsetof(huffVecResult, iResult);
	fprintf(hinc, ".equ		HuffVecResult_iResult                    ,	%d\n", offset);	

	//struct SubFrameConfigInfo
	fprintf(hinc, "\n");
	offset = offsetof(SubFrameConfigInfo, m_rgiSubFrameStart);
	fprintf(hinc, ".equ		SubFrameConfigInfo_m_rgiSubFrameStart    ,	%d\n", offset);
	
	//macros define
	fprintf(hinc, "\n");
	fprintf(hinc, ".equ		TRANSFORM_FRACT_BITS                     ,	%d\n", 5);
	fprintf(hinc, ".equ		WEIGHTFACTOR_FRACT_BITS                  ,	%d\n", 21);

	//WMA ARM macros in msaudio.h
	fprintf(hinc, "\n");
	fprintf(hinc, ".equ		WMA_OPT_LPCLSL_ARM                       ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_LPCSTD_ARM                       ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_FFT_ARM                          ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_AURECON_ARM                      ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_INVERSQUAN_LOWRATE_ARM           ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_SUBFRAMERECON_ARM                ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_SCALE_COEFFS_V3_ARM              ,	%d\n", EnableArm);

	//WMA ARM macros in msaudiodec.h
	fprintf(hinc, "\n");
	fprintf(hinc, ".equ		WMA_OPT_STRMDEC_ARM                      ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_LOSSLESSDECLSL_ARM               ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_LOSSLESSDECPROLSL_ARM            ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_HUFFDEC_ARM                      ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_CHANNEL_DOWNMIX_ARM              ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_REQUANTO16_ARM                   ,	%d\n", EnableArm);
	fprintf(hinc, ".equ		WMA_OPT_VOMEMORY_ARM                     ,	%d\n", EnableArm);
	fprintf(hinc, "\n");

	fclose(hinc);
	
	 return 0;
}


