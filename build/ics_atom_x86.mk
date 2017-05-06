# 
# This configure file is just for Linux projects against Android
#

# special macro definitions for building 
VOPREDEF:=-D__ANDROID__ -DANDROID -Ulinux

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
VOBRANCH?=trunk


# for detecting memory leak
VODML=
ifeq ($(VODML), yes)
VOPREDEF+=-DDMEMLEAK
endif

VOPREDEF+=-D__VOTT_X86__ -D__VONJ_ICS__
TCROOTPATH:=/opt/ics_atom_x86
GCCVER:=4.4.3
TCPATH:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/i686-android-linux-$(GCCVER)
CCTPRE:=$(TCPATH)/bin/i686-android-linux-
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
VOTP:=generic_x86
TARGETOUT:=out/target/product/$(VOTP)/obj
CCTLIB:=$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib
CCTINC:=-I$(TCROOTPATH)/dalvik/libnativehelper/include/nativehelper \
		-I$(TCROOTPATH)/system/core/include \
		-I$(TCROOTPATH)/hardware/libhardware/include \
		-I$(TCROOTPATH)/hardware/libhardware_legacy/include \
		-I$(TCROOTPATH)/hardware/ril/include \
		-I$(TCROOTPATH)/dalvik/libnativehelper/include \
		-I$(TCROOTPATH)/frameworks/base/include \
		-I$(TCROOTPATH)/frameworks/base/opengl/include \
		-I$(TCROOTPATH)/frameworks/base/native/include \
		-I$(TCROOTPATH)/external/skia/include \
		-I$(TCROOTPATH)/$(TARGETOUT)/include \
		-I$(TCROOTPATH)/bionic/libc/arch-x86/include \
		-I$(TCROOTPATH)/bionic/libc/include \
		-I$(TCROOTPATH)/bionic/libstdc++/include \
		-I$(TCROOTPATH)/bionic/libc/kernel/common \
		-I$(TCROOTPATH)/bionic/libc/kernel/arch-x86 \
		-I$(TCROOTPATH)/bionic/libm/include \
		-I$(TCROOTPATH)/bionic/libm/include/i387 \
		-I$(TCROOTPATH)/bionic/libthread_db/include

CCTCFLAGS:=-O2 -Ulinux -Wa,--noexecstack -Wstrict-aliasing=2 -fPIC -ffunction-sections -finline-functions -finline-limit=300 -fno-inline-functions-called-once -fno-short-enums -fstrict-aliasing -funswitch-loops -funwind-tables -march=atom -mstackrealign -DUSE_SSSE3 -DUSE_SSE2 -mfpmath=sse -mbionic -D__ANDROID__ -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -O2 -g -fno-strict-aliasing -DNDEBUG -UDEBUG -fno-use-cxa-atexit -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Wsign-promo -fno-rtti -DANDROID_TARGET_BUILD -finline-limit=64 -finline-functions -fno-inline-functions-called-once -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -O2 -fomit-frame-pointer -Wall -W -Wno-unused-parameter -Wwrite-strings  -fno-exceptions   -fno-rtti -Woverloaded-virtual -Wno-sign-promo

VOLDFLAGS:=-m32 -Wl,-z,noexecstack -Wl,--gc-sections -nostdlib -Wl,-soname,libmtp.so -shared -Bsymbolic -fno-exceptions -Wno-multichar -O2 -Ulinux -Wa,--noexecstack -Werror=format-security -Wstrict-aliasing=2 -fPIC -ffunction-sections -finline-functions -finline-limit=300 -fno-inline-functions-called-once -fno-short-enums -fstrict-aliasing -funswitch-loops -funwind-tables -include $(TCROOTPATH)system/core/include/arch/target_linux-x86/AndroidConfig.h -march=atom -mstackrealign -DUSE_SSSE3 -DUSE_SSE2 -mfpmath=sse -mbionic -D__ANDROID__ -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -O2 -g -fno-strict-aliasing -DNDEBUG -UDEBUG -Wl,--whole-archive   -Wl,--no-whole-archive 

LOCAL_PRELINK_MODULE := false
LIBGCCA:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/i686-android-linux-4.4.3/lib/gcc/i686-android-linux/4.4.3/libgcc.a

# for target exe
TELDFLAGS:=-m32 -Wl,-z,noexecstack -Wl,--gc-sections -nostdlib -Bdynamic -Wl,-dynamic-linker,/system/bin/linker -Wl,-z,nocopyreloc

VOTEDEPS:=$(TCROOTPATH)/out/target/product/generic_x86/obj/lib/crtbegin_dynamic.o $(LIBGCCA) $(TCROOTPATH)/out/target/product/generic_x86/obj/lib/crtend_android.o -lc -lm


# for target lib
CCTCRTBEGIN:=-L$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib/ $(TCROOTPATH)/out/target/product/generic_x86/obj/lib/crtbegin_so.o
#TLLDFLAGS:=-Wl,-shared,-Bsymbolic -m32

VOTLDEPS:=-L$(CCTLIB) -L$(CCTLIB) -lm -lc $(LIBGCCA) $(TCROOTPATH)/out/target/product/generic_x86/obj/lib/crtend_so.o



ifeq ($(VOTT), x86)
VOCFLAGS:=-march=i686 -mbionic
VOASFLAGS:=-march=i686 -mbionic
endif

#
# global link options

#global compiling options for ARM target
ifneq ($(VOTT), pc)
VOASFLAGS+=--strip-local-absolute -R
endif 


ifeq ($(VODBG), yes)
VOCFLAGS+=-DDEBUG -g
else
VOCFLAGS+=-DNDEBUG -UDEBUG -O3
endif

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

VOCFLAGS+=$(VOPREDEF) $(VOMM) #-Wall -fsigned-char -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check   #-ftree-loop-linear -mthumb -nostdinc -dD -fprefetch-loop-arrays


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
VOCFLAGS+=$(CCTCFLAGS) $(CCTINC)
VOCPPFLAGS:=$(VOCFLAGS)
ifeq ($(VOMT), lib)
VOLDFLAGS+=-shared
endif
endif

ifeq ($(VODBG), yes)
#VOLDFLAGS:=
endif

# where to place object files 

