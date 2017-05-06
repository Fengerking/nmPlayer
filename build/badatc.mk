# 
# This configure file is just for Linux projects against Android
#

# special macro definitions for building 
VOPREDEF:=-D__BADA__

VOPRJ ?= 
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

# for detecting memory leak
VODML=
ifeq ($(VODML), yes)
VOPREDEF+=-DDMEMLEAK
endif

VOPREDEF+=-DSHP -D__VOTT_ARM__ 
TCVENDOR=bada
TCROOTPATH:="c:/bada"
TCVERSION:=2.0.4
MODULETYPE?=WaveWVGA
TCPATH:=$(TCROOTPATH)/prebuilt/linux-x86/toolchain/arm-eabi-$(GCCVER)
CCTPRE:=arm-samsung-nucleuseabi-
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

CCTLIB:=$(TCROOTPATH)/$(TCVERSION)/Model/$(MODULETYPE)/Target
CCTINC:=-I$(TCROOTPATH)/$(TCVERSION)/include \

CCTCFLAGS:=-msoft-float -mthumb-interwork -fno-exceptions -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -fmessage-length=0 -finline-functions -finline-limit=600 -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fstrict-aliasing -funswitch-loops
#-fwide-exec-charset=charset=UTF-32 

# for target exe
TELDFLAGS:=-Wl,--no-undefined -Wl,-rpath-link=$(CCTLIB) -L$(CCTLIB) 
VOTEDEPS:=$(CCTLIB)/osp_rt0.a $(CCTLIB)/FOsp.so $(TCPATH)/StubDynCast.so

# for target lib
TLLDFLAGS:=-Wl,-shared,-Bsymbolic -Bsymbolic-functions -L$(CCTLIB)
VOTLDEPS:=$(CCTLIB)/osp_rt0.a $(CCTLIB)/FOsp.so $(TCPATH)/StubDynCast.so


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
VOCFLAGS:=-march=armv6j -mtune=arm1136jf-s -mfpu=vfp -mfloat-abi=hard -mapcs -mlong-calls
VOASFLAGS:=-march=armv6j -mcpu=arm1136jf-s -mfpu=arm1136jf-s -mfloat-abi=hard -mapcs-float -mapcs-reentrant
endif

#VOCFLAGS+=-DDXLNK=1
#
# global link options
VOLDFLAGS:=-Wl,-x,-X,--as-needed


ifeq ($(VOTT), v7)
VOCFLAGS+=-march=armv7-a -mtune=cortex-a8 -marm -mthumb-interwork -mfpu=neon -mfloat-abi=hard 
VOASFLAGS+=-march=armv7-a -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard
VOLDFLAGS+=-Wl,--fix-cortex-a8
endif

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

VOCFLAGS+=$(VOPREDEF) $(VOMM) -Wall -fsigned-char -fshort-wchar -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check 

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

