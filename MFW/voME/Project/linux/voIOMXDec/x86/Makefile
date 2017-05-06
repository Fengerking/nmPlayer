						#################################################################
						#   Visualon Makefile Template  							 	#
						#																#
						#	(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../../..

# module type
# please specify the type of your module: lib or exe
VOMT:=lib

VODBG=yes

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
#VOMM:=-D_ICS -D_JB43 -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_LINUX -DLINUX
#VOMM+=-DLOGE=ALOGE -DLOGW=ALOGW -DLOGI=ALOGI
VOMM:=-D_ICS -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D_LINUX -DLINUX -D_JB43
VOMM+=-DLOGE=ALOGE -DLOGW=ALOGW -DLOGI=ALOGI -D_VOMODULEID=0X21000000

# please specify the name of your module
VOTARGET:=libvoIOMXDec_ics


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/jb_atom_x86_4.2.mk

# dependent libraries.
VODEPLIBS:= -lbinder -lui -lmedia -ldl -lstdc++ -lutils -lcutils

# import lib
VOSTCLIBS:=../../../../../../Lib/ndk/x86/libvoVideoParser.a

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk
