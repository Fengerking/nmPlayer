# Source files for VMAP file parser.
# Dolby Du
# 2013.07.11


# path variables for CAPTION file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
MTV_COMMON_PATH:=$(SOURCE_PATH)/MTV/Common
SOURCE_VMAP_PATH:=$(SOURCE_PATH)/VMAP
SOURCE_VAST_PATH:=$(SOURCE_PATH)/VAST


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
		$(TRUNK_INCLUDE_PATH) \
		$(TRUNK_COMMON_PATH) \
		$(FILE_COMMON_PATH) \
		$(MTV_COMMON_PATH) \
		$(SOURCE_VMAP_PATH) \
		$(SOURCE_VAST_PATH) \


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
	$(TRUNK_COMMON_PATH)/voXMLLoad.cpp \
	$(FILE_COMMON_PATH)/CvoBaseMemOpr.cpp \
	$(MTV_COMMON_PATH)/CDumper.cpp \
	$(SOURCE_VMAP_PATH)/voVMAPParser.cpp \
	$(SOURCE_VMAP_PATH)/CVMAPParser.cpp \
	$(SOURCE_VAST_PATH)/CVASTParser.cpp \

