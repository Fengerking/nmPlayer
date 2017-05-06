						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v6(armv6), v7(armv7)
VOTT:=pc
 

#top root path
VOTOP:=../../../../../../..



# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), pc)
VOMM:= -D_IOS -D_VOMODULEID=0x020c0000   -DMULTI_CORE_DEC -DARM_C  -DADJ_NEW -DUSE_FRAME_THREAD
endif

# please specify the name of your module
VOTARGET:=libvoVC1Dec
VOMODVER?=3.0.0
VOSRCNO:=28978

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/ios.mk 


# dependent libraries.
VODEPLIBS:=-lcutils -ldl -lstdc++

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk
