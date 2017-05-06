/*******************************************************************************
	File:		yyVVDef_HEVC.h

	Contains:	yy player type define header file.

	Written by:	Fenger King

	Change History (most recent first):
	2014-02-25		Fenger			Create file

*******************************************************************************/
#ifndef __yyVVDef_HEVC_H__
#define __yyVVDef_HEVC_H__

#ifndef voCabacInit
#define voCabacInit yy_4431025531818be
#endif // voCabacInit

#ifndef voCabacInitExt
#define voCabacInitExt yy_574102553346784
#endif // voCabacInitExt

#ifndef voCabacDecodePCMAlignBits
#define voCabacDecodePCMAlignBits yy_950102553494ae1
#endif // voCabacDecodePCMAlignBits

#ifndef DeriveZsBlockAvail
#define DeriveZsBlockAvail yy_704102553653d6c
#endif // DeriveZsBlockAvail

#ifndef voChromaInFilterASM_H
#define voChromaInFilterASM_H yy_7e4102554741649
#endif // voChromaInFilterASM_H

#ifndef voChromaInFilterASM_V
#define voChromaInFilterASM_V yy_7f2102554906df1
#endif // voChromaInFilterASM_V

#ifndef voLumaInFilterASM_H
#define voLumaInFilterASM_H yy_719102555055af1
#endif // voLumaInFilterASM_H

#ifndef voLumaInFilterASM_V
#define voLumaInFilterASM_V yy_7271025552141bb
#endif // voLumaInFilterASM_V

#ifndef voPelLumaWeakASM_V
#define voPelLumaWeakASM_V yy_6b31025553626e9
#endif // voPelLumaWeakASM_V

#ifndef voPelLumaWeakASM_H
#define voPelLumaWeakASM_H yy_6a5102555521eb
#endif // voPelLumaWeakASM_H

#ifndef voLumaFilterHor_ASM
#define voLumaFilterHor_ASM yy_74310255568bb3
#endif // voLumaFilterHor_ASM

#ifndef voLumaFilterVer_ASM
#define voLumaFilterVer_ASM yy_747102555832ea6
#endif // voLumaFilterVer_ASM

#ifndef voChromaFilterHor_ASM
#define voChromaFilterHor_ASM yy_80e1025559912db
#endif // voChromaFilterHor_ASM

#ifndef voChromaFilterVer_ASM
#define voChromaFilterVer_ASM yy_81210255614153c
#endif // voChromaFilterVer_ASM

#ifndef voChromaFilterCross_ASM
#define voChromaFilterCross_ASM yy_8ef102556307e87
#endif // voChromaFilterCross_ASM

#ifndef voLumaFilterCross_ASM
#define voLumaFilterCross_ASM yy_82410255646390c
#endif // voLumaFilterCross_ASM

#ifndef voDeblockingFilterLCUStepNew
#define voDeblockingFilterLCUStepNew yy_ae710255661f3e
#endif // voDeblockingFilterLCUStepNew

#ifndef voSetEdgefilterCU
#define voSetEdgefilterCU yy_6a41025567799
#endif // voSetEdgefilterCU

#ifndef voSetEdgefilterPU
#define voSetEdgefilterPU yy_6b110255692124
#endif // voSetEdgefilterPU

#ifndef voSetEdgefilterTUVer
#define voSetEdgefilterTUVer yy_7e210255708305e
#endif // voSetEdgefilterTUVer

#ifndef voSetEdgefilterTUHor
#define voSetEdgefilterTUHor yy_7de10255724440d
#endif // voSetEdgefilterTUHor

#ifndef alloc_mMulLevelScale
#define alloc_mMulLevelScale yy_7e51025580254de
#endif // alloc_mMulLevelScale

#ifndef free_mMulLevelScale
#define free_mMulLevelScale yy_77c1025581739b3
#endif // free_mMulLevelScale

#ifndef setMMulLevelScale
#define setMMulLevelScale yy_6a7102558332d12
#endif // setMMulLevelScale

#ifndef alloc_ScalingList
#define alloc_ScalingList yy_6c71025584874d
#endif // alloc_ScalingList

#ifndef free_ScalingList
#define free_ScalingList yy_65e102558644dc8
#endif // free_ScalingList

#ifndef setDefaultScalingList
#define setDefaultScalingList yy_86e102558806443
#endif // setDefaultScalingList

#ifndef scaling_list_data
#define scaling_list_data yy_6f51025589566bb
#endif // scaling_list_data

#ifndef WaitForProgress
#define WaitForProgress yy_6111025594226a6
#endif // WaitForProgress

#ifndef ReportReady
#define ReportReady yy_47110255958701f
#endif // ReportReady

#ifndef WaitForProgress_mv
#define WaitForProgress_mv yy_753102559735d03
#endif // WaitForProgress_mv

#ifndef ReportProgress
#define ReportProgress yy_5d1102559897a5a
#endif // ReportProgress

#ifndef ReportProgress_mv
#define ReportProgress_mv yy_71310256004767d
#endif // ReportProgress_mv

#ifndef DecodeFrameMain
#define DecodeFrameMain yy_5b4102560204509
#endif // DecodeFrameMain

#ifndef InitH265Threads
#define InitH265Threads yy_544102560361238
#endif // InitH265Threads

#ifndef ReleaseH265Threads
#define ReleaseH265Threads yy_671102560513b25
#endif // ReleaseH265Threads

#ifndef IDST4X4ASMV7
#define IDST4X4ASMV7 yy_362102560671e1f
#endif // IDST4X4ASMV7

#ifndef IDCT4X4ASMV7
#define IDCT4X4ASMV7 yy_352102560826e5d
#endif // IDCT4X4ASMV7

#ifndef IDCT8X8ASMV7
#define IDCT8X8ASMV7 yy_35a102560981ad4
#endif // IDCT8X8ASMV7

#ifndef IDCT16X16ASMV7
#define IDCT16X16ASMV7 yy_3b81025611463cb
#endif // IDCT16X16ASMV7

#ifndef IDCT32X32ASMV7
#define IDCT32X32ASMV7 yy_3b4102561296bfc
#endif // IDCT32X32ASMV7

#ifndef IDCT_NZ_1X1_ASMV7
#define IDCT_NZ_1X1_ASMV7 yy_511102561457f96
#endif // IDCT_NZ_1X1_ASMV7

#ifndef IDST4X4_1X1_ASMV7
#define IDST4X4_1X1_ASMV7 yy_4da102561607ff5
#endif // IDST4X4_1X1_ASMV7

#ifndef IDCT16X16_4X4_ASMV7
#define IDCT16X16_4X4_ASMV7 yy_536102561764e45
#endif // IDCT16X16_4X4_ASMV7

#ifndef IDCT32X32_4X4_ASMV7
#define IDCT32X32_4X4_ASMV7 yy_53210256192323b
#endif // IDCT32X32_4X4_ASMV7

#ifndef IDCT_NONE_1X1
#define IDCT_NONE_1X1 yy_3cc102562072213
#endif // IDCT_NONE_1X1

#ifndef CopyBlock_4x4_neon
#define CopyBlock_4x4_neon yy_6d410256238260d
#endif // CopyBlock_4x4_neon

#ifndef CopyBlock_8x4_neon
#define CopyBlock_8x4_neon yy_6d8102562546b89
#endif // CopyBlock_8x4_neon

#ifndef CopyBlock_8x8_neon
#define CopyBlock_8x8_neon yy_6dc1025627030a
#endif // CopyBlock_8x8_neon

#ifndef CopyBlock_16x16_neon
#define CopyBlock_16x16_neon yy_73a10256285301c
#endif // CopyBlock_16x16_neon

#ifndef MC_InterLuma_4x4_neon
#define MC_InterLuma_4x4_neon yy_7ce10256301bdb
#endif // MC_InterLuma_4x4_neon

#ifndef MC_InterLuma_8x8_neon
#define MC_InterLuma_8x8_neon yy_7d61025631656ae
#endif // MC_InterLuma_8x8_neon

#ifndef MC_InterChroma_4x4_neon
#define MC_InterChroma_4x4_neon yy_89910256332732
#endif // MC_InterChroma_4x4_neon

#ifndef MC_InterChroma_8x8_neon
#define MC_InterChroma_8x8_neon yy_8a110256348120
#endif // MC_InterChroma_8x8_neon

#ifndef MC_InterLumaBi_4x4_neon
#define MC_InterLumaBi_4x4_neon yy_87910256363759a
#endif // MC_InterLumaBi_4x4_neon

#ifndef MC_InterLumaBi_8x8_neon
#define MC_InterLumaBi_8x8_neon yy_881102563792350
#endif // MC_InterLumaBi_8x8_neon

#ifndef MC_InterChromaBi_4x4_neon
#define MC_InterChromaBi_4x4_neon yy_9441025639422ee
#endif // MC_InterChromaBi_4x4_neon

#ifndef MC_InterChromaBi_8x8_neon
#define MC_InterChromaBi_8x8_neon yy_94c102564104b40
#endif // MC_InterChromaBi_8x8_neon

#ifndef AddAverageBi_4x4_neon
#define AddAverageBi_4x4_neon yy_7bd102564265878
#endif // AddAverageBi_4x4_neon

#ifndef AddAverageBi_8x8_neon
#define AddAverageBi_8x8_neon yy_7c5102564416b36
#endif // AddAverageBi_8x8_neon

#ifndef MC_InterLuma_neon
#define MC_InterLuma_neon yy_68f102564575cfd
#endif // MC_InterLuma_neon

#ifndef MC_InterChroma_neon
#define MC_InterChroma_neon yy_75a102564723e12
#endif // MC_InterChroma_neon

#ifndef MC_InterLumaBi_neon
#define MC_InterLumaBi_neon yy_73a102564881a49
#endif // MC_InterLumaBi_neon

#ifndef MC_InterChromaBi_neon
#define MC_InterChromaBi_neon yy_805102565045f32
#endif // MC_InterChromaBi_neon

#ifndef CopyBlock_neon
#define CopyBlock_neon yy_595102565193bf6
#endif // CopyBlock_neon

#ifndef MC_InterLumaWeighted
#define MC_InterLumaWeighted yy_7b1102565353a9e
#endif // MC_InterLumaWeighted

#ifndef MC_InterChromaWeighted
#define MC_InterChromaWeighted yy_87c10256550797d
#endif // MC_InterChromaWeighted

#ifndef MC_InterLumaWeightedBi
#define MC_InterLumaWeightedBi yy_85c102565665f49
#endif // MC_InterLumaWeightedBi

#ifndef MC_InterChromaWeightedBi
#define MC_InterChromaWeightedBi yy_92710256582ddc
#endif // MC_InterChromaWeightedBi

#ifndef CreateDpbList
#define CreateDpbList yy_506102575333c61
#endif // CreateDpbList

#ifndef PushDisPic
#define PushDisPic yy_3dc102575492fff
#endif // PushDisPic

#ifndef PopDisPic
#define PopDisPic yy_36b102575646c69
#endif // PopDisPic

#ifndef sortPicList
#define sortPicList yy_48010257580288f
#endif // sortPicList

#ifndef ClearDpbList
#define ClearDpbList yy_499102575963a61
#endif // ClearDpbList

#ifndef GetInterMergeCandidates
#define GetInterMergeCandidates yy_902102576891916
#endif // GetInterMergeCandidates

#ifndef ParseSliceParamID
#define ParseSliceParamID yy_66910257752401d
#endif // ParseSliceParamID

#ifndef xReconInter
#define xReconInter yy_4711025776771f0
#endif // xReconInter

#ifndef SetQPforQuant
#define SetQPforQuant yy_51d10257783384
#endif // SetQPforQuant

#ifndef xReconIntraQT
#define xReconIntraQT yy_512102577987f4f
#endif // xReconIntraQT

#ifndef H265_IntraLumaRecQTNew
#define H265_IntraLumaRecQTNew yy_7ba10257814494a
#endif // H265_IntraLumaRecQTNew

#ifndef H265_IntraChromaRecQTNew
#define H265_IntraChromaRecQTNew yy_88510257830677
#endif // H265_IntraChromaRecQTNew

#ifndef H265_IntraPrediction
#define H265_IntraPrediction yy_753102578454402
#endif // H265_IntraPrediction

#ifndef H265_MotionCompensation
#define H265_MotionCompensation yy_8aa1025786118d7
#endif // H265_MotionCompensation

#ifndef VO_ATOMIC_Inc
#define VO_ATOMIC_Inc yy_43a102580956ad4
#endif // VO_ATOMIC_Inc

#ifndef VO_ATOMIC_Dec
#define VO_ATOMIC_Dec yy_42c102581105a9f
#endif // VO_ATOMIC_Dec

#ifndef VO_ATOMIC_Cmpxchg
#define VO_ATOMIC_Cmpxchg yy_5ea102581264cd4
#endif // VO_ATOMIC_Cmpxchg

#ifndef setNDBBorderAvailabilityPerPPS
#define setNDBBorderAvailabilityPerPPS yy_b73102581425fa4
#endif // setNDBBorderAvailabilityPerPPS

#ifndef SAOProcessOneLCU
#define SAOProcessOneLCU yy_5c8102581572059
#endif // SAOProcessOneLCU

#ifndef processSao
#define processSao yy_42210258173127e
#endif // processSao

#ifndef processSaoBlock
#define processSaoBlock yy_60d1025818835
#endif // processSaoBlock

#ifndef xReadEpExGolomb
#define xReadEpExGolomb yy_5c6102582047cf
#endif // xReadEpExGolomb

#ifndef xReadUnaryMaxSymbol
#define xReadUnaryMaxSymbol yy_79f102582206732
#endif // xReadUnaryMaxSymbol

#ifndef PrepareDecodeFrame
#define PrepareDecodeFrame yy_6fe102582511af4
#endif // PrepareDecodeFrame

#ifndef DoDecodeFrame
#define DoDecodeFrame yy_4e210258266ecc
#endif // DoDecodeFrame

#ifndef DecodeNalu
#define DecodeNalu yy_3d41025828246cf
#endif // DecodeNalu

#ifndef PushTaskQ
#define PushTaskQ yy_3841025840737e6
#endif // PushTaskQ

#ifndef CheckPicFinished
#define CheckPicFinished yy_6241025842219d9
#endif // CheckPicFinished

#ifndef printCABACState
#define printCABACState yy_5781025851649f7
#endif // printCABACState

#ifndef printSliceInfo
#define printSliceInfo yy_5a910258532442b
#endif // printSliceInfo

#ifndef printLCUInfo
#define printLCUInfo yy_49d102585475078
#endif // printLCUInfo

#ifndef printQPInfo
#define printQPInfo yy_45a102585631481
#endif // printQPInfo

#ifndef printIntraPredictionInput
#define printIntraPredictionInput yy_a4c102585784087
#endif // printIntraPredictionInput

#ifndef printIntraPredictionOutput
#define printIntraPredictionOutput yy_acd102585947b44
#endif // printIntraPredictionOutput

#ifndef printxPredInterLuma
#define printxPredInterLuma yy_7c110258610590e
#endif // printxPredInterLuma

#ifndef printxPredInterChroma
#define printxPredInterChroma yy_88c10258625765f
#endif // printxPredInterChroma

#ifndef printMotionInfo
#define printMotionInfo yy_62f102586411850
#endif // printMotionInfo

#ifndef printReconInput
#define printReconInput yy_634102586562b00
#endif // printReconInput

#ifndef printDeQuantOut
#define printDeQuantOut yy_6171025867216d4
#endif // printDeQuantOut

#ifndef printResidualOutput
#define printResidualOutput yy_7f7102586887f61
#endif // printResidualOutput

#ifndef printSAOParam
#define printSAOParam yy_501102587033a8d
#endif // printSAOParam

#ifndef voCabacDecodeBin
#define voCabacDecodeBin yy_3a3766888466bb
#endif // voCabacDecodeBin

#ifndef voCabacDecodeBinEP
#define voCabacDecodeBinEP yy_6e17668899428b
#endif // voCabacDecodeBinEP

#ifndef voCabacDecodeBinsEP
#define voCabacDecodeBinsEP yy_694766891526a6
#endif // voCabacDecodeBinsEP

#ifndef vo_init_cabac_decoder
#define vo_init_cabac_decoder yy_5b37668931701f
#endif // vo_init_cabac_decoder

#ifndef voCabacDecodeBinTrm
#define voCabacDecodeBinTrm yy_76276689465d03
#endif // voCabacDecodeBinTrm

#ifndef H265DecGetParameter
#define H265DecGetParameter yy_1b2766826029
#endif // H265DecGetParameter

#ifndef H265DecGetOutputData
#define H265DecGetOutputData yy_41b76682914823
#endif // H265DecGetOutputData

#ifndef H265DecSetParameter
#define H265DecSetParameter yy_724766830718be
#endif // H265DecSetParameter

#ifndef ReleaseH265Threads_wave3D
#define ReleaseH265Threads_wave3D yy_4e976683226784
#endif // ReleaseH265Threads_wave3D

#ifndef InitH265Threads_wave3D
#define InitH265Threads_wave3D yy_52b76683384ae1
#endif // InitH265Threads_wave3D

#ifndef H265DecSetInputData
#define H265DecSetInputData yy_52576683533d6c
#endif // H265DecSetInputData

#ifndef H265DecUinit
#define H265DecUinit yy_91e76683692cd6
#endif // H265DecUinit

#ifndef DeleteH265Decoder
#define DeleteH265Decoder yy_725766838572ae
#endif // DeleteH265Decoder

#ifndef H265DecInit
#define H265DecInit yy_71f76684006952
#endif // H265DecInit

#ifndef CreateH265Decoder
#define CreateH265Decoder yy_aba76684165f90
#endif // CreateH265Decoder

#ifndef H265_IntraPred_Angular_mode_LT18_ASM
#define H265_IntraPred_Angular_mode_LT18_ASM yy_45976685877e87
#endif // H265_IntraPred_Angular_mode_LT18_ASM

#ifndef H265_IntraPred_Angular_mode_GE18_ASM
#define H265_IntraPred_Angular_mode_GE18_ASM yy_4537668603390c
#endif // H265_IntraPred_Angular_mode_GE18_ASM

#ifndef g_h265dec_inst
#define g_h265dec_inst yy_3f4766863499
#endif // g_h265dec_inst


#endif // __yyVVDef_HEVC_H__











