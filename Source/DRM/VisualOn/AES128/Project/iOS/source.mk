

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../Source
CDRMMN_PATH:=../../../../../Common

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/CvoBaseObject.cpp \
	$(CMNSRC_PATH)/voLog.c \
	$(CMNSRC_PATH)/voOSFunc_iOS.mm \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp\
	$(CSRC_PATH)/DRM_VisualOn_AES128.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../../../Include \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CDRMMN_PATH)