;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Filename wma_arm_version.h
;
; Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
;
; VisualOn WMA decoder assembly macros define
;
; Author:
;
;		Witten Wen 9-April-2008
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CAudioObjectDecoder_pau                  EQU	0
CAudioObjectDecoder_m_LLMdecsts          EQU	72
CAudioObjectDecoder_m_Colombdecsts       EQU	76
CAudioObjectDecoder_m_iResQ              EQU	200
CAudioObjectDecoder_m_iResQBits          EQU	204
CAudioObjectDecoder_m_ibstrm             EQU	224
CAudioObjectDecoder_m_fChannelFoldDown   EQU	360
CAudioObjectDecoder_m_fReQuantizeTo16    EQU	364
CAudioObjectDecoder_m_cDstChannel        EQU	368
CAudioObjectDecoder_m_rgrgfltChDnMixMtx  EQU	388
CAudioObjectDecoder_m_pctTemp            EQU	396
CAudioObjectDecoder_m_ppcbtTemp          EQU	404
CAudioObjectDecoder_m_fLtRtDownmix       EQU	436

CAudioObject_m_qstQuantStep              EQU	0
CAudioObject_m_cChannel                  EQU	38
CAudioObject_m_fNoiseSub                 EQU	56
CAudioObject_m_iMaxEscSize               EQU	68
CAudioObject_m_iBitDepthSelector         EQU	116
CAudioObject_m_nValidBitsPerSample       EQU	142
CAudioObject_m_iPCMSampleMin             EQU	144
CAudioObject_m_iPCMSampleMax             EQU	148
CAudioObject_m_bUnifiedPureLLMCurrFrm    EQU	172
CAudioObject_m_bFirstUnifiedPureLLMFrm   EQU	180
CAudioObject_m_bPureLosslessMode         EQU	208
CAudioObject_m_bDoInterChDecorr          EQU	216
CAudioObject_m_bSeekable                 EQU	224
CAudioObject_m_iCurrReconCoef            EQU	234
CAudioObject_m_cFrameSampleHalf          EQU	288
CAudioObject_m_cLowCutOff                EQU	296
CAudioObject_m_cHighCutOff               EQU	300
CAudioObject_m_cValidBarkBand            EQU	336
CAudioObject_m_rgiBarkIndex              EQU	340
CAudioObject_m_rgpcinfo                  EQU	352
CAudioObject_m_iFirstNoiseIndex          EQU	452
CAudioObject_m_tRandState                EQU	608
CAudioObject_m_cChInTile                 EQU	648
CAudioObject_m_rgiChInTile               EQU	652
CAudioObject_m_iResidueMemScaling        EQU	796
CAudioObject_m_cLastCodedIndex           EQU	800
CAudioObject_m_cLeftShiftBitsFixedPre    EQU	820
CAudioObject_m_cLeftShiftBitsFixedPost   EQU	824
CAudioObject_m_bFreqex                   EQU	872
CAudioObject_m_bDoFexOnCodedChannels     EQU	880

PerChannelInfo_size                      EQU	2592
PerChannelInfo_m_rgiCoefQ                EQU	0
PerChannelInfo_m_rgbBandNotCoded         EQU	12
PerChannelInfo_m_rgffltSqrtBWRatio       EQU	16
PerChannelInfo_m_rgiNoisePower           EQU	20
PerChannelInfo_m_rgiCoefRecon            EQU	76
PerChannelInfo_m_rgiPCMBuffer            EQU	80
PerChannelInfo_m_fiSinRampUpStart        EQU	92
PerChannelInfo_m_fiCosRampUpStart        EQU	96
PerChannelInfo_m_fiSinRampUpPrior        EQU	100
PerChannelInfo_m_fiCosRampUpPrior        EQU	104
PerChannelInfo_m_fiSinRampUpStep         EQU	108
PerChannelInfo_m_cSubFrameSampleHalf     EQU	138
PerChannelInfo_m_cSubbandAdjusted        EQU	140
PerChannelInfo_m_iSizePrev               EQU	142
PerChannelInfo_m_iSizeCurr               EQU	144
PerChannelInfo_m_wtMaxWeight             EQU	180
PerChannelInfo_m_cLeftShiftBitsQuant     EQU	184
PerChannelInfo_m_cLeftShiftBitsTotal     EQU	188
PerChannelInfo_m_qfltMaxQuantStep        EQU	192
PerChannelInfo_m_cLMSPredictors          EQU	210
PerChannelInfo_m_iSum                    EQU	216
PerChannelInfo_m_rgsubfrmconfig          EQU	456
PerChannelInfo_m_cSubFrameSampleHalfWithUpdate EQU	460
PerChannelInfo_m_iUpdSpdUpSamples        EQU	484
PerChannelInfo_m_iTransientPosition      EQU	488
PerChannelInfo_m_bNoDecodeForCx          EQU	2580

CWMAInputBitStream_m_pBuffer             EQU	36
CWMAInputBitStream_m_cbBuflen            EQU	40
CWMAInputBitStream_m_dwDot               EQU	44
CWMAInputBitStream_m_dwBitsLeft          EQU	48
CWMAInputBitStream_m_dwDotT              EQU	52
CWMAInputBitStream_m_cBitDotT            EQU	56
CWMAInputBitStream_m_cFrmBitCnt          EQU	72

MCLMSPredictor_m_iOrder_X_CH             EQU	4
MCLMSPredictor_m_iScaling                EQU	8
MCLMSPredictor_m_iScalingOffset          EQU	12
MCLMSPredictor_m_iRecent                 EQU	20
MCLMSPredictor_m_rgiPrevVal              EQU	24
MCLMSPredictor_m_rgiFilter               EQU	28
MCLMSPredictor_m_rgiFilterCurrTime       EQU	36
MCLMSPredictor_m_rgiUpdate               EQU	44
MCLMSPredictor_m_iUpdStepSize            EQU	48

LMSPredictor_m_iOrder                    EQU	0
LMSPredictor_m_iOrder_Div_8              EQU	4
LMSPredictor_m_iOrder_Div_16             EQU	8
LMSPredictor_m_iScaling                  EQU	20
LMSPredictor_m_iScalingOffset            EQU	24
LMSPredictor_m_iUpdStepSize_X_8          EQU	30
LMSPredictor_m_iRecent                   EQU	32
LMSPredictor_m_rgiPrevVal                EQU	36
LMSPredictor_m_rgiUpdate                 EQU	40
LMSPredictor_m_rgiFilter                 EQU	48

FastFloat_iFracBits                      EQU	0
FastFloat_iFraction                      EQU	4

tagRandState_iPrior                      EQU	0
tagRandState_uiRand                      EQU	4

HuffResult_state                         EQU	0
HuffResult_run                           EQU	4
HuffResult_level                         EQU	6

huffVecResult_size                       EQU	4
HuffVecResult_state                      EQU	0
HuffVecResult_iResult                    EQU	2

SubFrameConfigInfo_m_rgiSubFrameStart    EQU	12

TRANSFORM_FRACT_BITS                     EQU	5
WEIGHTFACTOR_FRACT_BITS                  EQU	21

WMA_OPT_LPCLSL_ARM                       EQU	1
WMA_OPT_LPCSTD_ARM                       EQU	1
WMA_OPT_FFT_ARM                          EQU	1
WMA_OPT_AURECON_ARM                      EQU	1
WMA_OPT_INVERSQUAN_LOWRATE_ARM           EQU	1
WMA_OPT_SUBFRAMERECON_ARM                EQU	1
WMA_OPT_SCALE_COEFFS_V3_ARM              EQU	1

WMA_OPT_STRMDEC_ARM                      EQU	1
WMA_OPT_LOSSLESSDECLSL_ARM               EQU	1
WMA_OPT_LOSSLESSDECPROLSL_ARM            EQU	1
WMA_OPT_HUFFDEC_ARM                      EQU	1
WMA_OPT_CHANNEL_DOWNMIX_ARM              EQU	1
WMA_OPT_REQUANTO16_ARM                   EQU	1
WMA_OPT_VOMEMORY_ARM                     EQU	1

             END
