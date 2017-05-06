# 
# This configure file is just for Linux projects relative with Android
#

# special macro definitions for building 
VOPREDEF:=-DSYMBIAN -DNNJ
VONJ ?= linux
VOPRJ ?= arkamys

VOTT ?= v4
 

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
VOVER=rel
ifeq ($(VOVER), eva)
VOPREDEF+=-D__VOVER_EVA__
endif


VOPREDEF+=-D__VOTT_ARM__
TCROOTPATH:=/opt/symbian
TCPATH:=$(TCROOTPATH)
CCTPRE:=$(TCPATH)/bin/arm-none-symbianelf-
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
CCTLIB:=$(TCROOTPATH)/lib
CCTINC:=-I$(TCROOTPATH)

CCTCFLAGS:=-Wall -DTI -pipe -mapcs -nostdinc

# for target exe
TELDFLAGS:=-rpath-link=$(CCTLIB) #-z nocopyreloc #-L$(CCTLIB) -nostdlib #-Wl,-dynamic-linker,/system/bin/linker --no-undefined 
VOTEDEPS:=

# for target lib
TLLDFLAGS:=-nostdlib -Wl,-shared,-Bsymbolic -L$(CCTLIB) -Wl,--no-whole-archive -Wl,--no-undefined $(TCPATH)/lib/gcc/arm-none-symbianelf/3.4.3/libgcc.a #-Wl,--whole-archive

VOTLDEPS:=

ifeq ($(VOTT), v4)
#VOCFLAGS:=-mtune=arm9tdmi -march=armv4t
VOCFLAGS:=-mtune=arm926ej-s -march=armv4t
VOASFLAGS:=-march=armv4t -mcpu=arm926ej-s -mfpu=softfpa
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
VOASFLAGS:=-march=armv6 -mfpu=softfpa
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

VOCFLAGS+=$(VOPREDEF) $(VOMM) #-Wall -fsigned-char -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check  ###-ftree-loop-linear  -mthumb -nostdinc  -dD -fprefetch-loop-arrays


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

