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

.set		CAudioObjectDecoder_pau                  ,	0
.set		CAudioObjectDecoder_m_LLMdecsts          ,	72
.set		CAudioObjectDecoder_m_Colombdecsts       ,	76
.set		CAudioObjectDecoder_m_iResQ              ,	200
.set		CAudioObjectDecoder_m_iResQBits          ,	204
.set		CAudioObjectDecoder_m_ibstrm             ,	224
.set		CAudioObjectDecoder_m_fChannelFoldDown   ,	360
.set		CAudioObjectDecoder_m_fReQuantizeTo16    ,	364
.set		CAudioObjectDecoder_m_cDstChannel        ,	368
.set		CAudioObjectDecoder_m_rgrgfltChDnMixMtx  ,	388
.set		CAudioObjectDecoder_m_pctTemp            ,	396
.set		CAudioObjectDecoder_m_ppcbtTemp          ,	404
.set		CAudioObjectDecoder_m_fLtRtDownmix       ,	436

.set		CAudioObject_m_qstQuantStep              ,	0
.set		CAudioObject_m_cChannel                  ,	38
.set		CAudioObject_m_fNoiseSub                 ,	56
.set		CAudioObject_m_iMaxEscSize               ,	68
.set		CAudioObject_m_iBitDepthSelector         ,	116
.set		CAudioObject_m_nValidBitsPerSample       ,	142
.set		CAudioObject_m_iPCMSampleMin             ,	144
.set		CAudioObject_m_iPCMSampleMax             ,	148
.set		CAudioObject_m_bUnifiedPureLLMCurrFrm    ,	172
.set		CAudioObject_m_bFirstUnifiedPureLLMFrm   ,	180
.set		CAudioObject_m_bPureLosslessMode         ,	208
.set		CAudioObject_m_bDoInterChDecorr          ,	216
.set		CAudioObject_m_bSeekable                 ,	224
.set		CAudioObject_m_iCurrReconCoef            ,	234
.set		CAudioObject_m_cFrameSampleHalf          ,	288
.set		CAudioObject_m_cLowCutOff                ,	296
.set		CAudioObject_m_cHighCutOff               ,	300
.set		CAudioObject_m_cValidBarkBand            ,	336
.set		CAudioObject_m_rgiBarkIndex              ,	340
.set		CAudioObject_m_rgpcinfo                  ,	352
.set		CAudioObject_m_iFirstNoiseIndex          ,	452
.set		CAudioObject_m_tRandState                ,	608
.set		CAudioObject_m_cChInTile                 ,	648
.set		CAudioObject_m_rgiChInTile               ,	652
.set		CAudioObject_m_iResidueMemScaling        ,	796
.set		CAudioObject_m_cLastCodedIndex           ,	800
.set		CAudioObject_m_cLeftShiftBitsFixedPre    ,	820
.set		CAudioObject_m_cLeftShiftBitsFixedPost   ,	824
.set		CAudioObject_m_bFreqex                   ,	872
.set		CAudioObject_m_bDoFexOnCodedChannels     ,	880

.set		PerChannelInfo_size                      ,	2592
.set		PerChannelInfo_m_rgiCoefQ                ,	0
.set		PerChannelInfo_m_rgbBandNotCoded         ,	12
.set		PerChannelInfo_m_rgffltSqrtBWRatio       ,	16
.set		PerChannelInfo_m_rgiNoisePower           ,	20
.set		PerChannelInfo_m_rgiCoefRecon            ,	76
.set		PerChannelInfo_m_rgiPCMBuffer            ,	80
.set		PerChannelInfo_m_fiSinRampUpStart        ,	92
.set		PerChannelInfo_m_fiCosRampUpStart        ,	96
.set		PerChannelInfo_m_fiSinRampUpPrior        ,	100
.set		PerChannelInfo_m_fiCosRampUpPrior        ,	104
.set		PerChannelInfo_m_fiSinRampUpStep         ,	108
.set		PerChannelInfo_m_cSubFrameSampleHalf     ,	138
.set		PerChannelInfo_m_cSubbandAdjusted        ,	140
.set		PerChannelInfo_m_iSizePrev               ,	142
.set		PerChannelInfo_m_iSizeCurr               ,	144
.set		PerChannelInfo_m_wtMaxWeight             ,	180
.set		PerChannelInfo_m_cLeftShiftBitsQuant     ,	184
.set		PerChannelInfo_m_cLeftShiftBitsTotal     ,	188
.set		PerChannelInfo_m_qfltMaxQuantStep        ,	192
.set		PerChannelInfo_m_cLMSPredictors          ,	210
.set		PerChannelInfo_m_iSum                    ,	216
.set		PerChannelInfo_m_rgsubfrmconfig          ,	456
.set		PerChannelInfo_m_cSubFrameSampleHalfWithUpdate ,	460
.set		PerChannelInfo_m_iUpdSpdUpSamples        ,	484
.set		PerChannelInfo_m_iTransientPosition      ,	488
.set		PerChannelInfo_m_bNoDecodeForCx          ,	2580

.set		CWMAInputBitStream_m_pBuffer             ,	36
.set		CWMAInputBitStream_m_cbBuflen            ,	40
.set		CWMAInputBitStream_m_dwDot               ,	44
.set		CWMAInputBitStream_m_dwBitsLeft          ,	48
.set		CWMAInputBitStream_m_dwDotT              ,	52
.set		CWMAInputBitStream_m_cBitDotT            ,	56
.set		CWMAInputBitStream_m_cFrmBitCnt          ,	72

.set		MCLMSPredictor_m_iOrder_X_CH             ,	4
.set		MCLMSPredictor_m_iScaling                ,	8
.set		MCLMSPredictor_m_iScalingOffset          ,	12
.set		MCLMSPredictor_m_iRecent                 ,	20
.set		MCLMSPredictor_m_rgiPrevVal              ,	24
.set		MCLMSPredictor_m_rgiFilter               ,	28
.set		MCLMSPredictor_m_rgiFilterCurrTime       ,	36
.set		MCLMSPredictor_m_rgiUpdate               ,	44
.set		MCLMSPredictor_m_iUpdStepSize            ,	48

.set		LMSPredictor_m_iOrder                    ,	0
.set		LMSPredictor_m_iOrder_Div_8              ,	4
.set		LMSPredictor_m_iOrder_Div_16             ,	8
.set		LMSPredictor_m_iScaling                  ,	20
.set		LMSPredictor_m_iScalingOffset            ,	24
.set		LMSPredictor_m_iUpdStepSize_X_8          ,	30
.set		LMSPredictor_m_iRecent                   ,	32
.set		LMSPredictor_m_rgiPrevVal                ,	36
.set		LMSPredictor_m_rgiUpdate                 ,	40
.set		LMSPredictor_m_rgiFilter                 ,	48

.set		FastFloat_iFracBits                      ,	0
.set		FastFloat_iFraction                      ,	4

.set		tagRandState_iPrior                      ,	0
.set		tagRandState_uiRand                      ,	4

.set		HuffResult_state                         ,	0
.set		HuffResult_run                           ,	4
.set		HuffResult_level                         ,	6

.set		huffVecResult_size                       ,	4
.set		HuffVecResult_state                      ,	0
.set		HuffVecResult_iResult                    ,	2

.set		SubFrameConfigInfo_m_rgiSubFrameStart    ,	12

.set		TRANSFORM_FRACT_BITS                     ,	5
.set		WEIGHTFACTOR_FRACT_BITS                  ,	21

.set		WMA_OPT_LPCLSL_ARM                       ,	1
.set		WMA_OPT_LPCSTD_ARM                       ,	1
.set		WMA_OPT_FFT_ARM                          ,	1
.set		WMA_OPT_AURECON_ARM                      ,	1
.set		WMA_OPT_INVERSQUAN_LOWRATE_ARM           ,	1
.set		WMA_OPT_SUBFRAMERECON_ARM                ,	1
.set		WMA_OPT_SCALE_COEFFS_V3_ARM              ,	1

.set		WMA_OPT_STRMDEC_ARM                      ,	1
.set		WMA_OPT_LOSSLESSDECLSL_ARM               ,	1
.set		WMA_OPT_LOSSLESSDECPROLSL_ARM            ,	1
.set		WMA_OPT_HUFFDEC_ARM                      ,	1
.set		WMA_OPT_CHANNEL_DOWNMIX_ARM              ,	1
.set		WMA_OPT_REQUANTO16_ARM                   ,	1
.set		WMA_OPT_VOMEMORY_ARM                     ,	1

