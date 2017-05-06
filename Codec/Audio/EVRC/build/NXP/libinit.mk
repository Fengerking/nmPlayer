#*****************************************************************************
#
# Common Library Makefile
#
#*****************************************************************************

ifeq ($(BUILD),ARM12LERVDS)
    TGT_OS=LERVDS
    TGT_ENDIAN=LITTLE
    TGT_CPU=ARM12
endif

ifeq ($(BUILD),ARM11LERVDS)
    TGT_OS=LERVDS
    TGT_ENDIAN=LITTLE
    TGT_CPU=ARM11
endif

ifeq ($(BUILD),ARM9LERVDS)
    TGT_OS=LERVDS
    TGT_ENDIAN=LITTLE
    TGT_CPU=ARM9
endif

#*****************************************************************************
#Library Name Defines
#*****************************************************************************
ifeq ($(TGT_OS),LERVDS)
	TGT_OS_BIN	= lervds
endif

ifeq ($(TGT_CPU),ARM11)
	TGT_CPU_BIN	= arm11
endif

ifeq ($(TGT_CPU),ARM12)
        TGT_CPU_BIN     = arm12
endif

ifeq ($(TGT_CPU),ARM9)
        TGT_CPU_BIN     = arm9
endif

CC  = $(TOOLS_DIR)armcc
AS  = $(TOOLS_DIR)armasm
LD  = $(TOOLS_DIR)armlink
AR  = $(TOOLS_DIR)armar

ifeq ($(TGT_CPU),ARM9)
    CFLAGS_CPU          = --cpu ARM9E-S         
endif

ifeq ($(TGT_CPU),ARM11)
    CFLAGS_CPU          = --cpu ARM1136J-S
endif

ifeq ($(TGT_CPU),ARM12)
    CFLAGS_CPU          = --cpu Cortex-A8
endif

AFLAGS_CPU      = $(CFLAGS_CPU)

CFLAGS_ENDIAN_LITTLE    = --littleend
CFLAGS_ENDIAN_BIG       = --bigend
AFLAGS_ENDIAN_LITTLE    = $(CFLAGS_ENDIAN_LITTLE)
AFLAGS_ENDIAN_BIG       = $(CFLAGS_ENDIAN_BIG)


LIB_ARGS                = --create
AFLAGS =--littleend --keep --apcs=inter  


#*****************************************************************************
#Set the Common Defines.
#*****************************************************************************
CFLAGS =  $(CFLAGS_CPU) $(CFLAGS_ENDIAN_$(TGT_ENDIAN)) \
                   -DTGT_CPU_$(TGT_CPU) -DTGT_ENDIAN_$(TGT_ENDIAN) -DTGT_OS_$(TGT_OS)
CPPFLAGS +=  $(CFLAGS_CPU) $(CFLAGS_ENDIAN_$(TGT_ENDIAN)) \
                   -DTGT_CPU_$(TGT_CPU) -DTGT_ENDIAN_$(TGT_ENDIAN) -DTGT_OS_$(TGT_OS)
AFLAGS +=  $(AFLAGS_CPU) $(AFLAGS_ENDIAN_$(TGT_ENDIAN))


PROGRAM     = $(CODEC)
LIBRARY = volib$(PROGRAM)

