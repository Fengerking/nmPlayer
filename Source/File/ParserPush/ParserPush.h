#pragma once
#ifndef __CParserPush_H__
#define __CParserPush_H__

#include "ParserOriginalPull.h"
#include "ParserOriginalPush.h"
#include "voSource.h"
#include "voOSFunc.h"

#define GUESSFILETYPE_BUFFERSIZE (8 * 1024)

class CParserPush
{
public:
	CParserPush(void);
	~CParserPush(void);

	VO_U32 Open(VO_PARSER_INIT_INFO *pParma);
	VO_U32 Close();

	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);
	//---
protected:

private:
	CParserOriginalPull m_ParserOriginalPull;
	CParserOriginalPush m_ParserOriginalPush;

	IParserPush *m_pParserPush;

	VO_BOOL m_bInitialized;
	

	VO_BYTE *m_pBuf;
	VO_S32 m_iBufLen;

	VO_S64 m_nTSOffsetTimeStamp;
	VO_S64 m_nLastTimeStamp;

	VO_BOOL m_bHaveTSCame;
};


#endif //__CParserPush_H__