						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:= -DVOARCHGEN -DVOARCH32 -DNEW_SDK


# please specify the name of your module
VOTARGET:=voMPEG4Enc


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/g1.mk 


# dependent libraries.
VODEPLIBS:= 


OBJS:= voMpegIdct.o voMpegMC.o block_trans.o cbp.o const_tab.o fdct.o interpolate.o preprocess.o \
			quant_h263.o sad.o h263huffman.o ME_frame.o ME_method.o cbr_rc.o EncBitStream.o\
			EncFrame.o EncMB.o image.o mbprediction.o mpeg_malloc.o encoder.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../src \
	  ../../src/midlevel\
	  ../../src/midlevel/h263\
	  ../../src/midlevel/motionestimation\
	  ../../src/midlevel/ratecontrol\
	  ../../src/lowlevel\
	  ../../src/common/src\
	  ../../../../../../Include\


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

