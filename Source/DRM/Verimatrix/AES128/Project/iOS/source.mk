

CMNSRC_PATH:=../../../../../../../Common
CSRCMN_PATH:=../../../../../../Common
CSRCLOCALMN_PATH:=../../../../../../File/Common
CSRC_PATH:=../../../Source
CDRMMN_PATH:=../../../../../Common
CAES128_PATH:=../../../../../VisualOn/AES128_2/Source

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CMNSRC_PATH)/voCMutex.cpp \
	$(CSRCMN_PATH)/CSourceIOUtility.cpp \
	$(CSRCLOCALMN_PATH)/base64.cpp \
	$(CSRCLOCALMN_PATH)/strutil.cpp \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp\
	$(CDRMMN_PATH)/DRMStreaming.cpp \
	$(CAES128_PATH)/aes-internal.c \
	$(CAES128_PATH)/aes-internal-dec.c \
	$(CAES128_PATH)/AES_CBC.cpp \
	$(CSRC_PATH)/voSecurityCBObj.mm \
	$(CSRC_PATH)/DRM_Verimatrix_AES128_iOS.mm

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../../Include \
	../../../../../../../Include \
	../../../../../../../Thirdparty/Verimatrix/inc \
	../../../../../../../Thirdparty/Import/OpenSSL/include \
	$(CMNSRC_PATH) \
	$(CSRCMN_PATH) \
	$(CSRC_PATH) \
	$(CAES128_PATH) \
	$(CDRMMN_PATH) \
	$(CSRCLOCALMN_PATH)