#ifndef _I_TS_PARSE_CTROLLER_H_
#define _I_TS_PARSE_CTROLLER_H_

#include "voYYDef_TS.h"
#include "voTSParser.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CStream;

class ITsParseController
{
public:
	ITsParseController(void)
		:m_pProc(VO_NULL)
		,m_pUserData(VO_NULL)
		, m_nStreamID(0)
		, m_nTimeStampOffset(-1)
		, m_nVideoTimeStampOffset(-1)
		, m_nAudioTimeStampOffset(-1)
	{

	};
	virtual ~ITsParseController(void){};

protected:
	VO_VOID	IssueParseResult(VO_U32 nType, VO_VOID* pResult, VO_VOID* pReserved=VO_NULL)
	{
		if (m_pProc)
		{
			VO_PARSER_OUTPUT_BUFFER out;
			out.nStreamID	= m_nStreamID;
			out.nType		= nType;
			out.pOutputData	= pResult;
			out.pUserData	= m_pUserData;
			out.pReserved	= pReserved;

			m_pProc(&out);
		}
	}

public:
	virtual VO_U32 Open(PARSERPROC pProc, VO_VOID* pUserData)=0;
	virtual VO_U32 Close()=0;
	virtual VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer)=0;
	virtual VO_U32 SetParam(VO_U32 uID, VO_PTR pParam)=0;
	virtual VO_U32 GetParam(VO_U32 uID, VO_PTR pParam)=0;
	virtual VO_U32 SelectProgram(VO_U32 uStreamID)=0;
	virtual VO_U32 SetParseType(VO_U32 nType)=0;
	virtual VO_U32 ResetState()=0;

	// especially for reader
	virtual VO_BOOL IsProgramInfoParsed()=0;
	virtual VO_U8	GetStreamCount()=0;
	virtual CStream*GetStreamByIdx(VO_U32 nIdx)=0;

	//
	VO_VOID			SetTimeStampOffset(VO_U64 nOffset){m_nTimeStampOffset=nOffset;};
	VO_U64			GetTimeStampOffset(){return m_nTimeStampOffset;};

protected:
	PARSERPROC	m_pProc;
	VO_VOID*	m_pUserData;
	VO_U32		m_nStreamID;
	VO_S64		m_nTimeStampOffset;
	VO_S64		m_nVideoTimeStampOffset;
	VO_S64		m_nAudioTimeStampOffset;

};



#endif // _I_TS_PARSE_CTROLLER_H_

#ifdef _VONAMESPACE
}
#endif