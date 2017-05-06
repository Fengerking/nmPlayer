LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := libyyAC3DecLib

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src
ASM_SRC6:=../../../../src/linux_asm/armv6
LOCAL_SRC_FILES := \
	\
	$(CMN_SRC)/cmnMemory.c \
	$(MOD_SRC)/ac3_dec.c \
	$(MOD_SRC)/ac3_dec_init.c \
	$(MOD_SRC)/ac3_info.c \
	$(MOD_SRC)/ac3d_tab.c \
	$(MOD_SRC)/ac3d_var.c \
	$(MOD_SRC)/bsi_d.c \
	$(MOD_SRC)/cifft.c \
	$(MOD_SRC)/crc_calc.c \
	$(MOD_SRC)/crc_tab.c \
	$(MOD_SRC)/dolbytab.c \
	$(MOD_SRC)/downmix.c \
	$(MOD_SRC)/dsp_misc.c \
	$(MOD_SRC)/idctsc.c \
	$(MOD_SRC)/mants_d.c \
	$(MOD_SRC)/matrix_d.c \
	$(MOD_SRC)/mem_align.c \
	$(MOD_SRC)/stream.c \
	$(MOD_SRC)/unpmants.c \
	$(MOD_SRC)/upk_subs.c \
	$(MOD_SRC)/voAC3Dec.c \
	$(MOD_SRC)/window_d.c \
	$(MOD_SRC)/xdcall.c \
	$(ASM_SRC6)/upk_subs_asm.S \
	$(ASM_SRC6)/crc_calc_asm.S \
	$(ASM_SRC6)/window_d_asm.S \
	$(ASM_SRC6)/cifft_asm.S 


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


VOMM:=-DLINUX -DARM -DARMV6 -DLITEND -DKCAPABLE -DAC3D_ARM_OPT -DHALFRATE -D_VONDBG

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x022c0000  -DNDEBUG -mfloat-abi=soft -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char

LOCAL_LDLIBS:= -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := libyyAC3Dec


LOCAL_STATIC_LIBRARIES := libyyAC3DecLib
include $(BUILD_SHARED_LIBRARY)
