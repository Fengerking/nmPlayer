# Source files for smooth streaming file parser.
# Aiven Yang
# 2013.07.12


# path variables for smooth streaming file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
SMIL_SOURCE_PATH:=$(SOURCE_FILE_PATH)/Smil

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:= \
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(FILE_COMMON_PATH) \
	$(SMIL_SOURCE_PATH) \
	$(SOURCE_PATH)/Common \
	$(SOURCE_PATH)/Include

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voXMLLoad.cpp \
	\
	$(SMIL_SOURCE_PATH)/CBS_SMIL.cpp\
	$(SMIL_SOURCE_PATH)/Smil.cpp \
	$(SMIL_SOURCE_PATH)/voGetSmilAPI.cpp \
	\
