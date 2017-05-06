						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../../../../../..

#VODBG=yes

# module type
# please specify the type of your module: lib or exe
VOMT:=lib

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
#ifeq ($(VOTT), pc)
VOMM:= -D_IOS -D_VOMODULEID=0x03010000 
#endif
ifeq ($(VOTT), v4)
VOMM:= -D_IOS -D_VOMODULEID=0x03010000  -DVOARMV4
endif
ifeq ($(VOTT), v6)
VOMM:= -D_IOS -D_VOMODULEID=0x03010000  -DVOARMV6
endif
ifeq ($(VOTT), v7)
VOMM:= -DD_IOS -DVOARMV7 -DLICENSE_CHECK
endif



# please specify the name of your module
VOTARGET:=libvoH264Enc

# please specify the version of your module
VOMODVER?=3.0.0.218

# please specify the SRCNO of your module
#VOSRCNO:=9979


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/ios.mk 


# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ -lcutils


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk 

#VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a

