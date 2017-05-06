#pragma once
#include "CRealTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct ra_block_struct
{
	VO_PBYTE	pData;
	VO_U32		ulDataLen;
	VO_U32		ulTimestamp;
	VO_U32		ulDataFlags;
} ra_block;
class CRealAudioTrack :
	public CRealTrack
{
public:
	CRealAudioTrack(PRealTrackInfo pTrackInfo, CRealReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CRealAudioTrack();

public:
	virtual VO_U32			GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			SetPosN(VO_S64* pPos);

	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodec(VO_U32* pCodec);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);

	virtual VO_BOOL			SetGlobalBufferExtSize(VO_U32 dwExtSize);

protected:
	virtual	VO_BOOL			ParseExtData(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize);
	virtual	VO_BOOL			ParseExtData_AudioFormat3(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize);
	virtual	VO_BOOL			ParseExtData_AudioFormat4(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize);
	virtual	VO_BOOL			ParseExtData_AudioFormat5(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize);

	virtual VO_VOID			Flush();

	virtual VO_BOOL			OnPacket(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp);

	inline VO_BOOL			AddPayloadInfo(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp);
	//return VO_TRUE mean do actual add!!
	inline VO_BOOL			AddTmpEntriesToIndex();

protected:
	//CBR && interleave
	VO_U16					m_wInterleaveFactor;    /* number of blocks per super block */

	VO_U16					m_wInterleaveBlockSize; /* size of each interleave block */
	VO_U16					m_wInterleaveFrameSize;
	VO_U16					m_wInterleaveFramesPerBlock;

	VO_U16*					m_pwInterleavePattern;
	VO_U16					m_wInterleavePatternSize;

	//count: m_wInterleaveFactor
	PBaseStreamMediaSampleIndexEntry*	m_ppTempEntries;

	VO_PBYTE*				m_ppTempBuf;
	VO_U16					m_wCurrTmpEntry;
	VO_BOOL					m_bSuperBlockSent;

	//VBR && no interleave
	VO_U16*					m_pwFrameSizes;

	ra_block				m_TmpBlock;
};

#ifdef _VONAMESPACE
}
#endif