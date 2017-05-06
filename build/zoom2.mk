# 
# This configure file is just for Linux projects relative with Android
#

# special macro definitions for building 
VOPREDEF:=-DLINUX -D_LINUX -D__VOPRJ_ZM2__

VOPRJ ?= ti.zoom2
VONJ ?= eclair
VOTT ?= v7
 

VOPREBUILTPATH?=unknown
ifeq ($(VOPREBUILTPATH),unknown)
VOPREBUILTPATH:=$(VOTOP)/Lib/Customer/google/eclair/v6
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

# control the version to release out
# available: eva(evaluation), rel(release)
VOVER=
ifeq ($(VOVER), eva)
VOPREDEF+=-D__VOVER_EVA__
endif


VOPREDEF+=-D__VOTT_ARM__
TCROOTPATH:=/opt/zoom2
TCPATH:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0
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
VOTP:=zoom2
CCTLIB:=$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib
CCTINC:=-I$(TCROOTPATH)/system/core/include \
	-I$(TCROOTPATH)/hardware/libhardware/include \
	-I$(TCROOTPATH)/hardware/ril/include \
	-I$(TCROOTPATH)/dalvik/libnativehelper/include \
	-I$(TCROOTPATH)/frameworks/base/include \
	-I$(TCROOTPATH)/external/skia/include \
	-I$(TCROOTPATH)/out/target/product/$(VOTP)/obj/include \
	-I$(TCROOTPATH)/bionic/libc/arch-arm/include \
	-I$(TCROOTPATH)/bionic/libc/include \
	-I$(TCROOTPATH)/bionic/libstdc++/include \
	-I$(TCROOTPATH)/bionic/libc/kernel/common \
	-I$(TCROOTPATH)/bionic/libc/kernel/arch-arm \
	-I$(TCROOTPATH)/bionic/libm/include \
	-I$(TCROOTPATH)/bionic/libm/include/arch/arm \
	-I$(TCROOTPATH)/bionic/libthread_db/include \
	-I$(TCROOTPATH)/bionic/libm/arm \
	-I$(TCROOTPATH)/bionic/libm \
	-I$(TCROOTPATH)/hardware/ti/omap3/liboverlay

CCTCFLAGS:=-msoft-float -mthumb-interwork -fno-exceptions -ffunction-sections -funwind-tables -fstack-protector -fmessage-length=0 -finline-functions -finline-limit=600 -fno-short-enums -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fstrict-aliasing -funswitch-loops
#-fwide-exec-charset=charset=UTF-32 

# for target exe
TELDFLAGS:=-Bdynamic -Wl,-T,$(TCROOTPATH)/build/core/armelf.x -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -Wl,--no-undefined -Wl,-rpath-link=$(CCTLIB) -L$(CCTLIB) -nostdlib

VOTEDEPS:=$(CCTLIB)/crtend_android.o $(CCTLIB)/crtbegin_dynamic.o $(TCPATH)/lib/gcc/arm-eabi/4.4.0/interwork/libgcc.a -lc -lm

# for target lib
TLLDFLAGS:=-nostdlib -Wl,-T,$(TCROOTPATH)/build/core/armelf.xsc -Wl,--gc-sections -Wl,-shared,-Bsymbolic -L$(CCTLIB) -Wl,--no-whole-archive -Wl,--no-undefined $(TCPATH)/lib/gcc/arm-eabi/4.4.0/interwork/libgcc.a #-Wl,--whole-archive

VOTLDEPS:=-lm -lc


ifeq ($(VOTT), v4)
VOCFLAGS:=-mtune=arm9tdmi -march=armv4t
VOASFLAGS:=-march=armv4t -mfpu=softfpa
endif

ifeq ($(VOTT), v5)
VOCFLAGS:=-march=armv5te
VOASFLAGS:=-march=armv5te -mfpu=vfp
endif

ifeq ($(VOTT), v5x)
VOCFLAGS:=-march=armv5te -mtune=xscale
VOASFLAGS:=-march=armv5te -mfpu=vfp
endif

ifeq ($(VOTT), v6)
VOCFLAGS:=-march=armv6 -mtune=arm1136jf-s 
VOASFLAGS:=-march=armv6
endif

ifeq ($(VOTT), v7)
VOCFLAGS+=-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp #-ftree-vectorize 
VOASFLAGS+=-march=armv7-a -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
endif

# audio render selection
# available: alsa, oss, msm
VOAR ?= msm

ifeq ($(VOAR), alsa)
VOPREDEF+=-D__VOAR_ALSA__
endif
ifeq ($(VOAR), oss)
VOPREDEF+=-D__VOAR_OSS__
endif
ifeq ($(VOAR), msm)
VOPREDEF+=-D__VOAR_MSM__
endif


# video render selection
# available: sdl, fb, dfb, qt, tx(tinyX), v4l2, sf(for android platform)

VOVR ?= sf

ifeq ($(VOVR), fb)
   VOPREDEF+=-D__VOVR_FB__
endif

ifeq ($(VOVR), v4l2)
  VOPREDEF+=-D__VOVR_V4L2__
endif

ifeq ($(VOVR), sf)
  VOPREDEF+=-D__VOVR_SURFACE__
endif


#global compiling options for ARM target
VOASFLAGS+=--strip-local-absolute -R

# global link options
VOLDFLAGS:=-Wl,-x,-X,--as-needed

ifeq ($(VODBG), yes)
ifeq ($(VOOPTIM),unknown)
VOCFLAGS+=-D_DEBUG -g -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
else
ifeq ($(VOOPTIM),debug)
VOCFLAGS+=-DNDEBUG -O3 -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
endif
endif
OBJDIR:=debug
else
VOCFLAGS+=-DNDEBUG -O3
OBJDIR:=release
endif

VOCFLAGS+=$(VOPREDEF) $(VOMM) -Wall -fsigned-char -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check  ###-ftree-loop-linear  -mthumb -nostdinc  -dD -fprefetch-loop-arrays


VOCFLAGS+=$(CCTCFLAGS) $(CCTINC)
VOCPPFLAGS:=-fno-rtti $(VOCFLAGS)

ifeq ($(VOMT), exe)
VOLDFLAGS+=$(TELDFLAGS)
endif

ifeq ($(VOMT), lib)
VOLDFLAGS+=$(TLLDFLAGS)
endif

ifeq ($(VODBG), yes)
#VOLDFLAGS:=
endif

# where to place object files 

