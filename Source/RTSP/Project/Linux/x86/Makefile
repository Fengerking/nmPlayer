						#################################################################
						#  									Visualon Makefile Template  							 	#
						#																																#
						#						(Please Keep Below Options Order Unchanged)					#
						#################################################################

# target type
# available: pc, v4(armv4), v5(armv5), v5x(armv5 xscale), v6(armv6), v7(cortex-a8 neon)
VOTT:=pc
 

#top root path
VOTOP:=../../../../..

#cd Numen/trunk/Source/RTSP/Project/Linux/x86/RTSP_LINUX_PLAYER
#./vomePlayer rtsp://75.126.7.231:554/3gp/IOT/3a_Supremecy_H264B_QCIF140kbps20FPS_eAACPlus20kbps_32khz.3gp
#cp *.txt /home/Number/log/
#make clean;make;make devel
#adb push voSrcRTSP.so /data/local/voOMXPlayer/lib 
#adb get /sdcard/visualonLog_rtsp
#adb logcat > c:\log.txt
#adb push cars.3gp  /sdcard

#adb shell
#cd /data/local/voOMXPlayer
#./voMMPlayer /sdcard/cars.3gp
#./voMMPlayer /sdcard/spiderman.3gp
# module type
# please specify the type of your module: lib or exe
VOMT:=lib


# module macros
# please append the additional macro definitions here for your module if necessary. 
# e.g. -DVISUALON, macro VISUALON defined for your module 
VOMM:=PC -DNDEBUG -DFULL_VERSION -DMAC -DG1 -DH264_ANNEXB_FORMAT 




# please specify the name of your module
VOTARGET:=voSrcRTSP


# please modify here to be sure to see the g1.mk
include $(VOTOP)/build/g1.mk 



# dependent dynamic libraries.
VODEPLIBS:=-ldl -lstdc++ #-lcutils





# module source
# please modify here to be sure to see the ms.mk which specifies all source info of your module
include ../ms.mk


# please specify where is the$(VOTOP)/../voRelease on your PC, relative path is suggested
VORELDIR:=$(VOTOP)/../voRelease


# please modify here to be sure to see the doit.mk
include $(VOTOP)/build/doit.mk 

