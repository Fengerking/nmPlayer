# Source files for DASH file parser.
# rodney zhang
# 2012.05.17


# path variables for DASH file parser.
TRUNK_PATH:=../../../../../../..
TRUNK_INCLUDE_PATH:=$(TRUNK_PATH)/Include
TRUNK_COMMON_PATH:=$(TRUNK_PATH)/Common
SOURCE_PATH:=$(TRUNK_PATH)/Source



# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	\
$(TRUNK_INCLUDE_PATH) \
$(TRUNK_COMMON_PATH) \
$(SOURCE_PATH)/AdaptiveStreaming/Common \
$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader \
$(SOURCE_PATH)/AdaptiveStreaming/Controller/Source \
$(SOURCE_PATH)/AdaptiveStreaming/DRM \
$(SOURCE_PATH)/File/Common \
$(SOURCE_PATH)/Common	\
$(SOURCE_PATH)/Include

# please list all objects needed by your module here
# List needs to be coherent, Don't insert extra comment line.
# Don't add the Common files to list, add them to the corresponding Common library
VOMSRC:= \
  $(TRUNK_COMMON_PATH)/CvoBaseObject.cpp \
  $(TRUNK_COMMON_PATH)/voLog.c \
	$(TRUNK_COMMON_PATH)/CDllLoad.cpp \
	$(TRUNK_COMMON_PATH)/voHalInfo.cpp \
	$(TRUNK_COMMON_PATH)/voCMutex.cpp \
	$(TRUNK_COMMON_PATH)/voOSFunc_iOS.mm \
	$(TRUNK_COMMON_PATH)/voProgramInfo.cpp \
	$(SOURCE_PATH)/Common/vo_thread.cpp \
	$(SOURCE_PATH)/Common/CDataBox.cpp \
	$(SOURCE_PATH)/Common/CSourceIOUtility.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/Controller/Source/voAdaptiveStreamParserWrapper.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/ChunkDownloader.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/DownloadList.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/ManifestWriter.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/StreamingDownloader.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/voHLSManifestWriter.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/voStreamingDownloader.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/AdaptDataSource.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/voStreamingDownloaderV3.cpp \
	$(SOURCE_PATH)/AdaptiveStreaming/StreamingDownloader/voStreamingDownloaderAPI.cpp

