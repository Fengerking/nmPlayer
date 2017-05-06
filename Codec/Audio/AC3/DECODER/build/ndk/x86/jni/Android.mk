LOCAL_PATH := $(call my-dir)
VOTOP?=../../../../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := libvoAC3Dec

CMN_SRC:=../../../../../../../../Common
MOD_SRC:=../../../../src/c_src

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
	$(MOD_SRC)/xdcall.c


LOCAL_C_INCLUDES := \
	../../../../../../../../Include \
	../../../../../../../../Common \
	../../../../inc \


#VOMM:=-DLINUX -DARM -DARMV6 -DLITEND -DKCAPABLE -DAC3D_ARM_OPT -DHALFRATE -DLCHECK
VOMM:=-DLINUX -DLITEND -DKCAPABLE -DHALFRATE -DLCHECK

# about info option, do not need to care it

#LOCAL_CFLAGS := -D_VOMODULEID=0x022c0000  -DNDEBUG -mfloat-abi=softfp -march=armv7-a -mtune=cortex-a8 -mfpu=neon
LOCAL_CFLAGS := -D_VOMODULEID=0x022c0000  -DNDEBUG -march=i686 -mtune=atom -mstackrealign -msse3 -mfpmath=sse -m32 -fsigned-char

LOCAL_LDLIBS:=../../../../../../../../Lib/ndk/x86/libvoCheck.a -llog

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

