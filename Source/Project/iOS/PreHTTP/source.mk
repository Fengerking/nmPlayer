

CMNSRC_PATH:=../../../../../Common
CSRCNW_PATH:=../../../../../Common/NetWork
CSRCMN_PATH:=../../../../Common
CSRC_PATH:=../../../../File/PreHTTP
CSRCPD_PATH:=../../../../File/PD2_Cookies

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/cmnVOMemory.cpp \
	$(CMNSRC_PATH)/CBaseConfig.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CSRCNW_PATH)/vo_socket.cpp \
	$(CSRC_PATH)/ASXParser.cpp \
	$(CSRC_PATH)/PreHTTP.cpp \
	$(CSRC_PATH)/voPreHTTP.cpp \
	$(CSRCMN_PATH)/vo_thread.cpp \
	$(CSRCPD_PATH)/vo_http_downloader.cpp \
	$(CSRCPD_PATH)/COpenSSL.cpp \
	$(CSRCPD_PATH)/voDLNA_Param.cpp \
	$(CSRCPD_PATH)/vo_http_cookie.cpp \
	$(CSRCPD_PATH)/vo_http_utils.cpp \
	$(CSRCPD_PATH)/vo_http_authentication.cpp \
	$(CSRCPD_PATH)/vo_http_md5.cpp \
	$(CSRCPD_PATH)/log.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../Include \
	../../../../../Thirdparty/OpenSSL \
	$(CMNSRC_PATH) \
	$(CSRCNW_PATH) \
	$(CSRC_PATH) \
	$(CSRCPD_PATH) \
	$(CSRCMN_PATH)