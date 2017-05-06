

CMNSRC_PATH:=../../../../../Common
CSRC_PATH:=../../../../File/WindowsMediaHTTPStreaming
CSRCMN_PATH:=../../../../Common
CSRCLOCALMN_PATH:=../../../../File/Common


# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voProgramInfo.cpp \
	$(CSRCBUf_PATH)/voSourceBufferManager.cpp \
	$(CSRCBUf_PATH)/voSourceDataBuffer.cpp \
	$(CSRCBUf_PATH)/voSourceSubtitleDataBuffer.cpp \
	$(CSRCBUf_PATH)/voSourceVideoDataBuffer.cpp \
	$(CSRCLOCALMN_PATH)/voSourceBaseWrapper.cpp \
	$(CSRC_PATH)/HTTP_Transaction.cpp \
	$(CSRC_PATH)/StreamCache.cpp \
	$(CSRC_PATH)/TCPClientSocket.cpp \
	$(CSRC_PATH)/Thread.cpp \
	$(CSRC_PATH)/voWMSP2.cpp \
	$(CSRC_PATH)/WMHTTP.cpp \
	$(CSRC_PATH)/WMSP2Manager.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../Include \
	../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRCMN_PATH)/Buffer \
	$(CSRCLOCALMN_PATH) \
	$(CSRC_PATH)