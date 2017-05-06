						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../../../../..

# module type
# please specify the type of your module: lib or exe
VOMT:=lib

#VODBG:=yes

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
ifeq ($(VOTT), pc)
VOMM:=-DCOMBINE_ALL_PROFILE -DNDEBUG -DDISABLE_LICENSE -D_IOS -D_VOMODULEID=0x02010000   -DUSE_JOBPOOL
# -DDISABLE_MCORE
endif

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARM_ASM -DCOMBINE_ALL_PROFILE -DNDEBUG -DDISABLE_LICENSE -D_IOS -D_VOMODULEID=0x02010000 
endif

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARM_ASM -DCOMBINE_ALL_PROFILE -DNDEBUG -DNEON -DDISABLE_LICENSE -D_IOS -D_VOMODULEID=0x02010000  -D_VONAMESPACE=libvoH264Dec
endif

# please specify the name of your module
VOTARGET:=libvoH264DecBP


# please modify here to be sure to see the ios.mk
include $(VOTOP)/build/ios.mk

# dependent dynamic libraries.
VODEPLIBS:=-ldl -lstdc++



# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../ms_bp.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
#VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the idoit.mk
include $(VOTOP)/build/idoit.mk

