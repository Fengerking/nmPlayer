#pragma once
#include "CMpegTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMpegVideoTrack :
	public CMpegTrack
{
DECLARE_USE_MPEG_GLOBAL_VARIABLE
public:
	CMpegVideoTrack(VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader *pReader, VO_MEM_OPERATOR* pMemOp,VO_U8 btSubStreamID = 0xFF);
	virtual ~CMpegVideoTrack();

public:
	virtual	VO_U32			GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32			GetFrameTime(VO_U32* pdwFrameTime);
	VO_U32					CheckHeadDataBuf(VO_PBYTE pBuf,VO_U32 nSize);
	virtual VO_U32 			GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
protected:
	virtual VO_VOID			Flush();

	virtual VO_BOOL			OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt = VO_FALSE);
	virtual VO_BOOL			SetExtData(VO_PBYTE pData, VO_U16 wSize);
	virtual VO_BOOL			ProcessPrevStartCode(VO_PBYTE pPESBuffer, CodeInfo iStartCode, VO_U16 wCurrPos);
	virtual VO_BOOL			AddPayload(VO_PBYTE pData, VO_U16 wSize);
	VO_U32					FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_U32 dwSize);

protected:
	VO_PBYTE				m_pSeqHeader;
	VO_U16					m_wSeqHeaderSize;

	VO_S32					m_wWidth;
	VO_S32					m_wHeight;
	VO_U16					m_wFrameRate;	//frame rate * 100
	VO_BOOL					m_bMPEG2;

	VO_U8					m_btLeftBytes;

	PBaseStreamMediaSampleIndexEntry	m_pTempEntry;
	VO_U32					m_uThumbnailCnt;
};
#ifdef _VONAMESPACE
}
#endif