# 
# This configure file is just for Linux projects against Android
#

# special macro definitions for building 
VOPREDEF:=-DLINUX -D_LINUX 

VOPRJ ?= 
VONJ ?= ics
VOTT ?= v6
 

VOPREBUILTPATH?=unknown
ifeq ($(VOPREBUILTPATH),unknown)
VOPREBUILTPATH:=$(VOTOP)/Lib/Customer/google/eclair/v6
endif


# control the version to release out
# available: eva(evaluation), rel(release)
VOVER=
ifeq ($(VOVER), eva)
VOPREDEF+=-D__VOVER_EVA__
endif

# for debug or not: yes for debug, any other for release
VODBG?=ye
VOOPTIM?=unknown
ifeq ($(VOOPTIM),debug)
VODBG:=yes
else
ifeq ($(VOOPTIM),release)
VODBG:=no
endif
endif


# SDK version 
VOMODVER?=3.0.0


# build number
VOBUILDNUM?=0000


# branch name
VOBRANCH?=master


# for detecting memory leak
VODML=
ifeq ($(VODML), yes)
VOPREDEF+=-DDMEMLEAK
endif

VOPREDEF+=-D__VOTT_ARM__ -D__VONJ_ICS__
TCROOTPATH:=/opt/ics-4.0.4
GCCVER:=4.4.3
TCPATH:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/arm-eabi-$(GCCVER)
CCTPRE:=$(TCPATH)/bin/arm-eabi-
AS:=$(CCTPRE)as
AR:=$(CCTPRE)ar
NM:=$(CCTPRE)nm
CC:=$(CCTPRE)gcc
GG:=$(CCTPRE)g++
LD:=$(CCTPRE)ld
SIZE:=$(CCTPRE)size
STRIP:=$(CCTPRE)strip
RANLIB:=$(CCTPRE)ranlib
OBJCOPY:=$(CCTPRE)objcopy
OBJDUMP:=$(CCTPRE)objdump
READELF:=$(CCTPRE)readelf
STRINGS:=$(CCTPRE)strings

# target product dependcy
# available: dream, generic
VOTP:=generic
CCTLIB:=$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib
CCTINC:=-I$(TCROOTPATH)/system/core/include \
	-I$(TCROOTPATH)/hardware/libhardware/include \
	-I$(TCROOTPATH)/hardware/ril/include \
	-I$(TCROOTPATH)/hardware/libhardware_legacy/include \
	-I$(TCROOTPATH)/dalvik/libnativehelper/include \
	-I$(TCROOTPATH)/dalvik/libnativehelper/include/nativehelper \
	-I$(TCROOTPATH)/frameworks/base/include \
	-I$(TCROOTPATH)/frameworks/base/native/include \
	-I$(TCROOTPATH)/frameworks/base/core/jni \
	-I$(TCROOTPATH)/frameworks/base/services/audioflinger \
	-I$(TCROOTPATH)/frameworks/base/opengl/include \
	-I$(TCROOTPATH)/external/skia/include \
	-I$(TCROOTPATH)/external/openssl/include \
	-I$(TCROOTPATH)/out/target/product/$(VOTP)/obj/include \
	-I$(TCROOTPATH)/bionic/libc/arch-arm/include \
	-I$(TCROOTPATH)/bionic/libc/include \
	-I$(TCROOTPATH)/bionic/libstdc++/include \
	-I$(TCROOTPATH)/bionic/libc/kernel/common \
	-I$(TCROOTPATH)/bionic/libc/kernel/arch-arm \
	-I$(TCROOTPATH)/bionic/libm/include \
	-I$(TCROOTPATH)/bionic/libm/include/arm \
	-I$(TCROOTPATH)/bionic/libthread_db/include \
	-I$(TCROOTPATH)/bionic/libm/arm \
	-I$(TCROOTPATH)/bionic/libm \
	-I$(TCROOTPATH)/frameworks/base/include/android_runtime 


CCTCFLAGS:=-msoft-float -mthumb-interwork -fno-exceptions -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -fmessage-length=0 -finline-functions -finline-limit=600 -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fstrict-aliasing -funswitch-loops
#-fwide-exec-charset=charset=UTF-32 

# for target exe
TELDFLAGS:=-nostdlib -Bdynamic -Wl,-T,$(TCROOTPATH)/build/core/armelf.x -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -Wl,--no-undefined -Wl,-rpath-link=$(CCTLIB) -L$(CCTLIB) 

VOTEDEPS:=$(CCTLIB)/crtbegin_dynamic.o $(CCTLIB)/crtend_android.o $(TCPATH)/lib/gcc/arm-eabi/$(GCCVER)/android/libgcc.a -lc -lm

# for target lib
CCTCRTBEGIN:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/arm-eabi-$(GCCVER)/lib/gcc/arm-eabi/$(GCCVER)/android/crtbegin.o
TLLDFLAGS:=-nostdlib -Wl,-T,$(TCROOTPATH)/build/core/armelf.xsc -Wl,--gc-sections -Wl,-shared,-Bsymbolic 

VOTLDEPS:=-L$(CCTLIB) -Wl,--no-whole-archive -Wl,--no-undefined $(TCPATH)/lib/gcc/arm-eabi/$(GCCVER)/android/libgcc.a -lm -lc


ifeq ($(VOTT), v4)
VOCFLAGS:=-mtune=arm9tdmi -march=armv4t
VOASFLAGS:=-march=armv4t -mfpu=softfpa
endif

ifeq ($(VOTT), v5)
VOCFLAGS:=-march=armv5te
VOASFLAGS:=-march=armv5te -mfpu=vfp
endif

ifeq ($(VOTT), v5x)
VOCFLAGS:=-march=iwmmxt2 -mtune=iwmmxt2 
VOASFLAGS:=-march=iwmmxt2 -mfpu=vfp
endif

ifeq ($(VOTT), v6)
#VOCFLAGS:=-march=armv6 -mtune=arm1136jf-s 
#VOASFLAGS:=-march=armv6
VOCFLAGS:=-march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -mapcs -mlong-calls
VOASFLAGS:=-march=armv6j -mcpu=arm1136jf-s -mfpu=arm1136jf-s -mfloat-abi=softfp -mapcs-float -mapcs-reentrant
endif

#
# global link options
VOLDFLAGS:=-Wl,-x,-X,--as-needed


ifeq ($(VOTT), v7)
VOCFLAGS+=-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
VOASFLAGS+=-march=armv7-a -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
VOLDFLAGS+=-Wl,-z,noexecstack -Wl,--icf=safe -Wl,--fix-cortex-a8
endif

#global compiling options for ARM target
ifneq ($(VOTT), pc)
VOASFLAGS+=--strip-local-absolute -R
endif 

VOCFLAGS+=-include $(TCROOTPATH)/system/core/include/arch/linux-arm/AndroidConfig.h

ifeq ($(VODBG), yes)
ifeq ($(VOOPTIM),unknown)
VOCFLAGS+=-D_DEBUG -g -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
else
ifeq ($(VOOPTIM),debug)
VOCFLAGS+=-DNDEBUG -O3 -D_LINUX_ANDROID -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
endif
endif
OBJDIR:=debug
else
VOCFLAGS+=-DNDEBUG -O3 -D_LINUX_ANDROID
OBJDIR:=release
endif

VOCFLAGS+=$(VOPREDEF) $(VOMM) -Wall -fsigned-char -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check   #-ftree-loop-linear -mthumb -nostdinc -dD -fprefetch-loop-arrays


ifneq ($(VOTT), pc)
VOCFLAGS+=$(CCTCFLAGS) $(CCTINC)
VOCPPFLAGS:=-fno-rtti $(VOCFLAGS)

ifeq ($(VOMT), exe)
VOLDFLAGS+=$(TELDFLAGS)
endif

ifeq ($(VOMT), lib)
VOLDFLAGS+=$(TLLDFLAGS)
endif
else
VOCPPFLAGS:=$(VOCFLAGS)
ifeq ($(VOMT), lib)
VOLDFLAGS+=-shared
endif
endif

ifeq ($(VODBG), yes)
#VOLDFLAGS:=
endif

# where to place object files 

