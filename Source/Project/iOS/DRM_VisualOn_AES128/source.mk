# Source files for DRM VisualOn AES128.
# rodney zhang
# 2012.06.08


# path variables for AUDIO file parser.
TRUNK_PATH:=../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common

SOURCE_PATH:=$(TRUNK_PATH)/Source
SOURCE_FILE_PATH:=$(SOURCE_PATH)/File

SOURCE_DRM_COMMON_PATH:=$(SOURCE_PATH)/DRM/Common
DRM_SOURCE_PATH:=$(SOURCE_PATH)/DRM/VisualOn/AES128/Source


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
	$(TRUNK_INCLUDE_PATH) \
	$(TRUNK_COMMON_PATH) \
	$(SOURCE_DRM_COMMON_PATH) \
	$(DRM_SOURCE_PATH)


# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
	$(SOURCE_DRM_COMMON_PATH)/DRM.cpp \
	$(SOURCE_DRM_COMMON_PATH)/voDRM.cpp\
	\
	$(DRM_SOURCE_PATH)/DRM_VisualOn_AES128.cpp
