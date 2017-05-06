						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: x86, x86_64
VOTT:=x86
 

#top root path
VOTOP:=../../../../..

#It will build debug library if you open the VODBG.
#Webuild will open it automatic when building a debug library.
VODBG=yes
IDBG=yes

# module type
# please specify the type of your module: lib or static_lib
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary.
# Add redefine macros of namespace by your module's name
# We define _IOS for iOS system, and define _MAC_OS for mac system
# e.g. VOMM:=-D_MAC_OS -D_VOMODULEID=0x00127000  -D_VONAMESPACE=libvompEngn
VOMM:=-D_MAC_OS -D_VOMODULEID=0x00127000  -D_VONAMESPACE=libvoSourceIO -DCARES_STATICLIB -DENABLE_ASYNCDNS#-D_VOLOG_RUN

# please specify the name of your module
VOTARGET:=libvoSourceIO


# please modify here to be sure to see the imac.mk
# e.g. include $(VOTOP)/build/imac.mk
include $(VOTOP)/build/imac.mk

#dependent static libraries(We don't need it now)
#Do not link shared libraries into your library, it should be link by the top application link
#Format: -L+ThePath; -l++DependentLibrary(Remove the prefix "lib" and suffix "a")
#e.g. VOSTCLIBS:=-L../dir -lvoXXXBase  #means your module depends on $(VOPREBUILTPATH)/libvoXXXBase.a
VOSTCLIBS:=-L$(VOTOP)/Thirdparty/c-ares/Mac_X86/ -lcares -lresolv

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
# e.g. include ../source.mk
include ../source.mk

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
#$(VOTOP)/../voRelease folder is local on the same level directory with the trunk folder
# x86, x86_64 corresponding path:x86, x86_64
# e.g. VORELDIR:=$(VOTOP)/../voRelease/MacOS/lib/x86
VORELDIR:=$(VOTOP)/../voRelease/MacOS/lib/x86

# please modify here to be sure to see the macdoit.mk
# e.g. include $(VOTOP)/build/macdoit.mk
include $(VOTOP)/build/macdoit.mk
