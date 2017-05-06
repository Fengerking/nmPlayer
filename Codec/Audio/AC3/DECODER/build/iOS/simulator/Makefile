#################################################################
#                                                               #
#  		Visualon Makefile Template                      #
#                                                               #
#        Please Keep Below Options Order Unchanged)		#
#################################################################

# target type
# available: pc, v6(armv6), v7(cortex-a8 neon)
VOTT:= pc
 

#top root path
VOTOP:=../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module  				

ifeq ($(VOTT), pc)
VOMM:= -DLITEND -DHALFRATE -DIOS -D_VOMODULEID=0x022c0000  
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
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/i386


# please modify here to be sure to see the idoit.mk
include $(VOTOP)/build/idoit.mk 

