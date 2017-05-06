# 
# This configure file is just for Linux projects against Android
#

# special macro definitions for building 
VOPREDEF:=-D_IOS
# platform type
VOPT:=2

VOPRJ ?= 
VONJ ?= 
VOTT ?= v7
 

VOPREBUILTPATH?=unknown
ifeq ($(VOPREBUILTPATH),unknown)
VOPREBUILTPATH:=$(VOTOP)/Lib/Customer/google/eclair/v6
endif


# control the version to release out
# available: eva(evaluation), rel(release)
VOVER:=
ifeq ($(VOVER), eva)
VOPREDEF+=-D__VOVER_EVA__
endif

# for debug or not: yes for debug, any other for release
VODBG?=ye
VOOPTIM?=unknown

# just for IOS & IMac debug web build
IDBG?=undef

ifeq ($(VOOPTIM),debug)
IDBG:=yes
VODBG:=no
else
ifeq ($(VOOPTIM),release)
IDBG:=no
VODBG:=no
endif
endif


# for turn gas-preprocessor on or off
VOGAS?=ye

# for detecting memory leak
VODML:=
ifeq ($(VODML), yes)
VOPREDEF+=-DDMEMLEAK
endif

ifeq ($(VOTT), i386)
VOTT:=pc
endif

ifneq ($(VOTT), pc)
VOPREDEF+=-D__VOTT_ARM__ 
endif

OBJC_MFLAGS:=\
-Wno-selector -Wno-strict-selector-match -Wno-undeclared-selector \
-Wprotocol \
"-DIBOutlet=__attribute__((iboutlet))" "-DIBOutletCollection(ClassName)=__attribute__((iboutletcollection(ClassName)))" "-DIBAction=void)__attribute__((ibaction)" 
#CLANG -Wduplicate-method-match  -Wno-deprecated-implementations

DEVELOPER_PATH:=$(shell xcode-select -print-path)

ifeq ($(wildcard $(DEVELOPER_PATH)),)
DEVELOPER_PATH?=/Developer-3.2.6
endif

ifeq ($(wildcard $(DEVELOPER_PATH)),)
DEVELOPER_PATH:=/Developer
endif

PLATFORMS:= $(DEVELOPER_PATH)/Platforms

ifneq ($(VOTT), pc)
IOS_PLATFORM:=  $(PLATFORMS)/iPhoneOS.platform
IOS_SDK:=$(shell ls $(IOS_PLATFORM)/Developer/SDKs/ | sort -nr | head -1)
IOS_ROOT:=$(IOS_PLATFORM)/Developer/SDKs/$(IOS_SDK)

IOS_CFLAGS:=    -isysroot "$(IOS_ROOT)" -miphoneos-version-min=4.3
IOS_LFLAGS:=    -syslibroot $(IOS_ROOT) 

else

IOS_PLATFORM:=  $(PLATFORMS)/iPhoneSimulator.platform
IOS_SDK:=$(shell ls $(IOS_PLATFORM)/Developer/SDKs/ | awk -F " " '{print $0}')
IOS_ROOT:=$(IOS_PLATFORM)/Developer/SDKs/$(IOS_SDK)

IOS_CFLAGS:=    -isysroot "$(IOS_ROOT)" -mmacosx-version-min=10.5 -D__IPHONE_OS_VERSION_MIN_REQUIRED=30000 -fexceptions -fasm-blocks
#CLANG -mios-simulator-version-min=4.3

IOS_LFLAGS:=    -syslibroot $(IOS_ROOT)
OBJC_MFLAGS+=   -fobjc-abi-version=2 -fobjc-legacy-dispatch 
endif

ifeq ($(VODBG), yes)
IOS_CFLAGS+=-g -O0
OBJDIR:=debug
else
IOS_CFLAGS+=-Os
OBJDIR:=release
endif

ifeq ($(IDBG), yes)
OBJDIR:=debug
endif

ifeq ($(IDBG), no)
OBJDIR:=release
endif

ifeq ($(VOTT), v7s)

ifeq ($(VODBG), yes)
OBJDIR:=debug/v7s
else
OBJDIR:=release/v7s
endif

ifeq ($(IDBG), yes)
OBJDIR:=debug/v7s
endif

ifeq ($(IDBG), no)
OBJDIR:=release/v7s
endif

endif

CLANG_PRE:=$(DEVELOPER_PATH)/Toolchains/XcodeDefault.xctoolchain/usr/bin/
CCTPRE:=$(IOS_PLATFORM)/Developer/usr/bin/

ifeq ($(wildcard $(CCTPRE)gcc),)
CCTPRE:=$(DEVELOPER_PATH)/usr/bin/
endif

ifeq ($(wildcard $(CLANG_PRE)),)
CLANG_PRE:=$(CCTPRE)
endif

AS:=$(CCTPRE)as
AR:=$(CCTPRE)ar
NM:=$(CCTPRE)nm
CLANG:=$(CLANG_PRE)clang
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
LIBTOOL:=$(CLANG_PRE)libtool

VOTLDEPS:=-lm -lc

ifeq ($(VOTT), pc)
VOCFLAGS:=-arch i386 
IOS_LFLAGS+=-arch_only i386
endif

ifeq ($(VOTT), v6)
VOCFLAGS:=-arch armv6
IOS_LFLAGS+=-arch_only armv6
endif

ifeq ($(VOTT), v7)
VOCFLAGS:=-arch armv7
IOS_LFLAGS+=-arch_only armv7
endif

ifeq ($(VOTT), v7s)
VOCFLAGS:=-arch armv7s
IOS_LFLAGS+=-arch_only armv7s
endif


VOCFLAGS+=-fmessage-length=0 \
-Wno-trigraphs -fpascal-strings -Os -Wno-missing-field-initializers -Wno-missing-prototypes -Wreturn-type \
-Wformat -Wno-missing-braces -Wparentheses -Wswitch -Wno-unused-function -Wno-unused-label \
-Wno-unused-parameter -Wunused-variable -Wunused-value \
-Wempty-body \
-Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion \
-Wno-shorten-64-to-32 \
-Wno-newline-eof \
-Wdeprecated-declarations \
-mfloat-abi=soft \
#-Wno-sign-conversion \
#CLANG -Wuninitialized


#global compiling options for ARM target

VOCFLAGS+=$(IOS_CFLAGS) $(VOPREDEF) $(VOMM)

ifneq ($(VOTT), pc)
VOASFLAGS:=$(VOCFLAGS) -no-integrated-as
#2012/3/8 Jeff: delete -fno-rtti as Xcode(Link static library error)
endif

VOCPPFLAGS:=$(VOCFLAGS)
VOCPPFLAGS+=-Wno-non-virtual-dtor -Wno-overloaded-virtual
#CLANG -Wno-exit-time-destructors \
#CLANG -Wno-c++11-extensions

VOCFLAGS+=-Wpointer-sign
#VOCFLAGS+=-std=c99 

OBJC_MMFLAGS:=$(OBJC_MFLAGS) 
#CLANG -Wno-arc-abi


