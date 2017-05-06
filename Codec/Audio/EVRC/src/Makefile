
#*****************************************************************************
# Release EVRC decoder RVDS .a to NXP
# EVRC Codec Library Makefile
#
#*****************************************************************************
include $(VOTOP)/build/NXP/libinit.mk

# Source directory of 'C' code
CSRCS_DIR = c_src

# Source directory of 'assembly(.s)' code
ASRCS_DIR = asm

# Library directory for codec libraries
LIB_DIR = .

#Define the include path
INCLUDES    = -I../../../../Common
INCLUDES   += -I../../../../Include
INCLUDES   += -I../inc

OPTIM_LEVEL             = -O3
OPTIM_TYPE              = -Otime

CFLAGS +=  $(OPTIM_LEVEL) $(OPTIM_TYPE)
CFLAGS +=  $(CFLAGS_CPU) $(CFLAGS_ENDIAN_$(TGT_ENDIAN)) \
                   -DTGT_CPU_$(TGT_CPU) -DTGT_ENDIAN_$(TGT_ENDIAN) -DTGT_OS_$(TGT_OS)    

CFLAGS  +=-DRVDS
ifeq ($(TGT_CPU), ARM11)
  CFLAGS += --memaccess=-UL41
endif 

ifeq ($(TGT_CPU), ARM9)
  CFLAGS  += -c --apcs /interwork --arm --c90 --no_debug_macros --split_sections --enum_is_int --loose_implicit_cast -Ono_known_library --no_hide_all --dllimport_runtime --memaccess -UL41 -Otime  #--cpu 6 --cpu=ARM9E-S
endif

ifeq ($(TGT_CPU), ARM12)
  CFLAGS  += --loose_implicit_cast --signed_chars --library_interface=aeabi_clib --apcs /noswst/interwork #--enum_is_int 
  CFLAGS  += --vectorize #-g --debug #-DASM_OPT #-DARMV6 #-DARMV7 #-DDEBUG # -DNDEBUG#pls add your neonimization macros to the tail
endif


ifeq ($(TGT_ENDIAN),BIG)
  CFLAGS  +=  -DENABLE_BIG_ENDIAN
endif
		
  AFLAGS  += --fpu=None --apcs=/noswst/interwork

ifeq ($(CODEC), EVRCDec)
    C_OBJS = $(CSRCS_DIR)/voEVRCDec.o
else
    C_OBJS = $(CSRCS_DIR)/voEVRCEnc.o
endif

# Put the C files here
C_OBJS += $(CSRCS_DIR)/a2lsp.o
C_OBJS += $(CSRCS_DIR)/acb_ex.o
C_OBJS += $(CSRCS_DIR)/acelp_pf.o
C_OBJS += $(CSRCS_DIR)/apf.o
C_OBJS += $(CSRCS_DIR)/auto.o
C_OBJS += $(CSRCS_DIR)/bitpack.o
C_OBJS += $(CSRCS_DIR)/bitupack.o
C_OBJS += $(CSRCS_DIR)/bl_intrp.o
C_OBJS += $(CSRCS_DIR)/bqiir.o
C_OBJS += $(CSRCS_DIR)/c3_10pf.o
C_OBJS += $(CSRCS_DIR)/c8_35pf.o
C_OBJS += $(CSRCS_DIR)/comacb.o
C_OBJS += $(CSRCS_DIR)/convh.o
C_OBJS += $(CSRCS_DIR)/cshift.o
C_OBJS += $(CSRCS_DIR)/d3_10pf.o
C_OBJS += $(CSRCS_DIR)/d8_35pf.o
C_OBJS += $(CSRCS_DIR)/d_fer.o
C_OBJS += $(CSRCS_DIR)/d_no_fer.o
C_OBJS += $(CSRCS_DIR)/d_rate_1.o
C_OBJS += $(CSRCS_DIR)/decode.o
C_OBJS += $(CSRCS_DIR)/durbin.o
C_OBJS += $(CSRCS_DIR)/encode.o
C_OBJS += $(CSRCS_DIR)/fcbgq.o
C_OBJS += $(CSRCS_DIR)/fer.o
C_OBJS += $(CSRCS_DIR)/filter.o
C_OBJS += $(CSRCS_DIR)/fndppf.o
C_OBJS += $(CSRCS_DIR)/getext1k.o
C_OBJS += $(CSRCS_DIR)/getgain.o
C_OBJS += $(CSRCS_DIR)/getres.o
C_OBJS += $(CSRCS_DIR)/impulser.o
C_OBJS += $(CSRCS_DIR)/interpol.o
C_OBJS += $(CSRCS_DIR)/intr_cos.o
C_OBJS += $(CSRCS_DIR)/inv_sqrt.o
C_OBJS += $(CSRCS_DIR)/lpcana.o
C_OBJS += $(CSRCS_DIR)/lsp2a.o
C_OBJS += $(CSRCS_DIR)/lspmaq.o
C_OBJS += $(CSRCS_DIR)/math_adv.o
C_OBJS += $(CSRCS_DIR)/math_ext32.o
C_OBJS += $(CSRCS_DIR)/maxeloc.o
C_OBJS += $(CSRCS_DIR)/mdfyorig.o
C_OBJS += $(CSRCS_DIR)/mem_align.o
C_OBJS += $(CSRCS_DIR)/mod.o
C_OBJS += $(CSRCS_DIR)/ns127.o
C_OBJS += $(CSRCS_DIR)/pit_shrp.o
C_OBJS += $(CSRCS_DIR)/pktoav.o
C_OBJS += $(CSRCS_DIR)/pre_enc.o
C_OBJS += $(CSRCS_DIR)/putacbc.o
C_OBJS += $(CSRCS_DIR)/r_fft.o
C_OBJS += $(CSRCS_DIR)/rda.o
C_OBJS += $(CSRCS_DIR)/rom.o
C_OBJS += $(CSRCS_DIR)/stream.o
C_OBJS += $(CSRCS_DIR)/synfltr.o
C_OBJS += $(CSRCS_DIR)/w2res.o
C_OBJS += $(CSRCS_DIR)/weight.o
C_OBJS += $(CSRCS_DIR)/zeroinpt.o

#put the asm files
A_OBJS	+= $(ASRCS_DIR)/bl_intrp_asm.o
A_OBJS  += $(ASRCS_DIR)/ConIR.o
A_OBJS  += $(ASRCS_DIR)/cor_h_vec.o
A_OBJS  += $(ASRCS_DIR)/GetResidual.o
A_OBJS  += $(ASRCS_DIR)/iir_asm.o
A_OBJS  += $(ASRCS_DIR)/Impulse_asm.o


LIB_OBJS = $(C_OBJS) $(A_OBJS)

all: 	LIB_$(TGT_CPU)_$(TGT_OS)
	@echo "---Successfully Build library $(CODEC) ---"

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

