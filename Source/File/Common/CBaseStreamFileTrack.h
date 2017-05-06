#pragma once
#include "voYYDef_filcmn.h"
#include "CBaseTrack.h"
#include "CGBuffer.h"
#include "CBaseStreamFileIndex.h"
#include "CvoFileParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CBaseStreamFileReader;
class CBaseStreamFileTrack :
	public CBaseTrack
{
public:
	CBaseStreamFileTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CBaseStreamFileReader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CBaseStreamFileTrack();

public:
	virtual VO_VOID		SetNewDataParser(CvoFileDataParser* pNewDataParser);
	virtual VO_VOID		SetParseForSelf(VO_BOOL bValue) {m_bParseForSelf = bValue;}
	virtual VO_BOOL		OnBlock(VO_U32 dwTimeStamp);

	virtual VO_U32		GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample);

	//dwBufferTime<Second>, dwExtSize<Byte>
	virtual VO_BOOL		InitGlobalBuffer();
	virtual VO_VOID		Flush();

	virtual VO_BOOL		SetBufferTime(VO_U32 dwBufferTime);
	virtual VO_BOOL		SetGlobalBufferExtSize(VO_U32 dwExtSize);

	virtual VO_BOOL		IsGlobalBufferFull() {return m_pGlobeBuffer->HasIdleBuffer(m_dwGBExtSize) ? VO_FALSE : VO_TRUE;}
	virtual VO_BOOL		IsIndexNull() {return m_pStreamFileIndex->IsNull();}

protected:
	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosN(VO_S64* pPos);
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosK(VO_S64* pPos);

	//video track must implement them!!
	virtual VO_U32		GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp) {return -1;}
	virtual VO_U64		GetFilePosByFileIndex(VO_U32 dwFileIndex) {return -1;}

	//return VO_FALSE mean end!!
	virtual VO_BOOL		TrackGenerateIndex();
	inline VO_BOOL		TrackGenerateIndexB(CvoFileDataParser* pDataParser);
	//0 - can generate index
	//1 - can not generate index, track buffer full
	//2 - can not generate index, file parse end
	virtual VO_U8		IsCannotGenerateIndex(CvoFileDataParser* pDataParser);

	virtual VO_U32		Prepare();
	virtual VO_U32		Unprepare();

protected:
	CBaseStreamFileReader*	m_pStreamFileReader;
	CBaseStreamFileIndex*	m_pStreamFileIndex;

	CGBuffer*				m_pGlobeBuffer;
	VO_U32					m_dwBufferTime;		//<S>
	VO_U32					m_dwGBExtSize;		//<Byte>

	CvoFileDataParser*		m_pDataParser;
	VO_BOOL					m_bParseForSelf;

	VO_U32					m_dwFileIndexInFastPlay;

	//used when two data parser combine to one, otherwise it is -1
	//until beyond this value, content can be processed actually
	//East 2009/12/07
	VO_U64					m_ullCurrParseFilePos;
};

#ifdef _VONAMESPACE
}
#endif
