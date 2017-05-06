# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= ../../../../../../Include \
		./ \
		../../source \
		../../source/vp8 \
		../../source/vp8/common \
		../../source/vp8/common/generic \
		../../source/vp8/encoder \
		../../source/vp8/encoder/generic \
		../../source/vpx \
		../../source/vpx/internal \
		../../source/vpx/src \
		../../source/vpx_mem \
		../../source/vpx_mem/include \
		../../source/vpx_ports \
		../../source/vpx_scale \
		../../source/vpx_scale/generic \
		../../source/build/make \
		../../public


# please list all objects needed by your target here
OBJS:= vpx_image.o vpx_encoder.o vpx_codec.o \
		vpx_mem.o \
		gen_scalers.o scalesystemdependent.o vpxscale.o yv12config.o yv12extend.o \
		vp8_cx_iface.o \
		alloccommon.o asm_com_offsets.o blockd.o debugmodes.o dequantize.o entropy.o entropymode.o entropymv.o extend.o filter.o findnearmv.o idct_blk.o idctllm.o loopfilter.o loopfilter_filters.o \
		mbpitch.o modecont.o modecontext.o postproc.o quant_common.o reconinter.o reconintra.o reconintra4x4.o setupintrarecon.o swapyv12buffer.o treecoder.o systemdependent.o \
		asm_enc_offsets.o bitstream.o dct.o encodeframe.o encodeintra.o encodemb.o encodemv.o ethreading.o firstpass.o lookahead.o mcomp.o modecosts.o onyx_if.o pickinter.o \
		picklpf.o psnr.o quantize.o ratectrl.o rdopt.o sad_c.o segmentation.o temporal_filter.o tokenize.o treewriter.o variance_c.o \
		csystemdependent.o \
		voClrConv.o voVP8EncFront.o voVP8EncParaMgr.o

			
ifeq ($(VOTT), v4)
OBJS+= arm_systemdependent.o arm_csystemdependent.o arm_cpudetect.o arm_scalesystemdependent.o \
		boolhuff.o
VOSRCDIR+= ../../source/vp8/common/arm \
					../../source/vp8/encoder/arm \
					../../source/vpx_scale/arm
endif	

ifeq ($(VOTT), v5)
OBJS+= arm_systemdependent.o arm_csystemdependent.o arm_cpudetect.o arm_scalesystemdependent.o \
		boolhuff_arm.o \
		boolhuff_armv5te.o vp8_packtokens_armv5.o vp8_packtokens_mbrow_armv5.o vp8_packtokens_partitions_armv5.o
VOSRCDIR+=  ../../source/vp8/common/arm \
					../../source/vp8/encoder/arm \
					../../source/vp8/encoder/arm/armv5te/gnu \
					../../source/vpx_scale/arm 
endif	

ifeq ($(VOTT), v6)
OBJS+= arm_systemdependent.o arm_csystemdependent.o arm_cpudetect.o arm_scalesystemdependent.o \
		bilinearfilter_arm.o dequantize_arm.o filter_arm.o loopfilter_arm.o \
		idct_blk_v6.o \
		bilinearfilter_v6.o copymem8x4_v6.o copymem8x8_v6.o copymem16x16_v6.o dc_only_idct_add_v6.o dequant_idct_v6.o dequantize_v6.o \
		filter_v6.o idct_v6.o intra4x4_predict_v6.o iwalsh_v6.o loopfilter_v6.o simpleloopfilter_v6.o sixtappredict8x4_v6.o \
		boolhuff_arm.o dct_arm.o variance_arm.o \
		boolhuff_armv5te.o vp8_packtokens_armv5.o vp8_packtokens_mbrow_armv5.o vp8_packtokens_partitions_armv5.o \
		vp8_fast_quantize_b_armv6.o vp8_mse16x16_armv6.o vp8_sad16x16_armv6.o vp8_short_fdct4x4_armv6.o vp8_subtract_armv6.o vp8_variance_halfpixvar16x16_h_armv6.o \
		vp8_variance_halfpixvar16x16_hv_armv6.o vp8_variance_halfpixvar16x16_v_armv6.o vp8_variance8x8_armv6.o vp8_variance16x16_armv6.o walsh_v6.o
VOSRCDIR+=   ../../source/vp8/common/arm \
					../../source/vp8/common/arm/armv6 \
					../../source/vp8/common/arm/armv6/gnu \
					../../source/vp8/encoder/arm \
					../../source/vp8/encoder/arm/armv5te/gnu \
					../../source/vp8/encoder/arm/armv6/gnu \
					../../source/vpx_scale/arm
endif	

ifeq ($(VOTT), v7)
OBJS+= arm_systemdependent.o arm_csystemdependent.o arm_cpudetect.o arm_scalesystemdependent.o \
		bilinearfilter_arm.o dequantize_arm.o filter_arm.o loopfilter_arm.o reconintra_arm.o \
		idct_blk_v6.o \
		bilinearfilter_v6.o copymem8x4_v6.o copymem8x8_v6.o copymem16x16_v6.o dc_only_idct_add_v6.o dequant_idct_v6.o dequantize_v6.o \
		filter_v6.o idct_v6.o intra4x4_predict_v6.o iwalsh_v6.o loopfilter_v6.o simpleloopfilter_v6.o sixtappredict8x4_v6.o \
		idct_blk_neon.o \
		bilinearpredict4x4_neon.o bilinearpredict8x4_neon.o bilinearpredict8x8_neon.o bilinearpredict16x16_neon.o buildintrapredictorsmby_neon.o \
		copymem8x4_neon.o copymem8x8_neon.o copymem16x16_neon.o dc_only_idct_add_neon.o dequant_idct_neon.o dequantizeb_neon.o idct_dequant_0_2x_neon.o \
		idct_dequant_full_2x_neon.o iwalsh_neon.o loopfilter_neon.o loopfiltersimplehorizontaledge_neon.o loopfiltersimpleverticaledge_neon.o \
		mbloopfilter_neon.o save_neon_reg.o shortidct4x4llm_neon.o sixtappredict4x4_neon.o sixtappredict8x4_neon.o sixtappredict8x8_neon.o sixtappredict16x16_neon.o \
		boolhuff_arm.o dct_arm.o variance_arm.o \
		boolhuff_armv5te.o vp8_packtokens_armv5.o vp8_packtokens_mbrow_armv5.o vp8_packtokens_partitions_armv5.o \
		vp8_fast_quantize_b_armv6.o vp8_mse16x16_armv6.o vp8_sad16x16_armv6.o vp8_short_fdct4x4_armv6.o vp8_subtract_armv6.o vp8_variance_halfpixvar16x16_h_armv6.o \
		vp8_variance_halfpixvar16x16_hv_armv6.o vp8_variance_halfpixvar16x16_v_armv6.o vp8_variance8x8_armv6.o vp8_variance16x16_armv6.o walsh_v6.o \
		picklpf_arm.o \
		fastquantizeb_neon.o sad8_neon.o sad16_neon.o shortfdct_neon.o subtract_neon.o variance_neon.o vp8_memcpy_neon.o vp8_mse16x16_neon.o \
		vp8_shortwalsh4x4_neon.o vp8_subpixelvariance8x8_neon.o vp8_subpixelvariance16x16_neon.o vp8_subpixelvariance16x16s_neon.o \
		yv12extend_arm.o \
		vp8_vpxyv12_copy_y_neon.o vp8_vpxyv12_copyframe_func_neon.o vp8_vpxyv12_copysrcframe_func_neon.o vp8_vpxyv12_extendframeborders_neon.o
VOSRCDIR+=   ../../source/vp8/common/arm \
					../../source/vp8/common/arm/armv6 \
					../../source/vp8/common/arm/armv6/gnu \
					../../source/vp8/common/arm/neon \
					../../source/vp8/common/arm/neon/gnu \
					../../source/vp8/encoder/arm \
					../../source/vp8/encoder/arm/armv5te/gnu \
					../../source/vp8/encoder/arm/armv6/gnu \
					../../source/vp8/encoder/arm/neon \
					../../source/vp8/encoder/arm/neon/gnu \
					../../source/vpx_scale/arm \
					../../source/vpx_scale/arm/neon \
					../../source/vpx_scale/arm/neon/gnu
endif	
