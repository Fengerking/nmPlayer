#################################################################
#                                                               #
#  		Visualon Makefile Template                      #
#                                                               #
#        Please Keep Below Options Order Unchanged)		#
#################################################################

# target type
# available: pc, v6(armv6), v7(cortex-a8 neon)
VOTT:= v6
 

#top root path
VOTOP:=../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module  				

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV6 -DIOS -D_VOMODULEID=0x022c0000  -DLITEND -DKCAPABLE -DHALFRATE -DLCHECK #-DMS_CHANNEL_CONFIG  #-DAC3D_ARM_OPT #If Support MS Multichannel configure, enable the "MS_CHANNEL_CONFIG" MACRO
endif				

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARMV6 -DARMV7 -DLITEND -DKCAPABLE -DHALFRATE -DLCHECK #-DMS_CHANNEL_CONFIG  #-DAC3D_ARM_OPT #If Support MS Multichannel configure, enable the "MS_CHANNEL_CONFIG" MACRO
endif

ifeq ($(VOTT), v7s)
VOMM:=-DARM -DARMV6 -DARMV7 -DLITEND -DKCAPABLE -DHALFRATE -DLCHECK #-DMS_CHANNEL_CONFIG  #-DAC3D_ARM_OPT #If Support MS Multichannel configure, enable the "MS_CHANNEL_CONFIG" MACRO
endif

# please specify the name of your module
VOTARGET:= libvoAC3Dec
VOSRCNO:=28358

# please modify here to be sure to see the ios.mk
include $(VOTOP)/build/ios.mk 

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/v6


# please modify here to be sure to see the idoit.mk
include $(VOTOP)/build/idoit.mk 

