

CMNSRC_PATH:=../../../../../../../Common
CSRC_PATH:=../../../Source
CDRMMN_PATH:=../../../../../Common

# please list all objects needed by your module here
VOMSRC:= \
	$(CMNSRC_PATH)/voLog.c \
	$(CDRMMN_PATH)/DRM.cpp \
	$(CDRMMN_PATH)/voDRM.cpp \
	$(CSRC_PATH)/DRMAdapter.cpp

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	../../../../../../Include \
	../../../../../../../Include \
	../../../../../../../Include/OSMP_V3 \
	../../../../../../Common \
	$(CMNSRC_PATH) \
	$(CSRC_PATH) \
	$(CDRMMN_PATH)