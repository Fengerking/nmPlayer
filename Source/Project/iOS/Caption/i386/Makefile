						#################################################################
						#				Visualon Makefile Template 					 	#
						#																#
						#		(Please Keep Below Options Order Unchanged)				#
						#################################################################

# target type
VOTT:=pc
 

#top root path
VOTOP:=../../../../..

#VODBG=yes

# module type
VOMT:=lib

# module macros
VOMM:=-D_IOS -D_VOMODULEID=0x00500000  -D_CLOSECAPTION -D_SOURCE2 -D_VONAMESPACE=libvoCaptionParser #-D_VOLOG_FUNC

# please specify the name of your module
VOTARGET:=libvoCaptionParser

#dependent static libraries
#do not link shared libraries into your library, it should be link by the top application link
#format: -l+thepath; -l++dependentlibrary(remove the prefix "lib" and suffix "a")
#e.g. vostclibs:=-l../dir -lvoxxxbase  #means your module depends on $(VOPREBUILTPATH)/libvoxxxbase.a

# please specify where is the vorelease on your pc, relative path is suggested
# vorelease folder is local on the same level directory with the trunk folder
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/i386


# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../source.mk

# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/ios.mk

# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk

