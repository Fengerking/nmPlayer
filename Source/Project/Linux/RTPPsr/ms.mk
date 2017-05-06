

OBJS:=base64.o \
	BitVector.o \
	isomio.o \
	isomutil.o \
	logtool.o \
	mp4cfg.o \
	strutil.o \
	DescrambleEngine.o \
	ParseEngine.o \
	parseutil.o \
	ReorderRTPPacket.o \
	RTPPacket.o \
	SynchronizeStreams.o \
	ParseEncMpeg4GenericAudioMode.o \
	ParseEncMpeg4GenericRTP.o \
	ParseEncMpeg4GenericVideoMode.o \
	ParseH264VideoRTP.o \
	ParseMp4aLatmRTP.o \
	ParseMpeg4GenericAudioMode.o \
	ParseMpeg4GenericMode.o \
	ParseMpeg4GenericRTP.o \
	ParseRichMediaXmlRTP.o \
	ParseRTCP.o \
	ParseRTP.o \
	sdp_error.o \
	sdp_media.o \
	sdp_parse.o \
	sdp_session.o \
	sdp_utility.o \	
	voRTPParser.o \
	AudioStream.o \
	DataStream.o \
	MediaStream.o \
	VideoStream.o \
	Lin2WinEvent.o \
	Lin2WinFile.o \
	Lin2WinPrivate.o \
	Lin2WinThread.o \
	vocrsapis.o \
	vosyncmutex.o \


VOSRCDIR:=../../../../../Include \
	../../../../MTV/DVBH/Include \
	../../../../MTV/DVBH/Common \
	../../../../MTV/DVBH/Common/voapi4win \
	../../../../MTV/DVBH/SDP \
	../../../../MTV/DVBH/Parser


