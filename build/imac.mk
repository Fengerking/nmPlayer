# 
# The building template for Mac OS 
# 
# David@2012/12/23
#

# special macro definitions for building 
VOPREDEF:=-D_MAC_OS

VOPRJ?= 
VONJ?= 
VOTT?=x86
 

VOPREBUILTPATH?=unknown
ifeq ($(VOPREBUILTPATH),unknown)
VOPREBUILTPATH:=$(VOTOP)/Lib/Customer/google/eclair/v6
endif



# for debug or not: yes for debug, any other for release
VODBG?=ye
VOOPTIM?=unknown
ifeq ($(VOOPTIM),debug)
IDBG:=yes
VODBG:=no
else
ifeq ($(VOOPTIM),release)
IDBG:=no
VODBG:=no
endif
endif

# just for IOS & IMac debug web build
IDBG?=ye

# for turn gas-preprocessor on or off
VOGAS?=ye

OBJL_MACROS:="-DIBOutletCollection(ClassName)=__attribute__((iboutletcollection(ClassName)))" "-DIBAction=void)__attribute__((ibaction)" 
#"-DIBOutlet=__attribute__((iboutlet))" 

DEVELOPER_PATH:=$(shell xcode-select -print-path)

SDKROOTTemp:=$(DEVELOPER_PATH)/Platforms/MacOSX.platform/Developer/SDKs/

ifeq ($(wildcard $(SDKROOTTemp)),)
SDKROOTTemp:=$(DEVELOPER_PATH)/SDKs/
endif

MAC_SDK_VERSION:=$(shell ls $(SDKROOTTemp) | sort -nr | head -1)
SDKROOT:=$(SDKROOTTemp)$(MAC_SDK_VERSION)

DEVROOT:=$(DEVELOPER_PATH)/Toolchains/XcodeDefault.xctoolchain

SYSBIN:=$(DEVROOT)/usr/bin
OPTBIN:=/opt/local/bin
ifeq ($(wildcard $(DEVROOT)),)
DEVROOT:=$(SDKROOT)
SYSBIN:=$(DEVELOPER_PATH)/usr/bin
endif

SYSINC:=$(DEVROOT)/usr/include

VO_BUILD := $(abspath $(dir $(abspath $(MAKEFILE))))
ifeq ($(wildcard $(DEVPATH)),)
DEVPATH:=$(abspath $(shell xcodebuild -find-executable xcodebuild))
endif

CCTPRE:=$(SYSBIN)/
AS:=$(CCTPRE)as
CC:=$(CCTPRE)clang
GG:=$(CCTPRE)clang++
LIBTOOL:=$(CCTPRE)libtool
ASM:=/opt/local/bin/yasm

ifeq ($(VOTT), x86)
ARCHX:=-arch i386
else
ARCHX:=-arch x86_64
endif

MIN_MACOSX:=-mmacosx-version-min=10.6 
MAC_CMNFLAGS:=-fmessage-length=0 -Wno-trigraphs -fpascal-strings -Wno-missing-field-initializers -Wno-missing-prototypes -Wreturn-type -Wformat -Wno-missing-braces -Wparentheses -Wswitch -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wshorten-64-to-32 -Wno-newline-eof -isysroot $(SDKROOT) -fasm-blocks -Wdeprecated-declarations $(MIN_MACOSX) -Wno-sign-conversion -I$(SYSINC) $(VOPREDEF) $(VOMM)

MAC_CMNCFLAGS:=-std=gnu99 -Wpointer-sign 
MAC_CMNCPPFLAGS:=-Wno-non-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-c++11-extensions -Winvalid-offsetof -fvisibility-inlines-hidden 
#-std=gnu++11

ifeq ($(VODBG), yes)
MAC_CMNFLAGS+=-g -O0
OBJDIR:=debug
else
MAC_CMNFLAGS+=-O3
OBJDIR:=release
endif

ifeq ($(IDBG), yes)
OBJDIR:=debug
endif

ifeq ($(IDBG), no)
OBJDIR:=release
endif

# only for objective language
MAC_CMNOBJLFLAGS:=-Wno-implicit-atomic-properties -Wno-receiver-is-weak -Wduplicate-method-match -Wno-selector -Wno-strict-selector-match -Wno-undeclared-selector -Wno-deprecated-implementations -Wprotocol 


# X86 cflags
MAC_CFLAGS:=-x c $(ARCHX) $(MAC_CMNCFLAGS) $(MAC_CMNFLAGS)

# X86 C++ flags
MAC_CPPFLAGS:= -x c++ $(ARCHX) $(MAC_CMNCPPFLAGS) $(MAC_CMNFLAGS) 


# X86 objective-c flags
MAC_OBJCFLAGS:=-x objective-c $(ARCHX) $(MAC_CMNCFLAGS) $(OBJL_MACROS) $(MAC_CMNOBJLFLAGS) $(MAC_CMNFLAGS) 

# X86 objective-c++ flags
MAC_OBJCPPFLAGS:=-x objective-c++ $(ARCHX) $(MAC_CMNCPPFLAGS) -Wno-arc-abi $(OBJL_MACROS) $(MAC_CMNOBJLFLAGS) $(MAC_CMNFLAGS) 

# global link options
ifeq ($(VOMT), lib)
VOLDFLAGS:=$(ARCHX) -dynamiclib -isysroot $(SDKROOT) $(MIN_MACOSX) -fobjc-link-runtime -framework Cocoa -single_module -compatibility_version 1 -current_version 1 
endif

ifeq ($(VOMT), static_lib)

ifeq ($(VOTT), x86)
ARCHX_LINK:=-arch_only i386
else
ARCHX_LINK:=-arch_only x86_64
endif

VOLDFLAGS:=$(ARCHX_LINK) -syslibroot $(SDKROOT) -framework Cocoa 
endif

#-install_name /usr/local/lib/voTestLib.dylib 

