/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _PROTECT_H_
#define _PROTECT_H_

#ifndef PROTECT_NAME
#define PROTECT_NAME
#endif

#ifndef PROTECT_NAME

#define quant_multi_table      __voMPEG4E0220	//__voMPEG4E0220 is used in asm code.so should enable it.

#else

//bitstream.h
#define Mpeg4WriteVolHeader                     __voMPEG4E0001
#define Mpeg4WriteVopHeader						__voMPEG4E0002
#define BitstreamWriteUserData						__voMPEG4E0003
//#define H263WritePicHeader							__voMPEG4E0004
#define WriteH263MBHeader							__voMPEG4E0005
#define H263VlcEncode								__voMPEG4E0006
#define EncodeH263Coeff							__voMPEG4E0008
#define H263CountBitsVectors						__voMPEG4E0009
#define InitBits								__voMPEG4E0010
//#define BitstreamReset								__voMPEG4E0011
#define GetBitsPos								__voMPEG4E0012
#define FlushBits								__voMPEG4E0013
#define BitstreamForward							__voMPEG4E0014
#define PutBit								__voMPEG4E0015
#define PutBits							__voMPEG4E0016
//#define padding_codes								__voMPEG4E0017
#define PadBits								__voMPEG4E0018
#define PadBitsAlways							__voMPEG4E0019
#define log2bin										__voMPEG4E0020

#define CalcCbp_C								    __voMPEG4E0021

//mbcoding.h
//#define InitMPEG4VLCTab								__voMPEG4E0022
#define MBSkip										__voMPEG4E0023

//zigzag.h
#define scan_tables									__voMPEG4E0025

//voMpegEncFastDct.h
#define FastDct_C									__voMPEG4E0026
//#define fdct_float								    __voMPEG4E0026
//#define fdct_arm									__voMPEG4E0027

//idct.h
#define idct_int32									__voMPEG4E0028
#define simple_idct_c								__voMPEG4E0029
#define simple_idct_arm								__voMPEG4E0031

//colorspace.h
#define YUV420toYUV420_C								__voMPEG4E0032

//frame.h
#define FrameClean									__voMPEG4E0034
#define FrameCreate								__voMPEG4E0036
#define FrameDestroy								__voMPEG4E0037
#define FrameCopy									__voMPEG4E0038
#define FrameSetEdge								__voMPEG4E0039
#define FramePsnr									__voMPEG4E0040
#define SeeToPsnr									__voMPEG4E0041
#define PlaneSSE									__voMPEG4E0042
#define FramePreProcess									__voMPEG4E0043

//interpolate8x8.h

//#define inplace16_interpolate_halfpel_wmmx0			__voMPEG4E0054
//#define inplace16_interpolate_halfpel_wmmx1			__voMPEG4E0055
//#define inplace16_interpolate_halfpel_arm			__voMPEG4E0056
#define interpolate8x8_halfpel_h_arm				__voMPEG4E0057
#define interpolate8x8_halfpel_v_arm				__voMPEG4E0058
#define interpolate8x8_halfpel_hv_arm				__voMPEG4E0059
#define inplace16_interpolate_halfpel_c				__voMPEG4E0060


//estimation.h
#define SearchData									__voMPEG4E0062
#define CheckChromaSAD								__voMPEG4E0063
#define SubpelRefine								__voMPEG4E0065
#define FullRefine_Fast								__voMPEG4E0066
#define GetMinFcode									__voMPEG4E0067

//motion.h
#define MotionEstimation							__voMPEG4E0068

//motion_inlines.h
#define GetSearchRange               __voMPEG4E0070
#define r_mvtab                          __voMPEG4E0071
#define CalculateMvBits                __voMPEG4E0072
#define SetInterMB                      __voMPEG4E0073
#define vector_repeats                 __voMPEG4E0074
#define GenerateMask                  __voMPEG4E0075

//sad.h
#define Sad16x16_C										__voMPEG4E0076
//#define sad16_wmmx									__voMPEG4E0077
//#define sad16_arm									__voMPEG4E0078
#define Sad8x8_C										__voMPEG4E0079
//#define Sad8Armv4									__voMPEG4E0080
//#define sad8_wmmx									__voMPEG4E0081
//#define dev16_c										__voMPEG4E0082
//#define dev16_wmmx									__voMPEG4E0083
//#define sad16v2_no4mv_arm							__voMPEG4E0084
#define Sad16x16FourMv_C									__voMPEG4E0086
//#define sad16v_wmmx									__voMPEG4E0087
//#define sad16v_wmmx2								__voMPEG4E0088

//prediction.h
#define ACDCPrediction								__voMPEG4E0089
#define GetPredACDC								__voMPEG4E0091
#define GetBlockPredictMV									__voMPEG4E0092
#define GetMacroBlockPredictMV								__voMPEG4E0093

//quant.h
#define QuantIntraH263_C							__voMPEG4E0094
#define QuantInterH263Mpeg4_C                   __voMPEG4E0095
#define QuantIntraMpeg4_C                    __voMPEG4E0096
#define DequantIntraH263Mpeg4_C				__voMPEG4E0097
//#define quant_h263_inter_wmmx						__voMPEG4E0102
//#define dequant_h263_intra_wmmx						__voMPEG4E0103
//#define QuantInterH263Armv6						__voMPEG4E0104

//mem_align.h
#define voMpegMalloc									__voMPEG4E0108
#define voMpegFree									__voMPEG4E0109

//plugin_single.h
#define RC_SINGLE									__voMPEG4E0110
//#define GetIniQuant							__voMPEG4E0111
#define RCCreate							__voMPEG4E0112
#define MpegRCDestroy							__voMPEG4E0113
#define MpegRCBefore							__voMPEG4E0114
#define MpegRCAfter								__voMPEG4E0115

//H.263-ctables.h
//#define vlc_mv_coeff								__voMPEG4E0117	
#define vlc_cbpcm_intra_coeff						__voMPEG4E0118
#define vlc_cbpcm_coeff								__voMPEG4E0119
#define vlc_cbpy_coeff								__voMPEG4E0120	
#define vlc_3d_coeff								__voMPEG4E0121


/*****************************************************************************
* transfer8to16 API
****************************************************************************/
#define ExpandByteToShort_C						__voMPEG4E0122


/*****************************************************************************
* transfer16to8 API
****************************************************************************/

#define transfer_16to8copy_c      __voMPEG4E0123

//#define transfer_16to8copy_wmmx   __voMPEG4E0124



/*****************************************************************************
* transfer8to16 + substraction *writeback* op API
****************************************************************************/
#define transfer_8to16sub_c        __voMPEG4E0125
//#define transfer_8to16sub_wmmx     __voMPEG4E0126
//#define transfer_8to16sub_arm      __voMPEG4E0127

/*****************************************************************************
* transfer8to16 + substraction *readonly* op API
****************************************************************************/

#define transfer_16to8add_c        __voMPEG4E0128
//#define transfer_16to8add_wmmx     __voMPEG4E0129
#define transfer_16to8add_arm      __voMPEG4E0130

/*****************************************************************************
* transfer8to8 + no op
****************************************************************************/
#define transfer8x8_copy_c          __voMPEG4E0131
//#define transfer8x8_copy_wmmx       __voMPEG4E0132

/*******************************************************************************
* mcommop_cc.h
********************************************************************************/


#define CopyBlock                   __voMPEG4E0133
#define CopyBlockHor                __voMPEG4E0134
#define CopyBlockVer                __voMPEG4E0135
#define CopyBlockHorVer             __voMPEG4E0136
#define CopyBlockHorRound           __voMPEG4E0137
#define CopyBlockVerRound           __voMPEG4E0138
#define CopyBlockHorVerRound        __voMPEG4E0139


// AddBlock DstPitch=8
#define AddBlock                    __voMPEG4E0143
#define AddBlockHor                 __voMPEG4E0144
#define AddBlockVer                 __voMPEG4E0145
#define AddBlockHorVer              __voMPEG4E0146


#define SubBlock                    __voMPEG4E0148
#define SubBlockHor                 __voMPEG4E0149
#define SubBlockVer                 __voMPEG4E0150
#define SubBlockHorVer              __voMPEG4E0151
#define SubBlockHorRound            __voMPEG4E0152
#define SubBlockVerRound            __voMPEG4E0153
#define SubBlockHorVerRound         __voMPEG4E0154


#define WMMXCopyBlock               __voMPEG4E0155
#define WMMXCopyBlockHor            __voMPEG4E0156
#define WMMXCopyBlockVer            __voMPEG4E0157
#define WMMXCopyBlockHorVer         __voMPEG4E0158
#define WMMXCopyBlockHorRound       __voMPEG4E0159
#define WMMXCopyBlockVerRound       __voMPEG4E0160
#define WMMXCopyBlockHorVerRound    __voMPEG4E0161
#define WMMXAddBlock                __voMPEG4E0162
#define WMMXAddBlockHor             __voMPEG4E0163 
#define WMMXAddBlockVer             __voMPEG4E0164
#define WMMXAddBlockHorVer          __voMPEG4E0165


#define ARMCopyBlock                __voMPEG4E0166
#define ARMCopyBlockHor             __voMPEG4E0167
#define ARMCopyBlockVer             __voMPEG4E0168
#define ARMCopyBlockHorVer          __voMPEG4E0169
#define ARMCopyBlockHorRound        __voMPEG4E0170
#define ARMCopyBlockVerRound        __voMPEG4E0171
#define ARMCopyBlockHorVerRound     __voMPEG4E0172
#define ARMAddBlock                 __voMPEG4E0173
#define ARMAddBlockHor              __voMPEG4E0174
#define ARMAddBlockVer              __voMPEG4E0175
#define ARMAddBlockHorVer           __voMPEG4E0176
#define ARMCopyBlock16x16           __voMPEG4E0179
#define ARMCopyBlock8x8             __voMPEG4E0180


#define AllCopyBlock                __voMPEG4E0181
#define AllSubBlock                 __voMPEG4E0182
#define AllAddBlock                 __voMPEG4E0183
//#define CopyBlock8x8_C              __voMPEG4E0185
//#define CopyBlock16x16              __voMPEG4E0186


//#define coeff_tab                   __voMPEG4E0187
//#define max_level                   __voMPEG4E0188
//#define max_run                     __voMPEG4E0189
#define mcbpc_intra_table             __voMPEG4E0191
#define mcbpc_inter_table             __voMPEG4E0192
#define cbp_lunina_table                    __voMPEG4E0193
//#define dcy_tab                     __voMPEG4E0194
//#define dcc_tab                     __voMPEG4E0195
#define mb_motion_table             __voMPEG4E0196
//#define mcbpc_intra_table           __voMPEG4E0197
//#define mcbpc_inter_table           __voMPEG4E0198
//#define cbpy_table                  __voMPEG4E0199



#define me_lambda_table				__voMPEG4E0201
#define CheckCandidate16FourMV            __voMPEG4E0202
#define CheckCandidate16        __voMPEG4E0203

#define MacroBlockModeDecision			__voMPEG4E0205		 
#define PreparePredictionsP         __voMPEG4E0206
#define MotionStatsPVOP             __voMPEG4E0207
#define MacroBlockMotionCompensation        __voMPEG4E0209

#define CodeVector                  __voMPEG4E0211
#define CodeCoeffInter              __voMPEG4E0212
#define CodeCoeffIntra              __voMPEG4E0213
#define CalcIntraCoeffBits         __voMPEG4E0214
#define CodeCoeffInter_CalcBits     __voMPEG4E0215
#define DeQuant_Table                      __voMPEG4E0216
#define EncodeMpeg4IntraMB              __voMPEG4E0217
#define EncodeMpeg4InterMB              __voMPEG4E0218

#define quant_multi_table                 __voMPEG4E0220
//#define dc_scalar                   __voMPEG4E0221
#define MacroBlockFDCT                      __voMPEG4E0222
#define MBiDCT                      __voMPEG4E0223
#define MacroBlockQuantIntra                __voMPEG4E0224
#define MacroBlockDeQuantIntra              __voMPEG4E0225
#define MacroBlockQuantInter                __voMPEG4E0226
#define MBDeQuantInter              __voMPEG4E0227
#define MacroBlockTrans8to16                __voMPEG4E0228
#define MBTrans16to8                __voMPEG4E0229
#define MacroBlockTransFormIntra           __voMPEG4E0230
#define MacroBlockTransFormInter           __voMPEG4E0231

#define  me_lambda_vec16           __voMPEG4E0232

//#define roundtab                    __voMPEG4E0233
#define round_tab_76                 __voMPEG4E0234
//#define round_tab_78                 __voMPEG4E0235
#define round_tab_79                 __voMPEG4E0236

//#define vlc_mv_coeff				__voMPEG4E0236
//#define vlc_cbpcm_intra_coeff       __voMPEG4E0236
//#define vlc_cbpcm_coeff             __voMPEG4E0236
//#define vlc_cbpy_coeff              __voMPEG4E0236
//#define vlc_3d_coeff                __voMPEG4E0236

#define GeDcScaler               __voMPEG4E0239


#define vop_presets					__voMPEG4E0240
#define motion_presets              __voMPEG4E0241
#define vomp4h263e                  __voMPEG4E0242
#define UpdateTC               __voMPEG4E0245
#define MBSetDquant                 __voMPEG4E0246
#define SetTimecodes               __voMPEG4E0247
#define voMpeg4EncFrame           __voMPEG4E0248
#define SetMBsQP         __voMPEG4E0249
#define CodeIntraMB                 __voMPEG4E0250
#define EncodeIVOP                  __voMPEG4E0251
#define PredictFcode                 __voMPEG4E0252
#define EncodePVOP                  __voMPEG4E0253
//#define vompeg4_parse_configuration_file    __voMPEG4E0254
#define	vompeg4_enc_init                    __voMPEG4E0255
#define voMpeg4WriteVIH            __voMPEG4E0256

//#define QuantIntraH263	        __voMPEG4E0257
#define quant_mpeg_intra			__voMPEG4E0258
//#define DequantIntraH263Mpeg4			__voMPEG4E0259
#define dequant_mpeg_intra			__voMPEG4E0260


#define CalcACDCCoeff             __voMPEG4E0263
#define CalACDCBits              __voMPEG4E0264
#define ApplyACDC                  __voMPEG4E0265

#define _me_DiamondSearch           __voMPEG4E0266
#define DiamondSearch            __voMPEG4E0267
#define _me_SquareSearch            __voMPEG4E0268

//#define sad16                       __voMPEG4E0269
//#define sad8                        __voMPEG4E0270
//#define dev16                       __voMPEG4E0271
//#define sad16v                      __voMPEG4E0272
//#define sad16v2                     __voMPEG4E0273

//#define QuantIntraH263            __voMPEG4E0274
//#define QuantInterH263Mpeg4            __voMPEG4E0275
//#define DequantIntraH263Mpeg4          __voMPEG4E0276
#define dequant_h263_inter          __voMPEG4E0277

//#define transfer8x8_copy            __voMPEG4E0278
//#define transfer_16to8add           __voMPEG4E0279
//#define transfer_16to8copy          __voMPEG4E0280
//#define transfer_8to16copy          __voMPEG4E0281
//#define transfer_8to16sub           __voMPEG4E0282

//#define	CalcCBP                    __voMPEG4E0284

//#define fdct                            __voMPEG4E0285
//#define h263_free_Ehuff                 __voMPEG4E0286
//#define h263_free_huff                  __voMPEG4E0287
#define InitH263VLCTab                  __voMPEG4E0288
#define h263_load_Etable                __voMPEG4E0289
#define h263_make_Ehuff                 __voMPEG4E0290

//#define inplace16_interpolate_halfpel_arm  __voMPEG4E0295
//#define vlc_3d                             __voMPEG4E0296
//#define vlc_cbpcm                          __voMPEG4E0297
//#define vlc_cbpcm_intra                    __voMPEG4E0298
//#define vlc_cbpy                           __voMPEG4E0301
//#define vlc_mv                             __voMPEG4E0302
//#define IdctCol8						   __voMPEG4E0303
#define HalfPixelRefine	 					__voMPEG4E0304
//#define IdctRow8C							__voMPEG4E0305
//#define IdctRow8							__voMPEG4E0306
//#define Idct8x8C							__voMPEG4E0307
#define CopyBlock16x16_C							__voMPEG4E0308
#define Inplace16InterpolateHP		__voMPEG4E0309
#define Clip_255Base_table            __voMPEG4E0310
#define Clip255_table								__voMPEG4E0311
//#define sad_abs								__voMPEG4E0312
//#define	InitContTab							__voMPEG4E0313
//#define idct_clp							__voMPEG4E0314
//#define idct_clip							__voMPEG4E0315
//#define sad_abs_tab							__voMPEG4E0316

//#define YUV420toYUV420_C					__voMPEG4E0317
#define UYVYtoYUV420_C					__voMPEG4E0318
#define YUYVtoYUV420_C					__voMPEG4E0319
#define UYVYtoYUV420RT180_C				__voMPEG4E0320
#define RGB565toYUV420Disable_C			__voMPEG4E0321
#define RGB565toYUV420FilpY_C			__voMPEG4E0322
#define RGB565toYUV420RT180_C			__voMPEG4E0323
#define RGB565toYV12RT90L_C				__voMPEG4E0324
#define VYUY2toYUV420Disable_C			__voMPEG4E0325
#define VYUY2toYUV420RT180_C				__voMPEG4E0326
#define YUYVtoYUV420RT180_C				__voMPEG4E0327

#define yuv420_to_yuv420_all				__voMPEG4E0328
#define uyvy_to_yuv420_all					__voMPEG4E0329
#define rgb565_to_yuv420_all				__voMPEG4E0330
#define vyuy2_to_yuv420_all					__voMPEG4E0331
#define yuyv_to_yuv420_all					__voMPEG4E0332
#define HalfpelRefineSad16x16				__voMPEG4E0333
#define MpegRCCreate								__voMPEG4E0334
#define GetMBQ								__voMPEG4E0335
#define IDCT_Block4x8_c						__voMPEG4E0336
#define IDCT_Const8x8_c						__voMPEG4E0337
#define voMpegSetConfig				__voMPEG4E0338
#define GetMBQP								__voMPEG4E0339
#define	CandidateSearch						__voMPEG4E0340

#define CInter8X8							__voMPEG4E0341
#define CInter8X8H							__voMPEG4E0342
#define CInter8X8V							__voMPEG4E0343
#define CInter8X8HV							__voMPEG4E0344
#define CInter8X8HRD						__voMPEG4E0345
#define CInter8X8VRD						__voMPEG4E0346
#define CInter8X8HVRD						__voMPEG4E0347
//#define CopyBlock16x16_C							__voMPEG4E0348
#define CopyBlock8x8_C							__voMPEG4E0349
#define voMpeg4EncFinish					__voMPEG4E0350
//#define QuantIntraMpeg4_C					__voMPEG4E0351
//#define QuantInter_Mpeg4_c                 __voMPEG4E0352

#define RGB32toYUV420Disable_C			__voMPEG4E0351
#endif//PROTECT_NAME
//ASSEMBLY 
//#define ARMV5E_InPlaceDeblockMBs			ARM_DM
//#define GetBlockLumaNxNInBound				ARM_GB1
//#define GetBlockChromaNxNInBound			ARM_GB2
//#define SetInPlaceInterMBStrength			SETINMS



#endif//_PROTECT_H_
