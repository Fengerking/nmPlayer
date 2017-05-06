#*****************************************************************************
# Release QCELP decoder RVDS .a to NXP
# QCELPDec Library Makefile
#
#*****************************************************************************
CODEC = QCELPDec
include $(VOTOP)/build/NXP/libinit.mk

# Source directory of 'C' code
CSRCS_DIR = c_src

# Source directory of 'assembly(.s)' code
ASRCS_DIR = asm

# Library directory for codec libraries
LIB_DIR = .

#Define the include path
INCLUDES    = -I../../../../../Common
INCLUDES   += -I../../../../../Include
INCLUDES   += -I../inc

OPTIM_LEVEL             = -O3
OPTIM_TYPE              = -Otime

CFLAGS +=  $(CFLAGS_CPU) $(CFLAGS_ENDIAN_$(TGT_ENDIAN)) \
                   -DTGT_CPU_$(TGT_CPU) -DTGT_ENDIAN_$(TGT_ENDIAN) -DTGT_OS_$(TGT_OS)    

CFLAGS  +=-DRVDS
ifeq ($(TGT_CPU), ARM11)
  CFLAGS += --memaccess=-UL41
endif 

ifeq ($(TGT_CPU), ARM9)
  CFLAGS  += -c --apcs /interwork --arm --c90  -O3 --no_debug_macros --split_sections --enum_is_int --loose_implicit_cast -Ono_known_library  --no_hide_all --dllimport_runtime --memaccess -UL41 -Otime  #--cpu 6 --cpu=ARM9E-S
  CFLAGS  += -DASM_OPT
endif

ifeq ($(TGT_CPU), ARM12)
   CFLAGS  += --loose_implicit_cast --signed_chars --library_interface=aeabi_clib --apcs /noswst/interwork --enum_is_int 
   CFLAGS  += --vectorize -DASM_OPT # -g --debug -DARMV6 #-DARMV7 #-DDEBUG # -DNDEBUG#pls add your neonimization macros to the tail
endif

ifeq ($(TGT_ENDIAN),BIG)
    CFLAGS  +=  -DENABLE_BIG_ENDIAN
endif
		
    AFLAGS  += --fpu=None --apcs=/noswst/interwork  #-g --debug 

# Put the C files
C_OBJS += $(CSRCS_DIR)/cb.o 
C_OBJS += $(CSRCS_DIR)/decode13.o
C_OBJS += $(CSRCS_DIR)/filter.o
C_OBJS += $(CSRCS_DIR)/log10.o
C_OBJS += $(CSRCS_DIR)/lpc.o
C_OBJS += $(CSRCS_DIR)/lsp.o
C_OBJS += $(CSRCS_DIR)/mem_align.o
C_OBJS += $(CSRCS_DIR)/pack13.o 
C_OBJS += $(CSRCS_DIR)/pitch.o 
C_OBJS += $(CSRCS_DIR)/qcelp13.o 
C_OBJS += $(CSRCS_DIR)/quant13.o 
C_OBJS += $(CSRCS_DIR)/quant.o 
C_OBJS += $(CSRCS_DIR)/stream.o
C_OBJS += $(CSRCS_DIR)/voQCELPDec.o
C_OBJS += $(CSRCS_DIR)/cmnMemory.o


#Put the asm files
A_OBJS	+= $(ASRCS_DIR)/wghtfilt_asm.o


LIB_OBJS = $(C_OBJS) $(A_OBJS)

all: 	LIB_$(TGT_CPU)_$(TGT_OS)
	@echo "--- Successfully Build library $(CODEC) ---"

#Build the library
LIB_ARM12_LERVDS: $(LIB_OBJS)
		$(AR) $(LIB_ARGS)  $(LIBRARY).a $(LIB_OBJS)

LIB_ARM11_LERVDS: $(LIB_OBJS)
		$(AR) $(LIB_ARGS)  $(LIBRARY).a $(LIB_OBJS)
		
LIB_ARM9_LERVDS: $(LIB_OBJS)
		$(AR) $(LIB_ARGS)  $(LIBRARY).a $(LIB_OBJS)

		
%.o: %.c
	$(CC) -W $(CFLAGS) $(INCLUDES)  -c  -o $@ $<

%.o: %.s
	$(AS) $(AFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -f $(LIB_OBJS) $(LIBRARY).a $(LIBRARY).so



