

CMNSRC_PATH:=../../../../../../../Common
CSRCMN_PATH:=../../../../../../Common
CSRCLOCALMN_PATH:=../../../../../../File/Common
CSRC_PATH:=../../../Source
CDRMMN_PATH:=../../../../../Common

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CSRCMN_PATH)/CSourceIOUtility.cpp \
	$(CSRCLOCALMN_PATH)/base64.cpp \
	$(CSRCLOCALMN_PATH)/strutil.cpp \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp\
	$(CDRMMN_PATH)/DRMStreaming.cpp \
	$(CSRC_PATH)/aes-internal.c \
	$(CSRC_PATH)/aes-internal-dec.c \
	$(CSRC_PATH)/AES_CBC.cpp \
	$(CSRC_PATH)/VO_AES128_SingleInstance.cpp \
	$(CSRC_PATH)/DRM_VisualOn_AES128.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../../Include \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRC_PATH) \
	$(CDRMMN_PATH) \
	$(CSRCLOCALMN_PATH)