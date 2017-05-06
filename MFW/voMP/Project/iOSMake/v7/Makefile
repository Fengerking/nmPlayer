						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc(i386/simulator), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../../..

VODBG=yes

# module type
# please specify the type of your module: lib or exe
VOMT:=lib

# module macros
# please append the additional macro definitions here for your module if necessary. 
VOMM:=-D_IOS -D_VOMODULEID=0x08040000  -D_VONAMESPACE=libvompEngn -D_UNUSE_FTU #-D_VOLOG_FUNC


# please specify the name of your module
VOTARGET:=libvompEngn

# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/ios.mk 


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../source.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/v7


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk

