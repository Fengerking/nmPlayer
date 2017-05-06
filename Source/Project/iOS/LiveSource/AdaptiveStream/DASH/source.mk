# Source files for DASH TWO file parser.
# rodney zhang
# 2012.08.09


# path variables for DASH file parser.
TRUNK_PATH:=../../../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
TRUNK_COMMON_NETWORK_PATH:=$(TRUNK_COMMON_PATH)/NetWork
TRUNK_UTILITY_VOUTF8CONV_PATH:=$(TRUNK_PATH)/Utility/voutf8conv

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
SOURCE_FILE_COMMON_PATH:=$(SOURCE_PATH)/File/Common
SOURCE_ADAPTIVESTREAMING_COMMON_PATH:=$(SOURCE_PATH)/AdaptiveStreaming/Common
DASH_SOURCE_PATH:=$(SOURCE_PATH)/AdaptiveStreaming/Dash


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(TRUNK_COMMON_BUFFER_PATH) \
	$(TRUNK_COMMON_NETWORK_PATH) \
	$(TRUNK_UTILITY_VOUTF8CONV_PATH) \
	$(SOURCE_FILE_COMMON_PATH) \
	$(SOURCE_ADAPTIVESTREAMING_COMMON_PATH) \
	$(DASH_SOURCE_PATH) \
	$(SOURCE_PATH)/Common \
	$(SOURCE_PATH)/Include \
	$(SOURCE_FILE_PATH)/XML \


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	\
	$(SOURCE_PATH)/Common/vo_thread.cpp \
	$(SOURCE_PATH)/Common/CDataBox.cpp \
	\
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voXMLLoad.cpp \
	$(TRUNK_COMMON_PATH)/XMLSaxDefaultHandler.cpp \
	\
	$(DASH_SOURCE_PATH)/VO_MPD_Parser.cpp \
	$(DASH_SOURCE_PATH)/C_MPD_Manager.cpp \
	$(DASH_SOURCE_PATH)/C_DASH_Entity.cpp \
	$(DASH_SOURCE_PATH)/Mpd_tag.cpp \
	$(DASH_SOURCE_PATH)/BaseUrl.cpp \
	$(DASH_SOURCE_PATH)/Common_Tag.cpp \
	$(DASH_SOURCE_PATH)/contentComponent_Tag.cpp \
	$(DASH_SOURCE_PATH)/Group_Tag.cpp \
	$(DASH_SOURCE_PATH)/Metrics.cpp \
	$(DASH_SOURCE_PATH)/Period_Tag.cpp \
	$(DASH_SOURCE_PATH)/ProgramInformation.cpp \
	$(DASH_SOURCE_PATH)/Repre_Tag.cpp \
	$(DASH_SOURCE_PATH)/Role_Tag.cpp \
	$(DASH_SOURCE_PATH)/SegInfo_Tag.cpp \
	$(DASH_SOURCE_PATH)/SegList_Tag.cpp \
	$(DASH_SOURCE_PATH)/SegmentBase_Tag.cpp \
	$(DASH_SOURCE_PATH)/SegmentTemplate_Tag.cpp \
	$(DASH_SOURCE_PATH)/SubRepre_Tag.cpp \
	$(DASH_SOURCE_PATH)/voAdaptiveStreamDASH.cpp
