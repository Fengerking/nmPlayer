#include "vortpbase.h"
#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "VideoStream.h"
#include "AudioStream.h"
#include "SynchronizeStreams.h"
#include "ParseEngine.h"

#ifdef _RTPPSR_LOG
#include "logtool.h"
#endif //_RTPPSR_LOG

CParseEngine::CParseEngine()
: m_pSDPParse(NULL)
{
	memset(&m_InitInfo, 0, sizeof(VO_PARSER_INIT_INFO));
}

CParseEngine::~CParseEngine()
{
	Close();
}

VO_U32 CParseEngine::Open(VO_PARSER_INIT_INFO * pInitInfo)
{
	m_InitInfo = *pInitInfo;

#ifdef _RTPPSR_LOG
	CLogTool::LogTool.EnableLog(true);
#endif //_RTPPSR_LOG

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseEngine::Close()
{
	return Reset();
}

VO_U32 CParseEngine::Reset()
{
	list_T<CMediaStream *>::iterator iter;
	for(iter=m_listMediaStream.begin(); iter!=m_listMediaStream.end(); ++iter)
	{
		delete *iter;
	}
	m_listMediaStream.clear();

	CSynchronizeStreams::DestroyInstance();

//	memset(&m_InitInfo, 0, sizeof(VO_PARSER_INIT_INFO));
	SAFE_DELETE(m_pSDPParse);

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseEngine::Process(VO_PARSER_INPUT_BUFFER * pBuffer)
{
#ifdef _RTPPSR_LOG
#if 0
	DumpStreamData(channel, pData, nSize);
#endif
#endif //_RTPPSR_LOG

	VO_U32 rc = VO_ERR_PARSER_ERROR;

	if(pBuffer->nStreamID & PACKET_SDP)
	{
		int sdpSize = pBuffer->nBufLen;
		char * _sdpData = new char[(sdpSize + 4) / 4 * 4];
		if(_sdpData == NULL)
			return VO_ERR_PARSER_OUT_OF_MEMORY;
		memcpy(_sdpData, pBuffer->pBuf, sdpSize);
		_sdpData[sdpSize] = '\0';

		rc = InitializeWithSDP(_sdpData);

		delete[] _sdpData;

		return rc;
	}

	list_T<CMediaStream *>::iterator iter;
	for(iter=m_listMediaStream.begin(); iter!=m_listMediaStream.end(); ++iter)
	{
		CMediaStream * pMediaStream = (CMediaStream *)(*iter);
		VO_U32 streamId = pBuffer->nStreamID & 0x00FFFFFF;
		if(streamId == pMediaStream->GetStreamId())
		{
			rc = pMediaStream->Process(pBuffer);
			return rc;
		}
	}

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CParseEngine::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_PARSER_NOT_IMPLEMENT;
}

VO_U32 CParseEngine::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_PARSER_NOT_IMPLEMENT;
}

VO_U32 CParseEngine::InitializeWithSDP(const char * _sdpText)
{
	VO_U32 rc = VO_ERR_PARSER_OK;

	m_pSDPParse = new CSDPParse();
	if(m_pSDPParse == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	if(!m_pSDPParse->OpenSDP(_sdpText))
		return VO_ERR_PARSER_INVALID_ARG;

	CSynchronizeStreams * pSynchronizeStreams = CSynchronizeStreams::CreateInstance();
	if(pSynchronizeStreams == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	VO_PARSER_STREAMINFO ParserStreamInfo;
	memset(&ParserStreamInfo, 0, sizeof(VO_PARSER_STREAMINFO));

	for(int index=0; index<m_pSDPParse->GetSDPMediaCount(); ++index)
	{
		CSDPMedia * pSDPMedia = m_pSDPParse->GetSDPMedia(index);

		char _mediaType[16];
		if(!pSDPMedia->ParseMediaType(_mediaType, 16))
			continue;

		CMediaStream * pMediaStream = NULL;
		if(_stricmp(_mediaType, "Video") == 0)
		{
			pMediaStream = new CVideoStream(&m_InitInfo);
			if(pMediaStream == NULL)
				return VO_ERR_PARSER_OUT_OF_MEMORY;

			rc = pMediaStream->Init(pSDPMedia);
			if(rc != VO_ERR_PARSER_OK)
			{
				SAFE_DELETE(pMediaStream);
				continue;
//				return rc;
			}

			STREAMINFO * pStreamInfo = pMediaStream->GetStreamInfo();
			ParserStreamInfo.nVideoCodecType = pStreamInfo->nCodecType;
			ParserStreamInfo.pVideoExtraData = pStreamInfo->pExtraData;
			ParserStreamInfo.nVideoExtraSize = pStreamInfo->nExtraSize;
		}
		else if(_stricmp(_mediaType, "Audio") == 0)
		{
			pMediaStream = new CAudioStream(&m_InitInfo);
			if(pMediaStream == NULL)
				return VO_ERR_PARSER_OUT_OF_MEMORY;

			rc = pMediaStream->Init(pSDPMedia);
			if(rc != VO_ERR_PARSER_OK)
			{
				SAFE_DELETE(pMediaStream);
				continue;
//				return rc;
			}

			STREAMINFO * pStreamInfo = pMediaStream->GetStreamInfo();
			ParserStreamInfo.nAudioCodecType = pStreamInfo->nCodecType;
			ParserStreamInfo.pAudioExtraData = pStreamInfo->pExtraData;
			ParserStreamInfo.nAudioExtraSize = pStreamInfo->nExtraSize;
		}
		else if(_stricmp(_mediaType, "Data") == 0)
		{
		}
		else
		{
		}

		pSynchronizeStreams->AddStream(pMediaStream);
		m_listMediaStream.push_back(pMediaStream);
	}

	if(m_listMediaStream.empty())
		return VO_ERR_PARSER_FAIL;

	VO_PARSER_OUTPUT_BUFFER OutputBuffer;
	memset(&OutputBuffer, 0, sizeof(VO_PARSER_OUTPUT_BUFFER));
	OutputBuffer.nStreamID = 0;
	OutputBuffer.nType = VO_PARSER_OT_STREAMINFO;
	OutputBuffer.pOutputData = &ParserStreamInfo;
	OutputBuffer.pUserData = m_InitInfo.pUserData;
	OutputBuffer.pReserved = NULL;

	m_InitInfo.pProc(&OutputBuffer);
 
	return VO_ERR_PARSER_OK;
}



#ifdef _RTPPSR_LOG
void DumpStreamData(int channelID, void * _data, int len)
{
	CLogTool::LogTool.LogOutput(_T("parser-dump.rtp"), &channelID, sizeof(channelID));
	CLogTool::LogTool.LogOutput(_T("parser-dump.rtp"), &len, sizeof(len));
	CLogTool::LogTool.LogOutput(_T("parser-dump.rtp"), _data, len);
}
#endif //_RTPPSR_LOG
