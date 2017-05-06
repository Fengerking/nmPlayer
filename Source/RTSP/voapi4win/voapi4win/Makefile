include ../config.mak
include ../comhead.mk

VO_INCS+=-I./ 

ifeq ($(BUILD_ADI_DSP), yes)
    VO_INCS+=-I/home/gtxia/cross_project/uClinux-distribution-PKG/uClinux-distribution/uClinux-dist/uClibc/include
endif
LDFLAGS+=-lpthread -ldl

ifeq ($(BUILD_TARGET_X86), yes)
  BUILD_DIR = ../build/x86/
endif

ifeq ($(BUILD_TARGET_ARM), yes)
  BUILD_DIR =../build/arm/
  VO_CFLAGS +=-DARM
endif

VO_CC+=$(GG)

CFLAGS=$(VO_CFLAGS) $(VO_INCS) 

SUF=cpp
SRCS=$(wildcard *.cpp) 
OBJS=$(SRCS:.$(SUF)=.o)

LIB_NAME=$(VOPREBUILTPATH)/libvocrsapis
LIB_STATIC=$(LIB_NAME).a
LIB_DYNAMIC=$(LIB_NAME).so

IS_LINK=no
DUMP_MSG=ye

include ../comtail.mk

