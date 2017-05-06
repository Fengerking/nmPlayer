# Source files for XML file parser.
# rodney zhang
# 2012.03.21


# path variables for XML file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File
FILE_COMMON_PATH:=$(SOURCE_FILE_PATH)/Common
XML_SOURCE_PATH:=$(SOURCE_FILE_PATH)/XML


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(FILE_COMMON_PATH) \
	$(XML_SOURCE_PATH) \

	

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(TRUNK_COMMON_PATH)/CPtrList.cpp \
	$(TRUNK_COMMON_PATH)/XMLSaxDefaultHandler.cpp \
	$(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	\
	$(XML_SOURCE_PATH)/XMLOP.cpp \
	$(XML_SOURCE_PATH)/voXMLParser.cpp \

	
