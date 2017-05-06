						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../../..

VODBG=yes

VOPRJ=x86_debug

# module type
# please specify the type of your module: lib or exe
VOMT:=lib

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-D_X86_VER `pkg-config sdl --cflags`X86 

# please specify the name of your module
VOTARGET:=voOMXME

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/g1.mk 

# dependent libraries.
VODEPLIBS:= -ldl `pkg-config sdl --libs`

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk

