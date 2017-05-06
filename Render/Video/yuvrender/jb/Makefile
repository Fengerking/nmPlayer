						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:= -DHAVE_PTHREADS -D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
VOMM+=-DLOGE=ALOGE -DLOGW=ALOGW -DLOGI=ALOGI -D_JB43 -D_VOMODULEID=0X05050000


# please specify the name of your module
VOTARGET:=libvoAndroidVR_S43


# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/jb4.3.mk 


# dependent libraries.
VODEPLIBS:= -lbinder -lui -lmedia -ldl -lstdc++ -lutils -lcutils -llog

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms_ics.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

