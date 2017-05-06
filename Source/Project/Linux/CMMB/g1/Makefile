						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

#top root path
VOTOP:=../../../../..

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-DMTV_CMMB -D_SUPPORT_CACH


# please specify the name of your module
VOTARGET:=voCMMBParser

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/donut.mk 

# dependent libraries.
VODEPLIBS:= -lstdc++ -ldl

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk

# import lib
VOSTCLIBS:=../../../../../Lib/Customer/htc/g1/voCheck.a

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/Customer/htc/g1


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

