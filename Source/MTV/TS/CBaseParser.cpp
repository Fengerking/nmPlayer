#include "cmnMemory.h"
#include "CBaseParser.h"
#include "CDumper.h"
#include "voLog.h"
#include "CBaseTools.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseParser::CBaseParser(VO_MEM_OPERATOR* pMemOp)
: CvoBaseMemOpr(pMemOp)
, m_pCallback(VO_NULL)
, m_pUserData(VO_NULL)
, m_nStreamID(0)
, m_pVideoFrameBuf(VO_NULL)
, m_llLastVideoTimeStamp(0)
{
	MemSet(&m_StreamInfo, 0, sizeof(VO_PARSER_STREAMINFO));
}

CBaseParser::~CBaseParser(void)
{
}

VO_U32 CBaseParser::Open(VO_PARSER_INIT_INFO* pParam)
{
	m_pCallback	= pParam->pProc;
	m_pUserData = pParam->pUserData;

	return VO_ERR_PARSER_OK;
}

VO_U32 CBaseParser::Close()
{
	if (m_StreamInfo.pAudioExtraData)
	{
		cmnMemFree(0, m_StreamInfo.pAudioExtraData);
		m_StreamInfo.pAudioExtraData	= NULL;
		m_StreamInfo.nAudioExtraSize	= 0;
	}
	if (m_StreamInfo.pVideoExtraData)
	{
		cmnMemFree(0, m_StreamInfo.pVideoExtraData);
		m_StreamInfo.pVideoExtraData	= NULL;
		m_StreamInfo.nVideoExtraSize	= 0;
	}
	return VO_ERR_PARSER_NOT_IMPLEMENT;
}

VO_U32 CBaseParser::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	return VO_ERR_PARSER_NOT_IMPLEMENT;
}

VO_U32 CBaseParser::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (VO_ERR_PARSER_OK == doSetParam(uID, pParam))
	{
		return VO_ERR_PARSER_OK;
	}

	if (VO_PID_PARSER_SELECT_PROGRAM == uID)
	{
		return VO_ERR_PARSER_OK;
	}
	else if (VO_PID_PARSER_QUERY_ESG == uID)
	{
		return VO_ERR_PARSER_OK;
	}

	return VO_ERR_PARSER_INVLAID_PARAM_ID;
}

VO_U32 CBaseParser::GetParam(VO_U32 uID, VO_PTR pParam)
{
	if (VO_ERR_PARSER_OK == doGetParam(uID, pParam))
	{
		return VO_ERR_PARSER_OK;
	}

// 	if (VO_PID_PARSER_SELECT_PROGRAM)
// 	{
// 		return VO_ERR_PARSER_OK;
// 	}
// 	else if (VO_PID_PARSER_QUERY_ESG)
// 	{
// 		return VO_ERR_PARSER_OK;
// 	}


	return VO_ERR_PARSER_INVLAID_PARAM_ID;
}

VO_U32 CBaseParser::SelectProgram(VO_U32 uStreamID)
{
	return VO_ERR_PARSER_NOT_IMPLEMENT;
}

VO_VOID	CBaseParser::IssueParseResult(VO_U32 nType, VO_VOID* pResult, VO_VOID* pReserved/*=VO_NULL*/)
{
	// debug info
	if (nType == VO_PARSER_OT_AUDIO)
	{
		VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pResult;
		//CDumper::WriteLog("post audio TS = %06llu, len = %06lu", buf->nStartTime, buf->nSize);
		CDumper::RecordAudioLog(buf->nStartTime, buf->nSize,((buf->pData[3] & 0x3) << 11) + (buf->pData[4] << 3) + (buf->pData[5] >> 5));
		CDumper::DumpAudioData(buf->pData, buf->nSize);
	}
	else if (nType == VO_PARSER_OT_VIDEO)
	{
		VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pResult;

		// test code
 		//CDumper::WriteLog("post video TS = %06llu, len = %06lu", buf->nStartTime, buf->nSize);
		CDumper::RecordVideoLog(buf->nStartTime, buf->nSize, buf->nFrameType==VO_VIDEO_FRAME_I?0:1);

		if (m_llLastVideoTimeStamp == 0)
			m_llLastVideoTimeStamp = buf->nStartTime;

		if (m_llLastVideoTimeStamp > buf->nStartTime)
		{
			CDumper::WriteLog((char *)"++++++video TS invalid: last = %06llu, curr = %06llu++++++", m_llLastVideoTimeStamp, buf->nStartTime);
		}
		m_llLastVideoTimeStamp = buf->nStartTime;

		CDumper::DumpVideoData(buf->pData, buf->nSize);
		// end
	}
	// end debug info

	if (m_pCallback)
	{
		VO_PARSER_OUTPUT_BUFFER out;
		out.nStreamID	= m_nStreamID;

		out.nType		= nType;
		out.pOutputData	= pResult;
		out.pUserData	= m_pUserData;
		out.pReserved	= pReserved;

		m_pCallback(&out);
	}
}

// Note: need to process audio
VO_BOOL CBaseParser::CheckHeadData(VO_U32 nCodecType, VO_BYTE* pData, VO_U32 nLen)
{
	if(m_StreamInfo.pVideoExtraData && m_StreamInfo.pAudioExtraData)
		return VO_TRUE;

	VO_BYTE*	pHeadData	= NULL;
	VO_U32		nHeadSize	= 0;
	VO_BOOL		bVideo		= VO_FALSE;
	VO_BOOL		bKeyFrame	= VO_FALSE;
	
	bKeyFrame = CBaseTools::GetHeadData(nCodecType, pData, nLen, bVideo, &pHeadData, nHeadSize);

	if (bKeyFrame && pHeadData && nHeadSize>0 && bVideo)
	{
		m_StreamInfo.nVideoCodecType = nCodecType;
		m_StreamInfo.pVideoExtraData = pHeadData;
		m_StreamInfo.nVideoExtraSize = nHeadSize;
		IssueParseResult(VO_PARSER_OT_STREAMINFO, &m_StreamInfo);

		//cmnMemFree(0, pHeadData);
		return VO_TRUE;
	}
	else if (bKeyFrame && pHeadData && nHeadSize>0 && !bVideo)
	{
		m_StreamInfo.nAudioCodecType = nCodecType;
		m_StreamInfo.pAudioExtraData = pHeadData;
		m_StreamInfo.nAudioExtraSize = nHeadSize;
		IssueParseResult(VO_PARSER_OT_STREAMINFO, &m_StreamInfo);

		//cmnMemFree(0, pHeadData);
		return VO_TRUE;
	}


	return VO_FALSE;
}
