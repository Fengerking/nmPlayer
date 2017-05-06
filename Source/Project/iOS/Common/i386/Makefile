						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc(simulator), v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../..

#VODBG=yes

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 

VOMM:=-D_IOS #-D_VOLOG_INFO

# please specify the name of your module
VOTARGET:=libvoSourceCommon

# please modify here to be sure to see the eclair.mk
include $(VOTOP)/build/ios.mk 

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../source.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease/iOS/static/i386


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/idoit.mk

