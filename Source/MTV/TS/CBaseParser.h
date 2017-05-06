#pragma once

#include "voYYDef_TS.h"
#include "CvoBaseObject.h"
#include "CvoBaseMemOpr.h"
//#include "CPtrList.h"

#include "voParser.h"
#include "fMacros.h"

//typedef struct 
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CBaseParser	:	public CvoBaseMemOpr, public CvoBaseObject
{
public:
	CBaseParser(VO_MEM_OPERATOR* pMemOp);
	virtual ~CBaseParser(void);

public:
	virtual VO_U32 Open(VO_PARSER_INIT_INFO* pParam);
	virtual	VO_U32 Close();
	virtual	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	virtual	VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 SelectProgram(VO_U32 uStreamID);

protected:
	virtual VO_U32 doSetParam(VO_U32 uID, VO_PTR pParam)=0;
	virtual VO_U32 doGetParam(VO_U32 uID, VO_PTR pParam)=0;

	virtual VO_BOOL CheckHeadData(VO_U32 nCodecType, VO_BYTE* pData, VO_U32 nLen);
protected:
	VO_VOID	IssueParseResult(VO_U32 nType, VO_VOID* pResult, VO_VOID* pReserved=VO_NULL);

protected:
	PARSERPROC		m_pCallback;
	VO_VOID*		m_pUserData;
	VO_U64			m_nStreamID;
	VO_PBYTE		m_pVideoFrameBuf;

	VO_U64			m_llLastVideoTimeStamp;

	VO_PARSER_STREAMINFO	m_StreamInfo;
};
#ifdef _VONAMESPACE
}
#endif