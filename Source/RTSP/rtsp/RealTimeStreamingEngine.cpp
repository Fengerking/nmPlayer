#include <memory.h>
#include <string.h>

#include "utility.h"
#include "TaskSchedulerEngine.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "RealTimeStreamingEngine.h"
#include "RTPParser.h"
#include "RTCPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
//---------------------  CStreamReader  --------------------------

CStreamReader::CStreamReader(PIVOSocket streamSock, StreamReaderFunc * streamReader, void * streamReaderParam,int type)
{
	m_streamSock = streamSock;
	m_streamReader = streamReader;
	m_streamReaderParam = streamReaderParam;
	m_type = type;
}

CStreamReader::~CStreamReader()
{

}

//----------       CRealTimeStreamingEngine           ----------------------------------

CRealTimeStreamingEngine * CRealTimeStreamingEngine::m_pRealTimeStreamingEngine = NULL;

CRealTimeStreamingEngine * CRealTimeStreamingEngine::CreateRealTimeStreamingEngine()
{
	if(m_pRealTimeStreamingEngine == NULL)
	{
		m_pRealTimeStreamingEngine = new CRealTimeStreamingEngine();TRACE_NEW("rts_m_pRealTimeStreamingEngine",m_pRealTimeStreamingEngine);
		sprintf(CLog::formatString,"CreateRealTimeStreamingEngine():%X\n",(unsigned int)m_pRealTimeStreamingEngine);
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
	}

	return m_pRealTimeStreamingEngine;
}

void CRealTimeStreamingEngine::DestroyRealTimeStreamingEngine()
{
	if(m_pRealTimeStreamingEngine != NULL)
	{
		sprintf(CLog::formatString,"DestroyRealTimeStreamingEngine():%X\n",(unsigned int)m_pRealTimeStreamingEngine);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		SAFE_DELETE(m_pRealTimeStreamingEngine);
		m_pRealTimeStreamingEngine = NULL;
		
	}
}

CRealTimeStreamingEngine::CRealTimeStreamingEngine()
{
	sdp2File = NULL;
	streamData = NULL;
	memset(m_streamReaderSet, 0, sizeof(m_streamReaderSet));
	m_streamReaderCount = 0;
	VO_FD_ZERO(&m_streamSockSet);
}
void CRealTimeStreamingEngine::Reset()
{
	for(int i=0; i<m_streamReaderCount; ++i)
	{
		SAFE_DELETE(m_streamReaderSet[i]);
	}
	VO_FD_ZERO(&m_streamSockSet);
	m_streamReaderCount=0;
	memset(m_streamReaderSet, 0, sizeof(m_streamReaderSet));
}
CRealTimeStreamingEngine::~CRealTimeStreamingEngine()
{
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","~CRealTimeStreamingEngine()\n");
	Reset();
	if(sdp2File)
	{
		fclose(sdp2File);
		sdp2File = NULL;
	}
	if(streamData)
	{
		SAFE_DELETE_ARRAY(streamData);
		streamData = NULL;
	}
}

void CRealTimeStreamingEngine::AddStreamReader(PIVOSocket streamSock, StreamReaderFunc * streamReader, void * streamReaderParam,int type)
{
	for(int i=0; i<m_streamReaderCount; ++i)
	{
		if(m_streamReaderSet[i]->m_streamSock == streamSock)
		{
			m_streamReaderSet[i]->m_streamReader = streamReader;
			m_streamReaderSet[i]->m_streamReaderParam = streamReaderParam;
			return;
		}
	}

	m_streamReaderSet[m_streamReaderCount] = new CStreamReader(streamSock, streamReader, streamReaderParam,type);TRACE_NEW("rts_m_streamReaderSet",m_streamReaderSet[m_streamReaderCount]);
	SLOG3(LL_RTP_ERR,"flow.txt","\nAddStreamReader,%x,%d,%d\n",(unsigned int)m_streamReaderSet[m_streamReaderCount],m_streamReaderCount,type);

	m_streamReaderCount++;
	VO_FD_SET(streamSock, &m_streamSockSet);
}

CStreamReader * CRealTimeStreamingEngine::FindStreamReader(PIVOSocket streamSock)
{
	for(int i=0; i<m_streamReaderCount; ++i)
	{
		if(m_streamReaderSet[i]->m_streamSock == streamSock)
		{
			return m_streamReaderSet[i];
		}
	}

	return NULL;
}

void CRealTimeStreamingEngine::RemoveStreamReader(PIVOSocket streamSock)
{
	if(CUserOptions::UserOptions.m_useTCP>0)
		return;

	if(FindStreamReader(streamSock) == NULL)
		return;

	int streamIndex = 0;
	for(int i=0; i<m_streamReaderCount; ++i)
	{
		if(m_streamReaderSet[i]->m_streamSock == streamSock)
		{
			VO_FD_CLR(streamSock, &m_streamSockSet);
			SAFE_DELETE(m_streamReaderSet[i]);
			continue;
		}

		m_streamReaderSet[streamIndex++] = m_streamReaderSet[i];
	}

	m_streamReaderSet[--m_streamReaderCount] = NULL;
}

bool CRealTimeStreamingEngine::StreamsReachable(unsigned int millisecond)
{
	return true;
}
#define MAX_BUFSIZE2 4096*16
int	CRealTimeStreamingEngine::ReadRealTimeStreamsLocal()
{
	if(sdp2File==NULL)
		return 0;
	char type;
	int size;
	int readBytes = fread(&type,1,1,sdp2File);
	//if(readBytes<1||type<LOCAL_VIDEO_RTP||type>LOCAL_AUDIO_RTCP)
	//	return 0;

	readBytes = fread(&size,1,4,sdp2File);
	if(readBytes!=4)
	{
		//CLog::Log.MakeLog(LL_RTSP_ERR,"sdp2.txt", "file is end\n");
		return 0;
	}
	if(size>MAX_BUFSIZE2)
		CLog::Log.MakeLog(LL_RTSP_ERR,"sdp2.txt", "the packet is over MAX_BUFSIZE2 bytes\n");
	readBytes = fread(streamData,1,size,sdp2File);
	
	CStreamReader * streamReader = FindStreamReader((PIVOSocket)portMap[(int)type]);
	if(streamReader)
	{
		if(type==LOCAL_VIDEO_RTP||type==LOCAL_AUDIO_RTP
#ifdef DIMS_DATA
			||type==LOCAL_DIMS_RTP
#endif//DIMS_DATA
			)
		{
			CRTPParser   * rtpParser = (CRTPParser *)streamReader->m_streamReaderParam;
			rtpParser->ReadRTPStreamFromLocal(streamData,size);
			if(type==LOCAL_AUDIO_RTP)
			{
				IVOThread::Sleep(30);
			}
			else
			{
				IVOThread::Sleep(0);
			}
		}
		else
		{
			CRTCPParser   * rtcpParser = (CRTCPParser *)streamReader->m_streamReaderParam;
			rtcpParser->ReadRTCPStreamFromLocal(streamData,size);
		}
	}
	return 1;
}

int CRealTimeStreamingEngine::SetLocalStream(char* fileName)
{
	if(sdp2File==NULL)
	{
		streamData = new unsigned char[MAX_BUFSIZE2];TRACE_NEW("rts_streamData",streamData);
		return (sdp2File=fopen(fileName,"rb"))!=NULL;
	}
	else
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"sdp2.txt", "the file has exist\n");
		return 0;
	}
}
int CRealTimeStreamingEngine::ReadRealTimeStreams(int flag)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return ReadRealTimeStreamsLocal();
#if TRACE_CRASH_SOCKET_THREAD
	CLog::Log.MakeLog(LL_RTSP_ERR,"Socket Thread.txt","Get Data begin\n");
#endif
#if TRACE_CP_SOCKET1
	if(CUserOptions::UserOptions.m_bMakeLog>1)
	{
		SIMPLE_LOG(LL_SOCKET_ERR,"netdataStatus.txt","before");
	}
#endif//_DEBUG
	int readResult = 0;
	do 
	{
		fd_set streamSockSet = m_streamSockSet;
		struct votimeval tv_timeout;
		tv_timeout.tv_sec  = 1;
		tv_timeout.tv_usec = 0; 
		if(m_streamSockSet.fd_count==0)
		{
			CLog::Log.MakeLog(LL_RTSP_ERR,"socketerr.txt","$$$$$m_streamSockSet.fd_count==0");
			return 0;
		}

		if(flag&VOS_STREAM_RTSP)
			CTaskSchedulerEngine::CreateTaskSchedulerEngine()->HandleTask();
	

	    readResult = 0;//PACKET_RTCP;
		int selectResult = IVOSocket::SelectSockGroup(VSE_FD_READ,&streamSockSet,1000);

		if(selectResult <= 0)
		{
			return selectResult;
		}
		for(int i=0; i<streamSockSet.fd_count; ++i)
		{
			if(CUserOptions::UserOptions.m_bMakeLog>1)
				if(m_streamSockSet.fd_array[0]==streamSockSet.fd_array[i]||m_streamSockSet.fd_array[2]==streamSockSet.fd_array[i])
				{
					sprintf(CLog::formatString,"selectResult=%d,count=%d,sock=%lu\n",selectResult,streamSockSet.fd_count,
						streamSockSet.fd_array[i]->GetSockID()
						);
					CLog::Log.MakeLog(LL_SOCKET_WARN,"netdataStatus.txt",CLog::formatString);
				}
				CStreamReader * streamReader = FindStreamReader(streamSockSet.fd_array[i]);
				if(streamReader == NULL||(streamReader->m_type&flag)==0)
					continue;
				readResult |= streamReader->m_streamReader(streamReader->m_streamReaderParam);
		}
		//IVOThread::Sleep(0);
	} while (0);
	


#if TRACE_CRASH_SOCKET_THREAD
	CLog::Log.MakeLog(LL_RTSP_ERR,"Socket Thread.txt","Get Data begin end\n");
#endif	
	return readResult;
}	


