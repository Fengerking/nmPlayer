
#include "CSMTHParser.h"
#include "voCheck.h"
//#include "CDumper.h"
//#include "voOSFunc.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CSMTH_Parser::CSMTH_Parser()
:m_pSmthParserCtrl(NULL)
,m_nFirstTimeStamp(0)
,program_time(0)
,m_pHLicCheck(NULL)
{
}

CSMTH_Parser::~CSMTH_Parser(void)
{
	;//Close();
}

void CSMTH_Parser::OnParsed(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
	//((CSMTH_Parser*)pBuf->pUserData)->HandleParseResult(pBuf);
}

void CSMTH_Parser::HandleParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
	//m_nStreamID = pBuf->nStreamID;

	//switch (pBuf->nType)
	//{
	//case VO_PARSER_OT_AUDIO:
	//case VO_PARSER_OT_VIDEO:
	//	{
	//		// tag: 20100422 process the time stamp offset
	//		VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;

	//		if (m_nFirstTimeStamp == 0xFFFFFFFF)
	//		{
	//			m_nFirstTimeStamp = buf->nStartTime;
	//		}
	//		buf->nStartTime -= m_nFirstTimeStamp;

	//		// end

	//		IssueParseResult(pBuf->nType, pBuf->pOutputData);
	//	}
	//	break;
	//case VO_PARSER_OT_ERROR:
	//case VO_PARSER_OT_STREAMINFO:
	//	{
	//		VO_U32 t = voOS_GetSysTime() - program_time;
	//		CDumper::WriteLog("Stream info detected use time = %d", t);

	//		IssueParseResult(pBuf->nType, pBuf->pOutputData);
	//	}
	//	break;
	//case VO_PARSER_OT_TS_PROGRAM_INFO:
	//	{
	//		VO_U32 t = voOS_GetSysTime() - program_time;
	//		CDumper::WriteLog("Program info detected use time = %d", t);

	//		IssueParseResult(pBuf->nType, pBuf->pOutputData);
	//	}
	//	break;
	//default:
	//	{
	//		if (m_pCallback)
	//		{
	//			pBuf->pUserData	= m_pUserData;
	//			m_pCallback(pBuf);
	//		}
	//	}
	//	break;
	//}
}
	

VO_U32 CSMTH_Parser::Open(VO_PARSER_INIT_INFO* pParam)
{
//	if(program_time == 0)
//		program_time = voOS_GetSysTime();

	//CBaseParser::Open(pParam);

	//Close();

	VO_U32 nResult = VO_ERR_PARSER_OK;
//	voCAutoLock autolock(&m_AutoLock);
	VOLOGINIT(pParam->strWorkPath);
	nResult = voCheckLibInit (&m_pHLicCheck, VO_INDEX_SRC_SSP, VO_LCS_WORKPATH_FLAG,0,pParam->strWorkPath); 
	if(nResult != VO_ERR_NONE )
	{
		if(m_pHLicCheck != NULL)
		{
			voCheckLibUninit(m_pHLicCheck);
			VOLOGE("CheckLib Fail!!!  pWorkingPath=%s",pParam->strWorkPath);
		}
		m_pHLicCheck = NULL;
		return nResult;
	}

	m_pSmthParserCtrl = new CSMTHParserCtrl;

	// tag: 20100423
	//m_pTsCtrl->SetParseType(PARSE_ALL);
	//m_pTsCtrl->SetParseType(PARSE_PLAYBACK);
	// end
	m_pSmthParserCtrl->Open(pParam->pProc, pParam->pUserData);

	return VO_ERR_PARSER_OK;
}

VO_U32 CSMTH_Parser::Close()
{
//	voCAutoLock autolock(&m_AutoLock);
	if(m_pHLicCheck != NULL)
	{
		voCheckLibUninit(m_pHLicCheck);
		m_pHLicCheck = NULL;
	}

	if (m_pSmthParserCtrl)
	{
		m_pSmthParserCtrl->Close();
		delete m_pSmthParserCtrl;
		m_pSmthParserCtrl = VO_NULL;
	}

	//CBaseParser::Close();
	VOLOGUNINIT();
	return VO_ERR_PARSER_OK;
}

VO_U32 CSMTH_Parser::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
	//VOLOGI("Recv data = %x, len = %d", pBuffer->pBuf[0], pBuffer->nBufLen);
	
	if (m_pSmthParserCtrl)
	{
		// test code
		if (pBuffer->nFlag & VO_PARSER_FLAG_STREAM_CHANGED)
		{
			;//program_time = voOS_GetSysTime();
		}
		//end
		return m_pSmthParserCtrl->Process(pBuffer);
	}

	return VO_ERR_PARSER_OK;
}

VO_U32 CSMTH_Parser::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (m_pSmthParserCtrl)
	{
		return m_pSmthParserCtrl->SetParam(uID, pParam);
	}

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CSMTH_Parser::GetParam(VO_U32 uID, VO_PTR pParam)
{
	/*if (m_pTsCtrl)
	{
		return m_pTsCtrl->GetParam(uID, pParam);
	}*/

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CSMTH_Parser::SelectProgram(VO_U32 uStreamID)
{
	/*if (m_pTsCtrl)
	{
		return m_pTsCtrl->SelectProgram(uStreamID);
	}*/

	return VO_ERR_PARSER_FAIL;
}

VO_BOOL CSMTH_Parser::GetLicState()
{
	if(m_pHLicCheck != NULL)
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}


