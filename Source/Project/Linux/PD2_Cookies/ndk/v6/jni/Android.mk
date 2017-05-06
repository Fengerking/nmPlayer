LOCAL_PATH:=$(call my-dir)
VOTOP?=../../../../../../..

include $(CLEAR_VARS)

LOCAL_ARM_MODE:=arm

LOCAL_MODULE:=libvoSrcPD


PDSRC_PATH:=../../../../../../File/PD2_Cookies
PDCMN_PATH:=../../../../../../File/Common
SRCCMN_PATH:=../../../../../../../Source/Common
TOP_INCLUDE_PATH:=../../../../../../../Include
TOP_CMN_NET_PATH:=../../../../../../../Common/NetWork
TOP_CMN_PATH:=../../../../../../../Common

LOCAL_SRC_FILES := \
	\
	$(PDSRC_PATH)/PD.cpp	\
	$(PDSRC_PATH)/vo_buffer_manager.cpp	\
	$(PDSRC_PATH)/vo_buffer_stream.cpp	\
	$(PDSRC_PATH)/vo_download_manager.cpp	\
	$(PDSRC_PATH)/vo_file_buffer.cpp	\
	$(PDSRC_PATH)/vo_file_stream.cpp	\
	$(PDSRC_PATH)/vo_headerdata_buffer.cpp	\
	$(PDSRC_PATH)/vo_http_downloader.cpp	\
	$(PDSRC_PATH)/vo_largefile_buffer.cpp	\
	$(PDSRC_PATH)/vo_largefile_buffer_manager.cpp	\
	$(PDSRC_PATH)/vo_mem_stream.cpp	\
	$(PDSRC_PATH)/vo_PD_manager.cpp	\
	$(PDSRC_PATH)/vo_smallfile_buffer_manager.cpp	\
	$(PDSRC_PATH)/COpenSSL.cpp	\
	$(SRCCMN_PATH)/vo_thread.cpp	\
	$(TOP_CMN_PATH)/voThread.cpp	\
 	$(PDCMN_PATH)/CvoBaseMemOpr.cpp	\
	$(PDCMN_PATH)/CvoBaseFileOpr.cpp	\
	$(TOP_CMN_PATH)/CDllLoad.cpp	\
	$(TOP_CMN_PATH)/cmnMemory.c	\
	$(TOP_CMN_PATH)/cmnFile.cpp	\
	$(TOP_CMN_PATH)/CvoBaseObject.cpp	\
	$(TOP_CMN_PATH)/voCMutex.cpp	\
	$(TOP_CMN_PATH)/voOSFunc.cpp	\
	$(TOP_CMN_PATH)/voCSemaphore.cpp  \
	$(TOP_CMN_NET_PATH)/vo_socket.cpp	\
	$(PDSRC_PATH)/vo_playlist_parser.cpp	\
	$(PDSRC_PATH)/vo_playlist_m3u_parser.cpp	\
	$(PDSRC_PATH)/vo_playlist_pls_parser.cpp	\
	$(TOP_CMN_PATH)/voHalInfo.cpp \
	$(PDCMN_PATH)/voSourceBaseWrapper.cpp \
	$(PDSRC_PATH)/voSource2PDWrapper.cpp \
	$(TOP_CMN_PATH)/voLog.c	\
	$(PDSRC_PATH)/vo_http_authentication.cpp	\
	$(PDSRC_PATH)/vo_http_md5.cpp	\
	$(PDSRC_PATH)/voDLNA_Param.cpp \
	$(PDSRC_PATH)/vo_http_cookie.cpp \
	$(PDSRC_PATH)/vo_http_utils.cpp 
	
	
		


LOCAL_C_INCLUDES:= \
	$(TOP_INCLUDE_PATH)	\
	$(TOP_CMN_PATH)	\
	$(TOP_CMN_NET_PATH)	\
	$(SRCCMN_PATH) \
	$(PDSRC_PATH) \
	$(PDCMN_PATH)	\
	$(TOP_CMN_NET_PATH)/ares
	


VOMM:=-DPD -DLINUX -D_LINUX -D_LINUX_ANDROID -DLINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO -D__VO_NDK__ -D_PD_SOURCE2  -DENABLE_ASYNCDNS -DCARES_STATICLIB

# about info option, do not need to care it

LOCAL_CFLAGS := -D_VOMODULEID=0x00125000  -DNDEBUG -mfloat-abi=softfp -march=armv6j -mtune=arm1136jf-s -mfpu=vfp -fsigned-char -msoft-float
LOCAL_LDLIBS := -llog -ldl  -lgcc ../../../../../../../Lib/ndk/libvodl.so	../../../../../../../Lib/ndk/libvoCheck.a ../../../../../../../Lib/ndk/libcares.a

include $(VOTOP)/build/vondk.mk
include $(BUILD_SHARED_LIBRARY)

