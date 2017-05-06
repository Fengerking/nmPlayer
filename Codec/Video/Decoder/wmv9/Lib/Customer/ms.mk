
# please list all objects needed by your module here

OBJS:=\
		       Affine.o effects_wmv.o idctdec_wmv9.o InterlaceDecPV2.o localhuffman_wmv.o \
			   reflibrary_wmv9.o tables_wmv.o wmvdec.o blkdec_wmv.o FrameTimer.o init_wmv.o \
               InterlaceDecTable.o motioncomp_wmv.o share.o threadctr_dec_wmv9.o deblock.o \
               frmdec_wmv.o InterlaceCore_wmv9.o InterlaceDecV2.o multiresdec_wmv9.o sideroll_wmv.o \
               uncache_wmv.o dectable.o headdec.o InterlaceDec_BFrame.o InterlaceDec_wmv9.o \
               multires_wmv9.o spatialpredictor_wmv.o vopdec9_wmv9.o dectable_mp4x.o huffdec_wmv.o \
               InterlaceDecBV2.o interpolate_wmv9.o  pageroll_wmv.o  startcodedec.o  vopdec_BFrame.o \
               deinterlace_wmv9.o idctdec_wmv2.o InterlaceDecCoreV2.o intradec.o  postfilter_wmv.o \
               strmdec_wmv.o vopdec_mp4x.o idctdec_wmv9_emb.o idctdec_emb.o \
			   interpolate_wmv9_emb.o motioncomp_emb.o vopdec9_wmv9_emb.o vopdecB_emb.o vopdec_emb.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../Src/c \
	  ../../../../Src/emb \
	  ../../../../../../../../Include\


# asm objects and directories.
ifeq ($(VOTT), v4)
OBJS+= \
	    affine_arm.o effects_arm.o  idct_arm.o interpolate_arm.o loopfilter_arm.o \
        motioncomp_arm3.o multires_arm.o spatialpredictor_arm.o blkdec_arm.o filter.o \
        idct_wmv9_arm_2.o  interpolate_wmv9_arm_2.o macros_arm.o motioncomp_arm4.o \
        postfilter_arm.o vopdec_arm2.o dynamic_edgepad_arm.o huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o motioncomp_arm2.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o 

VOSRCDIR+= ../../../../Src/armX_gnu \
                     ../../../../Src/armX_gnu/incgen
endif

ifeq ($(VOTT), v6)
OBJS+= \
	    affine_arm.o effects_arm.o  idct_arm.o interpolate_arm.o loopfilter_arm.o \
        motioncomp_arm3.o multires_arm.o spatialpredictor_arm.o blkdec_arm.o filter.o \
        idct_wmv9_arm_2.o  interpolate_wmv9_arm_2.o macros_arm.o motioncomp_arm4.o \
        postfilter_arm.o vopdec_arm2.o dynamic_edgepad_arm.o huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o motioncomp_arm2.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o \
        idct_wmv9_arm_ARMv6.o interpolate_wmv9_arm_ARMv6.o loopfilter_arm_ARMv6.o \
        motioncomp_arm_ARMv6.o  repeatpad_arm_ARMv6.o 

VOSRCDIR+= ../../../../Src/armX_gnu \
                     ../../../../Src/armX_gnu/incgen
endif

ifeq ($(VOTT), v7)
OBJS+= \
	    affine_arm.o effects_arm.o  idct_arm.o interpolate_arm.o loopfilter_arm.o \
        motioncomp_arm3.o multires_arm.o spatialpredictor_arm.o blkdec_arm.o filter.o \
        idct_wmv9_arm_2.o  interpolate_wmv9_arm_2.o macros_arm.o motioncomp_arm4.o \
        postfilter_arm.o vopdec_arm2.o dynamic_edgepad_arm.o huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o motioncomp_arm2.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o \
        idct_wmv9_arm_ARMv6.o interpolate_wmv9_arm_ARMv6.o loopfilter_arm_ARMv6.o \
        motioncomp_arm_ARMv6.o  repeatpad_arm_ARMv6.o \
        idct_wmv9_arm_2_ARMv7.o idct_wmv9_arm_ARMv7.o interpolate_wmv9_arm_2_ARMv7.o \
        interpolate_wmv9_arm_ARMv7.o  loopfilter_arm_ARMv7.o motioncomp_arm_ARMv7.o 

VOSRCDIR+= ../../../../Src/armX_gnu \
                     ../../../../Src/armX_gnu/incgen
endif

ifeq ($(VOTT), v7s)
OBJS+= \
	    affine_arm.o effects_arm.o  idct_arm.o interpolate_arm.o loopfilter_arm.o \
        motioncomp_arm3.o multires_arm.o spatialpredictor_arm.o blkdec_arm.o filter.o \
        idct_wmv9_arm_2.o  interpolate_wmv9_arm_2.o macros_arm.o motioncomp_arm4.o \
        postfilter_arm.o vopdec_arm2.o dynamic_edgepad_arm.o huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o motioncomp_arm2.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o \
        idct_wmv9_arm_ARMv6.o interpolate_wmv9_arm_ARMv6.o loopfilter_arm_ARMv6.o \
        motioncomp_arm_ARMv6.o  repeatpad_arm_ARMv6.o \
        idct_wmv9_arm_2_ARMv7.o idct_wmv9_arm_ARMv7.o interpolate_wmv9_arm_2_ARMv7.o \
        interpolate_wmv9_arm_ARMv7.o  loopfilter_arm_ARMv7.o motioncomp_arm_ARMv7.o 

VOSRCDIR+= ../../../../Src/armX_gnu \
                     ../../../../Src/armX_gnu/incgen
endif