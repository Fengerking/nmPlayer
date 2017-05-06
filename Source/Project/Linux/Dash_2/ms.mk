
# please list all objects needed by your module here
OBJS:=  voCMutex.o \
	voOSFunc.o \
	CDllLoad.o \
	voThread.o \
	voXMLLoad.o \
	CvoBaseObject.o \
	fortest.o \
	vo_socket.o \
	vo_http_stream.o \
	vo_mpd_reader.o \
	vo_mpd_manager.o \
	vo_stream.o \
	vo_mem_stream.o \
	vo_mpd_streaming.o \
	CLiveSrcDASH.o \
	voLiveSource.o \
	CLiveSrcBase.o \
	CLiveParserBase.o \
	BaseUrl.o \
	Common_Tag.o\
	contentComponent_Tag.o\
	Group_Tag.o\
	Metrics.o\
	Mpd_tag.o\
	Period_Tag.o\
	ProgramInformation.o\
	Repre_Tag.o\
	Role_Tag.o\
	SegInfo_Tag.o\
	SegList_Tag.o\
	SegmentBase_Tag.o\
	SegmentTemplate_Tag.o\
	vo_AdaptionSet.o\
	vo_MPD.o\
	vo_mpd_common.o\
	ConfigFile.o \
	cmnMemory.o \
	cmnFile.o \
	tinystr.o \
	tinyxml.o \
	tinyxmlerror.o \
	tinyxmlparser.o \
	ChannelInfo.o \
	voutf8conv.o \
	vo_network_judgment.o \
	vo_thread.o	\
	CPtrList.o 



# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../Source/dash2 \
	      ../../../../../../Include \
			 ../../../../../../Include/vome \
		  ../../../../../../Common \
		 ../../../../../../Common/NetWork \
		../../../../../../MFW/voME/Common \
		../../../../../../Source/File/XML \
		../../../../../../Source/MTV/LiveSource/Common \
		../../../../../../Source/MTV/CMMB/TinyXML \
		../../../../../../Utility/voutf8conv \
		../../../../../../Source/File/Common
		
		
			

