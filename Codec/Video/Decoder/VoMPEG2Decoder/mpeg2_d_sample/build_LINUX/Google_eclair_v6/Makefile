#################################################################
#  	Visualon Makefile Template  		 	#
#       (Please Keep Below Options Order Unchanged)	#
#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:= v6
 

#top root path
VOTOP:=../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), v5)
VOMM:=-DARM -DVOARM -DRVDS -DLICENSEFILE
endif				

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV6 -DVOARMV6 -DRVDS -DLICENSEFILE
endif				

ifeq ($(VOTT), v7)
VOMM:=-DARM -DVOARMv7 -DRVDS -DLICENSEFILE 
endif	


# please specify the name of your module
VOTARGET:= libvoMPEG2Dec
VOMODVER?=3.0.0
VOSRCNO:=17932

# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/eclair.mk 

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++

# dependent libraries.
VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

