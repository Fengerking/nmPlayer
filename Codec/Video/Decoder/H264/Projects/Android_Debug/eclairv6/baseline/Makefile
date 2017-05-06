						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=v6
 

#top root path
VOTOP:=../../../../../../../..
#cd Numen/trunk/Test/h264decspeed
#cd Numen/trunk/Codec/Video/Decoder/H264/Projects/Android_Debug/eclairv6/baseline
#make clean;make;make devel
#adb push voH264Dec.so /data/local/voOMXPlayer/lib 

#adb push cars.3gp  /sdcard

#adb shell
#cd /data/local/voOMXPlayer
#./voMMPlayer /sdcard/cars.3gp
#./voMMPlayer /sdcard/spiderman.3gp


###############speed test############################
#	adb push h264speed.cfg /data/voplayer ; make sure voplayer existing und /data of G1
#	
#	adb push akiyo_cif_30fps_384k.264 /sdcard/
#	adb push cars_QCIF181kbps.264 /sdcard/
#	adb push foreman_cif_30fps_768k.264 /sdcard/
#	adb push foreman_qvga_15fps_384kbps_051018.264 /sdcard/
#	adb push foreman_vlc_frame_QCIF.264 /data/local
#	adb push h264decspeed /data/local
#	adb push matrix_VGA.383kbps.264 /sdcard/
#	adb push news_384k.264 /sdcard/
#	adb push libvoH264Dec.so /system/lib
#	adb push silent_384k.264 /sdcard/

#	; then connect to g1 with
#	adb shell
#	su				; change to root
#	cd /data/voplayer
#	chmod 777 h264decspeed
#	./h264decspeed
#######################################################
# module type
# please specify the type of your module: lib or exe
VOMT:=lib

#VODBG:=yes
# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=-DNDEBUG  -DARM  -DARM_ASM -DCOMBINE_ALL_PROFILE -DDEBUG_INFO 
#-DNDEBUG -DARM_ASM -DARM -DDISABLE_LICENSE 



# please specify the name of your module
VOTARGET:=libvoH264DecBP


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/eclair.mk 



# dependent dynamic libraries.
VODEPLIBS:=-ldl -lstdc++ -lcutils



#dependent static libraries.
#VOSTCLIBS:=../../../../corelib/RVDS/H264_D_Core_ARM9_baseline.a  $(VOPREBUILTPATH)/libvoCheck.a




# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../../ms_bp.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

