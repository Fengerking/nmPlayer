						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:= v5
 

#top root path
VOTOP:=../../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), v4)
VOMM:=-DARM -D_ARM_
endif				

ifeq ($(VOTT), v5)
VOMM:=-DARM -DARMV5 -D_ARM_
endif

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV6 -D_ARM_
endif				

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARMV7 -D_ARM_
endif	


# please specify the name of your module
VOTARGET:= voOGGDec


# please modify here to be sure to see the g1.mk
ifeq ($(VOTT), v7)
include $(VOTOP)/build/zoom2.mk
else
include $(VOTOP)/build/g1.mk 
endif

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ 

# please modify here to add the lib dependencies.
VOSTCLIBS:=../../../../../../../../Lib/Customer/moto/wrigley/voCheck.a

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR=$(VOTOP)/../voRelease/Customer/moto/wrigley


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 


