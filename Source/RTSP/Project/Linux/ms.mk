
# please list all objects needed by your module here
OBJS:=RTCPParser.o RTCPReportPacket.o RTPPacket.o RTPPacketReorder.o RTPParser.o RTPSenderTable.o RTSPClientEngine.o RTSPSession.o\
RTSPSource.o RTSPTrack.o TaskSchedulerEngine.o voRTSPSource.o HashTable.o network.o utility.o VOCrossPlatformWrapper_Linux.o\
AMRAudioRTPParser.o AMRWBPAudioRTPParser.o ASFHeadParser.o ASFRTPParser.o ASFRTSPClientEngine.o DIMSRTPParser.o H263VideoRTPParser.o H264VideoRTPParser.o\
MediaFrameSink.o mediastream.o MediaStreamSocket.o MPEG4ESVideoRTPParser.o MPEG4GenericAudioRTPParser.o MPEG4LATMAudioRTPParser.o QcelpAudioRTPParser.o RDTMediaStream.o\
RDTParser.o RealRTSPClientEngine.o RealTimeStreamingEngine.o RTSPMediaStreamSyncEngine.o voRTSPSDK2.o BitVector.o ParseSPS.o vostream_digest_auth.o\
MP3AudioRTPParser.o helix_date.o md5.o md5hl.o memory_utils.o pack_utils.o ra_depack.o ra_depack_internal.o\
rasl.o rdtpck.o rm_error_default.o rm_io_default.o rm_memory_default.o rm_packet.o rm_property.o rm_stream.o\
rv_depack.o rv_depack_internal.o stream_hdr_utils.o string_utils.o tngpktrtsp.o getmp4vol.o challenge.o\
Lin2WinEvent.o Lin2WinThread.o vosyncmutex.o vocrsapis.o Lin2WinPrivate.o VOUtility.o voLog_android.o\
voSource2ParserWrapper.o voLog.o




# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../Include \
		../../../base \
		../../../rtsp \
		../../../engine_sdk \
		../../../third_party \
		../../../third_party/real_streaming/include \
		../../../third_party/real_streaming/src \
		../../../third_party/real_streaming/src/common \
		../../../third_party/real_streaming/src/common/pub \
		../../../third_party/real_streaming/src/rdt \
		../../../third_party/real_streaming/src/rv_depack \
		../../../third_party/real_streaming/voSDKInc \
		../../../voapi4win/voapi4win \
		../../../voapi4win 	
		


