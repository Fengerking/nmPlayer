						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################
#	\\10.2.64.166
#	cd svn/trunk/Codec/Video/Decoder/voVp6Dec/on2VP6lib/Customer/Google/eclair_v7
#	make clean ; make ;make devel 

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:= -D_IOS -D_VOMODULEID=0x03080000  -DRVDS -DVOARMV7  -DSTABILITY -DVOMJPEGENC


# please specify the name of your module
VOTARGET:=libvoMJPEGEnc
VOMODVER?=3.0.0
VOSRCNO:=18245


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/ios.mk 


# dependent libraries.
VODEPLIBS:=-ldl -lstdc++

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk 

