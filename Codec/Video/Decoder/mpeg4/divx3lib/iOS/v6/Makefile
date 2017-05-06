						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v6(armv6), v7(armv7)
VOTT:=v6
 

#top root path
VOTOP:=../../../../../../..



# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), pc)
VOMM:= -D_IOS -D_VOMODULEID=0x02070000  -DARM -D_ARM_ -D__arm -DARM_C -DVODIVX3
endif
ifeq ($(VOTT), v6)
VOMM:= -D_IOS -D_VOMODULEID=0x02070000  -DNEW_SDK -DMPEG4 -DRVDS -DVOARMV6 -DVODIVX3
endif
ifeq ($(VOTT), v7)
VOMM:= -D_IOS -D_VOMODULEID=0x02070000  -DNEW_SDK -DMPEG4 -DRVDS -DVOARMV7 -DVODIVX3
endif

# please specify the name of your module
VOTARGET:=libvoDIVX3Dec
VOMODVER?=3.0.0

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/ios.mk 


# dependent libraries.
VODEPLIBS:=-lcutils -ldl -lstdc++

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk
