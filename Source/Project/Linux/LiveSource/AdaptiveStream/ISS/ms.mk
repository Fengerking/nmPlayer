
# please list all objects needed by your module here
OBJS:=  voCMutex.o \
	CDllLoad.o \
	voHalInfo.o \
	voOSFunc.o \
	CvoBaseObject.o \
	fCodec.o \
	vo_thread.o \
	voLog.o \
	CCodeCC.o \
	voXMLLoad.o \
	ISS_ManifestParser.o \
	ISS_ManifestManager.o \
	ISS_SmoothStreaming.o \
	voAdaptiveStreamISS.o


# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../../Include \
../../../../../../../../../Common \
$(CSRC_PATH)/AdaptiveStreaming/Common \
$(CSRC_PATH)/AdaptiveStreaming/ISS \
$(CSRC_PATH)/File/XML \
$(CSRC_PATH)/File/Common \
$(CSRC_PATH)/Common
