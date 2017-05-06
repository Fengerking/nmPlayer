# 
# This configure file is just for Linux projects against Android
#

# special macro definitions for building 
VOPREDEF:=-DLINUX -D_LINUX -DNNJ

VONJ ?= _LINUX_X86
VOPRJ ?= x86

VOTT ?= pc
 

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

VOPREDEF+=-D__VOTT_PC__
CC:=gcc
GG:=g++
AR:=ar
LD:=ld
SIZE:=size
STRIP:=strip
RANLIB:=true
OBJCOPY:=objcopy
OBJDUMP:=objdump
READELF:=readelf
STRINGS:=strings
VOTEDEPS:=
VOTLDEPS:=


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
ifneq ($(VOTT), pc)
VOASFLAGS+=--strip-local-absolute -R
endif 

# global link options
VOLDFLAGS:=-Wl,-x,-X,--as-needed,--sort-common,--sort-section=alignment #,--oformat=elf32-i386 #elf_i386
#VOLDFLAGS:=-x -X --as-needed --sort-common --sort-section=alignment --oformat=elf32-i386 

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

VOCFLAGS+=$(VOPREDEF) $(VOMM) -m32 -Wall -fsigned-char -fomit-frame-pointer -fno-leading-underscore -fpic -fPIC -pipe -ftracer -fforce-addr -fno-bounds-check  ###-ftree-loop-linear  -mthumb -nostdinc  -dD -fprefetch-loop-arrays


VOCPPFLAGS:=$(VOCFLAGS)
ifeq ($(VOMT), lib)
VOLDFLAGS+=-shared
endif

ifeq ($(VODBG), yes)
#VOLDFLAGS:=
endif

VOLDFLAGS+=-Wl,-melf_i386 #-oformat=elf32-little #elf32-i386
#VOLDFLAGS+=--oformat=elf32_i386

# where to place object files 

