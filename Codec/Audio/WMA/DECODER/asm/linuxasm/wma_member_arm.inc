@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Filename wma_arm_version.h
@
@ Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
@
@ VisualOn WMA decoder assembly macros define
@
@ Author:
@
@		Witten Wen 9-April-2008
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.equ		CAudioObjectDecoder_pau                  ,	0
.equ		CAudioObjectDecoder_m_LLMdecsts          ,	72
.equ		CAudioObjectDecoder_m_Colombdecsts       ,	76
.equ		CAudioObjectDecoder_m_iResQ              ,	200
.equ		CAudioObjectDecoder_m_iResQBits          ,	204
.equ		CAudioObjectDecoder_m_ibstrm             ,	224
.equ		CAudioObjectDecoder_m_fChannelFoldDown   ,	360
.equ		CAudioObjectDecoder_m_fReQuantizeTo16    ,	364
.equ		CAudioObjectDecoder_m_cDstChannel        ,	368
.equ		CAudioObjectDecoder_m_rgrgfltChDnMixMtx  ,	388
.equ		CAudioObjectDecoder_m_pctTemp            ,	396
.equ		CAudioObjectDecoder_m_ppcbtTemp          ,	404
.equ		CAudioObjectDecoder_m_fLtRtDownmix       ,	436

.equ		CAudioObject_m_qstQuantStep              ,	0
.equ		CAudioObject_m_cChannel                  ,	38
.equ		CAudioObject_m_fNoiseSub                 ,	56
.equ		CAudioObject_m_iMaxEscSize               ,	68
.equ		CAudioObject_m_iBitDepthSelector         ,	116
.equ		CAudioObject_m_nValidBitsPerSample       ,	142
.equ		CAudioObject_m_iPCMSampleMin             ,	144
.equ		CAudioObject_m_iPCMSampleMax             ,	148
.equ		CAudioObject_m_bUnifiedPureLLMCurrFrm    ,	172
.equ		CAudioObject_m_bFirstUnifiedPureLLMFrm   ,	180
.equ		CAudioObject_m_bPureLosslessMode         ,	208
.equ		CAudioObject_m_bDoInterChDecorr          ,	216
.equ		CAudioObject_m_bSeekable                 ,	224
.equ		CAudioObject_m_iCurrReconCoef            ,	234
.equ		CAudioObject_m_cFrameSampleHalf          ,	288
.equ		CAudioObject_m_cLowCutOff                ,	296
.equ		CAudioObject_m_cHighCutOff               ,	300
.equ		CAudioObject_m_cValidBarkBand            ,	336
.equ		CAudioObject_m_rgiBarkIndex              ,	340
.equ		CAudioObject_m_rgpcinfo                  ,	352
.equ		CAudioObject_m_iFirstNoiseIndex          ,	452
.equ		CAudioObject_m_tRandState                ,	608
.equ		CAudioObject_m_cChInTile                 ,	648
.equ		CAudioObject_m_rgiChInTile               ,	652
.equ		CAudioObject_m_iResidueMemScaling        ,	796
.equ		CAudioObject_m_cLastCodedIndex           ,	800
.equ		CAudioObject_m_cLeftShiftBitsFixedPre    ,	820
.equ		CAudioObject_m_cLeftShiftBitsFixedPost   ,	824
.equ		CAudioObject_m_bFreqex                   ,	872
.equ		CAudioObject_m_bDoFexOnCodedChannels     ,	880

.equ		PerChannelInfo_size                      ,	2592
.equ		PerChannelInfo_m_rgiCoefQ                ,	0
.equ		PerChannelInfo_m_rgbBandNotCoded         ,	12
.equ		PerChannelInfo_m_rgffltSqrtBWRatio       ,	16
.equ		PerChannelInfo_m_rgiNoisePower           ,	20
.equ		PerChannelInfo_m_rgiCoefRecon            ,	76
.equ		PerChannelInfo_m_rgiPCMBuffer            ,	80
.equ		PerChannelInfo_m_fiSinRampUpStart        ,	92
.equ		PerChannelInfo_m_fiCosRampUpStart        ,	96
.equ		PerChannelInfo_m_fiSinRampUpPrior        ,	100
.equ		PerChannelInfo_m_fiCosRampUpPrior        ,	104
.equ		PerChannelInfo_m_fiSinRampUpStep         ,	108
.equ		PerChannelInfo_m_cSubFrameSampleHalf     ,	138
.equ		PerChannelInfo_m_cSubbandAdjusted        ,	140
.equ		PerChannelInfo_m_iSizePrev               ,	142
.equ		PerChannelInfo_m_iSizeCurr               ,	144
.equ		PerChannelInfo_m_wtMaxWeight             ,	180
.equ		PerChannelInfo_m_cLeftShiftBitsQuant     ,	184
.equ		PerChannelInfo_m_cLeftShiftBitsTotal     ,	188
.equ		PerChannelInfo_m_qfltMaxQuantStep        ,	192
.equ		PerChannelInfo_m_cLMSPredictors          ,	210
.equ		PerChannelInfo_m_iSum                    ,	216
.equ		PerChannelInfo_m_rgsubfrmconfig          ,	456
.equ		PerChannelInfo_m_cSubFrameSampleHalfWithUpdate ,	460
.equ		PerChannelInfo_m_iUpdSpdUpSamples        ,	484
.equ		PerChannelInfo_m_iTransientPosition      ,	488
.equ		PerChannelInfo_m_bNoDecodeForCx          ,	2580

.equ		CWMAInputBitStream_m_pBuffer             ,	36
.equ		CWMAInputBitStream_m_cbBuflen            ,	40
.equ		CWMAInputBitStream_m_dwDot               ,	44
.equ		CWMAInputBitStream_m_dwBitsLeft          ,	48
.equ		CWMAInputBitStream_m_dwDotT              ,	52
.equ		CWMAInputBitStream_m_cBitDotT            ,	56
.equ		CWMAInputBitStream_m_cFrmBitCnt          ,	72

.equ		MCLMSPredictor_m_iOrder_X_CH             ,	4
.equ		MCLMSPredictor_m_iScaling                ,	8
.equ		MCLMSPredictor_m_iScalingOffset          ,	12
.equ		MCLMSPredictor_m_iRecent                 ,	20
.equ		MCLMSPredictor_m_rgiPrevVal              ,	24
.equ		MCLMSPredictor_m_rgiFilter               ,	28
.equ		MCLMSPredictor_m_rgiFilterCurrTime       ,	36
.equ		MCLMSPredictor_m_rgiUpdate               ,	44
.equ		MCLMSPredictor_m_iUpdStepSize            ,	48

.equ		LMSPredictor_m_iOrder                    ,	0
.equ		LMSPredictor_m_iOrder_Div_8              ,	4
.equ		LMSPredictor_m_iOrder_Div_16             ,	8
.equ		LMSPredictor_m_iScaling                  ,	20
.equ		LMSPredictor_m_iScalingOffset            ,	24
.equ		LMSPredictor_m_iUpdStepSize_X_8          ,	30
.equ		LMSPredictor_m_iRecent                   ,	32
.equ		LMSPredictor_m_rgiPrevVal                ,	36
.equ		LMSPredictor_m_rgiUpdate                 ,	40
.equ		LMSPredictor_m_rgiFilter                 ,	48

.equ		FastFloat_iFracBits                      ,	0
.equ		FastFloat_iFraction                      ,	4

.equ		tagRandState_iPrior                      ,	0
.equ		tagRandState_uiRand                      ,	4

.equ		HuffResult_state                         ,	0
.equ		HuffResult_run                           ,	4
.equ		HuffResult_level                         ,	6

.equ		huffVecResult_size                       ,	4
.equ		HuffVecResult_state                      ,	0
.equ		HuffVecResult_iResult                    ,	2

.equ		SubFrameConfigInfo_m_rgiSubFrameStart    ,	12

.equ		TRANSFORM_FRACT_BITS                     ,	5
.equ		WEIGHTFACTOR_FRACT_BITS                  ,	21

.equ		WMA_OPT_LPCLSL_ARM                       ,	1
.equ		WMA_OPT_LPCSTD_ARM                       ,	1
.equ		WMA_OPT_FFT_ARM                          ,	1
.equ		WMA_OPT_AURECON_ARM                      ,	1
.equ		WMA_OPT_INVERSQUAN_LOWRATE_ARM           ,	1
.equ		WMA_OPT_SUBFRAMERECON_ARM                ,	1
.equ		WMA_OPT_SCALE_COEFFS_V3_ARM              ,	1

.equ		WMA_OPT_STRMDEC_ARM                      ,	1
.equ		WMA_OPT_LOSSLESSDECLSL_ARM               ,	1
.equ		WMA_OPT_LOSSLESSDECPROLSL_ARM            ,	1
.equ		WMA_OPT_HUFFDEC_ARM                      ,	1
.equ		WMA_OPT_CHANNEL_DOWNMIX_ARM              ,	1
.equ		WMA_OPT_REQUANTO16_ARM                   ,	1
.equ		WMA_OPT_VOMEMORY_ARM                     ,	1

