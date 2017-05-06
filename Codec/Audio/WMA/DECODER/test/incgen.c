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
*		incen.c
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
#if 1
#include	<stdio.h>
//#include	<stddef.h>
//#include	"macros.h"
//#include	"msaudio.h"
#include	"msaudiodec.h"
#include	"strmdec_wma.h"
#include	"huffdec.h"

#ifdef LINUX
const char *incname = "incgen\\wma_member_arm.inc";
#else	//LINUX
const char *incname = "\\Storage Card\\WMA\\wma_member_arm.inc";
//const char *incname = "\\ResidentFlash\\wma2\\wma_member_arm.inc";
//const char *incname = "\\SDMMC\\WMA\\wma_member_arm.inc";
#endif	//LINUX

int EnableArm	=	1;
int DisableArm	=	0;
void wmain( int argc, char **argv )
{
	FILE *hinc = NULL;
	int offset, StructSize;
	char introduce1[] = ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;";
	char introduce2[] = "; Filename wma_arm_version.h";
	char introduce3[] = ";";
	char introduce4[] = "; Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.";
	char introduce5[] = ";";
	char introduce6[] = "; VisualOn WMA decoder assembly macros define";
	char introduce7[] = ";";
	char introduce8[] = "; Author:";
	char introduce9[] = ";";
	char introduce10[] = ";		Witten Wen 9-April-2008";
	char introduce11[] = ";";
	char introduce12[] = ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;";

	hinc = fopen(incname, "wb");
	
	//file introduce 
	fprintf(hinc, "%s\n%s\n%s\n%s\n", introduce1, introduce2, introduce3, introduce4);
	fprintf(hinc, "%s\n%s\n%s\n%s\n", introduce5, introduce6, introduce7, introduce8);
	fprintf(hinc, "%s\n%s\n%s\n%s\n\n", introduce9, introduce10, introduce11, introduce12);

	//struct CAudioObjectDecoder
	offset = offsetof(CAudioObjectDecoder, pau);
	fprintf(hinc, "CAudioObjectDecoder_pau                  EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_LLMdecsts);
	fprintf(hinc, "CAudioObjectDecoder_m_LLMdecsts          EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_Colombdecsts);
	fprintf(hinc, "CAudioObjectDecoder_m_Colombdecsts       EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_iResQ);
	fprintf(hinc, "CAudioObjectDecoder_m_iResQ              EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_iResQBits);
	fprintf(hinc, "CAudioObjectDecoder_m_iResQBits          EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_ibstrm);
	fprintf(hinc, "CAudioObjectDecoder_m_ibstrm             EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_fChannelFoldDown);
	fprintf(hinc, "CAudioObjectDecoder_m_fChannelFoldDown   EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_fReQuantizeTo16);
	fprintf(hinc, "CAudioObjectDecoder_m_fReQuantizeTo16    EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_cDstChannel);
	fprintf(hinc, "CAudioObjectDecoder_m_cDstChannel        EQU	%d\n", offset);	
	offset = offsetof(CAudioObjectDecoder, m_rgrgfltChDnMixMtx);
	fprintf(hinc, "CAudioObjectDecoder_m_rgrgfltChDnMixMtx  EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_pctTemp);
	fprintf(hinc, "CAudioObjectDecoder_m_pctTemp            EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_ppcbtTemp);
	fprintf(hinc, "CAudioObjectDecoder_m_ppcbtTemp          EQU	%d\n", offset);
	offset = offsetof(CAudioObjectDecoder, m_fLtRtDownmix);
	fprintf(hinc, "CAudioObjectDecoder_m_fLtRtDownmix       EQU	%d\n", offset);	
	

	//struct CAudioObject
	fprintf(hinc, "\n");
	offset = offsetof(CAudioObject, m_qstQuantStep);
	fprintf(hinc, "CAudioObject_m_qstQuantStep              EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_cChannel);
	fprintf(hinc, "CAudioObject_m_cChannel                  EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_fNoiseSub);
	fprintf(hinc, "CAudioObject_m_fNoiseSub                 EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_iMaxEscSize);
	fprintf(hinc, "CAudioObject_m_iMaxEscSize               EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_iBitDepthSelector);
	fprintf(hinc, "CAudioObject_m_iBitDepthSelector         EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_nValidBitsPerSample);
	fprintf(hinc, "CAudioObject_m_nValidBitsPerSample       EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_iPCMSampleMin);
	fprintf(hinc, "CAudioObject_m_iPCMSampleMin             EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_iPCMSampleMax);
	fprintf(hinc, "CAudioObject_m_iPCMSampleMax             EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_bUnifiedPureLLMCurrFrm);
	fprintf(hinc, "CAudioObject_m_bUnifiedPureLLMCurrFrm    EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_bFirstUnifiedPureLLMFrm);
	fprintf(hinc, "CAudioObject_m_bFirstUnifiedPureLLMFrm   EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_bPureLosslessMode);
	fprintf(hinc, "CAudioObject_m_bPureLosslessMode         EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_bDoInterChDecorr);
	fprintf(hinc, "CAudioObject_m_bDoInterChDecorr          EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_bSeekable);
	fprintf(hinc, "CAudioObject_m_bSeekable                 EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_iCurrReconCoef);
	fprintf(hinc, "CAudioObject_m_iCurrReconCoef            EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_cFrameSampleHalf);
	fprintf(hinc, "CAudioObject_m_cFrameSampleHalf          EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLowCutOff);
	fprintf(hinc, "CAudioObject_m_cLowCutOff                EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_cHighCutOff);
	fprintf(hinc, "CAudioObject_m_cHighCutOff               EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_cValidBarkBand);
	fprintf(hinc, "CAudioObject_m_cValidBarkBand            EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_rgiBarkIndex);
	fprintf(hinc, "CAudioObject_m_rgiBarkIndex              EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_rgpcinfo);
	fprintf(hinc, "CAudioObject_m_rgpcinfo                  EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_iFirstNoiseIndex);
	fprintf(hinc, "CAudioObject_m_iFirstNoiseIndex          EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_tRandState);
	fprintf(hinc, "CAudioObject_m_tRandState                EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_cChInTile);
	fprintf(hinc, "CAudioObject_m_cChInTile                 EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_rgiChInTile);
	fprintf(hinc, "CAudioObject_m_rgiChInTile               EQU	%d\n", offset);	
	offset = offsetof(CAudioObject, m_iResidueMemScaling);
	fprintf(hinc, "CAudioObject_m_iResidueMemScaling        EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLastCodedIndex);
	fprintf(hinc, "CAudioObject_m_cLastCodedIndex           EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLeftShiftBitsFixedPre);
	fprintf(hinc, "CAudioObject_m_cLeftShiftBitsFixedPre    EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_cLeftShiftBitsFixedPost);
	fprintf(hinc, "CAudioObject_m_cLeftShiftBitsFixedPost   EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_bFreqex);
	fprintf(hinc, "CAudioObject_m_bFreqex                   EQU	%d\n", offset);
	offset = offsetof(CAudioObject, m_bDoFexOnCodedChannels);
	fprintf(hinc, "CAudioObject_m_bDoFexOnCodedChannels     EQU	%d\n", offset);		

	//struct PerChannelInfo
	fprintf(hinc, "\n");
	StructSize = sizeof(PerChannelInfo);
	fprintf(hinc, "PerChannelInfo_size                      EQU	%d\n", StructSize);
	offset = offsetof(PerChannelInfo, m_rgiCoefQ);
	fprintf(hinc, "PerChannelInfo_m_rgiCoefQ                EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgbBandNotCoded);
	fprintf(hinc, "PerChannelInfo_m_rgbBandNotCoded         EQU	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_rgffltSqrtBWRatio);
	fprintf(hinc, "PerChannelInfo_m_rgffltSqrtBWRatio       EQU	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_rgiNoisePower);
	fprintf(hinc, "PerChannelInfo_m_rgiNoisePower           EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgiCoefRecon);
	fprintf(hinc, "PerChannelInfo_m_rgiCoefRecon            EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgiPCMBuffer);
	fprintf(hinc, "PerChannelInfo_m_rgiPCMBuffer            EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiSinRampUpStart);
	fprintf(hinc, "PerChannelInfo_m_fiSinRampUpStart        EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiCosRampUpStart);
	fprintf(hinc, "PerChannelInfo_m_fiCosRampUpStart        EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiSinRampUpPrior);
	fprintf(hinc, "PerChannelInfo_m_fiSinRampUpPrior        EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiCosRampUpPrior);
	fprintf(hinc, "PerChannelInfo_m_fiCosRampUpPrior        EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_fiSinRampUpStep);
	fprintf(hinc, "PerChannelInfo_m_fiSinRampUpStep         EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSubFrameSampleHalf);
	fprintf(hinc, "PerChannelInfo_m_cSubFrameSampleHalf     EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSubbandAdjusted);
	fprintf(hinc, "PerChannelInfo_m_cSubbandAdjusted        EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iSizePrev);
	fprintf(hinc, "PerChannelInfo_m_iSizePrev               EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iSizeCurr);
	fprintf(hinc, "PerChannelInfo_m_iSizeCurr               EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_wtMaxWeight);
	fprintf(hinc, "PerChannelInfo_m_wtMaxWeight             EQU	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_cLeftShiftBitsQuant);
	fprintf(hinc, "PerChannelInfo_m_cLeftShiftBitsQuant     EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cLeftShiftBitsTotal);
	fprintf(hinc, "PerChannelInfo_m_cLeftShiftBitsTotal     EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_qfltMaxQuantStep);
	fprintf(hinc, "PerChannelInfo_m_qfltMaxQuantStep        EQU	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_cLMSPredictors);
	fprintf(hinc, "PerChannelInfo_m_cLMSPredictors          EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iSum);
	fprintf(hinc, "PerChannelInfo_m_iSum                    EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgsubfrmconfig);
	fprintf(hinc, "PerChannelInfo_m_rgsubfrmconfig          EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSubFrameSampleHalfWithUpdate);
	fprintf(hinc, "PerChannelInfo_m_cSubFrameSampleHalfWithUpdate EQU	%d\n", offset);	
	offset = offsetof(PerChannelInfo, m_iUpdSpdUpSamples);
	fprintf(hinc, "PerChannelInfo_m_iUpdSpdUpSamples        EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iTransientPosition);
	fprintf(hinc, "PerChannelInfo_m_iTransientPosition      EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_bNoDecodeForCx);
	fprintf(hinc, "PerChannelInfo_m_bNoDecodeForCx          EQU	%d\n", offset);	
	//for PerChannelInfo test
/*	offset = offsetof(PerChannelInfo, m_cLastCodedIndex);
	fprintf(hinc, "PerChannelInfo_m_cLastCodedIndex         EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_cSamplesLeft);
	fprintf(hinc, "PerChannelInfo_m_cSamplesLeft            EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_ppcinfoShr);
	fprintf(hinc, "PerChannelInfo_m_ppcinfoShr          EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_iStartPtInFile);
	fprintf(hinc, "PerChannelInfo_m_iStartPtInFile          EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgrgdblLLMLPCFilter);
	fprintf(hinc, "PerChannelInfo_m_rgrgdblLLMLPCFilter          EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgrgfltLLMLPCFilterRec);
	fprintf(hinc, "PerChannelInfo_m_rgrgfltLLMLPCFilterRec          EQU	%d\n", offset);
	offset = offsetof(PerChannelInfo, m_rgiLPCFilter);
	fprintf(hinc, "PerChannelInfo_m_rgiLPCFilter          EQU	%d\n", offset);
*/
	//struct CWMAInputBitStream
	fprintf(hinc, "\n");
	offset = offsetof(CWMAInputBitStream, m_pBuffer);
	fprintf(hinc, "CWMAInputBitStream_m_pBuffer             EQU	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_cbBuflen);
	fprintf(hinc, "CWMAInputBitStream_m_cbBuflen            EQU	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_dwDot);
	fprintf(hinc, "CWMAInputBitStream_m_dwDot               EQU	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_dwBitsLeft);
	fprintf(hinc, "CWMAInputBitStream_m_dwBitsLeft          EQU	%d\n", offset);	
	offset = offsetof(CWMAInputBitStream, m_dwDotT);
	fprintf(hinc, "CWMAInputBitStream_m_dwDotT              EQU	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_cBitDotT);
	fprintf(hinc, "CWMAInputBitStream_m_cBitDotT            EQU	%d\n", offset);
	offset = offsetof(CWMAInputBitStream, m_cFrmBitCnt);
	fprintf(hinc, "CWMAInputBitStream_m_cFrmBitCnt          EQU	%d\n", offset);

	//struct MCLMSPredictor for lossless
	fprintf(hinc, "\n");
	offset = offsetof(MCLMSPredictor, m_iOrder_X_CH);
	fprintf(hinc, "MCLMSPredictor_m_iOrder_X_CH             EQU	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_iScaling);
	fprintf(hinc, "MCLMSPredictor_m_iScaling                EQU	%d\n", offset);
	offset = offsetof(MCLMSPredictor, m_iScalingOffset);
	fprintf(hinc, "MCLMSPredictor_m_iScalingOffset          EQU	%d\n", offset);
	offset = offsetof(MCLMSPredictor, m_iRecent);
	fprintf(hinc, "MCLMSPredictor_m_iRecent                 EQU	%d\n", offset);
	offset = offsetof(MCLMSPredictor, m_rgiPrevVal);
	fprintf(hinc, "MCLMSPredictor_m_rgiPrevVal              EQU	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_rgiFilter);
	fprintf(hinc, "MCLMSPredictor_m_rgiFilter               EQU	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_rgiFilterCurrTime);
	fprintf(hinc, "MCLMSPredictor_m_rgiFilterCurrTime       EQU	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_rgiUpdate);
	fprintf(hinc, "MCLMSPredictor_m_rgiUpdate               EQU	%d\n", offset);	
	offset = offsetof(MCLMSPredictor, m_iUpdStepSize);
	fprintf(hinc, "MCLMSPredictor_m_iUpdStepSize            EQU	%d\n", offset);

	//struct LMSPredictor
	fprintf(hinc, "\n");
	offset = offsetof(LMSPredictor, m_iOrder);
	fprintf(hinc, "LMSPredictor_m_iOrder                    EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iOrder_Div_8);
	fprintf(hinc, "LMSPredictor_m_iOrder_Div_8              EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iOrder_Div_16);
	fprintf(hinc, "LMSPredictor_m_iOrder_Div_16             EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iScaling);
	fprintf(hinc, "LMSPredictor_m_iScaling                  EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iScalingOffset);
	fprintf(hinc, "LMSPredictor_m_iScalingOffset            EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iUpdStepSize_X_8);
	fprintf(hinc, "LMSPredictor_m_iUpdStepSize_X_8          EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_iRecent);
	fprintf(hinc, "LMSPredictor_m_iRecent                   EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_rgiPrevVal);
	fprintf(hinc, "LMSPredictor_m_rgiPrevVal                EQU	%d\n", offset);
	offset = offsetof(LMSPredictor, m_rgiUpdate);
	fprintf(hinc, "LMSPredictor_m_rgiUpdate                 EQU	%d\n", offset);	
	offset = offsetof(LMSPredictor, m_rgiFilter);
	fprintf(hinc, "LMSPredictor_m_rgiFilter                 EQU	%d\n", offset);
	

	//struct FastFloat
	fprintf(hinc, "\n");
	offset = offsetof(FastFloat, iFracBits);
	fprintf(hinc, "FastFloat_iFracBits                      EQU	%d\n", offset);
	offset = offsetof(FastFloat, iFraction);
	fprintf(hinc, "FastFloat_iFraction                      EQU	%d\n", offset);

	//struct tagRandState
	fprintf(hinc, "\n");
	offset = offsetof(struct tagRandState, iPrior);
	fprintf(hinc, "tagRandState_iPrior                      EQU	%d\n", offset);
	offset = offsetof(struct tagRandState, uiRand);
	fprintf(hinc, "tagRandState_uiRand                      EQU	%d\n", offset);
	
	//struct huffResult
	fprintf(hinc, "\n");
	offset = offsetof(huffResult, state);
	fprintf(hinc, "HuffResult_state                         EQU	%d\n", offset);
	offset = offsetof(huffResult, run);
	fprintf(hinc, "HuffResult_run                           EQU	%d\n", offset);
	offset = offsetof(huffResult, level);
	fprintf(hinc, "HuffResult_level                         EQU	%d\n", offset);

	//struct huffVecResult
	fprintf(hinc, "\n");
	StructSize = sizeof(huffVecResult);
	fprintf(hinc, "huffVecResult_size                       EQU	%d\n", StructSize);
	offset = offsetof(huffVecResult, state);
	fprintf(hinc, "HuffVecResult_state                      EQU	%d\n", offset);
	offset = offsetof(huffVecResult, iResult);
	fprintf(hinc, "HuffVecResult_iResult                    EQU	%d\n", offset);	

	//struct SubFrameConfigInfo
	fprintf(hinc, "\n");
	offset = offsetof(SubFrameConfigInfo, m_rgiSubFrameStart);
	fprintf(hinc, "SubFrameConfigInfo_m_rgiSubFrameStart    EQU	%d\n", offset);

	//macros define
	fprintf(hinc, "\n");
	fprintf(hinc, "TRANSFORM_FRACT_BITS                     EQU	%d\n", 5);
	fprintf(hinc, "WEIGHTFACTOR_FRACT_BITS                  EQU	%d\n", 21);

	//WMA ARM macros in msaudio.h
	fprintf(hinc, "\n");
	fprintf(hinc, "WMA_OPT_LPCLSL_ARM                       EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_LPCSTD_ARM                       EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_FFT_ARM                          EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_AURECON_ARM                      EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_INVERSQUAN_LOWRATE_ARM           EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_SUBFRAMERECON_ARM                EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_SCALE_COEFFS_V3_ARM              EQU	%d\n", EnableArm);

	//WMA ARM macros in msaudiodec.h
	fprintf(hinc, "\n");
	fprintf(hinc, "WMA_OPT_STRMDEC_ARM                      EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_LOSSLESSDECLSL_ARM               EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_LOSSLESSDECPROLSL_ARM            EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_HUFFDEC_ARM                      EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_CHANNEL_DOWNMIX_ARM              EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_REQUANTO16_ARM                   EQU	%d\n", EnableArm);
	fprintf(hinc, "WMA_OPT_VOMEMORY_ARM                     EQU	%d\n", EnableArm);
	fprintf(hinc, "\n             END\n");

	fclose(hinc);
}
#endif
