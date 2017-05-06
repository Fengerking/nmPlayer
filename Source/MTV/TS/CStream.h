#pragma once
#include "voYYDef_TS.h"
#include "TsParserBaseDef.h"
#include "StreamFormat.h"
#include "CvoBaseMemOpr.h"
#include "voParser.h"

// added by Lin Jun 20110112
//#define USE_CACHE

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif



#define MAX_CACHE_SIZE 1024*1024

class CStream : public CStreamFormat, public CvoBaseMemOpr
{
public:
	CStream(VOSTREAMPARSESTREAMINFO* pInfo);
	virtual ~CStream(void);

public:
	VO_U32	GetStreamID(){return m_nStreamID;};
	VO_VOID	SetStreamID(VO_U32 nID){m_nStreamID=nID;};
	VO_VOID Reset();
    VO_U32  GetTrackType();

	VO_VOID	OnNewFrame(VOSTREAMPARSEFRAMEINFO* pFrameInfo);
	VO_VOID	OnFrameData(void* pData, int nSize);
	VO_BOOL OnFrameEnd(int* sizes, int* count, VO_BYTE** pOutBuf, VO_U64* pTimeStamp);
	VO_BOOL	IsSync(VO_BYTE* pFrame, VO_U32 nFrameLen);
	VO_S64	GetTimeStamp(){return m_nTimeStamp;}
	VO_BYTE* GetFrameBuf(){return m_pFrameBuf;}
	VO_U32 GetBufferSize(){return m_nCurrBufPos;}
	VO_CHAR*   GetLanguageDesc() { return m_chLanguage;}
	VO_VOID	SetNewFrameSize(VO_U32  ulFrameSize){m_nCurrBufPos=ulFrameSize;}
	VO_VOID	SetNewTimeStamp(VO_S64 illTimeStamp){ m_nTimeStamp = illTimeStamp;}



private:
	VO_BYTE*	m_pFrameBuf;
	VO_U32		m_nFrameBufLen;
	VO_U32		m_nCurrBufPos;
	VO_U32		m_nStreamID;
	VO_S64		m_nTimeStamp;
    VO_CHAR     m_chLanguage[16];



	// added bu Lin Jun 20110112
#ifdef USE_CACHE
	VO_U32		m_nCurrCachePos;
	VO_U32		m_nCacheSize;
#endif
	//end
};

#ifdef _VONAMESPACE
}
#endif

