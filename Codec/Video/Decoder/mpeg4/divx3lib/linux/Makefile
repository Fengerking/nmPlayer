

# please modify here to be sure to see the config.mk
include ../../../../../../config.mk 

# please identify what type of your module: library or executable
# available: lib, exe
VOMT:=lib


# dependent libraries. if VOMT==lib, ignore it & keep it empty
DEPLIBS:=


# please list all objects needed by your target here
OBJS:=voDivx3Haff.o voDivx3MB.o voDivx3Parser.o voDivx3Tab.o voMpeg4ACDC.o voMpeg4DecFront.o voMpeg4Decoder.o voMpeg4Gmc.o \
			voMpeg4Haff.o voMpeg4MB.o voMpeg4Parser.o voMpeg4Postprocess.o voMpeg4Rvlc.o voMpegBuf.o voMpegIdct.o\
			voMpegMC.o voMpeg4MCQpel.o voMpegMem.o voMpegReadbits.o voMpeg4Frame.o


# please list all directories that your all of your source files(.h .c .cpp) locate 
VOSRCDIR:=../../src \
	  ../../src/front\
	  ../../../../../../Include\

VOCFLAGS+=-DNEW_SDK -DMPEG4 -DRVDS -DVODIVX3	
				
ifeq ($(VOTT), v4)
OBJS+= voIdctArmV4V6_gnu.o voMpegMCArmV4V6_gnu.o 
VOSRCDIR+= ../../src/asm/arm

VOCFLAGS+=-DVOARMV4
endif				

ifeq ($(VOTT), v6)
OBJS+= voIdctArmV4V6_gnu.o voMpegMCArmV4V6_gnu.o voMpegFillEdgesArmV6_gnu.o voMpeg4MCQpelArmV6_gnu.o voMpeg4MCQpelAddArmV6_gnu.o
VOSRCDIR+= ../../src/asm/arm

VOCFLAGS+=-DVOARMV6
endif				

ifeq ($(VOTT), v7)
OBJS+= voIdctArmV7_gnu.o voMpegMCArmV7_gnu.o voMpegFillEdgesArmV6_gnu.o voMpeg4MCQpelArmV6_gnu.o voMpeg4MCQpelAddArmV6_gnu.o
VOSRCDIR+= ../../src/asm/arm/ARMV7\
		../../../../src/asm/arm
	  

VOCFLAGS+=-DVOARMV7
endif		


# please give out the real name of your module
VOTARGET=libvoDIVX3Dec
VOMODVER?=3.0.0


# please identify where is the$(VOTOP)/../voRelease on your PC
VORELDIR:=/home/renjie$(VOTOP)/../voRelease


# please modify here to be sure to see the vogenlib.mk
include ../../../../../../doit.mk

VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a