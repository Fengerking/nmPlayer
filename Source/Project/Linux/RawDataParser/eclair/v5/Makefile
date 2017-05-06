						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v5
 

#top root path
VOTOP:=../../../../../..

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-D_RAW_DATA_PARSER -DVO_FILE_USE_BIG_ENDIAN -D_G1_FORCE_UTF16_2_UTF8


# please specify the name of your module
VOTARGET:=libvoRawDataFR

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk 

# dependent libraries.
VODEPLIBS:= -lstdc++ -ldl

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../mseclair.mk

OBJS+=voutf8conv.o

VOSRCDIR+=../../../../../../Utility/voutf8conv

# import lib
VOSTCLIBS:=$(VOPREBUILTPATH)/libvoCheck.a

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

