#ifndef _NCNAMETAB_H_
#define _NCNAMETAB_H_

#ifdef VODIVX3
#define PROTECT_NAME			1	//for copy right protection
#if PROTECT_NAME

//bitstream
#define bitinit							__DIVX3D0001
#define bitload							__DIVX3D0002
#define bitshowlarge					__DIVX3D0003
#define GMCLuma_C					__DIVX3D0004
#define GMCChroma_C					__DIVX3D0005
#define GMCGetAvgMV_C				__DIVX3D0006
#define GMCLuma1Pt_C				__DIVX3D0007
#define GMCChroma1Pt_C				__DIVX3D0008
#define GMCGetAvgMV1pt_C			__DIVX3D0009
#define IniGMCParam					__DIVX3D0010


#define generate_GMCimage				__DIVX3D0011
#define H263GetHdr						__DIVX3D0012
#define H263DecIntraMB					__DIVX3D0013
#define H263DecInterMB					__DIVX3D0014
#define H263DecPVOP						__DIVX3D0015
#define H263DecIVOP						__DIVX3D0016
#define H263DecBVOP						__DIVX3D0017
#define H263GetBlockVld					__DIVX3D0018

#define IDCT_Col8						__DIVX3D0019
#define IDCT_RowConst					__DIVX3D0020
#define IDCT_Row8						__DIVX3D0021
#define IDCT_Row4						__DIVX3D0022
#define IDCTBlock8x8_C					__DIVX3D0023
#define IDCTBlock4x8_C					__DIVX3D0024
#define ReconCopyMB					__DIVX3D0025
#define ReconIntraMB				__DIVX3D0026
#define ReconInterMB				__DIVX3D0027
#define ReconInterMB_B				__DIVX3D0028
#define ReconCopyMB_B				__DIVX3D0029
#define image_create					__DIVX3D0030

#define image_destroy					__DIVX3D0031
#define SwapImage						__DIVX3D0032
#define ColoneImage						__DIVX3D0033
#define SetImageEdge					__DIVX3D0034
#define MCMpegBlock_C						__DIVX3D0035
#define MCMpegBlockHor_C						__DIVX3D0036
#define MCMpegBlockVer_C						__DIVX3D0037
#define MCMpegBlockHorVer_C						__DIVX3D0038
#define MCMpegBlockRdHor_C					__DIVX3D0039
#define MCMpegBlockRdVer_C					__DIVX3D0040
#define MCMpegBlockRdHorVer_C					__DIVX3D0041
#define MCMpegBlockAdd_C						__DIVX3D0042
#define MCMpegBlockAddHor_C						__DIVX3D0043
#define MCMpegBlockAddVer_C						__DIVX3D0044
#define MCMpegBlockAddHorVer_C					__DIVX3D0045
//#define CopyBlock16x16							__DIVX3D0046
//#define CopyBlock8x8							__DIVX3D0047
#define AvgBlockb_C						__DIVX3D0048

#define alignMalloc						__DIVX3D0049
#define alignFree						__DIVX3D0050
#define DecVOP							__DIVX3D0051
#define mp4_create						__DIVX3D0052
#define mp4_delete						__DIVX3D0053
#define h263_create						__DIVX3D0054
#define ConfigDecoder						__DIVX3D0055
#define getmatrix						__DIVX3D0056
#define GetVOLHeader						__DIVX3D0058

#define MPEG4GetHdr					__DIVX3D0060
#define Resync							__DIVX3D0061
#define GetDCsizeLum					__DIVX3D0062
#define GetDCsizeChr					__DIVX3D0063
#define GetDCdiff						__DIVX3D0064
#define MPEG4DecIntraMB					__DIVX3D0065
#define GetChromaMV						__DIVX3D0066
#define GetChromaMV4						__DIVX3D0067
#define MPEG4DecInterMB					__DIVX3D0068
#define GetCBPY							__DIVX3D0069
#define CheckResyncMarker					__DIVX3D0070
#define DP_mbIntra_mpeg4 				__DIVX3D0071
#define ResetDPBuf						__DIVX3D0072
#define MPEG4DecDPIVOP					__DIVX3D0073
#define MPEG4DecDPPVOP					__DIVX3D0074
#define MPEG4DecDPBVOP					__DIVX3D0075
#define MPEG4DecVOP						__DIVX3D0076
#define MPEG4DecIVOP						__DIVX3D0077
#define MPEG4DecPVOP						__DIVX3D0078
#define MPEG4DecDirectMB						__DIVX3D0079
#define MPEG4DecBVOP						__DIVX3D0080
#define GetMV							__DIVX3D0081
#define GetPMV							__DIVX3D0082
#define getMVData						__DIVX3D0083
#define clear32							__DIVX3D0084
#define RescuePredict					__DIVX3D0085
#define InitBitStream						__DIVX3D0086
#define MPEG4DecVOP_s					__DIVX3D0087
#define Mpeg4GetBlockVld				__DIVX3D0089
#define MpegGetBlockVld					__DIVX3D0090

// #define bitget							__DIVX3D0103
// #define biteof							__DIVX3D0104
// #define bitbytealign					__DIVX3D0105
// #define bittonextbyte					__DIVX3D0106
// #define bitbytepos						__DIVX3D0107
#define image_null						__DIVX3D0108
#define GetMCBPC_I						__DIVX3D0109
#define GetMCBPC_P						__DIVX3D0110
#define clearblock						__DIVX3D0111
#define ac_recon						__DIVX3D0112
#define dc_recon						__DIVX3D0113
#define GetBits							__DIVX3D0114
#define EofBits							__DIVX3D0115
//#define SetInPlaceInterMBStrength				__DIVX3D0116		the ARMV5_inplaceDeblock refers the symbol,so do not change it
#define bytealign						__DIVX3D0117	
#define BitsToNextByte					__DIVX3D0118
#define bytepos							__DIVX3D0119
#define MCMpeg4BlockQpel		__DIVX3D0120
#define MCMpeg4BlockQpelAdd		__DIVX3D0121


//data structure
#define bitstream						__DIVX3D0188
#define IDCT_BLOCK_PTR					__DIVX3D0189
#define IDCT_CONST8X8_PTR				__DIVX3D0190
#define IDCT_BLOCK						__DIVX3D0191
#define IDCT_CONST8X8					__DIVX3D0192
#define copyblock						__DIVX3D0193
#define addblock						__DIVX3D0194
#define VO_MPEG4_DEC						__DIVX3D0195
#define VO_IMGYUV							__DIVX3D0196
#define video							__DIVX3D0197
#define vld_intra						__DIVX3D0198
#define vld_inter						__DIVX3D0199

#define remove_frame_from_dpb					__DIVX3D0204
#define get_smallest_poc						__DIVX3D0205
#define remove_unused_frame_from_dpb			__DIVX3D0206
#define output_one_frame_from_dpb				__DIVX3D0207
#define flush_dpb								__DIVX3D0208
#define alloc_ref_pic_list0_reordering_buffer	__DIVX3D0209
#define free_ref_pic_list0_reordering_buffer	__DIVX3D0210
#define DpbFrPostProc							__DIVX3D0211
#define alloc_ref_pic_list1_reordering_buffer	__DIVX3D0212
#define free_ref_pic_list1_reordering_buffer	__DIVX3D0213
#define fill_frame_num_gap						__DIVX3D0214
#define dpb_split_field							__DIVX3D0215
#define DpbFldPostProc							__DIVX3D0216
#define DpbCombineYUVFields						__DIVX3D0217
#define dpb_combine_field						__DIVX3D0218
#define RBSPtoSODB								__DIVX3D0219
#define EBSPtoRBSP								__DIVX3D0220
#define NALUtoRBSP								__DIVX3D0221
#define avd_assignSE2partition					__DIVX3D0222
#define Alloc_Neighbor_Info						__DIVX3D0223
#define Free_Neighbor_Info						__DIVX3D0224
#define Init_Slice_Neighbor_Info				__DIVX3D0225
#define update_MB_neighbor_info					__DIVX3D0226
#define update_neighborABCD_info				__DIVX3D0227
#define Init_MB_Neighbor_Info					__DIVX3D0228
#define Finish_MB_Neighbor_Info					__DIVX3D0229
#define NeighborAB_Inference_CABAC				__DIVX3D0230
#define write_picture							__DIVX3D0231
#define init_out_buffer							__DIVX3D0232
#define uninit_out_buffer						__DIVX3D0233
#define clear_picture							__DIVX3D0234
#define write_unpaired_field					__DIVX3D0235
#define flush_direct_output						__DIVX3D0236
#define write_stored_frame						__DIVX3D0237
#define direct_output							__DIVX3D0238
#define ReadHRDParameters						__DIVX3D0239
#define ReadVUI									__DIVX3D0240
#define InterpretSPS							__DIVX3D0241
#define InterpretPPS							__DIVX3D0242
#define ProcessSPS								__DIVX3D0243
#define SPSConsistencyCheck						__DIVX3D0244
#define ProcessPPS								__DIVX3D0245
#define UpdateSizeNActivateSPS					__DIVX3D0246
#define UseParameterSet							__DIVX3D0247
#define interpret_recovery_point_info			__DIVX3D0248
#define InterpretSEIMessage						__DIVX3D0249
#define interpret_spare_pic						__DIVX3D0250
#define interpret_subsequence_info				__DIVX3D0251
#define ReconInterMBInterlace				__DIVX3D0252



#ifdef VOARMV4
//#define arm_copy8x8               __DIVX3D0155
//#define arm_copy8x8Hor            __DIVX3D0156
//#define arm_copy8x8ver            __DIVX3D0157
//#define arm_copy8x8HorVer			__DIVX3D0158
//#define arm_copy8x8HorRound			__DIVX3D0159
//#define arm_copy8x8verRound				__DIVX3D0160
//#define arm_copy8x8HorVerRound		__DIVX3D0161
//#define ArmInter2_8x8 					 __DIVX3D0162
//#define ArmInter2_8x8H					__DIVX3D0163 
//#define ArmInter2_8x8V					__DIVX3D0164
//#define ArmInter2_8x8HV				__DIVX3D0165
#else
//#define wmmx_copy8x8               __DIVX3D0155
//#define wmmx_copy8x8Hor            __DIVX3D0156
//#define wmmx_copy8x8ver            __DIVX3D0157
//#define wmmx_copy8x8HorVer         __DIVX3D0158
//#define wmmx_copy8x8HorRound       __DIVX3D0159
//#define wmmx_copy8x8verRound       __DIVX3D0160
//#define wmmx_copy8x8HorVerRound    __DIVX3D0161
//#define wmmx_addblk                __DIVX3D0162
//#define wmmx_addblkh             __DIVX3D0163 
//#define wmmx_addblkv             __DIVX3D0164
//#define wmmx_addblkhv          __DIVX3D0165
#endif

#define AllCopyBlock                __DIVX3D0300
#define AllSubBlock                 __DIVX3D0301

#define Predict_16x16				__DIVX3D0302
#define Predict_8x8				    __DIVX3D0303
//#define get_average_mv			__DIVX3D0304
#define Predict_1pt_16x16			__DIVX3D0305
#define Predict_1pt_8x8				__DIVX3D0306
#define get_average_mv_1pt		    __DIVX3D0307
//#define IDCTBlock8x8				__DIVX3D0308
//#define IDCTBlock4x8			    __DIVX3D0309
//#define IDCT_Const8x8				__DIVX3D0310

//#define AllCopyBlock              __DIVX3D0311
#define AllAddBlock                 __DIVX3D0312          
//#define CopyBlock16x16            __DIVX3D0313
//#define CopyBlock8x8              __DIVX3D0314
#define CopyBlock16x16_C				__DIVX3D0315
#define CopyBlock8x8_C					__DIVX3D0316

#define scanTab                        __DIVX3D0317              
#define roundtab                    __DIVX3D0318
#define DQtab                       __DIVX3D0319
#define MCBPCtabIntra               __DIVX3D0320
#define MCBPCtabInter               __DIVX3D0321
#define CBPYtab                     __DIVX3D0322


#define roundtab_76                 __DIVX3D0323
#define roundtab_78                 __DIVX3D0324
#define roundtab_79                 __DIVX3D0325

#define nIntraDCThreshold          __DIVX3D0326
#define def_quant_intra             __DIVX3D0327
#define def_quant_inter             __DIVX3D0328

#define idct_col_half               __DIVX3D0329
//#define  IDCT_Row4                __DIVX3D0201
#define IDCTBlock4x4_C             __DIVX3D0330
#define IDCT_Block2x2               __DIVX3D0331
#define IDCT_Const4x4_c             __DIVX3D0332

#define fpReconInterMB           __DIVX3D0333
#define mp4_mbinterB_recon          __DIVX3D0334

//#define IDCTBlock4x8               __DIVX3D0207
//#define IDCTBlock8x8               __DIVX3D0208
//#define IDCT_Const8x8               __DIVX3D0209

//#define H263GetHdr                 __DIVX3D0210
#define gethdr_intel_h263           __DIVX3D0335

#define wmmx_transb4x8           __DIVX3D0336
#define rvld_inter				 __DIVX3D0337
#define rvld_intra				 __DIVX3D0338
#define RL_VLC_ELEM				 __DIVX3D0339       

#define deblock8x8_h			__DIVX3D0340		
#define deblock8x8_v			__DIVX3D0341	
#define init_deblock			__DIVX3D0342	
#define initvlc					__DIVX3D0343	
#define GetDivx3VLC					__DIVX3D0344

#define mv0_code				__DIVX3D0345
#define mv0_data				__DIVX3D0346
#define mv1_code				__DIVX3D0347
#define mv1_data				__DIVX3D0348
#define rl0_code				__DIVX3D0349
#define rl0_data				__DIVX3D0350
#define rl1_code				__DIVX3D0351
#define rl1_data				__DIVX3D0352
#define rl2_code				__DIVX3D0353
#define rl2_data				__DIVX3D0354
#define rl3_code				__DIVX3D0355
#define rl3_data				__DIVX3D0356
#define rl4_code				__DIVX3D0357
#define rl4_data				__DIVX3D0358
#define rl5_code				__DIVX3D0359
#define rl5_data				__DIVX3D0360
#define dc0_lum_code			__DIVX3D0361
#define dc1_lum_code			__DIVX3D0362		
#define dc0_chr_code			__DIVX3D0363		
#define dc1_chr_code			__DIVX3D0364	
#define mb_inter_code			__DIVX3D0365		
#define mb_intra_code			__DIVX3D0366			
#define initrl					__DIVX3D0367
// #define get012					__DIVX3D0368				
#define DIVX3GetHdr			__DIVX3D0369

#define mvchroma3				__DIVX3D0370
#define vld_block2				__DIVX3D0371
#define DIVX3DecIntraMB				__DIVX3D0372
#define DIVX3DecInterMB				__DIVX3D0373
#define dc_scaler				__DIVX3D0374
#define getMV2					__DIVX3D0375
#define ext_header				__DIVX3D0376
#define DIVX3DecIVOP				__DIVX3D0377
#define DIVX3RescuePredict	__DIVX3D0378
#define DIVX3DecPVOP				__DIVX3D0379
#define DIVX3DecBVOP				__DIVX3D0380
#define buildvlc				__DIVX3D0381

#define H263ReconDCAC				__DIVX3D0382
#define H263UpdateDCAC			__DIVX3D0383

#define vld_intra_aic			__DIVX3D0384
#define h263_lf_strength		__DIVX3D0385
#define mba_length				__DIVX3D0386
#define mba_max					__DIVX3D0387
#define Mpeg4GetBlockRvld		__DIVX3D0388

#define H263GetMBA				__DIVX3D0389
#define sprite_trajectory_len   __DIVX3D0390

#define image_destroy_bframe    __DIVX3D0392
#define GetSpritetrajectory     __DIVX3D0393
#define GMCSanitize            __DIVX3D0394

#define MPEG4DecGmcMB             __DIVX3D0395
#define idct_311_generic	    __DIVX3D0396
#define idctcol_inter_311		__DIVX3D0397
#define idct_intra_generic		__DIVX3D0398
#endif//PROTECT_NAME
//ASSEMBLY 
#define ARMV5E_InPlaceDeblockMBs			ARM_DM
#define GetBlockLumaNxNInBound				ARM_GB1
#define GetBlockChromaNxNInBound			ARM_GB2
#define SetInPlaceInterMBStrength			SETINMS

#define qpel_h_8x8_c			__DIVX3D0399
#define qpel_ha_8x8_c			__DIVX3D0400
#define qpel_ha_up_8x8_c		__DIVX3D0401	
#define qpel_v_8x8_c			__DIVX3D0402
#define qpel_va_8x8_c			__DIVX3D0403
#define qpel_va_up_8x8_c		__DIVX3D0404


#define qpel_h_8x8_b_c			__DIVX3D0405
#define qpel_ha_8x8_b_c			__DIVX3D0406
#define qpel_ha_up_8x8_b_c		__DIVX3D0407
#define qpel_v_8x8_b_c			__DIVX3D0408
#define qpel_va_8x8_b_c			__DIVX3D0409
#define qpel_va_up_8x8_b_c		__DIVX3D0410

#define MVtab                       __DIVX3D0411
#define decode_slice_jmp            __DIVX3D0412
#define errMessage                  __DIVX3D0413
#define errormsg                    __DIVX3D0414
#define vois_I_frame                __DIVX3D0415
#define GetDPDCDiff                 __DIVX3D0416

#define default_h263_chroma_qscale_table   __DIVX3D0417
#define h263_chroma_qscale_table           __DIVX3D0418
#define S263GetHdr                        __DIVX3D0419
#define IDCTBlock1x1_C                    __DIVX3D0420
#define MallocMem                         __DIVX3D0421
#define FreeMem                           __DIVX3D0422
#define SetMem                           __DIVX3D0423
#define CopyMem                           __DIVX3D0424
#define GetFrameBufIdx						__DIVX3D0425
#define FrameBufCtl                        __DIVX3D0426
#define IDCT_Col4                          __DIVX3D0427
#define CorrectMV							__DIVX3D0428

#define 	voMPEG4DecInit		 __DIVX3D0429
#define 	voMPEG4DecUninit		 __DIVX3D0430
#define 	voMPEG4DecSetParameter		 __DIVX3D0431
#define 	voMPEG4DecGetParameter		 __DIVX3D0432
#define 	voMPEG4DecSetInputData		 __DIVX3D0433
#define 	voMPEG4DecGetOutputData		 __DIVX3D0434
// #define 	yyGetMPEG4DecFunc		 __DIVX3D0435
#define 	GetInnerMemSize		 __DIVX3D0436
#define 	voMPEGDecFrame		 __DIVX3D0437
#define 	voMPEG4DecCreate		 __DIVX3D0438
#define 	voMPEG4DecFinish		 __DIVX3D0439
#define 	MBrMPEG4VOP		 __DIVX3D0440
#define 	MBrH263IVOP		 __DIVX3D0441
#define 	MBrH263PVOP		 __DIVX3D0442
#define 	InitialMBThread		 __DIVX3D0443
#define 	ReleaseMBThread		 __DIVX3D0444
#define 	ResetMBThread		 __DIVX3D0445
#define 	SetMBThreadVOPFunc		 __DIVX3D0446
// #define 	fill_edge_y_armv6		 __DIVX3D0447
// #define 	fill_edge_uv_armv6		 __DIVX3D0448
#define 	IdctColunm8		 __DIVX3D0449
#define 	IdctColunm4		 __DIVX3D0450
#define 	IdctLine8		 __DIVX3D0451
#define 	IdctLine4		 __DIVX3D0452
#define 	IdctBlock1x1_C		 __DIVX3D0453
#define 	IdctBlock8x8_C		 __DIVX3D0454
#define 	IdctBlock4x8_C		 __DIVX3D0455
#define 	IdctBlock4x4_C		 __DIVX3D0456
#define	MCMpegBlockRd_C	__voDIVX3D0472
#define	MCMpegBlockAddRd_C	__voDIVX3D0473
#define	MCMpegBlockAddRdHor_C	__voDIVX3D0474
#define	MCMpegBlockAddRdVer_C	__voDIVX3D0475
#define	MCMpegBlockAddRdHorVer_C	__voDIVX3D0476
#define GetPMV_interlace __voDIVX3D0477
#define MPEG4DecMBRow __voDIVX3D0478
#define WaitForParser __voDIVX3D0479
#define WaitForSubThdFinish __voDIVX3D0480
#define	MCMpeg4BlockQpel12Add_C		__voDIVX3D0481
#define	MCMpeg4BlockQpel12_C		__voDIVX3D0482
#define	MCMpeg4BlockQpel1Add_C		__voDIVX3D0483
#define	MCMpeg4BlockQpel1_C		__voDIVX3D0484
#define	MCMpeg4BlockQpel2Add_C		__voDIVX3D0485
#define	MCMpeg4BlockQpel2_C		__voDIVX3D0486
#define	MCMpeg4BlockQpel3Add_C		__voDIVX3D0487
#define	MCMpeg4BlockQpel3_C		__voDIVX3D0488
#define	MCMpeg4BlockQpel4Add_C		__voDIVX3D0489
#define	MCMpeg4BlockQpel4_C		__voDIVX3D0490
#define	MCMpeg4BlockQpel8Add_C		__voDIVX3D0491
#define	MCMpeg4BlockQpel8_C		__voDIVX3D0492

#if defined(_IOS)
#define	Arm11IdctA		_voDIVX3D0493
#define	Arm11IdctB		_voDIVX3D0494
#define	Arm11IdctC1		_voDIVX3D0495
#define	Arm11IdctC		_voDIVX3D0496
#define	Arm11IdctD		_voDIVX3D0497
#define	Arm11InterBQ1		_voDIVX3D0498
#define	Arm11InterBQ12		_voDIVX3D0499
#define	Arm11InterBQ2		_voDIVX3D0500
#define	Arm11InterBQ3		_voDIVX3D0501
#define	Arm11InterBQ4		_voDIVX3D0502
#define	Arm11InterBQ8		_voDIVX3D0503
#define	Arm11InterQ12		_voDIVX3D0504
#define	Arm11InterQ1		_voDIVX3D0505
#define	Arm11InterQ2		_voDIVX3D0506
#define	Arm11InterQ3		_voDIVX3D0507
#define	Arm11InterQ4		_voDIVX3D0508
#define	Arm11InterQ8		_voDIVX3D0509
#define	MPEG4DEC_VO_Armv7Copy16x16		_voDIVX3D0510
#define	MPEG4DEC_VO_Armv7Copy8x8		_voDIVX3D0511
#define	MPEG4DEC_VO_Armv7Inter2_8x8		_voDIVX3D0512
#define	MPEG4DEC_VO_Armv7Inter2_8x8H		_voDIVX3D0513
#define	MPEG4DEC_VO_Armv7Inter2_8x8V		_voDIVX3D0514
#define	MPEG4DEC_VO_Armv7Inter2_8x8HV		_voDIVX3D0515
#define	MPEG4DEC_VO_Armv7Inter8x8		_voDIVX3D0516
#define	MPEG4DEC_VO_Armv7Inter8x8H		_voDIVX3D0517
#define	MPEG4DEC_VO_Armv7Inter8x8HV		_voDIVX3D0518
#define	MPEG4DEC_VO_Armv7Inter8x8HRD		_voDIVX3D0519
#define	MPEG4DEC_VO_Armv7Inter8x8HVRD		_voDIVX3D0520
#define	MPEG4DEC_VO_Armv7Inter8x8V		_voDIVX3D0521
#define	MPEG4DEC_VO_Armv7Inter8x8VRD		_voDIVX3D0522
#define	fill_edge_uv_armv6		_voDIVX3D0523
#define	fill_edge_y_armv6		_voDIVX3D0524

#define	_Arm11IdctA		__voDIVX3D0493
#define	_Arm11IdctB		__voDIVX3D0494
#define	_Arm11IdctC1		__voDIVX3D0495
#define	_Arm11IdctC		__voDIVX3D0496
#define	_Arm11IdctD		__voDIVX3D0497
#define	_Arm11InterBQ1		__voDIVX3D0498
#define	_Arm11InterBQ12		__voDIVX3D0499
#define	_Arm11InterBQ2		__voDIVX3D0500
#define	_Arm11InterBQ3		__voDIVX3D0501
#define	_Arm11InterBQ4		__voDIVX3D0502
#define	_Arm11InterBQ8		__voDIVX3D0503
#define	_Arm11InterQ12		__voDIVX3D0504
#define	_Arm11InterQ1		__voDIVX3D0505
#define	_Arm11InterQ2		__voDIVX3D0506
#define	_Arm11InterQ3		__voDIVX3D0507
#define	_Arm11InterQ4		__voDIVX3D0508
#define	_Arm11InterQ8		__voDIVX3D0509
#define	_MPEG4DEC_VO_Armv7Copy16x16		__voDIVX3D0510
#define	_MPEG4DEC_VO_Armv7Copy8x8		__voDIVX3D0511
#define	_MPEG4DEC_VO_Armv7Inter2_8x8		__voDIVX3D0512
#define	_MPEG4DEC_VO_Armv7Inter2_8x8H		__voDIVX3D0513
#define	_MPEG4DEC_VO_Armv7Inter2_8x8V		__voDIVX3D0514
#define	_MPEG4DEC_VO_Armv7Inter2_8x8HV		__voDIVX3D0515
#define	_MPEG4DEC_VO_Armv7Inter8x8		__voDIVX3D0516
#define	_MPEG4DEC_VO_Armv7Inter8x8H		__voDIVX3D0517
#define	_MPEG4DEC_VO_Armv7Inter8x8HV		__voDIVX3D0518
#define	_MPEG4DEC_VO_Armv7Inter8x8HRD		__voDIVX3D0519
#define	_MPEG4DEC_VO_Armv7Inter8x8HVRD		__voDIVX3D0520
#define	_MPEG4DEC_VO_Armv7Inter8x8V		__voDIVX3D0521
#define	_MPEG4DEC_VO_Armv7Inter8x8VRD		__voDIVX3D0522
#define	_fill_edge_uv_armv6		__voDIVX3D0523
#define	_fill_edge_y_armv6		__voDIVX3D0524

#endif

#else

#define PROTECT_NAME			1	//for copy right protection
#if PROTECT_NAME

//bitstream
#define bitinit							__voMPEG4D0001
#define bitload							__voMPEG4D0002
#define bitshowlarge					__voMPEG4D0003
#define GMCLuma_C					__voMPEG4D0004
#define GMCChroma_C					__voMPEG4D0005
#define GMCGetAvgMV_C				__voMPEG4D0006
#define GMCLuma1Pt_C				__voMPEG4D0007
#define GMCChroma1Pt_C				__voMPEG4D0008
#define GMCGetAvgMV1pt_C			__voMPEG4D0009
#define IniGMCParam					__voMPEG4D0010


#define generate_GMCimage				__voMPEG4D0011
#define H263GetHdr						__voMPEG4D0012
#define H263DecIntraMB					__voMPEG4D0013
#define H263DecInterMB					__voMPEG4D0014
#define H263DecPVOP						__voMPEG4D0015
#define H263DecIVOP						__voMPEG4D0016
#define H263DecBVOP						__voMPEG4D0017
#define H263GetBlockVld					__voMPEG4D0018

#define IDCT_Col8						__voMPEG4D0019
#define IDCT_RowConst					__voMPEG4D0020
#define IDCT_Row8						__voMPEG4D0021
#define IDCT_Row4						__voMPEG4D0022
#define IDCTBlock8x8_C					__voMPEG4D0023
#define IDCTBlock4x8_C					__voMPEG4D0024
#define ReconCopyMB					__voMPEG4D0025
#define ReconIntraMB				__voMPEG4D0026
#define ReconInterMB				__voMPEG4D0027
#define ReconInterMB_B				__voMPEG4D0028
#define ReconCopyMB_B				__voMPEG4D0029
#define image_create					__voMPEG4D0030

#define image_destroy					__voMPEG4D0031
#define SwapImage						__voMPEG4D0032
#define ColoneImage						__voMPEG4D0033
#define SetImageEdge					__voMPEG4D0034
#define MCMpegBlock_C						__voMPEG4D0035
#define MCMpegBlockHor_C						__voMPEG4D0036
#define MCMpegBlockVer_C						__voMPEG4D0037
#define MCMpegBlockHorVer_C						__voMPEG4D0038
#define MCMpegBlockRdHor_C					__voMPEG4D0039
#define MCMpegBlockRdVer_C					__voMPEG4D0040
#define MCMpegBlockRdHorVer_C					__voMPEG4D0041
#define MCMpegBlockAdd_C						__voMPEG4D0042
#define MCMpegBlockAddHor_C						__voMPEG4D0043
#define MCMpegBlockAddVer_C						__voMPEG4D0044
#define MCMpegBlockAddHorVer_C					__voMPEG4D0045
//#define CopyBlock16x16							__voMPEG4D0046
//#define CopyBlock8x8							__voMPEG4D0047
#define AvgBlockb_C						__voMPEG4D0048

#define alignMalloc						__voMPEG4D0049
#define alignFree						__voMPEG4D0050
#define DecVOP							__voMPEG4D0051
#define mp4_create						__voMPEG4D0052
#define mp4_delete						__voMPEG4D0053
#define h263_create						__voMPEG4D0054
#define ConfigDecoder						__voMPEG4D0055
#define getmatrix						__voMPEG4D0056
#define GetVOLHeader						__voMPEG4D0058

#define MPEG4GetHdr					__voMPEG4D0060
#define Resync							__voMPEG4D0061
#define GetDCsizeLum					__voMPEG4D0062
#define GetDCsizeChr					__voMPEG4D0063
#define GetDCdiff						__voMPEG4D0064
#define MPEG4DecIntraMB					__voMPEG4D0065
#define GetChromaMV						__voMPEG4D0066
#define GetChromaMV4						__voMPEG4D0067
#define MPEG4DecInterMB					__voMPEG4D0068
#define GetCBPY							__voMPEG4D0069
#define CheckResyncMarker					__voMPEG4D0070
#define DP_mbIntra_mpeg4 				__voMPEG4D0071
#define ResetDPBuf						__voMPEG4D0072
#define MPEG4DecDPIVOP					__voMPEG4D0073
#define MPEG4DecDPPVOP					__voMPEG4D0074
#define MPEG4DecDPBVOP					__voMPEG4D0075
#define MPEG4DecVOP						__voMPEG4D0076
#define MPEG4DecIVOP						__voMPEG4D0077
#define MPEG4DecPVOP						__voMPEG4D0078
#define MPEG4DecDirectMB						__voMPEG4D0079
#define MPEG4DecBVOP						__voMPEG4D0080
#define GetMV							__voMPEG4D0081
#define GetPMV							__voMPEG4D0082
#define getMVData						__voMPEG4D0083
#define clear32							__voMPEG4D0084
#define RescuePredict					__voMPEG4D0085
#define InitBitStream						__voMPEG4D0086
#define MPEG4DecVOP_s					__voMPEG4D0087
#define Mpeg4GetBlockVld				__voMPEG4D0089
#define MpegGetBlockVld					__voMPEG4D0090

// #define bitget							__voMPEG4D0103
// #define biteof							__voMPEG4D0104
// #define bitbytealign					__voMPEG4D0105
// #define bittonextbyte					__voMPEG4D0106
// #define bitbytepos						__voMPEG4D0107
#define image_null						__voMPEG4D0108
#define GetMCBPC_I						__voMPEG4D0109
#define GetMCBPC_P						__voMPEG4D0110
#define clearblock						__voMPEG4D0111
#define ac_recon						__voMPEG4D0112
#define dc_recon						__voMPEG4D0113
#define GetBits							__voMPEG4D0114
#define EofBits							__voMPEG4D0115
//#define SetInPlaceInterMBStrength				__voMPEG4D0116		the ARMV5_inplaceDeblock refers the symbol,so do not change it
#define bytealign						__voMPEG4D0117	
#define BitsToNextByte					__voMPEG4D0118
#define bytepos							__voMPEG4D0119
#define MCMpeg4BlockQpel		__voMPEG4D0120
#define MCMpeg4BlockQpelAdd		__voMPEG4D0121


//data structure
#define bitstream						__voMPEG4D0188
#define IDCT_BLOCK_PTR					__voMPEG4D0189
#define IDCT_CONST8X8_PTR				__voMPEG4D0190
#define IDCT_BLOCK						__voMPEG4D0191
#define IDCT_CONST8X8					__voMPEG4D0192
#define copyblock						__voMPEG4D0193
#define addblock						__voMPEG4D0194
#define VO_MPEG4_DEC						__voMPEG4D0195
#define VO_IMGYUV							__voMPEG4D0196
#define video							__voMPEG4D0197
#define vld_intra						__voMPEG4D0198
#define vld_inter						__voMPEG4D0199

#define remove_frame_from_dpb					__voMPEG4D0204
#define get_smallest_poc						__voMPEG4D0205
#define remove_unused_frame_from_dpb			__voMPEG4D0206
#define output_one_frame_from_dpb				__voMPEG4D0207
#define flush_dpb								__voMPEG4D0208
#define alloc_ref_pic_list0_reordering_buffer	__voMPEG4D0209
#define free_ref_pic_list0_reordering_buffer	__voMPEG4D0210
#define DpbFrPostProc							__voMPEG4D0211
#define alloc_ref_pic_list1_reordering_buffer	__voMPEG4D0212
#define free_ref_pic_list1_reordering_buffer	__voMPEG4D0213
#define fill_frame_num_gap						__voMPEG4D0214
#define dpb_split_field							__voMPEG4D0215
#define DpbFldPostProc							__voMPEG4D0216
#define DpbCombineYUVFields						__voMPEG4D0217
#define dpb_combine_field						__voMPEG4D0218
#define RBSPtoSODB								__voMPEG4D0219
#define EBSPtoRBSP								__voMPEG4D0220
#define NALUtoRBSP								__voMPEG4D0221
#define avd_assignSE2partition					__voMPEG4D0222
#define Alloc_Neighbor_Info						__voMPEG4D0223
#define Free_Neighbor_Info						__voMPEG4D0224
#define Init_Slice_Neighbor_Info				__voMPEG4D0225
#define update_MB_neighbor_info					__voMPEG4D0226
#define update_neighborABCD_info				__voMPEG4D0227
#define Init_MB_Neighbor_Info					__voMPEG4D0228
#define Finish_MB_Neighbor_Info					__voMPEG4D0229
#define NeighborAB_Inference_CABAC				__voMPEG4D0230
#define write_picture							__voMPEG4D0231
#define init_out_buffer							__voMPEG4D0232
#define uninit_out_buffer						__voMPEG4D0233
#define clear_picture							__voMPEG4D0234
#define write_unpaired_field					__voMPEG4D0235
#define flush_direct_output						__voMPEG4D0236
#define write_stored_frame						__voMPEG4D0237
#define direct_output							__voMPEG4D0238
#define ReadHRDParameters						__voMPEG4D0239
#define ReadVUI									__voMPEG4D0240
#define InterpretSPS							__voMPEG4D0241
#define InterpretPPS							__voMPEG4D0242
#define ProcessSPS								__voMPEG4D0243
#define SPSConsistencyCheck						__voMPEG4D0244
#define ProcessPPS								__voMPEG4D0245
#define UpdateSizeNActivateSPS					__voMPEG4D0246
#define UseParameterSet							__voMPEG4D0247
#define interpret_recovery_point_info			__voMPEG4D0248
#define InterpretSEIMessage						__voMPEG4D0249
#define interpret_spare_pic						__voMPEG4D0250
#define interpret_subsequence_info				__voMPEG4D0251
#define ReconInterMBInterlace				__voMPEG4D0252



#ifdef VOARMV4
//#define arm_copy8x8               __voMPEG4D0155
//#define arm_copy8x8Hor            __voMPEG4D0156
//#define arm_copy8x8ver            __voMPEG4D0157
//#define arm_copy8x8HorVer			__voMPEG4D0158
//#define arm_copy8x8HorRound			__voMPEG4D0159
//#define arm_copy8x8verRound				__voMPEG4D0160
//#define arm_copy8x8HorVerRound		__voMPEG4D0161
//#define ArmInter2_8x8 					 __voMPEG4D0162
//#define ArmInter2_8x8H					__voMPEG4D0163 
//#define ArmInter2_8x8V					__voMPEG4D0164
//#define ArmInter2_8x8HV				__voMPEG4D0165
#else
//#define wmmx_copy8x8               __voMPEG4D0155
//#define wmmx_copy8x8Hor            __voMPEG4D0156
//#define wmmx_copy8x8ver            __voMPEG4D0157
//#define wmmx_copy8x8HorVer         __voMPEG4D0158
//#define wmmx_copy8x8HorRound       __voMPEG4D0159
//#define wmmx_copy8x8verRound       __voMPEG4D0160
//#define wmmx_copy8x8HorVerRound    __voMPEG4D0161
//#define wmmx_addblk                __voMPEG4D0162
//#define wmmx_addblkh             __voMPEG4D0163 
//#define wmmx_addblkv             __voMPEG4D0164
//#define wmmx_addblkhv          __voMPEG4D0165
#endif

#define AllCopyBlock                __voMPEG4D0300
#define AllSubBlock                 __voMPEG4D0301

#define Predict_16x16				__voMPEG4D0302
#define Predict_8x8				    __voMPEG4D0303
//#define get_average_mv			__voMPEG4D0304
#define Predict_1pt_16x16			__voMPEG4D0305
#define Predict_1pt_8x8				__voMPEG4D0306
#define get_average_mv_1pt		    __voMPEG4D0307
//#define IDCTBlock8x8				__voMPEG4D0308
//#define IDCTBlock4x8			    __voMPEG4D0309
//#define IDCT_Const8x8				__voMPEG4D0310

//#define AllCopyBlock              __voMPEG4D0311
#define AllAddBlock                 __voMPEG4D0312          
//#define CopyBlock16x16            __voMPEG4D0313
//#define CopyBlock8x8              __voMPEG4D0314
#define CopyBlock16x16_C				__voMPEG4D0315
#define CopyBlock8x8_C					__voMPEG4D0316

#define scanTab                        __voMPEG4D0317              
#define roundtab                    __voMPEG4D0318
#define DQtab                       __voMPEG4D0319
#define MCBPCtabIntra               __voMPEG4D0320
#define MCBPCtabInter               __voMPEG4D0321
#define CBPYtab                     __voMPEG4D0322


#define roundtab_76                 __voMPEG4D0323
#define roundtab_78                 __voMPEG4D0324
#define roundtab_79                 __voMPEG4D0325

#define nIntraDCThreshold          __voMPEG4D0326
#define def_quant_intra             __voMPEG4D0327
#define def_quant_inter             __voMPEG4D0328

#define idct_col_half               __voMPEG4D0329
//#define  IDCT_Row4                __voMPEG4D0201
#define IDCTBlock4x4_C             __voMPEG4D0330
#define IDCT_Block2x2               __voMPEG4D0331
#define IDCT_Const4x4_c             __voMPEG4D0332

#define fpReconInterMB           __voMPEG4D0333
#define mp4_mbinterB_recon          __voMPEG4D0334

//#define IDCTBlock4x8               __voMPEG4D0207
//#define IDCTBlock8x8               __voMPEG4D0208
//#define IDCT_Const8x8               __voMPEG4D0209

//#define H263GetHdr                 __voMPEG4D0210
#define gethdr_intel_h263           __voMPEG4D0335

#define wmmx_transb4x8           __voMPEG4D0336
#define rvld_inter				 __voMPEG4D0337
#define rvld_intra				 __voMPEG4D0338
#define RL_VLC_ELEM				 __voMPEG4D0339       

#define deblock8x8_h			__voMPEG4D0340		
#define deblock8x8_v			__voMPEG4D0341	
#define init_deblock			__voMPEG4D0342	
#define initvlc					__voMPEG4D0343	
#define GetDivx3VLC					__voMPEG4D0344

#define mv0_code				__voMPEG4D0345
#define mv0_data				__voMPEG4D0346
#define mv1_code				__voMPEG4D0347
#define mv1_data				__voMPEG4D0348
#define rl0_code				__voMPEG4D0349
#define rl0_data				__voMPEG4D0350
#define rl1_code				__voMPEG4D0351
#define rl1_data				__voMPEG4D0352
#define rl2_code				__voMPEG4D0353
#define rl2_data				__voMPEG4D0354
#define rl3_code				__voMPEG4D0355
#define rl3_data				__voMPEG4D0356
#define rl4_code				__voMPEG4D0357
#define rl4_data				__voMPEG4D0358
#define rl5_code				__voMPEG4D0359
#define rl5_data				__voMPEG4D0360
#define dc0_lum_code			__voMPEG4D0361
#define dc1_lum_code			__voMPEG4D0362		
#define dc0_chr_code			__voMPEG4D0363		
#define dc1_chr_code			__voMPEG4D0364	
#define mb_inter_code			__voMPEG4D0365		
#define mb_intra_code			__voMPEG4D0366			
#define initrl					__voMPEG4D0367
// #define get012					__voMPEG4D0368				
#define DIVX3GetHdr			__voMPEG4D0369

#define mvchroma3				__voMPEG4D0370
#define vld_block2				__voMPEG4D0371
#define DIVX3DecIntraMB				__voMPEG4D0372
#define DIVX3DecInterMB				__voMPEG4D0373
#define dc_scaler				__voMPEG4D0374
#define getMV2					__voMPEG4D0375
#define ext_header				__voMPEG4D0376
#define DIVX3DecIVOP				__voMPEG4D0377
#define DIVX3RescuePredict	__voMPEG4D0378
#define DIVX3DecPVOP				__voMPEG4D0379
#define DIVX3DecBVOP				__voMPEG4D0380
#define buildvlc				__voMPEG4D0381

#define H263ReconDCAC				__voMPEG4D0382
#define H263UpdateDCAC			__voMPEG4D0383

#define vld_intra_aic			__voMPEG4D0384
#define h263_lf_strength		__voMPEG4D0385
#define mba_length				__voMPEG4D0386
#define mba_max					__voMPEG4D0387
#define Mpeg4GetBlockRvld		__voMPEG4D0388

#define H263GetMBA				__voMPEG4D0389
#define sprite_trajectory_len   __voMPEG4D0390

#define image_destroy_bframe    __voMPEG4D0392
#define GetSpritetrajectory     __voMPEG4D0393
#define GMCSanitize            __voMPEG4D0394

#define MPEG4DecGmcMB             __voMPEG4D0395
#define idct_311_generic	    __voMPEG4D0396
#define idctcol_inter_311		__voMPEG4D0397
#define idct_intra_generic		__voMPEG4D0398
#endif//PROTECT_NAME
//ASSEMBLY 
#define ARMV5E_InPlaceDeblockMBs			ARM_DM
#define GetBlockLumaNxNInBound				ARM_GB1
#define GetBlockChromaNxNInBound			ARM_GB2
#define SetInPlaceInterMBStrength			SETINMS

#define qpel_h_8x8_c			__voMPEG4D0399
#define qpel_ha_8x8_c			__voMPEG4D0400
#define qpel_ha_up_8x8_c		__voMPEG4D0401	
#define qpel_v_8x8_c			__voMPEG4D0402
#define qpel_va_8x8_c			__voMPEG4D0403
#define qpel_va_up_8x8_c		__voMPEG4D0404


#define qpel_h_8x8_b_c			__voMPEG4D0405
#define qpel_ha_8x8_b_c			__voMPEG4D0406
#define qpel_ha_up_8x8_b_c		__voMPEG4D0407
#define qpel_v_8x8_b_c			__voMPEG4D0408
#define qpel_va_8x8_b_c			__voMPEG4D0409
#define qpel_va_up_8x8_b_c		__voMPEG4D0410

#define MVtab                       __voMPEG4D0411
#define decode_slice_jmp            __voMPEG4D0412
#define errMessage                  __voMPEG4D0413
#define errormsg                    __voMPEG4D0414
#define vois_I_frame                __voMPEG4D0415
#define GetDPDCDiff                 __voMPEG4D0416

#define default_h263_chroma_qscale_table   __voMPEG4D0417
#define h263_chroma_qscale_table           __voMPEG4D0418
#define S263GetHdr                        __voMPEG4D0419
#define IDCTBlock1x1_C                    __voMPEG4D0420
#define MallocMem                         __voMPEG4D0421
#define FreeMem                           __voMPEG4D0422
#define SetMem                           __voMPEG4D0423
#define CopyMem                           __voMPEG4D0424
#define GetFrameBufIdx						__voMPEG4D0425
#define FrameBufCtl                        __voMPEG4D0426
#define IDCT_Col4                          __voMPEG4D0427
#define CorrectMV							__voMPEG4D0428

#define 	voMPEG4DecInit		 __voMPEG4D0429
#define 	voMPEG4DecUninit		 __voMPEG4D0430
#define 	voMPEG4DecSetParameter		 __voMPEG4D0431
#define 	voMPEG4DecGetParameter		 __voMPEG4D0432
#define 	voMPEG4DecSetInputData		 __voMPEG4D0433
#define 	voMPEG4DecGetOutputData		 __voMPEG4D0434
// #define 	yyGetMPEG4DecFunc		 __voMPEG4D0435
#define 	GetInnerMemSize		 __voMPEG4D0436
#define 	voMPEGDecFrame		 __voMPEG4D0437
#define 	voMPEG4DecCreate		 __voMPEG4D0438
#define 	voMPEG4DecFinish		 __voMPEG4D0439
#define 	MBrMPEG4VOP		 __voMPEG4D0440
#define 	MBrH263IVOP		 __voMPEG4D0441
#define 	MBrH263PVOP		 __voMPEG4D0442
#define 	InitialMBThread		 __voMPEG4D0443
#define 	ReleaseMBThread		 __voMPEG4D0444
#define 	ResetMBThread		 __voMPEG4D0445
#define 	SetMBThreadVOPFunc		 __voMPEG4D0446
// #define 	fill_edge_y_armv6		 __voMPEG4D0447
// #define 	fill_edge_uv_armv6		 __voMPEG4D0448
#define 	IdctColunm8		 __voMPEG4D0449
#define 	IdctColunm4		 __voMPEG4D0450
#define 	IdctLine8		 __voMPEG4D0451
#define 	IdctLine4		 __voMPEG4D0452
#define 	IdctBlock1x1_C		 __voMPEG4D0453
#define 	IdctBlock8x8_C		 __voMPEG4D0454
#define 	IdctBlock4x8_C		 __voMPEG4D0455
#define 	IdctBlock4x4_C		 __voMPEG4D0456
#define	MCMpegBlockRd_C	__voMPEG4D0472
#define	MCMpegBlockAddRd_C	__voMPEG4D0473
#define	MCMpegBlockAddRdHor_C	__voMPEG4D0474
#define	MCMpegBlockAddRdVer_C	__voMPEG4D0475
#define	MCMpegBlockAddRdHorVer_C	__voMPEG4D0476
#define GetPMV_interlace __voMPEG4D0477
#define MPEG4DecMBRow __voMPEG4D0478
#define WaitForParser __voMPEG4D0479
#define WaitForSubThdFinish __voMPEG4D0480
#define	MCMpeg4BlockQpel12Add_C		__voMPEG4D0481
#define	MCMpeg4BlockQpel12_C		__voMPEG4D0482
#define	MCMpeg4BlockQpel1Add_C		__voMPEG4D0483
#define	MCMpeg4BlockQpel1_C		__voMPEG4D0484
#define	MCMpeg4BlockQpel2Add_C		__voMPEG4D0485
#define	MCMpeg4BlockQpel2_C		__voMPEG4D0486
#define	MCMpeg4BlockQpel3Add_C		__voMPEG4D0487
#define	MCMpeg4BlockQpel3_C		__voMPEG4D0488
#define	MCMpeg4BlockQpel4Add_C		__voMPEG4D0489
#define	MCMpeg4BlockQpel4_C		__voMPEG4D0490
#define	MCMpeg4BlockQpel8Add_C		__voMPEG4D0491
#define	MCMpeg4BlockQpel8_C		__voMPEG4D0492

#if defined(_IOS)
#define	Arm11IdctA		_voMPEG4D0493
#define	Arm11IdctB		_voMPEG4D0494
#define	Arm11IdctC1		_voMPEG4D0495
#define	Arm11IdctC		_voMPEG4D0496
#define	Arm11IdctD		_voMPEG4D0497
#define	Arm11InterBQ1		_voMPEG4D0498
#define	Arm11InterBQ12		_voMPEG4D0499
#define	Arm11InterBQ2		_voMPEG4D0500
#define	Arm11InterBQ3		_voMPEG4D0501
#define	Arm11InterBQ4		_voMPEG4D0502
#define	Arm11InterBQ8		_voMPEG4D0503
#define	Arm11InterQ12		_voMPEG4D0504
#define	Arm11InterQ1		_voMPEG4D0505
#define	Arm11InterQ2		_voMPEG4D0506
#define	Arm11InterQ3		_voMPEG4D0507
#define	Arm11InterQ4		_voMPEG4D0508
#define	Arm11InterQ8		_voMPEG4D0509
#define	MPEG4DEC_VO_Armv7Copy16x16		_voMPEG4D0510
#define	MPEG4DEC_VO_Armv7Copy8x8		_voMPEG4D0511
#define	MPEG4DEC_VO_Armv7Inter2_8x8		_voMPEG4D0512
#define	MPEG4DEC_VO_Armv7Inter2_8x8H		_voMPEG4D0513
#define	MPEG4DEC_VO_Armv7Inter2_8x8V		_voMPEG4D0514
#define	MPEG4DEC_VO_Armv7Inter2_8x8HV		_voMPEG4D0515
#define	MPEG4DEC_VO_Armv7Inter8x8		_voMPEG4D0516
#define	MPEG4DEC_VO_Armv7Inter8x8H		_voMPEG4D0517
#define	MPEG4DEC_VO_Armv7Inter8x8HV		_voMPEG4D0518
#define	MPEG4DEC_VO_Armv7Inter8x8HRD		_voMPEG4D0519
#define	MPEG4DEC_VO_Armv7Inter8x8HVRD		_voMPEG4D0520
#define	MPEG4DEC_VO_Armv7Inter8x8V		_voMPEG4D0521
#define	MPEG4DEC_VO_Armv7Inter8x8VRD		_voMPEG4D0522
#define	fill_edge_uv_armv6		_voMPEG4D0523
#define	fill_edge_y_armv6		_voMPEG4D0524

#define	_Arm11IdctA		__voMPEG4D0493
#define	_Arm11IdctB		__voMPEG4D0494
#define	_Arm11IdctC1		__voMPEG4D0495
#define	_Arm11IdctC		__voMPEG4D0496
#define	_Arm11IdctD		__voMPEG4D0497
#define	_Arm11InterBQ1		__voMPEG4D0498
#define	_Arm11InterBQ12		__voMPEG4D0499
#define	_Arm11InterBQ2		__voMPEG4D0500
#define	_Arm11InterBQ3		__voMPEG4D0501
#define	_Arm11InterBQ4		__voMPEG4D0502
#define	_Arm11InterBQ8		__voMPEG4D0503
#define	_Arm11InterQ12		__voMPEG4D0504
#define	_Arm11InterQ1		__voMPEG4D0505
#define	_Arm11InterQ2		__voMPEG4D0506
#define	_Arm11InterQ3		__voMPEG4D0507
#define	_Arm11InterQ4		__voMPEG4D0508
#define	_Arm11InterQ8		__voMPEG4D0509
#define	_MPEG4DEC_VO_Armv7Copy16x16		__voMPEG4D0510
#define	_MPEG4DEC_VO_Armv7Copy8x8		__voMPEG4D0511
#define	_MPEG4DEC_VO_Armv7Inter2_8x8		__voMPEG4D0512
#define	_MPEG4DEC_VO_Armv7Inter2_8x8H		__voMPEG4D0513
#define	_MPEG4DEC_VO_Armv7Inter2_8x8V		__voMPEG4D0514
#define	_MPEG4DEC_VO_Armv7Inter2_8x8HV		__voMPEG4D0515
#define	_MPEG4DEC_VO_Armv7Inter8x8		__voMPEG4D0516
#define	_MPEG4DEC_VO_Armv7Inter8x8H		__voMPEG4D0517
#define	_MPEG4DEC_VO_Armv7Inter8x8HV		__voMPEG4D0518
#define	_MPEG4DEC_VO_Armv7Inter8x8HRD		__voMPEG4D0519
#define	_MPEG4DEC_VO_Armv7Inter8x8HVRD		__voMPEG4D0520
#define	_MPEG4DEC_VO_Armv7Inter8x8V		__voMPEG4D0521
#define	_MPEG4DEC_VO_Armv7Inter8x8VRD		__voMPEG4D0522
#define	_fill_edge_uv_armv6		__voMPEG4D0523
#define	_fill_edge_y_armv6		__voMPEG4D0524

#endif

#endif //VODIVX3

#endif//NCNAMETAB
