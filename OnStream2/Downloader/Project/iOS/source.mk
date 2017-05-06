INC_PATH            :=../../../../../Include
CMNSRC_PATH         :=../../../../../Common
DLDCMNSRC_PATH	    :=../../../Source
DLDSRC_PATH	    :=../../../Source/iOS
OSTREAMIOS_V3_PATH  :=../../../../Player/Source/iOS/OSMP_V3
#V3_INC_PATH         :=../../../../Include/OSMP_V3/iOS


# please list all objects needed by your module here
VOMSRC:= $(DLDCMNSRC_PATH)/CDownloaderAPI.cpp \
    $(DLDCMNSRC_PATH)/CDownloaderImpl.cpp \
    \
    $(DLDSRC_PATH)/VODLDInitParam.mm \
    $(DLDSRC_PATH)/VODownloader.mm \
    $(DLDSRC_PATH)/VOOSMPDownloaderImpl.mm \
    \
    $(CMNSRC_PATH)/voLog.c \
    $(CMNSRC_PATH)/voOSFunc_iOS.mm \
    $(CMNSRC_PATH)/CDllLoad.cpp \
    $(CMNSRC_PATH)/CvoBaseObject.cpp \

# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=	$(INC_PATH) \
    $(INC_PATH)/OSMP_V3/iOS \
    $(CMNSRC_PATH) \
    $(DLDCMNSRC_PATH) \
    $(DLDSRC_PATH) \
    $(OSTREAMIOS_V3_PATH) \
#	$V3_INC_PATH \

