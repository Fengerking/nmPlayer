#################################################################
#  	Visualon Makefile Template  		 	#
#       (Please Keep Below Options Order Unchanged)	#
#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../../../../..

#VODBG=yes

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), pc)
VOMM:= -D_MAC_OS -D_IOS -D_VOMODULEID=0x02050000  -DLICENSEFILE
endif	

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV6 -DVOARMV6 -D_MAC_OS -D_IOS -D_VOMODULEID=0x02050000  -DLICENSEFILE
endif				

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARMV7  -D_MAC_OS -DVOARMV7 -D_IOS -D_VOMODULEID=0x02050000  -DLICENSEFILE
endif	

ifeq ($(VOTT), v7s)
VOMM:=-DARM -DARMV7  -D_MAC_OS -DVOARMV7 -D_IOS -D_VOMODULEID=0x02050000  -DLICENSEFILE
endif	

# please specify the name of your module
VOTARGET:= libvoRealVideoDec
VOMODVER?=3.0.0

# dependent libraries.
VODEPLIBS:=-lcutils -ldl -lstdc++

# dependent libraries.
#VOSTCLIBS:= $(VOPREBUILTPATH)/libvoCheck.a

# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/ios.mk


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease

# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk 

