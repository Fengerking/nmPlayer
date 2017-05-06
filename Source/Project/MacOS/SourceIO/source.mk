
# path variables for SourceIO.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
CMNSRC_PATH:=$(TRUNK_PATH)/Common
CMNSRC_NETWORK_PATH=$(CMNSRC_PATH)/NetWork
SOURCE_PATH:=$(TRUNK_PATH)/Source
CSR_CMN_PATH=$(SOURCE_PATH)/Common
CSRC_PATH=$(SOURCE_PATH)/IO
ARES_HEAD_PATH:=$(CMNSRC_NETWORK_PATH)/ares

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/cmnFile.cpp \
	$(CMNSRC_PATH)/voCSemaphore.cpp \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSR_CMN_PATH)/vo_thread.cpp \
	$(CMNSRC_NETWORK_PATH)/vo_socket.cpp \
	$(CSRC_PATH)/voSourceIO.cpp \
	$(CSRC_PATH)/CSourceIOLocalFile.cpp \
	$(CSRC_PATH)/DRMLocal.cpp \
	$(CSRC_PATH)/vo_http_session.cpp \
	$(CSRC_PATH)/vo_http_stream.cpp \
	$(CSRC_PATH)/vo_mem_stream.cpp \
	$(CSRC_PATH)/vo_http_cookie.cpp \
	$(CSRC_PATH)/vo_http_sessions_info.cpp	\
	$(CSRC_PATH)/vo_http_authentication.cpp \
	$(CSRC_PATH)/vo_http_utils.cpp \
	$(CSRC_PATH)/vo_http_md5.cpp	\
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CSRC_PATH)/COpenSSL.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
		$(TRUNK_INCLUDE_PATH) \
		$(CMNSRC_PATH) \
		$(CMNSRC_NETWORK_PATH) \
		$(SOURCE_PATH) \
		$(CSR_CMN_PATH) \
		$(CSRC_PATH) \
		$(ARES_HEAD_PATH)
