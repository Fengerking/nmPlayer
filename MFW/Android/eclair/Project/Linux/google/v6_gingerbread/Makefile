						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

#top root path
VOTOP:=../../../../../../..


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
# If you want to enable the DRM module, please add -D_ENABLE_DRM
# If you want to return "video/x-msvideo" as AVI file MIME type, please add -D_MIME_X_MSVIDEO
VOMM:= -D_GingerBread 


# please specify the name of your module
VOTARGET:=libvoAndroid


# please modify here to be sure to see the g1.mk

include $(VOTOP)/build/gingerbread.mk 


# dependent libraries.
VODEPLIBS:= -ldl -lstdc++ -lui -lutils -lcutils -lmedia -lbinder -landroid_runtime

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

