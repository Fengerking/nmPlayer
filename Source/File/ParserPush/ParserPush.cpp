#include "ParserPush.h"
#include "voString.h"
#include "voLog.h"
#include "fileformat_checker.h"




CParserPush::CParserPush(void)
	: m_pParserPush(NULL)
	, m_bInitialized(VO_FALSE)
	, m_iBufLen(0)
	, m_pBuf(NULL)
	, m_nLastTimeStamp(0)
	, m_nTSOffsetTimeStamp(0)
	, m_bHaveTSCame(VO_FALSE)
{
}

CParserPush::~CParserPush(void)
{
	Close();
}


VO_U32 CParserPush::Open(VO_PARSER_INIT_INFO *pParma)
{
	m_ParserOriginalPull.Open(pParma);
	m_ParserOriginalPush.Open(pParma);

	return 0;
}

VO_U32 CParserPush::Close()
{
	VOLOGF();

	if (m_pParserPush)
		m_pParserPush->Close();

	m_pParserPush = NULL;

	return 0;
}

VO_U32 CParserPush::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	//VO_PARSER_FLAG_STREAM_CHANGED	:	Maybe,	need get_rightlibrary(),	Wait until end
	//VO_PARSER_FLAG_STREAM_END		:	Must,	need getLastTimeStamp(),	Wait until end
	if (VO_PARSER_FLAG_STREAM_CHANGED == pBuffer->nFlag || VO_PARSER_FLAG_STREAM_END == pBuffer->nFlag)
	{
		if (NULL != m_pParserPush)
		{
			m_pParserPush->Reset();

			m_nLastTimeStamp = m_pParserPush->getLastTimeStamp();

			m_pParserPush = NULL;
			m_bInitialized = VO_FALSE;
		}
	}

	if (VO_PARSER_FLAG_STREAM_END == pBuffer->nFlag)
		return VO_ERR_NONE;

	VOLOGI("Size %u %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", pBuffer->nBufLen, pBuffer->pBuf[0], pBuffer->pBuf[1], pBuffer->pBuf[2], pBuffer->pBuf[3], pBuffer->pBuf[4], pBuffer->pBuf[5], pBuffer->pBuf[6], pBuffer->pBuf[7], pBuffer->pBuf[8], pBuffer->pBuf[9], pBuffer->pBuf[10]);

	if (!m_bInitialized)
	{
		if (!m_pBuf)
			m_pBuf = new VO_BYTE[GUESSFILETYPE_BUFFERSIZE];

		VO_S32 iLen = pBuffer->nBufLen < GUESSFILETYPE_BUFFERSIZE - m_iBufLen ? pBuffer->nBufLen : GUESSFILETYPE_BUFFERSIZE - m_iBufLen;

		memcpy(m_pBuf + m_iBufLen, pBuffer->pBuf, iLen);
		m_iBufLen += iLen;

		if (GUESSFILETYPE_BUFFERSIZE == m_iBufLen)
		{
			int iSkip = 0;
			VO_S32 iRet = 0;
			do {
				iRet = SkipID3v2Data(m_pBuf + iSkip, GUESSFILETYPE_BUFFERSIZE);
				iSkip += iRet;
			} while (iRet);

			VO_TCHAR szDllFile[256] = {0};
			VO_TCHAR szAPIName[128] = {0};

			if( !get_rightlibrary(m_pBuf, GUESSFILETYPE_BUFFERSIZE, szDllFile, szAPIName) ) {
				VOLOGI("!get_rightlibrary");
				return VO_FALSE;
			}

			if (0 == vostrncmp( _T("voTsParser"), szDllFile, 10) )
			{
				VOLOGI("m_ParserOriginalPush");

				if (!m_bHaveTSCame)
				{
					m_nTSOffsetTimeStamp = m_nLastTimeStamp;

					m_bHaveTSCame = VO_TRUE;
				}

				m_pParserPush = &m_ParserOriginalPush;
				m_pParserPush->setAddEndTimeStamp(m_nTSOffsetTimeStamp);
			}
			else
			{
				VOLOGI("m_ParserOriginalPull");

				m_pParserPush = &m_ParserOriginalPull;
				m_pParserPush->setAddEndTimeStamp(m_nLastTimeStamp);
			}

			m_pParserPush->Open();
			m_pParserPush->setDLL(szDllFile, szAPIName);

			m_bInitialized = VO_TRUE;

			VO_PARSER_INPUT_BUFFER input_buffer = {0};
			if (pBuffer->nBufLen == iLen)
				input_buffer.nFlag	= pBuffer->nFlag;
			input_buffer.pBuf		= m_pBuf + iSkip;
			input_buffer.nBufLen	= GUESSFILETYPE_BUFFERSIZE - iSkip;

			m_pParserPush->Process(&input_buffer);

			if (pBuffer->nBufLen - iLen > 0)
			{
				input_buffer.nFlag		= pBuffer->nFlag;
				input_buffer.pBuf		= pBuffer->pBuf + iLen;
				input_buffer.nBufLen	= pBuffer->nBufLen - iLen;

				m_pParserPush->Process(&input_buffer);
			}

			delete []m_pBuf;
			m_pBuf = NULL;
			m_iBufLen = 0;
		}

		return pBuffer->nBufLen;
	}

	return m_pParserPush->Process(pBuffer);
}

VO_U32 CParserPush::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return m_pParserPush->SetParam(uID, pParam);
}

VO_U32 CParserPush::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return m_pParserPush->GetParam(uID, pParam);
}