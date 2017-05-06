#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "ReorderRTPPacket.h"
#include "RTPPacket.h"
#include "SynchronizeStreams.h"
#include "DescrambleEngine.h"
#include "ParseRTP.h"

#ifdef _RTPPSR_LOG
#include "logtool.h"
#endif //_RTPPSR_LOG

//#define _BURST

CParseRTP::CParseRTP(CMediaStream * pMediaStream)
: m_pMediaStream(pMediaStream)
, m_pParserInitInfo(NULL)
, m_pReorderRTPPacket(NULL)
, m_lastRTPTimestamp(0)
, m_pDescrambleEngine(NULL)
, m_cFrame(0)
, m_sysTime(0)
{
	memset(&m_FrameBuffer, 0, sizeof(VO_MTV_FRAME_BUFFER));
	memset(&m_OutputBuffer, 0, sizeof(VO_PARSER_OUTPUT_BUFFER));
}

CParseRTP::~CParseRTP()
{
	SAFE_DELETE(m_pReorderRTPPacket);
	SAFE_DELETE(m_pDescrambleEngine);
	SAFE_DELETE_ARRAY(m_FrameBuffer.pData);
}

VO_U32 CParseRTP::Init()
{
	m_pParserInitInfo = m_pMediaStream->GetParserInitInfo();
	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	m_pReorderRTPPacket = new CReorderRTPPacket();
	if(m_pReorderRTPPacket == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	m_pDescrambleEngine = new CDescrambleEngine();
	if(m_pDescrambleEngine == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;
	if(!m_pDescrambleEngine->Init(pSDPMedia))
		return VO_ERR_PARSER_ERROR;

	STREAMINFO * pStreamInfo = m_pMediaStream->GetStreamInfo();
	m_FrameBuffer.pData = new VO_BYTE[pStreamInfo->nMaxFrameSize];
	if(m_FrameBuffer.pData == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;
	m_FrameBuffer.nPos = VO_MTV_FRAME_POS_WHOLE;
	m_FrameBuffer.nCodecType = pStreamInfo->nCodecType;

	m_OutputBuffer.nStreamID = m_pMediaStream->GetStreamId();
	m_OutputBuffer.nType = pStreamInfo->nMediaType;
	m_OutputBuffer.pOutputData = &m_FrameBuffer;
	m_OutputBuffer.pUserData = m_pParserInitInfo->pUserData;
	m_OutputBuffer.pReserved = NULL;
	
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseRTP::Process(unsigned char * _rtpData, int rtpDataSize)
{
	CRTPPacket * pRTPPacket = NULL;

	do
	{
		pRTPPacket = m_pReorderRTPPacket->GetEmptyRTPPacket();
		if(pRTPPacket == NULL)
			break;
		pRTPPacket->AssignRTPData(_rtpData, rtpDataSize);
		if(!m_pDescrambleEngine->DescrambleRTPPacket(pRTPPacket))
			break;
		if(!pRTPPacket->ParseRTPCommonHeader())
			break;

#ifdef _RTPPSR_LOG
//	DumpRTPInfo(pRTPPacket);
#endif //_RTPPSR_LOG

		m_pReorderRTPPacket->ReorderRTPPacket(pRTPPacket);

		return ParseRTPPayload();

	}while(0);

	m_pReorderRTPPacket->ReleaseRTPPacket(pRTPPacket);

	return VO_ERR_PARSER_ERROR;
}

VO_U32 CParseRTP::ParseRTPPayload()
{
	return VO_ERR_PARSER_OK;
}

bool CParseRTP::DetectStreamLoop(CRTPPacket * pRTPPacket)
{
	if(m_lastRTPTimestamp == 0)
	{
		m_lastRTPTimestamp = pRTPPacket->Timestamp();
	}

	int timestampDiff = pRTPPacket->Timestamp() - m_lastRTPTimestamp;
	m_lastRTPTimestamp = pRTPPacket->Timestamp();
	double timeDiff = timestampDiff / (double)m_pMediaStream->m_clockRate;
	if(timeDiff >= 10.0 || timeDiff <= -10.0)
	{
		CSynchronizeStreams * pSyncStreams = CSynchronizeStreams::CreateInstance();
		pSyncStreams->ResynchronizeAllStreams();
		return true;
	}

	return false;
}

void CParseRTP::OnFrameStart(unsigned int rtpTimestamp)
{
	__int64 nStartTime = 0;
	CSynchronizeStreams * pSynchronizeStreams = CSynchronizeStreams::CreateInstance();
	pSynchronizeStreams->CalculateFramePresentationTime(m_pMediaStream, rtpTimestamp, &nStartTime);

	m_FrameBuffer.nSize = 0;
	m_FrameBuffer.nStartTime = nStartTime;
	m_FrameBuffer.nEndTime = nStartTime + 1;
}

void CParseRTP::OnFrameData(void * _frameData, int frameSize)
{
///<add by dolby,protect the m_FrameBuffer from buffer overflow
	STREAMINFO * pStreamInfo = m_pMediaStream->GetStreamInfo();
	if(_frameData == NULL || frameSize <= 0 || m_FrameBuffer.nSize > pStreamInfo->nMaxFrameSize) 
		return;

	VO_PBYTE pData = m_FrameBuffer.pData + m_FrameBuffer.nSize;
///<add by dolby,protect the m_FrameBuffer from buffer overflow
	
	if (m_FrameBuffer.nSize + frameSize > pStreamInfo->nMaxFrameSize)
	{
		memcpy(pData, (VO_PBYTE)_frameData, pStreamInfo->nMaxFrameSize - m_FrameBuffer.nSize);
		m_FrameBuffer.nSize = pStreamInfo->nMaxFrameSize;
#if (defined _WIN32 && defined _DEBUG)
		MessageBox(NULL,L"buffer overflow",L"error",MB_OK);
#endif
	}
	else
	{
		memcpy(pData, (VO_PBYTE)_frameData, frameSize);
		m_FrameBuffer.nSize += frameSize;
	}
}

void CParseRTP::OnFrameEnd()
{
	++m_cFrame;

	m_pParserInitInfo->pProc(&m_OutputBuffer);


#if defined(_BURST) //&& defined(_WIN32_WCE)
	Sleep(5);
#endif

#ifdef _RTPPSR_LOG
	DumpFrameInfo();
#endif //_RTPPSR_LOG
}

void CParseRTP::OnFrameError(int errorCode)
{
	m_FrameBuffer.nSize = 0;
}


#ifdef _RTPPSR_LOG
void CParseRTP::DumpRTPInfo(CRTPPacket * pRTPPacket)
{
	if(CLogTool::LogTool.GetHandle(_T("VideoRTP.txt")) == NULL)
	{
		CLogTool::LogTool.LogOutput(_T("VideoRTP.txt"), "SeqNum        Timestamp\r\n");
	}
	if(CLogTool::LogTool.GetHandle(_T("RMXRTP.txt")) == NULL)
	{
//		CLogTool::LogTool.LogOutput(_T("RMXRTP.txt"), "SeqNum        Timestamp\r\n");
	}
	if(CLogTool::LogTool.GetHandle(_T("AudioRTP.txt")) == NULL)
	{
		CLogTool::LogTool.LogOutput(_T("AudioRTP.txt"), "SeqNum        Timestamp\r\n");
	}


	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	char _mediaType[16] = {0};
	pSDPMedia->ParseMediaType(_mediaType, 16);

	char _encodingName[32] = {0};
	pSDPMedia->ParseAttribute_rtpmap_encodingName(_encodingName, 32);

	if(_stricmp(_mediaType, "Video") == 0)
	{
		if(_stricmp(_encodingName, "H264") == 0)
		{
			CLogTool::LogTool.LogOutput(_T("VideoRTP.txt"), "%d        %u\r\n", pRTPPacket->SeqNum(), pRTPPacket->Timestamp());
		}
		else if(_stricmp(_encodingName, "richmedia+xml") == 0)
		{
//			CLogTool::LogTool.LogOutput(_T("RMXRTP.txt"), "%d        %u\r\n", pRTPPacket->SeqNum(), pRTPPacket->Timestamp());
		}
	}
	else if(_stricmp(_mediaType, "Audio") == 0)
	{
		CLogTool::LogTool.LogOutput(_T("AudioRTP.txt"), "%d        %u\r\n", pRTPPacket->SeqNum(), pRTPPacket->Timestamp());
	}
}

void CParseRTP::DumpFrameInfo()
{
	if(m_sysTime == 0)
		m_sysTime = GetTickCount();

	if(CLogTool::LogTool.GetHandle(_T("VideoFrame.txt")) == NULL)
	{
		CLogTool::LogTool.LogOutput(_T("VideoFrame.txt"), "No.    ElapseTime    FrameTime    FrameSize\r\n");
	}
	if(CLogTool::LogTool.GetHandle(_T("RMXFrame.txt")) == NULL)
	{
//		CLogTool::LogTool.LogOutput(_T("RMXFrame.txt"), "No.    ElapseTime    FrameTime    FrameSize\r\n");
	}
	if(CLogTool::LogTool.GetHandle(_T("AudioFrame.txt")) == NULL)
	{
		CLogTool::LogTool.LogOutput(_T("AudioFrame.txt"), "No.    ElapseTime    FrameTime    FrameSize\r\n");
	}


	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	char _mediaType[16] = {0};
	pSDPMedia->ParseMediaType(_mediaType, 16);

	char _encodingName[32] = {0};
	pSDPMedia->ParseAttribute_rtpmap_encodingName(_encodingName, 32);

	unsigned long elapseTime = GetTickCount() - m_sysTime;
	unsigned int frameTime = (unsigned int)(m_FrameBuffer.nStartTime);
	unsigned int frameSize = m_FrameBuffer.nSize;
	if(_stricmp(_mediaType, "Video") == 0)
	{
		if(_stricmp(_encodingName, "H264") == 0)
		{
			CLogTool::LogTool.LogOutput(_T("VideoFrame.txt"), "%u        %u        %u        %u\r\n", m_cFrame, elapseTime, frameTime, frameSize);
		}
		else if(_stricmp(_encodingName, "richmedia+xml") == 0)
		{
//			CLogTool::LogTool.LogOutput(_T("RMXFrame.txt"), "%u        %u        %u        %u\r\n", m_cFrame, elapseTime, frameTime, frameSize);
		}
	}
	else if(_stricmp(_mediaType, "Audio") == 0)
	{
		CLogTool::LogTool.LogOutput(_T("AudioFrame.txt"), "%u        %u        %u        %u\r\n", m_cFrame, elapseTime, frameTime, frameSize);
	}
}
#endif //_RTPPSR_LOG
