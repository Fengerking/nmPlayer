						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################
#	\\10.2.64.166
#	cd svn/trunk/Codec/Video/Decoder/voVp6Dec/on2VP6lib/Customer/Google/eclair_v4
#	make clean ; make ;make devel 

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v4


# module type
# please specify the type of your module: lib or exe
VOMT:=exe


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:= -DANDRIOD 


# please specify the name of your module
VOTARGET:=voVP6DecTst

# please modify here to be sure to see the g1.mk
include ../../../../../build/eclair.mk 


# dependent libraries.
VODEPLIBS:=-ldl -lstdc++ -L. -lvoVP6Dec

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ms.mk


# please specify where is the voRelease on your PC, relative path is suggested
VORELDIR:=../../../../../../../../voRelease/


# please modify here to be sure to see the doit.mk
include ../../../../../build/doit.mk 

