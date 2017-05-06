						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc(i386/simulator), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../../../../../..

VODBG=yes

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary.
# e.g. -D_IOS -D_VOMODULEID=0x09010000 , macro _IOS defined for your module(We define _IOS for iOS system, and define _MAC_OS for mac system)
VOMM:=-D_IOS -D_VOMODULEID=0x09010000  -D_VONAMESPACE=libvoOSPlayer
#-D_VOLOG_FUNC -D_Discretix_TEMP


# please specify the name of your module
VOTARGET:=libvoOSPlayer


# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/ios.mk


#dependent static libraries
#Do not link shared libraries into your library, it should be link by the top application link
#Format: -L+ThePath; -l++DependentLibrary(Remove the prefix "lib" and suffix "a")
#e.g. VOSTCLIBS:=-L../dir -lvoXXXBase  #means your module depends on $(VOPREBUILTPATH)/libvoXXXBase.a


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../source.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
#$(VOTOP)/../voRelease folder is local on the same level directory with the trunk folder
# pc(simulator),v6,v7 corresponding path:i386,v6,v7
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/v7


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk

