
# please list all objects needed by your module here
OBJS:=CBuffering.o CPDSessionWrapper2.o CPDTrackWrapper2.o CBaseReadIn.o CFileReadIn.o\
CMemReadIn.o ASFHeadParser.o cmdhdl.o CSCHttpCmdHandle.o CThroughputer.o CWMSHttpCmdHandle.o CWriteThread.o ftpcmdhdl.o\
ftpget.o httpcmdhdl.o httpget.o netget.o netstrength.o voNetGet.o CBaseSink.o VOCrossPlatformWrapper_Linux.o\
CFileSink.o CMemSink.o CDFInfo.o CDownloader.o CHttpDownloadFile.o CNetGet.o CPDEventThread.o UFileReader.o\
VOUtility.o Lin2WinEvent.o Lin2WinThread.o vosyncmutex.o vocrsapis.o Lin2WinPrivate.o CDllLoad2.o CvoBaseObject.o voOSFunc.o voLog_android.o






# please list all directories that all source files relative with your module(.h .c .cpp) locate 
VOSRCDIR:=../../../../../Include \
		../../../../../Common \
		../../../voPDEngine \
		../../../voPDEngine/engine_head \
		../../../voPDEngine/Protocol \
		../../../third_party \
		../../../Platform/g1 \
		../../../../RTSP/voapi4win/voapi4win \
		../../../../RTSP/voapi4win/	
		


