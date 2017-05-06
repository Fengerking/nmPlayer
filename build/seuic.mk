# 
# This configure file is just for Linux projects against Android
#

# special macro definitions for building 
VOPREDEF:=-DLINUX -D_LINUX -DANDROID -DSEUIC

VOPRJ ?= 
VONJ ?= gingerbread
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

VOPREDEF+=-D__VOTT_ARM__ -D__VONJ_GINGERBREAD__
TCVENDOR=seuic
TCROOTPATH:=/opt/nj23_seuic_tiger-db
GCCVER:=4.4.2
TCPATH:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/uc4-1.0-beta-hard-RHELAS4
CCTPRE:=$(TCPATH)/bin/unicore32-linux-
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
VOTP:=tiger_db
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
	-I$(TCROOTPATH)/frameworks/base/libs/audioflinger \
	-I$(TCROOTPATH)/frameworks/base/opengl/include \
	-I$(TCROOTPATH)/external/skia/include \
	-I$(TCROOTPATH)/external/openssl/include \
	-I$(TCROOTPATH)/out/target/product/$(VOTP)/obj/include \
	-I$(TCROOTPATH)/bionic/libc/arch-unicore32/include \
	-I$(TCROOTPATH)/bionic/libc/include \
	-I$(TCROOTPATH)/bionic/libstdc++/include \
	-I$(TCROOTPATH)/bionic/libc/kernel/common \
	-I$(TCROOTPATH)/bionic/libc/kernel/arch-unicore32 \
	-I$(TCROOTPATH)/bionic/libm/include \
	-I$(TCROOTPATH)/bionic/libm/include/unicore32 \
	-I$(TCROOTPATH)/bionic/libthread_db/include \
	-include $(TCROOTPATH)/system/core/include/arch/linux-unicore32/AndroidConfig.h \
	-I$(TCROOTPATH)/system/core/include/arch/linux-unicore32 \
	-I$(TCROOTPATH)/frameworks/base/include/android_runtime 

CCTCFLAGS:=-Wno-multichar -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -mhard-float -fno-exceptions -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -fmessage-length=0 -finline-limit=300 -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fno-strict-aliasing -funswitch-loops 
#-fwide-exec-charset=charset=UTF-32 

# for target exe
TELDFLAGS:=-nostdlib -Bdynamic -Wl,-T,$(TCROOTPATH)/build/core/unicore32elf.x -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,--hash-style=both -Wl,-z,nocopyreloc -Wl,--no-undefined -Wl,-rpath-link=$(CCTLIB) -L$(CCTLIB) 

VOTEDEPS:=$(CCTLIB)/crtbegin_dynamic.o $(CCTLIB)/crtend_android.o $(TCPATH)/lib/gcc/unicore32-linux/$(GCCVER)/libgcc.a -lc -lm

# for target lib
CCTCRTBEGIN:=$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib/crtbegin_com.o
CCTCRTEND:=$(TCROOTPATH)/out/target/product/$(VOTP)/obj/lib/crtend_com.o
TLLDFLAGS:=-nostdlib -Wl,-T,$(TCROOTPATH)/build/core/unicore32elf.xsc -Wl,--gc-sections -Wl,--hash-style=both -Wl,-shared,-Bsymbolic 

VOTLDEPS:=-L$(CCTLIB) -Wl,--no-whole-archive -Wl,--no-undefined $(TCPATH)/lib/gcc/unicore32-linux/$(GCCVER)/libgcc.a $(CCTCRTEND) -lm -lc


ifeq ($(VOTT), v6)
VOCFLAGS:=#-march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -mapcs -mtpcs-leaf-frame -mlong-calls
VOASFLAGS:=#-march=armv6j -mcpu=arm1136jf-s -mfpu=arm1136jf-s -mfloat-abi=softfp -mapcs-float -mapcs-reentrant
endif

#
# global link options
VOLDFLAGS:=-Wl,-x,-X,--as-needed


#global compiling options for ARM target
ifneq ($(VOTT), pc)
VOASFLAGS+=--strip-local-absolute -R
endif 


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

VOCFLAGS+=$(VOPREDEF) $(VOMM) -Wall -fsigned-char -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check   #-ftree-loop-linear -mthumb -nostdinc -dD -fprefetch-loop-arrays


ifneq ($(VOTT), pc)
VOCFLAGS+=$(CCTCFLAGS) $(CCTINC)
VOCPPFLAGS:=-fno-rtti -Wsign-promo $(VOCFLAGS)

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

