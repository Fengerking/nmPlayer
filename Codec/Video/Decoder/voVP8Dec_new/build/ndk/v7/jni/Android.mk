LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libvoVP8Dec


CMNSRC_PATH:=../../../../../../../../Common
CSRC_PATH:=../../../../src/
CSRC_VP8_PATH:=../../../../src/libvpx-v1.1.0/vp8
CSRC_VPX_PATH:=../../../../src/libvpx-v1.1.0/vpx
CSRC_VPXSCALE_PATH:=../../../../src/libvpx-v1.1.0/vpx_scale
CSRC_VPXPORT_PATH:=../../../../src/libvpx-v1.1.0/vpx_ports
CSRC_VPXMEM_PATH:=../../../../src/libvpx-v1.1.0/vpx_mem
CSRC_VP8COM_PATH:=../../../../src/libvpx-v1.1.0/vp8/common
CSRC_VP8DEC_PATH:=../../../../src/libvpx-v1.1.0/vp8/decoder
CSRC_VP8ENC_PATH:=../../../../src/libvpx-v1.1.0/vp8/encoder

LOCAL_SRC_FILES := \
	\
	$(CSRC_PATH)/vpx_config_arm.c\
	$(CSRC_VP8COM_PATH)/alloccommon.c\
	$(CSRC_VP8COM_PATH)/arm/armv6/idct_blk_v6.c\
	$(CSRC_VP8COM_PATH)/arm/bilinearfilter_arm.c\
	$(CSRC_VP8COM_PATH)/arm/dequantize_arm.c\
	$(CSRC_VP8COM_PATH)/arm/filter_arm.c\
	$(CSRC_VP8COM_PATH)/arm/loopfilter_arm.c\
	$(CSRC_VP8COM_PATH)/arm/neon/idct_blk_neon.c\
	$(CSRC_VP8COM_PATH)/arm/reconintra_arm.c\
	$(CSRC_VP8COM_PATH)/arm/variance_arm.c\
	$(CSRC_VP8COM_PATH)/asm_com_offsets.c\
	$(CSRC_VP8COM_PATH)/blockd.c\
	$(CSRC_VP8COM_PATH)/debugmodes.c\
	$(CSRC_VP8COM_PATH)/dequantize.c\
	$(CSRC_VP8COM_PATH)/entropy.c\
	$(CSRC_VP8COM_PATH)/entropymode.c\
	$(CSRC_VP8COM_PATH)/entropymv.c\
	$(CSRC_VP8COM_PATH)/extend.c\
	$(CSRC_VP8COM_PATH)/filter.c\
	$(CSRC_VP8COM_PATH)/findnearmv.c\
	$(CSRC_VP8COM_PATH)/generic/systemdependent.c\
	$(CSRC_VP8COM_PATH)/idct_blk.c\
	$(CSRC_VP8COM_PATH)/idctllm.c\
	$(CSRC_VP8COM_PATH)/loopfilter.c\
	$(CSRC_VP8COM_PATH)/loopfilter_filters.c\
	$(CSRC_VP8COM_PATH)/mbpitch.c\
	$(CSRC_VP8COM_PATH)/modecont.c\
	$(CSRC_VP8COM_PATH)/quant_common.c\
	$(CSRC_VP8COM_PATH)/reconinter.c\
	$(CSRC_VP8COM_PATH)/reconintra.c\
	$(CSRC_VP8COM_PATH)/reconintra4x4.c\
	$(CSRC_VP8COM_PATH)/rtcd.c\
	$(CSRC_VP8COM_PATH)/sad_c.c\
	$(CSRC_VP8COM_PATH)/setupintrarecon.c\
	$(CSRC_VP8COM_PATH)/swapyv12buffer.c\
	$(CSRC_VP8COM_PATH)/treecoder.c\
	$(CSRC_VP8COM_PATH)/variance_c.c\
	$(CSRC_VP8DEC_PATH)/asm_dec_offsets.c\
	$(CSRC_VP8DEC_PATH)/dboolhuff.c\
	$(CSRC_VP8DEC_PATH)/decodemv.c\
	$(CSRC_VP8DEC_PATH)/decodframe.c\
	$(CSRC_VP8DEC_PATH)/detokenize.c\
	$(CSRC_VP8DEC_PATH)/onyxd_if.c\
	$(CSRC_VP8DEC_PATH)/threading.c\
	$(CSRC_VP8_PATH)/vp8_dx_iface.c\
	$(CSRC_PATH)/voVP8DecFront.c\
	$(CSRC_PATH)/voVP8Memory.c\
	$(CSRC_PATH)/frameThd.c\
	$(CSRC_VPX_PATH)/src/vpx_codec.c\
	$(CSRC_VPX_PATH)/src/vpx_decoder.c\
	$(CSRC_VPX_PATH)/src/vpx_image.c\
	$(CSRC_VPXMEM_PATH)/vpx_mem.c\
	$(CSRC_VPXPORT_PATH)/arm_cpudetect.c\
	$(CSRC_VPXSCALE_PATH)/arm/neon/yv12extend_arm.c\
	$(CSRC_VPXSCALE_PATH)/generic/gen_scalers.c\
	$(CSRC_VPXSCALE_PATH)/generic/vpxscale.c\
	$(CSRC_VPXSCALE_PATH)/generic/yv12config.c\
	$(CSRC_VPXSCALE_PATH)/generic/yv12extend.c\
	$(CSRC_VP8COM_PATH)/arm/armv6/bilinearfilter_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/copymem16x16_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/copymem8x4_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/copymem8x8_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/dc_only_idct_add_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/dequant_idct_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/dequantize_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/filter_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/idct_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/intra4x4_predict_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/iwalsh_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/loopfilter_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/simpleloopfilter_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/sixtappredict8x4_v6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/vp8_sad16x16_armv6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/vp8_variance16x16_armv6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/vp8_variance8x8_armv6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/vp8_variance_halfpixvar16x16_h_armv6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/vp8_variance_halfpixvar16x16_hv_armv6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/armv6/vp8_variance_halfpixvar16x16_v_armv6_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/bilinearpredict16x16_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/bilinearpredict4x4_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/bilinearpredict8x4_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/bilinearpredict8x8_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/buildintrapredictorsmby_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/copymem16x16_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/copymem8x4_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/copymem8x8_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/dc_only_idct_add_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/dequant_idct_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/dequantizeb_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/idct_dequant_0_2x_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/idct_dequant_full_2x_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/iwalsh_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/loopfilter_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/loopfiltersimplehorizontaledge_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/loopfiltersimpleverticaledge_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/mbloopfilter_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/sad16_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/sad8_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/save_reg_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/shortidct4x4llm_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/sixtappredict16x16_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/sixtappredict4x4_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/sixtappredict8x4_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/sixtappredict8x8_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/variance_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/vp8_subpixelvariance16x16_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/vp8_subpixelvariance16x16s_neon_gnu.S\
  $(CSRC_VP8COM_PATH)/arm/neon/vp8_subpixelvariance8x8_neon_gnu.S\
  $(CSRC_VPXSCALE_PATH)/arm/neon/vp8_vpxyv12_copy_y_neon_gnu.S\
  $(CSRC_VPXSCALE_PATH)/arm/neon/vp8_vpxyv12_copyframe_func_neon_gnu.S\
  $(CSRC_VPXSCALE_PATH)/arm/neon/vp8_vpxyv12_copysrcframe_func_neon_gnu.S\
  $(CSRC_VPXSCALE_PATH)/arm/neon/vp8_vpxyv12_extendframeborders_neon_gnu.S
                                                 
	
	
LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../inc/arm\
	../../../../src\
	../../../../src/libvpx-v1.1.0\
  ../../../../src/libvpx-v1.1.0/vpx\
  ../../../../src/libvpx-v1.1.0/vp8/common\
  ../../../../src/libvpx-v1.1.0/vp8/decoder\
  ../../../../src/libvpx-v1.1.0/vpx_scale




VOMM:=-DLINUX -DLICENSEFILE -DVOOPT -DVOANDROID

LOCAL_CFLAGS := -D_VOMODULEID=0x020b0000  $(VOHIDESYM) -fPIC -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LOCAL_LDLIBS := ../../../../../../../../Lib/ndk/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

                                                                                