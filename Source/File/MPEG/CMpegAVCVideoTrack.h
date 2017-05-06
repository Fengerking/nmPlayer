#pragma once
#include "CMpegVideoTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMpegAVCVideoTrack :
	public CMpegVideoTrack
{
DECLARE_USE_MPEG_GLOBAL_VARIABLE
public:
	CMpegAVCVideoTrack(VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader *pReader, VO_MEM_OPERATOR* pMemOp,VO_U8 btSubStreamID = 0xFF);
	virtual ~CMpegAVCVideoTrack();

public:
	VO_U32			GetHeadData(VO_CODECBUFFER* pHeadData);
	VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	VO_U32			GetCodecCC(VO_U32* pCC);
	VO_U32			GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	VO_U32			GetFrameTime(VO_U32* pdwFrameTime);

protected:
	VO_VOID			Flush();

	VO_BOOL			OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt = VO_FALSE);
	VO_BOOL			SetExtData(VO_PBYTE pData, VO_U16 wSize);
	VO_BOOL			ProcessPrevStartCode(VO_PBYTE pPESBuffer,VO_U16 wCurrPos);
	VO_BOOL			AddPayload(VO_PBYTE pData, VO_U16 wSize);
	VO_PBYTE		FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_PBYTE pBufferEnd);
	VO_BOOL			AssemblePayloadToFrame(VO_PBYTE pData, VO_U16 wSize);
protected:
	VO_U8					m_wCntByteSyncWord;
	VO_U32					m_LeftData;
	VO_U8					m_bDelimiter;
};

#ifdef _VONAMESPACE
}
#endif