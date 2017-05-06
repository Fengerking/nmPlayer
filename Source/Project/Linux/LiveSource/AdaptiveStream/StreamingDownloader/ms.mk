
# please list all objects needed by your module here
OBJS:=  voCMutex.o \
	CDllLoad.o \
	voOSFunc.o \
	vo_thread.o \
	voLog.o \
	ChunkDownloader.o \
	DownloadList.o \
	ManifestWriter.o \
	StreamingDownloader.o \
	voHLSManifestWriter.o \
	voStreamingDownloader.o \
	voStreamingDownloaderAPI.o

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../../Include \
../../../../../../../../../Common \
$(CSRC_PATH)/AdaptiveStreaming/Common \
$(CSRC_PATH)/AdaptiveStreaming/StreamingDownloader \
$(CSRC_PATH)/File/Common \
$(CSRC_PATH)/Common
