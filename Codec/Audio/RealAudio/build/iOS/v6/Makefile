# target6
# available: pc, v6(armv6), v7(cortex-a8 neon)
VOTT:= v6
 

#top root path
VOTOP:=../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV4 -DARMV6 -DLCHECK
endif

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARMV4 -DARMV6 -DARMV7
endif


# please specify the name of your module
VOTARGET:= libvoRADec
VOSRCNO:=27349

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/ios.mk

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ 


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/v6


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk
