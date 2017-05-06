# Source files for CAPTION file parser.
# rodney zhang
# 2012.03.28


# path variables for CAPTION file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
MTV_COMMON_PATH:=$(SOURCE_PATH)/MTV/Common
MTV_CAPTION_PATH:=$(SOURCE_PATH)/MTV/ClosedCaption


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(FILE_COMMON_PATH) \
		$(MTV_COMMON_PATH) \
		$(MTV_CAPTION_PATH)


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/cmnFile.cpp \
	$(TRUNK_COMMON_PATH)/cmnVOMemory.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(MTV_COMMON_PATH)/CDumper.cpp \
	$(MTV_CAPTION_PATH)/CCaptionParser.cpp \
	$(MTV_CAPTION_PATH)/CCCH264PacketParser.cpp \
	$(MTV_CAPTION_PATH)/CCCharParser.cpp \
	$(MTV_CAPTION_PATH)/CCCPacketParser.cpp \
	$(MTV_CAPTION_PATH)/CHTMLPacker.cpp \
	$(MTV_CAPTION_PATH)/ParseSEI.cpp \
	$(MTV_CAPTION_PATH)/voCaptionParser.cpp \
	$(MTV_CAPTION_PATH)/CC708CharParser.cpp \
	$(MTV_CAPTION_PATH)/CCCharData.cpp \
	$(MTV_CAPTION_PATH)/CSubtilePacker.cpp
