						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../../../..

# module type
# please specify the type of your module: lib or exe
VOMT:=lib

#VODBG:=yes
#VOGAS:=yes

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module
ifeq ($(VOTT), pc)
VOMM:=-D_IOS -DLCHECK 
endif

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARM_ASM -DCOMBINE_ALL_PROFILE -DNDEBUG -DYOUTUBE_HIGH_PROFILE -DLCHECK -D_IOS
endif

ifeq ($(VOTT), v7)
VOMM:=-DARM -D_IOS -DVOARMV7 -DHAVE_NEON=1 -DASM_OPT -DNDEBUG  -DLCHECK -O2
endif


# please specify the name of your module
VOTARGET:=libvoG711Dec
VOMODVER?=3.0.0
VOSRCNO:=12300

# please modify here to be sure to see the ios.mk
include $(VOTOP)/build/ios.mk


# dependent dynamic libraries.
VODEPLIBS:=-ldl -lstdc++ -lcutils



#dependent static libraries.
VOSTCLIBS:=



# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/v7


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk 
