

CMNSRC_PATH:=../../../../../../../Common
CSRCMN_PATH:=../../../../../../Common
CSRC_PATH:=../../../Source
CDRMMN_PATH:=../../../../../Common
CAES128_PATH:=../../../../AES128_2/Source
CSRCIO_PATH:=../../../../../../IO

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSRCMN_PATH)/CSourceIOUtility.cpp \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CAES128_PATH)/aes-internal.c \
	$(CAES128_PATH)/aes-internal-dec.c \
	$(CAES128_PATH)/AES_CBC.cpp \
	$(CSRC_PATH)/voDRMCommonAES128.cpp \
	$(CSRC_PATH)/AES128_CommonAPI.cpp \
	$(CSRC_PATH)/VOAES128_CBC.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/CDllLoad.cpp \
	$(CMNSRC_PATH)/NetWork/vo_socket.cpp \
	$(CSRCMN_PATH)/vo_thread.cpp \

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../../Include \
	../../../../../../../Include \
	../../../../../../../Include/OSMP_V3 \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CMNSRC_PATH)/NetWork \
	$(CMNSRC_PATH)/NetWork/ares \
	$(CSRC_PATH) \
	$(CDRMMN_PATH) \
	$(CAES128_PATH) \
	$(CSRCIO_PATH)