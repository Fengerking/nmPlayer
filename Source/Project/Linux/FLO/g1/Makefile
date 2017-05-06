						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

#top root path
VOTOP:=../../../../..

#cd Numen/trunk/Source/Project/Linux/FLO/g1
# module type
# please specify the type of your module: lib or exe
#/opt/myandroid/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/bin/arm-eabi-objdump -S ./libvoFLOEngine.so >dump.txt
#adb push libvoFLOEngine.so /data/data/com.qualcomm.mediaflo.flotestplayer/lib
VOMT:=lib

#VODBG=yes

# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:= -DMEDIA_FLO -DH264_ANNEXB_FORMAT -D_ARM_VER -DFORCE_RTSP_LOG -DENABLE_DX=1 -DUSE_DX_SO=0 -DENABLE_LOG=1


# please specify the name of your module
VOTARGET:=libvoFLOEngine

# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk 

# dependent libraries.
VODEPLIBS:= -ldl -lstdc++ -lcutils -L../../../../../Thirdparty/Discretix/bin/Android/static -lDxHost -lobf

# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk

# import lib
VOSTCLIBS:=

# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk

