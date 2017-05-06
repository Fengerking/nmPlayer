/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#ifndef _PROTECT_H_
#define _PROTECT_H_
#define NAME_PROTECT
#ifdef NAME_PROTECT

//idct_add
#define Initialize_Decoder				__voMPEG2D0001	
#define Reference_IDCT					__voMPEG2D0002			
#define Add_Block						__voMPEG2D0003

//image
#define image_null						__voMPEG2D0004
#define image_create					__voMPEG2D0005
#define image_destroy					__voMPEG2D0006
#define image_swap						__voMPEG2D0007
#define image_copy						__voMPEG2D0008
//#define image_setedges					__voMPEG2D0009

//mpeg2_hr
#define extension_and_user_data			__voMPEG2D0010
#define sequence_header					__voMPEG2D0011
#define mpeg_decode_extension			__voMPEG2D0012

//mpeg2_mb
#define IDCT_Block4x4_c					__voMPEG2D0013
#define IDCT_Block4x8_c					__voMPEG2D0014
#define IDCT_Block8x8_c					__voMPEG2D0015
#define IdctBlock1x1_C					__voMPEG2D0016
#define wmmx_CopyBlock16x16             __voMPEG2D0017
#define wmmx_CopyBlock8x8				__voMPEG2D0018
#define wmmx_CopyBlock8x16              __voMPEG2D0019
#define wmmx_CopyBlock4x8				__voMPEG2D0020
#define WmmxIdctC						__voMPEG2D0021
//#define wmmx_transb4x8					__voMPEG2D0017
//#define wmmx_transb8x8					__voMPEG2D0018
//#define wmmx_transc8x8					__voMPEG2D0019
//#define arm_transb4x8					__voMPEG2D0020
//#define arm_transb8x8					__voMPEG2D0021
//#define arm_transc8x8					__voMPEG2D0022
#define MotionComp						__voMPEG2D0023
#define IDCT_Const4x4_c					__voMPEG2D1023
//mpeg2_mc
#define CopyBlockHorVer					__voMPEG2D0027
#define CopyBlockVer					__voMPEG2D0026
#define CopyBlockHor					__voMPEG2D0025
#define CopyBlock						__voMPEG2D0024
#define CopyBlockHorRound				__voMPEG2D0028
#define CopyBlockVerRound				__voMPEG2D0029
#define CopyBlockHorVerRound			__voMPEG2D0030
#define CopyBlock16x16_C				__voMPEG2D0031
#define CopyBlock8x8_C					__voMPEG2D0032
#define CopyBlock4x4_C					__voMPEG2D0033
#define AddBlock						__voMPEG2D0034
#define AddBlockHor						__voMPEG2D0035
#define AddBlockVer						__voMPEG2D0036
#define AddBlockHorVer					__voMPEG2D0037

#define wmmx_copyblk					__voMPEG2D0038
#define wmmx_copyblkh					__voMPEG2D0039
#define wmmx_copyblkv					__voMPEG2D0040
#define wmmx_copyblkhv					__voMPEG2D0041
#define wmmx_copyblkhrd					__voMPEG2D0042
#define wmmx_copyblkvrd					__voMPEG2D0043
#define wmmx_copyblkhvrd				__voMPEG2D0044
#define wmmx_addblk						__voMPEG2D0045
#define wmmx_addblkh					__voMPEG2D0046
#define wmmx_addblkv					__voMPEG2D0047
#define wmmx_addblkhv					__voMPEG2D0048

#define ArmInter8x8						__voMPEG2D0049
#define ArmInter8x8H					__voMPEG2D0050
#define ArmInter8x8V					__voMPEG2D0051
#define ArmInter8x8HV					__voMPEG2D0052
#define arm_copyblkhrd					__voMPEG2D0053
#define arm_copyblkvrd					__voMPEG2D0054
#define arm_copyblkhvrd					__voMPEG2D0055
#define ArmInter2_8x8					__voMPEG2D0056
#define ArmInter2_8x8H					__voMPEG2D0057
#define ArmInter2_8x8V					__voMPEG2D0058
#define ArmInter2_8x8HV					__voMPEG2D0059
#define ArmCopy16x16					__voMPEG2D0060
#define ArmCopy8x8						__voMPEG2D0061
#define AllCopyBlock					__voMPEG2D0062
#define AllAddBlock						__voMPEG2D0063


#define Image							__voMPEG2D0064
#define Mp2DecLocal					__voMPEG2D0065
#define CodecIdDec					__voMPEG2D0066
#define MpegDecode						__voMPEG2D0067

//mpeg_malloc
//#define mpeg_malloc						__voMPEG2D0068
//#define mpeg_free						__voMPEG2D0069

//mpeg_stream
#define GetBits							__voMPEG2D0070
#define EofBits							__voMPEG2D0071
#define bytealign						__voMPEG2D0072
#define BitsToNextByte					__voMPEG2D0073
#define bytepos							__voMPEG2D0074

//readbits
#define InitBitStream						__voMPEG2D0075
// #define UPDATE_CACHE						__voMPEG2D0076
#define showbitslarge					__voMPEG2D0077

//files

#define mpeg1_vld						__voMPEG2D0078
#define	mpeg2_vld						__voMPEG2D0079

#define scan							__voMPEG2D0080
#define dec_buf_ini						__voMPEG2D0081
#define sequence_extension				__voMPEG2D0082

#define sequence_display_extension		__voMPEG2D0083
#define quant_matrix_extension			__voMPEG2D0084
#define sequence_scalable_extension		__voMPEG2D0085
#define picture_display_extension		__voMPEG2D0086
#define picture_coding_extension		__voMPEG2D0087

#define IDCT_Col8						__voMPEG2D0088
#define IDCT_Row8						__voMPEG2D0089
#define IDCT_Row4						__voMPEG2D0090
#define idct_col_half					__voMPEG2D0091
#define IDCT_Row4_half					__voMPEG2D0092
#define IDCT_Block2x2					__voMPEG2D0093

#define CopyBlock4x4_01					__voMPEG2D0094
#define CopyBlock4x4_02					__voMPEG2D0095
#define CopyBlock4x4_03					__voMPEG2D0096
#define CopyBlock4x4_10					__voMPEG2D0097	
#define CopyBlock4x4_11					__voMPEG2D0098
#define CopyBlock4x4_12					__voMPEG2D0099
#define CopyBlock4x4_13					__voMPEG2D0100
#define CopyBlock4x4_20					__voMPEG2D0101
#define CopyBlock4x4_21					__voMPEG2D0102
#define CopyBlock4x4_22					__voMPEG2D0103
#define CopyBlock4x4_23					__voMPEG2D0104
#define CopyBlock4x4_30					__voMPEG2D0105
#define CopyBlock4x4_31					__voMPEG2D0106
#define CopyBlock4x4_32					__voMPEG2D0107
#define CopyBlock4x4_33					__voMPEG2D0108

#define CopyBlock4x4_01R				__voMPEG2D0109
#define CopyBlock4x4_02R				__voMPEG2D0110
#define CopyBlock4x4_03R				__voMPEG2D0111
#define CopyBlock4x4_10R				__voMPEG2D0112
#define CopyBlock4x4_11R				__voMPEG2D0113
#define CopyBlock4x4_12R				__voMPEG2D0114
#define CopyBlock4x4_13R				__voMPEG2D0115
#define CopyBlock4x4_20R				__voMPEG2D0116
#define CopyBlock4x4_21R				__voMPEG2D0117
#define CopyBlock4x4_22R				__voMPEG2D0118
#define CopyBlock4x4_23R				__voMPEG2D0119
#define CopyBlock4x4_30R				__voMPEG2D0120
#define CopyBlock4x4_31R				__voMPEG2D0121
#define CopyBlock4x4_32R				__voMPEG2D0122
#define CopyBlock4x4_33R				__voMPEG2D0123
#define AddBlock4x4_00					__voMPEG2D0124
#define AddBlock4x4_01					__voMPEG2D0125
#define AddBlock4x4_02					__voMPEG2D0126
#define AddBlock4x4_03					__voMPEG2D0127
#define AddBlock4x4_10					__voMPEG2D0128
#define AddBlock4x4_11					__voMPEG2D0129
#define AddBlock4x4_12					__voMPEG2D0130
#define AddBlock4x4_13					__voMPEG2D0131
#define AddBlock4x4_20					__voMPEG2D0132
#define AddBlock4x4_21					__voMPEG2D0133
#define AddBlock4x4_22					__voMPEG2D0134
#define AddBlock4x4_23					__voMPEG2D0135
#define AddBlock4x4_30					__voMPEG2D0136
#define AddBlock4x4_31					__voMPEG2D0137
#define AddBlock4x4_32					__voMPEG2D0138
#define AddBlock4x4_33					__voMPEG2D0139

#define Picture                         __voMPEG2D0140
#define clearblock                      __voMPEG2D0141
#define getDCsizeLum					__voMPEG2D0142
#define getDCsizeChr                    __voMPEG2D0143
#define getDCdiff                       __voMPEG2D0144
#define mpeg1_dec_mb_intra              __voMPEG2D0145
#define mpeg2_dec_mb_intra              __voMPEG2D0146
#define non_linear_qscale				__voMPEG2D0147
#define get_qscale                      __voMPEG2D0148
#define readskip                        __voMPEG2D0149
#define getMV                           __voMPEG2D0150
#define update_mv                       __voMPEG2D0151
#define decodeInter_mpeg1				__voMPEG2D0152
#define decodeInter_mpeg2				__voMPEG2D0153
#define dec_mb_inter                    __voMPEG2D0154
#define IVOP_Slice						__voMPEG2D0155
#define PVOP_Slice						__voMPEG2D0156
#define BVOP_Slice						__voMPEG2D0157
#define dec_frame                       __voMPEG2D0158
	
#define FrameRate                       __voMPEG2D0159
#define MPEG1_IntraMatrix				__voMPEG2D0160
#define mcbp_p                          __voMPEG2D0161
#define mb_type_p                       __voMPEG2D0162
#define mb_type_b                       __voMPEG2D0163
#define skip                            __voMPEG2D0164
#define mv_tab                          __voMPEG2D0165
#define mp2FindStartCode				__voMPEG2D0166
#define mv_tab                          __voMPEG2D0165
#define mv_tab                          __voMPEG2D0165

//dual prime
#define dualPrimeArithmetic             __voMPEG2D0167
#define getDmvector						__voMPEG2D0168

#define IDCT_Block4x8_2_c               __voMPEG2D0169
#define CopyBlock8x16_C                 __voMPEG2D0170
#define CopyBlock4x8_C                  __voMPEG2D0171
#define	CopyBlock_4x8					__voMPEG2D0172
#define	CopyBlockHor4x8                 __voMPEG2D0173
#define	CopyBlockVer4x8                 __voMPEG2D0174
#define	CopyBlockHorVer4x8              __voMPEG2D0175
#define AddBlock4x8						__voMPEG2D0176
#define AddBlockHor4x8					__voMPEG2D0177
#define AddBlockVer4x8					__voMPEG2D0178
#define AddBlockHorVer4x8               __voMPEG2D0179
#define AllCopyBlock4_8                 __voMPEG2D0180
#define AllAddBlock4x8					__voMPEG2D0181

#define ArmIdctA						__voMPEG2D0182
#define ArmIdctB						__voMPEG2D0183
#define ArmIdctC						__voMPEG2D0184
#define ArmIdctD						__voMPEG2D0185

#define ArmInter4x8						__voMPEG2D0186
#define ArmInter4x8V					__voMPEG2D0187	
#define ArmInter4x8H					__voMPEG2D0188	
#define ArmInter4x8HV					__voMPEG2D0189	
#define ArmInter2_4x8					__voMPEG2D0190	
#define ArmInter2_4x8V					__voMPEG2D0191	
#define ArmInter2_4x8H					__voMPEG2D0192	
#define ArmInter2_4x8HV                 __voMPEG2D0193
#define ArmCopy4x8						__voMPEG2D0194	
#define ArmCopy8x16						__voMPEG2D0195

#define wmmx4x8_copyblk                 __voMPEG2D0196
#define wmmx4x8_copyblkv                __voMPEG2D0197
#define wmmx4x8_copyblkh                __voMPEG2D0198
#define wmmx4x8_copyblkhv               __voMPEG2D0199                                                        	  	  	  	
#define wmmx4x8_addblk                  __voMPEG2D0200
#define wmmx4x8_addblkv                 __voMPEG2D0201
#define wmmx4x8_addblkh                 __voMPEG2D0202
#define wmmx4x8_addblkhv                __voMPEG2D0203

#define BCopyBlock_8x4                  __voMPEG2D0204
#define BCopyBlockHor8x4				__voMPEG2D0205
#define BCopyBlockVer8x4				__voMPEG2D0206
#define BCopyBlockHorVer8x4             __voMPEG2D0207
#define CopyBlock8x4_C                  __voMPEG2D0209

// AddBlock DstPitch=8
#define BAddBlock8x4					__voMPEG2D0210
#define BAddBlockHor8x4                 __voMPEG2D0211
#define BAddBlockVer8x4                 __voMPEG2D0212
#define BAddBlockHorVer8x4              __voMPEG2D0213

#define BArmInter8x4                    __voMPEG2D0214
#define BArmInter8x4H					__voMPEG2D0215
#define BArmInter8x4V					__voMPEG2D0216
#define BArmInter8x4HV					__voMPEG2D0217
#define BArmInter2_8x4					__voMPEG2D0218
#define BArmInter2_8x4H                 __voMPEG2D0219
#define BArmInter2_8x4V                 __voMPEG2D0220
#define BArmInter2_8x4HV				__voMPEG2D0221
#define ArmCopy8x4						__voMPEG2D0223

#define BAllCopyBlock8x4				__voMPEG2D0224
#define BAllAddBlock8x4                 __voMPEG2D0225

#define CopyBlock4x8_01                 __voMPEG2D0226
#define CopyBlock4x8_03                 __voMPEG2D0227
#define CopyBlock4x8_11                 __voMPEG2D0228
#define CopyBlock4x8_13                 __voMPEG2D0229

#define AddBlock4x8_01					__voMPEG2D0230
#define AddBlock4x8_03					__voMPEG2D0231
#define AddBlock4x8_11					__voMPEG2D0232
#define AddBlock4x8_13					__voMPEG2D0233

#define NewAllCopyBlock4_8              __voMPEG2D0234
#define NewAllAddBlock4x8               __voMPEG2D0235

#define BCopyBlock16x8_C                __voMPEG2D0236
#define BCopyBlock8x4_C                 __voMPEG2D0237

#define ArmIdctE                        __voMPEG2D0238
#define BArmCopy16x8					__voMPEG2D0239
#define BArmCopy8x4                     __voMPEG2D0240
#define ArmIdctF                        __voMPEG2D0241
#define IDCT_Block8x4_c                 __voMPEG2D0242
#define IDCT_Block4x4_2_c               __voMPEG2D0243

#define Bmpeg2_dec_mb_intra             __voMPEG2D0244
#define BdecodeInter_mpeg1			    __voMPEG2D0245
#define BdecodeInter_mpeg2			    __voMPEG2D0246

#define buf_seq_ctl                     __voMPEG2D0247
#define reset_buf_seq                   __voMPEG2D0248
#define get_frame_type                  __voMPEG2D0249  

//#define Armv7IdctA                  __voMPEG2D0247
//#define Armv7IdctB                  __voMPEG2D0248
//#define Armv7IdctC                  __voMPEG2D0249
//#define Armv7IdctD                  __voMPEG2D0250
//#define Armv7IdctE                  __voMPEG2D0251
//#define Armv7IdctF                  __voMPEG2D0252

//huwei 20090903 IDCT4x4
#define clearblock1x1                 __voMPEG2D0253
#define clearblock4x8                 __voMPEG2D0254 
#define IDCT_Col4                     __voMPEG2D0255 
#define IDCT_Block4x4_Block8x8_c      __voMPEG2D0256
#define ArmIdctG                      __voMPEG2D0257 
//#define Armv7IdctG                  __voMPEG2D0258

#define MallocMem                     __voMPEG2D0259
#define FreeMem                       __voMPEG2D0260
#define SetMem                        __voMPEG2D0261
#define MoveMem                       __voMPEG2D0262
#define CopyMem                       __voMPEG2D0263
	
#define	GetBits32	__voMPEG2D0264
#define	MCMpegBlock_C	__voMPEG2D0265
#define	MCMpegBlockHor_C	__voMPEG2D0266
#define	MCMpegBlockVer_C	__voMPEG2D0267
#define	MCMpegBlockHorVer_C	__voMPEG2D0268
#define	MCMpegBlockAdd_C	__voMPEG2D0269
#define	MCMpegBlockAddHor_C	__voMPEG2D0270
#define	MCMpegBlockAddVer_C	__voMPEG2D0271
#define	MCMpegBlockAddHorVer_C	__voMPEG2D0272
#define	MCMpegBlock4x8_01	__voMPEG2D0273
#define	MCMpegBlock4x8_03	__voMPEG2D0274
#define	MCMpegBlock4x8_11	__voMPEG2D0275
#define	MCMpegBlock4x8_13	__voMPEG2D0276
#define	MCMpegBlockAdd4x8_01	__voMPEG2D0277
#define	MCMpegBlockAdd4x8_03	__voMPEG2D0278
#define	MCMpegBlockAdd4x8_11	__voMPEG2D0279
#define	MCMpegBlockAdd4x8_13	__voMPEG2D0280
#define	MCMpegBlock4x8_C	__voMPEG2D0281
#define	MCMpegBlock4x8Hor_C	__voMPEG2D0282
#define	MCMpegBlock4x8Ver_C	__voMPEG2D0283
#define	MCMpegBlock4x8HorVer_C	__voMPEG2D0284
#define	MCMpegBlockAdd4x8_C	__voMPEG2D0285
#define	MCMpegBlockAdd4x8Hor_C	__voMPEG2D0286
#define	MCMpegBlockAdd4x8Ver_C	__voMPEG2D0287
#define	MCMpegBlockAdd4x8HorVer_C	__voMPEG2D0288
#define	BMCMpegBlockVer8x4_C	__voMPEG2D0289
#define	BMCMpegBlockHorVer8x4_C	__voMPEG2D0290
#define	BMCMpegBlockAddVer8x4_C	__voMPEG2D0291
#define	BMCMpegBlockAddHorVer8x4_C	__voMPEG2D0292
#define	IdctBlock4x8_C	__voMPEG2D0293
#define	IdctBlock4x8Two_C	__voMPEG2D0294
#define	IdctBlock8x8_C	__voMPEG2D0295
#define	IdctBlock8x4_C	__voMPEG2D0296
#define	IdctBlock4x4Two_C	__voMPEG2D0297
#define	IdctBlock4x4_C	__voMPEG2D0298
#define	Bit16_IDCT_Block4x4_Block8x8_c	__voMPEG2D0299
#define	Bit16_IDCT_Block4x8_c	__voMPEG2D0300
#define	Bit16_IDCT_Block8x8_c	__voMPEG2D0301
#define	Bit16_IDCT_Const8x8_c	__voMPEG2D0302
#define	Bit16_Sat_Add_c	__voMPEG2D0303

#if defined(_IOS)
#define	Arm11IdctA	__voMPEG2D0304
#define	Arm11IdctB	__voMPEG2D0305
#define	Arm11IdctC	__voMPEG2D0306
#define	Arm11IdctD	__voMPEG2D0307
#define	_Arm11IdctA	___voMPEG2D0304
#define	_Arm11IdctB	___voMPEG2D0305
#define	_Arm11IdctC	___voMPEG2D0306
#define	_Arm11IdctD	___voMPEG2D0307
#endif

#define	IdctColunm4	__voMPEG2D0308
#define	IdctColunm8	__voMPEG2D0309
#define	IdctLine8	__voMPEG2D0310
#define	IdctLine4	__voMPEG2D0311

#endif
#endif//_PROTECT_H_
