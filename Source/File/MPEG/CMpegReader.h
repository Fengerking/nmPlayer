#pragma once

#include "CBaseStreamFileReader.h"
#include "CMpegTrack.h"
#include "CMpegParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct  
{
	VO_S64 dwStreamID;
	VO_S64 dwTimeStamp;
	VO_U64 dwPESPos;
	VO_VOID Init(){dwStreamID = -1;dwTimeStamp = -1;dwPESPos = 0;}
}MPEGPESInfo;

class CMpegReader :
	public CBaseStreamFileReader
{
public:
	CMpegReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CMpegReader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();

public:
	virtual VO_VOID		OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);

	inline VO_U8		OnPacket(CGFileChunk* pFileChunk, VO_U8 btStreamID, VO_U16 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt = VO_FALSE);
	virtual	VO_U32		GetBitrate();

	virtual VO_VOID		SetLastTimeStamp(VO_S64 dwTimeStamp);

	virtual VO_VOID		Flush();
	VO_VOID				Flush2();
	virtual VO_U32		OnDrmCallBack(VO_PTR pParam); 
	CMpegTrack*			GetTrackByIndex(VO_U32 nIndex);
	VO_BOOL				IsTrackBufferHasEnoughFrame();
	VO_U32				GetDRMType(VO_U32 * pDrmType);
	inline 	VO_VOID     SetPESInfo(VO_S64 dwStreamID,VO_S64 llTs,VO_U64 dwPESInfo){m_CurPESInfo.dwStreamID = dwStreamID;m_CurPESInfo.dwTimeStamp = llTs;m_CurPESInfo.dwPESPos = dwPESInfo;}
protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);
	VO_BOOL				IsCanGenerateIndex();
	VO_U32				GetUsedTrackCnt();
	VO_S64				GetFilePosByTime(VO_S64 llTs,VO_S64 *llRealTs,VO_S64 dwStreamID,MPEGPESInfo* pMinPESInfo,MPEGPESInfo* pMaxPESInfo);
	VO_U32				FindAndCreateTrack();

protected:
	CMpegDataParser		m_DataParser;
	VO_S64				m_dwFirstTimeStamp;
	VO_S64				m_dwLastTimeStamp;
	VO_BOOL				m_ForStreaming;
	VO_PBYTE			m_pVideoCacheHeadDataBuf;
	VO_U32				m_nVideoCacheBufSize;
	VO_BOOL				m_bNeedCheckCache;
	VO_BOOL				m_bOpenEnd;
	VO_S32				m_nCurBufTime;
	MPEGPESInfo			m_CurPESInfo;
	VO_U32				m_uInitTrackCnt;
};

#ifdef _VONAMESPACE
}
#endif