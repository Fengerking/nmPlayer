#pragma once
#include "CBaseReader.h"
#include "CMemPool.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

VO_BOOL skip_bits_func(CGFileChunk* _chunk, VO_U32 _size, VO_U8* _left_data, VO_U8* _left_len);
VO_BOOL read_bits_func(CGFileChunk* _chunk, VO_U32* _data, VO_U8 _size, VO_U8* _left_data, VO_U8* _left_len);

#define read_bits(_value, _bits)\
{\
	_value = 0;\
	if(!read_bits_func(&m_chunk, &_value, _bits, &bTmpData, &bTmpLen))\
		return VO_FALSE;\
}

#define skip_bits(_bits)\
{\
	if(!skip_bits_func(&m_chunk, _bits, &bTmpData, &bTmpLen))\
		return VO_FALSE;\
}



typedef struct tagFramePos
{
	VO_U32			file_pos;
	VO_U32			media_time;		//<MS>

	tagFramePos*	next;
} FramePos, *PFramePos;

class CFramePosChain : 
	public CvoBaseObject
{
public:
	CFramePosChain(CMemPool* pMemPool);
	virtual ~CFramePosChain();

public:
	VO_BOOL		Add(VO_U32 dwFilePos, VO_U32 dwTimeStamp);
	PFramePos	GetFrameByFilePos(VO_U32 dwFilePos, VO_BOOL bIsPrev = VO_TRUE);
	PFramePos	GetFrameByTime(VO_U32 dwTimeStamp, VO_BOOL bIsPrev = VO_TRUE);
	PFramePos	GetFrameByIndex(VO_U32 dwIndex);
	PFramePos	GetFrameByIndex(VO_U32 dwIndex, PFramePos pFrameRef, VO_U32 dwIndexRef);
	VO_U32		GetFramIndex(PFramePos pFrameRef);
	VO_U32		GetCount() {return m_dwCount;}

	VO_VOID		Release();

protected:
	PFramePos	m_pHead;
	PFramePos	m_pTail;
	VO_U32		m_dwCount;
	CMemPool*	m_pMemPool;
};


class CBaseAudioReader :
	public CBaseReader
{
public:
	CBaseAudioReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CBaseAudioReader();

public:
	virtual VO_U32		GetInfo(VO_SOURCE_INFO* pSourceInfo);

	virtual VO_U32		SetTrackParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetTrackParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo);
	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample) = 0;
	virtual VO_U32		SetPos(VO_S64* pPos) = 0;

	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData) = 0;
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32		GetCodec(VO_U32* pCodec);
	virtual VO_U32		GetCodecCC(VO_U32* pCC) = 0;
	virtual VO_U32		GetFirstFrame(VO_SOURCE_SAMPLE* pSample) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate) = 0;
	virtual VO_U32		GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx) {return VO_ERR_NOT_IMPLEMENT;}

	virtual VO_U32		SetPlayMode(VO_SOURCE_PLAYMODE PlayMode);
	virtual VO_U32		GetDuration() {return m_dwDuration;}

	virtual VO_U32		SetEndOfStream(VO_BOOL bEndOfStream);
	virtual VO_BOOL		IsEndOfStream() {return m_bEndOfStream;}

protected:
	virtual	VO_U32		GetFileHeadSize(VO_U32* pdwFileHeadSize);

protected:
	virtual	VO_BOOL		SkipID3V2Header();
	//only get sample content(Buffer and Size)!!
	virtual VO_U32		ReadSampleContent(VO_SOURCE_SAMPLE* pSample);
	virtual VO_BOOL		ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync) {return VO_FALSE;}
	virtual VO_VOID		NewReadSampleBuffer();

	virtual VO_BOOL		PCM24216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign);
	virtual VO_BOOL		PCM32216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign);

protected:
	VO_U32				m_ullFileHeadSize;

	VO_U32				m_dwDuration;			//<MS>

	VO_SOURCE_PLAYMODE	m_PlayMode;

	VO_U32				m_dwMaxSampleSize;

	VO_PBYTE			m_pBuf;
	VO_U32				m_dwBufLength;
	VO_U32				m_dwBufReaded;

	VO_BOOL				m_bEndOfStream;

	CMemPool			m_memPool;
};

#ifdef _VONAMESPACE
}
#endif
