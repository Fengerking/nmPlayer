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
#cd Numen/trunk/MFW/Android/jni/project/USA_mobiTV/v6
#adb2 push libvomejni.so /data/data/com.visualon.vome/lib
#adb push libmobitvjni_2x.so /data/data/com.mobitv.client.android/lib

# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-DUSA_MOBITV -D__VOPRJ_MTV__ -DHAVE_PTHREADS 



# please specify the name of your module
VOTARGET:=libmobitvjni_2x


# please modify here to be sure to see the cupcake.mk eclair.mk
include $(VOTOP)/build/eclair.mk


# dependent libraries.
VODEPLIBS:=-lmedia -lui -lcutils -landroid_runtime -lnativehelper -lutils -ldl -lstdc++  -lbinder
#       ../../../source/USA_mobiTV/lib/libMKPlayer.a  -L../../../source/USA_mobiTV/lib  -lmkplayer
# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

