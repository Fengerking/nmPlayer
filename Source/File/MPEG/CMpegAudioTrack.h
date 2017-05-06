#pragma once
#include "CMpegTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct 
{
	VO_U8	nStreamNum;
	VO_U8	nFrameCnt;
	VO_U16	FrameDataOffset;
	struct  
	{
		VO_U8 emphasis		: 1;
		VO_U8 mute 			: 1;
		VO_U8 reserved		: 1;
		VO_U8 FrmNum		: 5;

	}LPCMParameter1;
	struct  
	{
		VO_U8 Quantization	: 2;
		VO_U8 Sample_rate 	: 2;
		VO_U8 reserved		: 1;
		VO_U8 channels		: 3;

	}LPCMParameter2;
	VO_U8	LPCMParameter3_Dynamic_Range;

}ExtentedTrackHeader;

typedef struct
{
	VO_U32			m_nSampleSize;
	VO_S32			m_nPosBuf;
	VO_U32			m_nCurBufSize;
	VO_S64			m_nFirstTimeStamp;

	VO_U32			IsWholeSample();
	VO_BOOL			ResetSample();
	VO_VOID			ResetSampleBuffer();
}PCMSamplePack;

class CMpegAudioTrack :
	public CMpegTrack ,public ExtentedTrackHeader ,public PCMSamplePack
{
DECLARE_USE_MP3_GLOBAL_VARIABLE
public:
	CMpegAudioTrack(VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader *pReader, VO_MEM_OPERATOR* pMemOp, VO_U8 btSubStreamID = 0xFF);
	virtual ~CMpegAudioTrack();

public:
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_VOID			Flush();
	VO_U32					GetParameter(VO_U32 uID, VO_PTR pParam);
protected:
	virtual VO_BOOL			OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt = VO_FALSE);
	virtual VO_BOOL			SetExtData(VO_PBYTE pData, VO_U16 wSize);
	virtual VO_U32			ParExtentedStream(VO_PBYTE pData, VO_U32 wLen);
	VO_BOOL					ParAc3HeadData(VO_PBYTE pData, VO_U32 wLen);
	VO_BOOL					ParDtsHeadData(VO_PBYTE pData, VO_U32 wLen);
protected:
	VO_U32					m_dwSampleRate;
	VO_U16					m_wChannels;
	VO_U8					m_btAudioLayer;
	VO_U8					m_btVersion;
};
#ifdef _VONAMESPACE
}
#endif
