						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc(i386/simulator), v6(armv6), v7(cortex-a8 neon)
VOTT:=v7
 

#top root path
VOTOP:=../..

#It will build debug library if you open the VODBG.
#Webuild will open it automatic when building a debug library.
#e.g. VODBG=yes


# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary.
# Add redefine macros of namespace by your module's name
# We define _IOS for iOS system, and define _MAC_OS for mac system
# e.g. VOMM:=-D_IOS -D_VOMODULEID=0x08040000  -D_VONAMESPACE=libvompEngn
VOMM:=-D_IOS -D_VOMODULEID=0x08040000  -D_VONAMESPACE=

# please specify the name of your module
VOTARGET:=


# please modify here to be sure to see the ios.mk
# e.g. include $(VOTOP)/build/ios.mk
include 

#dependent static libraries(We don't need it now)
#Do not link shared libraries into your library, it should be link by the top application link
#Format: -L+ThePath; -l++DependentLibrary(Remove the prefix "lib" and suffix "a")
#e.g. VOSTCLIBS:=-L../dir -lvoXXXBase  #means your module depends on $(VOPREBUILTPATH)/libvoXXXBase.a


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
# e.g. include ../source.mk
include 

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
#$(VOTOP)/../voRelease folder is local on the same level directory with the trunk folder
# pc(simulator),v6,v7 corresponding path:i386,v6,v7
# e.g. VORELDIR:=$(VOTOP)/../voRelease/iOS/static/v7
VORELDIR:=

# please modify here to be sure to see the idoit.mk
# e.g. include $(VOTOP)/build/idoit.mk
include 
