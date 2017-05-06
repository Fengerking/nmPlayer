						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target6
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:= v4
 

#top root path
VOTOP:=../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), v4)
VOMM:=-DARM -DLCHECK
endif				

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV4 -DLCHECK
endif				

ifeq ($(VOTT), v7)
VOMM:= -DARM -DARMV4 -DARMV7 -DLCHECK
endif	

# please specify the name of your module
VOTARGET:= voRADec


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/g1.mk 

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ 

VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a  \
					$(VOTOP)/../voRelease/Customer/Google/cupcake/lib/v4/voAACDec.a

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

