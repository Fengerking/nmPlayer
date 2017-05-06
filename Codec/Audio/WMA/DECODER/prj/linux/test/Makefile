						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:= v6
 

#top root path
VOTOP:=../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= exe


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), v4)
VOMM:=-DARM -D_ARM_ -DBUILD_INTEGER -DDISABLE_UES -DENABLE_LPC -DENABLE_ALL_ENCOPT -DWMA_DECPK_BUILD #-DDEBUG_2009q1
endif				

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARMV6 -D_ARM_ -DBUILD_WITHOUT_C_LIB -DBUILD_INTEGER -DDISABLE_UES -DENABLE_LPC -DENABLE_ALL_ENCOPT -DWMA_DECPK_BUILD
endif				

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARMV7 -D_ARM_ -DBUILD_WITHOUT_C_LIB -DBUILD_INTEGER -DDISABLE_UES -DENABLE_LPC -DENABLE_ALL_ENCOPT -DWMA_DECPK_BUILD
endif	


# please specify the name of your module
VOTARGET:= wmatest

#VODBG=yes

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk
#ifeq ($(VOTT), v7)
#include $(VOTOP)/build/zoom2.mk
#else
#include $(VOTOP)/build/cupcake.mk 
#include $(VOTOP)/build/g1.mk 
#include $(VOTOP)/build/arm2009q1.mk 
#endif

# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ -lui -lutils -lcutils

# please modify here to add the lib dependencies.
VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms_test.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR=$(VOTOP)/../voRelease/Customer/htc/g1


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

