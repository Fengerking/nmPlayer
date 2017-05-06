# Source files for Contorller TWO file parser.
# rodney zhang
# 2012.08.09


# path variables for Contorller file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
ADSMANAGER_PATH:=$(SOURCE_PATH)/AdsManager/Source


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(SOURCE_PATH)/Common \
	$(SOURCE_PATH)/Include \
	$(ADSMANAGER_PATH)


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voCSemaphore.cpp \
	\
	$(SOURCE_PATH)/Common/CSourceIOUtility.cpp \
	$(SOURCE_PATH)/vo_thread.cpp \
	\
	$(ADSMANAGER_PATH)/AdsManager.cpp \
	$(ADSMANAGER_PATH)/vo_ads_manager.cpp \
	$(ADSMANAGER_PATH)/vo_ads_utility.cpp \
	$(ADSMANAGER_PATH)/vo_smil.cpp \
	$(ADSMANAGER_PATH)/vo_vmap.cpp \
	$(ADSMANAGER_PATH)/vo_message_thread.cpp

