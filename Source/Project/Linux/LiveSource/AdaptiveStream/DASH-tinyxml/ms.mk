
# please list all objects needed by your module here
OBJS:=  voCMutex.o \
	voOSFunc.o \
	CvoBaseObject.o \
	voCMutex.o \
	VO_MPD_Parser.o \
	C_MPD_Manager.o \
	C_DASH_Entity.o \
	voAdaptiveStreamDASH.o \
	Mpd_tag.o \
	BaseUrl.o \
	Common_Tag.o \
	contentComponent_Tag.o \
	Group_Tag.o \
	Metrics.o \
	Period_Tag.o \
	ProgramInformation.o \
	Repre_Tag.o \
	Role_Tag.o \
	SegInfo_Tag.o \
	SegList_Tag.o \
	SegmentBase_Tag.o \
	SegmentTemplate_Tag.o \
	SubRepre_Tag.o \
	vo_AdaptionSet.o \
	tinystr.o \
	tinyxml.o \
	tinyxmlerror.o \
	tinyxmlparser.o \
	ChannelInfo.o \
	voutf8conv.o \
	vo_network_judgment.o \
	CPtrList.o 



# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../../../../../Include \
../../../../../../../../../Include/vome \
../../../../../../../../../Common \
 ../../../../../../../../../Thirdparty/ndk \
../../../../../../../../../Common \
../../../../../../../../../Common/NetWork \
../../../../../../../../../MFW/voME/Common \
$(CSRC_PATH)/AdaptiveStreaming/Include \
$(CSRC_PATH)/AdaptiveStreaming/Dash \
$(CSRC_PATH)/File/Common \
$(CSRC_PATH)/File/Common/Utility \
$(CSRC_PATH)/MTV/CMMB/TinyXML \
$(CSRC_PATH)/File/XML \

		
			

