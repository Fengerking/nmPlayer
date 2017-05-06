						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v6(armv6), v7(armv7)
VOTT:=v4
 

#top root path
VOTOP:=../../../../../../../../..



# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), v4)
VOMM:= -DVOANDROID  -DMULTI_CORE_DEC -DARM_C  -DNEW_C -DADJ_NEW -DUSE_FRAME_THREAD
endif

# please specify the name of your module
VOTARGET:=libvoVC1Dec

# SDK version 
VOMODVER?=3.1.0
VOSRCNO:=9738

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk 


# dependent libraries.
VODEPLIBS:=-lcutils -ldl -lstdc++

# dependent libraries.
VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a 


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk
