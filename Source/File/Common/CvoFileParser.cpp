#include "CvoFileParser.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CvoFileBaseParser::CvoFileBaseParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoBaseMemOpr(pMemOp)
	, m_pFileChunk(pFileChunk)
{
}

CvoFileBaseParser::~CvoFileBaseParser()
{
}

CvoFileHeaderParser::CvoFileHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileBaseParser(pFileChunk, pMemOp)
{
}

CvoFileHeaderParser::~CvoFileHeaderParser()
{
}

CvoFileDataParser::CvoFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileBaseParser(pFileChunk, pMemOp)
	, m_btBlockStream(0xFF)
	, m_bParseEnd(VO_FALSE)
	, m_fODCallback(VO_NULL)
	, m_pODUser(VO_NULL)
	, m_pODUserData(VO_NULL)
	, m_fOBCallback(VO_NULL)
	, m_pOBUser(VO_NULL)
	, m_pOBUserData(VO_NULL)
{
}

CvoFileDataParser::~CvoFileDataParser()
{
}

VO_BOOL CvoFileDataParser::SetStartFilePos(VO_U64 ullStartFilePos, VO_BOOL bInclHeader /* = VO_FALSE */)
{
	return m_pFileChunk->FLocate(ullStartFilePos);
}

VO_VOID CvoFileDataParser::SetOnDataCallback(VO_PTR pUser, VO_PTR pUserData, ONDATACALLBACK fCallback)
{
	m_fODCallback = fCallback;
	m_pODUser = pUser;
	m_pODUserData = pUserData;
}

VO_VOID CvoFileDataParser::SetOnBlockCallback(VO_PTR pUser, VO_PTR pUserData, ONBLOCKCALLBACK fCallback)
{
	m_fOBCallback = fCallback;
	m_pOBUser = pUser;
	m_pOBUserData = pUserData;
}

VO_BOOL CvoFileDataParser::Step()
{
	if(!StepB())
		return VO_FALSE;

	return VO_TRUE;
}