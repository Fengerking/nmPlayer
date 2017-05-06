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
VOMM:= -DNEW_SDK -DMPEG4 -DRVDS 


# please specify the name of your module
VOTARGET:=libvoMPEG4Dec
VOMODVER?=3.0.0


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/pc.mk


# dependent libraries.
VODEPLIBS:= 


OBJS:=voDivx3Haff.o voDivx3MB.o voDivx3Parser.o voDivx3Tab.o voMpeg4ACDC.o voMpeg4DecFront.o voMpeg4Decoder.o voMpeg4Gmc.o \
			voMpeg4Haff.o voMpeg4MB.o voMpeg4Parser.o voMpeg4Postprocess.o voMpeg4Rvlc.o voMpegBuf.o voMpegIdct.o\
			voMpegMC.o voMpeg4MCQpel.o voMpegMem.o voMpegReadbits.o voMpeg4Frame.o
			

# please list all directories that all source files relative with your module(.h .c .cpp) locate 

VOSRCDIR:=../../src \
	  ../../src/front\
	  ../../../../../../Include\


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a

