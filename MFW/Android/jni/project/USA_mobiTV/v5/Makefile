						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

#top root path
VOTOP:=../../../../../..
#cd Numen/trunk/MFW/Android/jni/project/USA_mobiTV/v5

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-DUSA_MOBITV -D__VOPRJ_MTV__ -DHAVE_PTHREADS -DVORTSPIMP 



# please specify the name of your module
VOTARGET:=libvomejni


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/cupcake.mk 


# dependent libraries.
VODEPLIBS:=-lmedia -lui -lcutils -landroid_runtime -lnativehelper -lutils -ldl -lstdc++ $(VOPREBUILTPATH)/libvoSrcRTSP.a

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../voms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

