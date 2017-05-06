#include "ParserOriginalPush.h"
#include "voLog.h"

typedef VO_S32 (VO_API *pvoGetParserAPI)(VO_PARSER_API * pParser);

//FILE *g_pSave = NULL;
void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	CParserOriginalPush *pParsrPush = (CParserOriginalPush *)pData->pUserData;

	return pParsrPush->doParserProc(pData);
}

CParserOriginalPush::CParserOriginalPush(void)
	: m_parser_handle(NULL)
	, m_nTSPacketSize(0)
	, m_iBufLen(0)
	, m_nAddendTimeStamp(0)
	, m_nLastTimeStamp(0)
{
	//g_pSave = fopen("C:\\Users\\liu_chenhui\\Desktop\\BC", "w+b");
}


CParserOriginalPush::~CParserOriginalPush(void)
{
	//fclose(g_pSave);

	Close();
}

VO_U32 CParserOriginalPush::Open(VO_PARSER_INIT_INFO *pParma)
{
	if (pParma)
		memcpy( &m_parser_init_info, pParma, sizeof(VO_PARSER_INIT_INFO) );

	return 0;
}

VO_U32 CParserOriginalPush::Close()
{
	VOLOGF();

	if (!m_parser_handle)
		return 0;

	m_parser_api.Close(m_parser_handle);
	m_dlEngine.FreeLib();
	m_parser_handle = NULL;

	memset( &m_parser_api, 0, sizeof(VO_PARSER_API) );

	return 0;
}

VO_U32 CParserOriginalPush::Reset()
{
	VOLOGF();

	return 0;
}

VO_U32 CParserOriginalPush::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	if (0 == m_nTSPacketSize)
	{
		m_nTSPacketSize = m_CheckTSPacketSize.Check(pBuffer->pBuf, pBuffer->nBufLen);
		if (m_nTSPacketSize == 0) {
			VOLOGE("!TSPacketSize ERR");
			return -1;
		}
	}

	VO_U32 uiOffset = 0;

	VO_PARSER_INPUT_BUFFER input_buffer = {0};

	if (m_iBufLen > 0)
	{
		int iLen = m_nTSPacketSize - m_iBufLen < pBuffer->nBufLen ? m_nTSPacketSize - m_iBufLen : pBuffer->nBufLen;
		memcpy(m_Buffer + m_iBufLen, pBuffer->pBuf, iLen);

		m_iBufLen += iLen;
		if (m_iBufLen < m_nTSPacketSize)
			return pBuffer->nBufLen;

		input_buffer.pBuf		= m_Buffer;
		input_buffer.nBufLen	= m_nTSPacketSize;

		int iRet = m_parser_api.Process(m_parser_handle, &input_buffer);
		if (VO_ERR_PARSER_OK != iRet) {
			VOLOGE("!m_parser_ts.Process ERR");
			return iRet;
		}
		//else if (g_pSave)
		//{
		//	fwrite(input_buffer.pBuf, 1, input_buffer.nBufLen, g_pSave);
		//	fflush(g_pSave);
		//}

		m_iBufLen = 0;
		uiOffset += iLen;
	}

	memset( &input_buffer, 0, sizeof(VO_PARSER_INPUT_BUFFER) );
	input_buffer.pBuf = pBuffer->pBuf + uiOffset;
	VO_S32 iCount = (pBuffer->nBufLen - uiOffset) / m_nTSPacketSize;
	input_buffer.nBufLen = m_nTSPacketSize * iCount;

	VO_S32 iRet = m_parser_api.Process(m_parser_handle, &input_buffer);
	if (VO_ERR_PARSER_OK != iRet) {
		VOLOGE("!m_parser_ts.Process ERR");
		return iRet;
	}
	//else if (g_pSave)
	//{
	//	fwrite(input_buffer.pBuf, 1, input_buffer.nBufLen, g_pSave);
	//	fflush(g_pSave);
	//}

	uiOffset += input_buffer.nBufLen;

	if (uiOffset < pBuffer->nBufLen)
	{
		memcpy(m_Buffer, pBuffer->pBuf + uiOffset, pBuffer->nBufLen - uiOffset);
		m_iBufLen = pBuffer->nBufLen - uiOffset;
	}

	return pBuffer->nBufLen;
}


void CParserOriginalPush::doParserProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
	VO_MTV_FRAME_BUFFER *pMTV_Frame_Buf = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;
	VOLOGI("Size %d doParserProc %lld + %lld", pMTV_Frame_Buf->nSize, pMTV_Frame_Buf->nStartTime, m_nAddendTimeStamp);

	switch (pData->nType)
	{
	case VO_PARSER_OT_AUDIO:
	case VO_PARSER_OT_VIDEO:
		{
			//fwrite(pMTV_Frame_Buf->pData, 1, pMTV_Frame_Buf->nSize, g_pSave);

			pMTV_Frame_Buf->nStartTime	+= m_nAddendTimeStamp;
			//pMTV_Frame_Buf->nEndTime	+= m_nAddendTimeStamp;
			VOLOGI("TYPE %d doParserProc %lld", pData->nType, pMTV_Frame_Buf->nStartTime);

			if (m_nLastTimeStamp < pMTV_Frame_Buf->nStartTime)
				m_nLastTimeStamp = pMTV_Frame_Buf->nStartTime;
		}
		break;
	default:
		break;
	}

	pData->pUserData = m_parser_init_info.pUserData;

	return m_parser_init_info.pProc(pData);
}

VO_U32 CParserOriginalPush::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.SetParam(&m_parser_handle, uID, pParam);
}

VO_U32 CParserOriginalPush::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.GetParam(&m_parser_handle, uID, pParam);
}

VO_U32 CParserOriginalPush::setDLL(VO_TCHAR *szDllFile, VO_TCHAR *szAPIName)
{
	if (NULL != m_parser_handle)
		return VO_TRUE;

	vostrcpy(m_dlEngine.m_szDllFile, szDllFile);
	vostrcpy(m_dlEngine.m_szAPIName, szAPIName);

	if(m_dlEngine.LoadLib(NULL) == 0) {
		VOLOGE("!LoadLib ERR");
		return VO_FALSE;
	}

	pvoGetParserAPI getapi = (pvoGetParserAPI)m_dlEngine.m_pAPIEntry;
	if (getapi)
	{
		getapi(&m_parser_api);

		VO_PARSER_INIT_INFO	parser_init_info = {0};
		parser_init_info.pMemOP		= m_parser_init_info.pMemOP;
		parser_init_info.pProc		= ParserProc;
		parser_init_info.pUserData	= this;

		int iRet = m_parser_api.Open(&m_parser_handle, &parser_init_info);
		if (VO_ERR_PARSER_OK != iRet) {
			VOLOGE("!ts->Open");
			return iRet;
		}

		VO_S32 iMedia = VO_MEDIA_AUDIO_VIDEO;

		VO_PARSER_OUTPUT_BUFFER output_buf = {0};
		output_buf.nType		= VO_PARSER_OT_MEDIATYPE;
		output_buf.pOutputData	= &iMedia;
		output_buf.pUserData	= m_parser_init_info.pUserData;

		m_parser_init_info.pProc(&output_buf);
	}
	else
	{
		VOLOGI("!getapi");
		return VO_ERR_BASE;
	}

	return VO_ERR_NONE;
}

VO_S64 CParserOriginalPush::getLastTimeStamp()
{
	return m_nLastTimeStamp;
}

VO_VOID CParserOriginalPush::setAddEndTimeStamp(VO_S64 ll)
{
	m_nAddendTimeStamp = ll;
}