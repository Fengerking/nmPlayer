#include <memory.h>
#include <string.h>

#include "utility.h"
#include "TaskSchedulerEngine.h"
#include "RealTimeStreamingEngine.h"
#include "RTPParser.h"
#include "RTCPParser.h"

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
		m_pRealTimeStreamingEngine = new CRealTimeStreamingEngine();
	}

	return m_pRealTimeStreamingEngine;
}

void CRealTimeStreamingEngine::DestroyRealTimeStreamingEngine()
{
	if(m_pRealTimeStreamingEngine != NULL)
	{
		SAFE_DELETE(m_pRealTimeStreamingEngine);
	}
}

CRealTimeStreamingEngine::CRealTimeStreamingEngine()
{
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
	m_streamReaderCount = 0;
	memset(m_streamReaderSet, 0, sizeof(m_streamReaderSet));
}

CRealTimeStreamingEngine::~CRealTimeStreamingEngine()
{
	Reset();
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

	m_streamReaderSet[m_streamReaderCount] = new CStreamReader(streamSock, streamReader, streamReaderParam, type);
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

int CRealTimeStreamingEngine::ReadRealTimeStreams(int flag)
{	
	VOSocketGroup streamSockSet = m_streamSockSet;
	struct timeval tv_timeout;
	tv_timeout.tv_sec  = 0;
	tv_timeout.tv_usec = 300; 
	
	if(flag & VOS_STREAM_RTSP)
		CTaskSchedulerEngine::CreateTaskSchedulerEngine()->HandleTask();

	
	int readResult = 0; //PACKET_RTCP;
	int selectResult = IVOSocket::SelectSockGroup(VSE_FD_READ, &streamSockSet, 50);
	if(selectResult <= 0)
		return selectResult;
	
	for(int i=0; i<streamSockSet.fd_count; ++i)
	{
		CStreamReader * streamReader = FindStreamReader(streamSockSet.fd_array[i]);
		if(streamReader == NULL)
			continue;
		readResult |= streamReader->m_streamReader(streamReader->m_streamReaderParam);
		//SLOG2(LL_RTP_ERR,"netPoll.txt","%d:sock=%d\n",streamSockSet.fd_count,streamSockSet.fd_array[i]->GetSockID());
	}
		
	return readResult;
}
