						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

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
VOMM:=-DCOMBINE_ALL_PROFILE -DNDEBUG -DYOUTUBE_HIGH_PROFILE -DDISABLE_LICENSE -D_MAC_OS -DDISABLE_MCORE
endif

ifeq ($(VOTT), v6)
VOMM:=-DARM -DARM_ASM -DCOMBINE_ALL_PROFILE -DNDEBUG -DYOUTUBE_HIGH_PROFILE -DDISABLE_LICENSE -D_IOS -D_VOMODULEID=0x02010000  -DDEBUG_INFO -DTRACE_INPUT_RAW -DTRACE_MEM
endif

ifeq ($(VOTT), v7)
VOMM:=-DARM -DARM_ASM -DCOMBINE_ALL_PROFILE -DNDEBUG -DNEON -DYOUTUBE_HIGH_PROFILE -DDISABLE_LICENSE -D_IOS -D_VOMODULEID=0x02010000 
endif


# please specify the name of your module
VOTARGET:=libvoH264Dec
VOMODVER?=3.0.0
VOSRCNO:=12300

# please modify here to be sure to see the ios.mk
include $(VOTOP)/build/ios.mk


# dependent dynamic libraries.
VODEPLIBS:=-ldl -lstdc++ -lcutils



#dependent static libraries.
VOSTCLIBS:=-L../bp -lvoH264DecBP



# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk 

