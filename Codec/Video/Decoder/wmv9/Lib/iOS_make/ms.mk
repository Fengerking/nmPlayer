
# please list all objects needed by your module here

VOMSRC:=\
		      Affine.c effects_wmv.c idctdec_wmv9.c InterlaceDecPV2.c localhuffman_wmv.c \
			   reflibrary_wmv9.c tables_wmv.c wmvdec.c blkdec_wmv.c FrameTimer.c init_wmv.c \
               InterlaceDecTable.c motioncomp_wmv.c share.c threadctr_dec_wmv9.c deblock.c \
               frmdec_wmv.c InterlaceCore_wmv9.c InterlaceDecV2.c multiresdec_wmv9.c sideroll_wmv.c \
               uncache_wmv.c dectable.c headdec.c InterlaceDec_BFrame.c InterlaceDec_wmv9.c \
               multires_wmv9.c spatialpredictor_wmv.c vopdec9_wmv9.c dectable_mp4x.c huffdec_wmv.c \
               InterlaceDecBV2.c interpolate_wmv9.c  pageroll_wmv.c  startcodedec.c  vopdec_BFrame.c \
               deinterlace_wmv9.c idctdec_wmv2.c InterlaceDecCoreV2.c intradec.c  postfilter_wmv.c \
               strmdec_wmv.c vopdec_mp4x.c idctdec_wmv9_emb.c idctdec_emb.c \
			   interpolate_wmv9_emb.c motioncomp_emb.c vopdec9_wmv9_emb.c vopdecB_emb.c vopdec_emb.c
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../Src/c \
	  ../../../Src/emb \
	  ../../../../../../../Include\


# asm objects and directories.

ifeq ($(VOTT), v6)
VOMSRC+= \
	      affine_arm.s effects_arm.s  idct_arm.s interpolate_arm.s loopfilter_arm.s \
        motioncomp_arm3.s multires_arm.s spatialpredictor_arm.s blkdec_arm.s filter.s \
        idct_wmv9_arm_2.s  interpolate_wmv9_arm_2.s macros_arm.s motioncomp_arm4.s \
        postfilter_arm.s vopdec_arm2.s dynamic_edgepad_arm.s huffman_arm.s idct_wmv9_arm.s \
        interpolate_wmv9_arm.s motioncomp_arm2.s motioncomp_arm.s repeatpad_arm.s  vopdec_arm.s \
        idct_wmv9_arm_ARMv6.s interpolate_wmv9_arm_ARMv6.s loopfilter_arm_ARMv6.s \
        motioncomp_arm_ARMv6.s  repeatpad_arm_ARMv6.s

VOSRCDIR+= \
	../../../Src/armX_iOS
endif

ifeq ($(VOTT), v7)
VOMSRC+= \
	      affine_arm.s effects_arm.s  idct_arm.s interpolate_arm.s loopfilter_arm.s \
        motioncomp_arm3.s multires_arm.s spatialpredictor_arm.s blkdec_arm.s filter.s \
        idct_wmv9_arm_2.s  interpolate_wmv9_arm_2.s macros_arm.s motioncomp_arm4.s \
        postfilter_arm.s vopdec_arm2.s dynamic_edgepad_arm.s huffman_arm.s idct_wmv9_arm.s \
        interpolate_wmv9_arm.s motioncomp_arm2.s motioncomp_arm.s repeatpad_arm.s  vopdec_arm.s \
        idct_wmv9_arm_ARMv6.s interpolate_wmv9_arm_ARMv6.s loopfilter_arm_ARMv6.s \
        motioncomp_arm_ARMv6.s  repeatpad_arm_ARMv6.s \
        idct_wmv9_arm_2_ARMv7.s idct_wmv9_arm_ARMv7.s interpolate_wmv9_arm_2_ARMv7.s \
        interpolate_wmv9_arm_ARMv7.s  loopfilter_arm_ARMv7.s motioncomp_arm_ARMv7.s

VOSRCDIR+= \
	../../../Src/armX_iOS
endif

ifeq ($(VOTT), v7s)
VOMSRC+= \
	      affine_arm.s effects_arm.s  idct_arm.s interpolate_arm.s loopfilter_arm.s \
        motioncomp_arm3.s multires_arm.s spatialpredictor_arm.s blkdec_arm.s filter.s \
        idct_wmv9_arm_2.s  interpolate_wmv9_arm_2.s macros_arm.s motioncomp_arm4.s \
        postfilter_arm.s vopdec_arm2.s dynamic_edgepad_arm.s huffman_arm.s idct_wmv9_arm.s \
        interpolate_wmv9_arm.s motioncomp_arm2.s motioncomp_arm.s repeatpad_arm.s  vopdec_arm.s \
        idct_wmv9_arm_ARMv6.s interpolate_wmv9_arm_ARMv6.s loopfilter_arm_ARMv6.s \
        motioncomp_arm_ARMv6.s  repeatpad_arm_ARMv6.s \
        idct_wmv9_arm_2_ARMv7.s idct_wmv9_arm_ARMv7.s interpolate_wmv9_arm_2_ARMv7.s \
        interpolate_wmv9_arm_ARMv7.s  loopfilter_arm_ARMv7.s motioncomp_arm_ARMv7.s

VOSRCDIR+= \
	../../../Src/armX_iOS
endif
