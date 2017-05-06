						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################
#/opt/eclair/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-objdump -S ./libvoSrcRTSP.so >rtspasm.txt
# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

#top root path
VOTOP:=../../../../..

# module type
# please specify the type of your module: lib or exe
VOMT:=lib
#VODBG:=yes

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=  -DMAC -DG1 -DH264_ANNEXB_FORMAT -DFORCE_ADD_ADTS #-D_VOLOG_INFO



# please specify the name of your module
VOTARGET:=libvoSrcRTSP


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk 

# dependent dynamic libraries.
VODEPLIBS:=-ldl -lstdc++ -lcutils -lgcc

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/

# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

