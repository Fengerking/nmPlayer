

CMNSRC_PATH:=../../../../../../../Common
CSRCMN_PATH:=../../../../../../Common
CSRCLOCALMN_PATH:=../../../../../../File/Common
CSRC_PATH:=../../../Source
CDRMMN_PATH:=../../../../../Common

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/voThread.cpp \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSRCMN_PATH)/CSourceIOUtility.cpp \
	$(CSRCLOCALMN_PATH)/base64.cpp \
	$(CSRCLOCALMN_PATH)/strutil.cpp \
	$(CMNSRC_PATH)/voHalInfo.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/DRMStreaming.cpp \
	$(CDRMMN_PATH)/voDRM.cpp\
	$(CSRC_PATH)/DRM_Discretix_PlayReady.cpp


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../../Include \
	../../../../../../../Include \
	../../../../../../../Thirdparty/Discretix/inc \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRC_PATH) \
	$(CSRC_PATH)/inc \
	$(CDRMMN_PATH) \
	$(CSRCLOCALMN_PATH)

