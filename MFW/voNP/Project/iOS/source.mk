

NPSRC_PATH   :=../../../Source
CMNSRC_PATH  :=../../../../../Common

# please list all objects needed by your module here
VOMSRC:=    $(NPSRC_PATH)/voHLSWrapAPI.cpp \
    $(NPSRC_PATH)/CBaseNativePlayer.cpp \
    $(NPSRC_PATH)/CBaseServer.cpp \
    $(NPSRC_PATH)/CBaseWrap.cpp \
    $(NPSRC_PATH)/CHLSHttpRequest.cpp \
    $(NPSRC_PATH)/CHLSHttpRespond.cpp \
    $(NPSRC_PATH)/CHLSHttpSocket.cpp \
    $(NPSRC_PATH)/CHLSListenSocket.cpp \
    $(NPSRC_PATH)/CHLSM3u8Stream.cpp \
    $(NPSRC_PATH)/CHLSServer.cpp \
    $(NPSRC_PATH)/CHLSServerWrap.cpp \
    $(NPSRC_PATH)/CMemStream.cpp \
    $(NPSRC_PATH)/CTsMuxStream.cpp \
    $(NPSRC_PATH)/CAVFrameTools.cpp \
    $(NPSRC_PATH)/CAVFrameRW.cpp \
    $(NPSRC_PATH)/voOSTools.cpp \
    $(NPSRC_PATH)/iOSApiWrapper.mm \
    $(NPSRC_PATH)/iOS/CAVNativePlayer.mm \
    $(NPSRC_PATH)/iOS/CAVPlayer.mm \
    \
    $(CMNSRC_PATH)/CPtrList.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \
    $(CMNSRC_PATH)/fVideoHeadDataInfo.cpp \
    $(CMNSRC_PATH)/voCMutex.cpp \
    $(CMNSRC_PATH)/voCSemaphore.cpp \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/voThread.cpp \
    $(CMNSRC_PATH)/cmnFile.cpp \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNSRC_PATH)/voHalInfo.cpp \
    \
    $(CMNSRC_PATH)/NetWork/vo_socket.cpp \
    \
    $(CMNSRC_PATH)/iOS/voRunRequestOnMain.mm \
    \
    $(CMNSRC_PATH)/httpSvr/CBaseSocket.cpp \
    $(CMNSRC_PATH)/httpSvr/CHttpRequest.cpp \
    $(CMNSRC_PATH)/httpSvr/CHttpRespond.cpp \
    $(CMNSRC_PATH)/httpSvr/CHttpSocket.cpp \
    $(CMNSRC_PATH)/httpSvr/CHttpStream.cpp \
    $(CMNSRC_PATH)/httpSvr/CListenSocket.cpp \
   
# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		../../../../../Include \
		$(NPSRC_PATH) \
		$(CMNSRC_PATH) \
		../../Common \
		$(CMNSRC_PATH)/httpSvr \
		$(CMNSRC_PATH)/NetWork \
		$(CMNSRC_PATH)/iOS \
		$(NPSRC_PATH)/iOS \

