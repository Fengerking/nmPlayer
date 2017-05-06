#################################################################
#  	Visualon Makefile Template  		 	#
#       (Please Keep Below Options Order Unchanged)	#
#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:= v6
 

#top root path
VOTOP:=../../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:= lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-DMSVC_AMRWBPLUS #-DARM -DARMV6 -DLCHECK 


# please specify the name of your module
VOTARGET:= libvoAMRWBPDec
VOMODVER?=3.0.0
VOSRCNO:=6742

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/seuic.mk 

# dependent libraries.
#VODEPLIBS:=-ldl -lstdc++ -lcutils
VOCFLAGS += -mno-long-calls 
#VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

