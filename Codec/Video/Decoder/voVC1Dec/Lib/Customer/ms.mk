
# please list all objects needed by your module here

OBJS:=\
		      idctdec_wmv9.o InterlaceDecPV2.o localhuffman_wmv.o \
			   reflibrary_wmv9.o tables_wmv.o wmvdec.o blkdec_wmv.o init_wmv.o \
               InterlaceDecTable.o motioncomp_wmv.o share.o threadctr_dec_wmv9.o deblock.o \
               frmdec_wmv.o InterlaceCore_wmv9.o InterlaceDecV2.o multiresdec_wmv9.o \
               dectable.o headdec.o InterlaceDec_BFrame.o InterlaceDec_wmv9.o \
               multires_wmv9.o spatialpredictor_wmv.o vopdec9_wmv9.o huffdec_wmv.o \
               InterlaceDecBV2.o interpolate_wmv9.o startcodedec.o  vopdec_BFrame.o \
               idctdec_wmv2.o InterlaceDecCoreV2.o intradec.o\
               strmdec_wmv.o idctdec_wmv9_emb.o idctdec_emb.o \
			   interpolate_wmv9_emb.o motioncomp_emb.o vopdec9_wmv9_emb.o vopdecB_emb.o vopdec_emb.o voVc1Thread.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../../../../Src/c \
	  ../../../../../Src/emb \
	  ../../../../../../../../../Include\


# asm objects and directories.
#ifeq ($(VOTT), v4)
#OBJS+= \
#	      idct_arm.o interpolate_arm.o loopfilter_arm.o \
#        multires_arm.o filter.o \
#        idct_wmv9_arm_2.o  interpolate_wmv9_arm_2.o macros_arm.o \
#        postfilter_arm.o vopdec_arm2.o huffman_arm.o idct_wmv9_arm.o \
#        interpolate_wmv9_arm.o motioncomp_arm2.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o 
#
#VOSRCDIR+= ../../../../../Src/armX_gnu \
#                     ../../../../../Src/armX_gnu/incgen
#endif

ifeq ($(VOTT), v6)
OBJS+= \
	      loopfilter_arm.o \
        multires_arm.o \
        interpolate_wmv9_arm_2.o macros_arm.o \
        postfilter_arm.o vopdec_arm2.o huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o  motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o \
        idct_wmv9_arm_ARMv6.o interpolate_wmv9_arm_ARMv6.o loopfilter_arm_ARMv6.o \
        motioncomp_arm_ARMv6.o  repeatpad_arm_ARMv6.o

VOSRCDIR+= ../../../../../Src/armX_gnu \
                     ../../../../../Src/armX_gnu/incgen
endif

ifeq ($(VOTT), v7)
OBJS+= \
	      loopfilter_arm.o \
        multires_arm.o \
        interpolate_wmv9_arm_2.o macros_arm.o  \
        postfilter_arm.o vopdec_arm2.o  huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o \
        idct_wmv9_arm_ARMv6.o interpolate_wmv9_arm_ARMv6.o loopfilter_arm_ARMv6.o \
        motioncomp_arm_ARMv6.o  repeatpad_arm_ARMv6.o \
        idct_wmv9_arm_2_ARMv7.o idct_wmv9_arm_ARMv7.o interpolate_wmv9_arm_2_ARMv7.o \
        interpolate_wmv9_arm_ARMv7.o  loopfilter_arm_ARMv7.o motioncomp_arm_ARMv7.o \
        voVC1Trans_ARMV7.o voVC1InterpolateOnly_ARMv7.o voVC1Interpolate_ARMV7.o\
        voVC1Trans_Overlap_ARMV7.o

VOSRCDIR+= ../../../../../Src/armX_gnu \
                     ../../../../../Src/armX_gnu/incgen
endif

ifeq ($(VOTT), v7s)
OBJS+= \
	      loopfilter_arm.o \
        multires_arm.o \
        interpolate_wmv9_arm_2.o macros_arm.o  \
        postfilter_arm.o vopdec_arm2.o  huffman_arm.o idct_wmv9_arm.o \
        interpolate_wmv9_arm.o motioncomp_arm.o repeatpad_arm.o  vopdec_arm.o \
        idct_wmv9_arm_ARMv6.o interpolate_wmv9_arm_ARMv6.o loopfilter_arm_ARMv6.o \
        motioncomp_arm_ARMv6.o  repeatpad_arm_ARMv6.o \
        idct_wmv9_arm_2_ARMv7.o idct_wmv9_arm_ARMv7.o interpolate_wmv9_arm_2_ARMv7.o \
        interpolate_wmv9_arm_ARMv7.o  loopfilter_arm_ARMv7.o motioncomp_arm_ARMv7.o \
        voVC1Trans_ARMV7.o voVC1InterpolateOnly_ARMv7.o voVC1Interpolate_ARMV7.o\
        voVC1Trans_Overlap_ARMV7.o

VOSRCDIR+= ../../../../../Src/armX_gnu \
                     ../../../../../Src/armX_gnu/incgen
endif
