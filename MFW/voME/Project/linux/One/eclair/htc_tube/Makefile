						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


#VODBG=yes

VOSRCNO:=11801
# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-D_ARM_VER -DPD -D_HTC -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE  
#-D_VOLOG_FUNC


# please specify the name of your module
VOTARGET:=libvoOMXOne


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk 


# dependent libraries.
VODEPLIBS:= -ldl -lstdc++ -lutils -lcutils

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../mseclair.mk

