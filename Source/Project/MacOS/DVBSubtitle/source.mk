TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source/DVBSubtitle

VOSRCDIR:= $(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(SOURCE_PATH)

VOMSRC:= \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(SOURCE_PATH)/CSubtitlePacker.cpp \
	$(SOURCE_PATH)/DVBParser.cpp \
	$(SOURCE_PATH)/tools.cpp \
	$(SOURCE_PATH)/voDVBSubtitleParser.cpp
