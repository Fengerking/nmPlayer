#################################################################
#  	Visualon Makefile Template  		 	#
#       (Please Keep Below Options Order Unchanged)	#
#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), v4)
VOMM:=-DARM
endif				

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV6 -DVOARMV6  -DLINUX -DLICENSEFILE
endif				

ifeq ($(VOTT), v7)
VOMM:=-DARM  -DVOARMV7 -DRVDS -DVOANDROID -DLINUX #-DLICENSEFILE
endif	


# please specify the name of your module
VOTARGET:= voH265Dec
VOMODVER?=0.9.0
VOSRCNO:=12315

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++

# dependent libraries.
#VOSTCLIBS:=../../../../../../../Lib/Customer/google/cupcake/v7/libvoCheck.a


# please modify here to be sure to see the zoom2.mk
#include ../../../../../../../build/zoom2.mk 

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ 


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the voRelease on your PC, relative path is suggested
VORELDIR:=../../../../../../../../../voRelease


# please modify here to be sure to see the doit.mk
include ../../../../../../../../build/doit.mk 

