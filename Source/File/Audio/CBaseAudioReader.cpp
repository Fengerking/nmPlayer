#include "CBaseAudioReader.h"
#include "ID3v2DataStruct.h"
#include "fCodec.h"

#define LOG_TAG	"CBaseAudioReader"
#include "voLog.h"

#define VO_AUDIOREADER_GETSAMPLEREADSIZE	0x2000		//8K

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

const VO_U8 bits_flag[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};

VO_BOOL skip_bits_func(CGFileChunk* _chunk, VO_U32 _size, VO_U8* _left_data, VO_U8* _left_len)
{
	if(_size <= *_left_len)
		*_left_len -= (VO_U8)_size;
	else
	{
		_size -= *_left_len;
		if(!_chunk->FSkip((_size + 7) / 8 - 1))
			return VO_FALSE;
		if(!_chunk->FRead(_left_data, 1))
			return VO_FALSE;
		*_left_len = 8 - _size % 8;
	}

	return VO_TRUE;
}

VO_BOOL read_bits_func(CGFileChunk* _chunk, VO_U32* _data, VO_U8 _size, VO_U8* _left_data, VO_U8* _left_len)
{
	VO_U8 bCompare = 0;
	VO_S32 i = 0;
	if(*_left_len == 0)
	{
		if(!_chunk->FRead(_left_data, 1))
			return VO_FALSE;

		*_left_len = 8;
	}

	if(_size <= *_left_len)
	{
		bCompare = 0;
		for(i = 0; i < _size; i++)
			bCompare |= bits_flag[8 - *_left_len + i];
		*_left_len -= _size;
		*_data = (*_left_data & bCompare) >> *_left_len;
	}
	else
	{
		VO_S32 nForward = VO_S32(_size) - *_left_len;
		if(*_left_len > 0)
		{
			bCompare = 0;
			for(i = 0; i < *_left_len; i++)
				bCompare |= bits_flag[8 - *_left_len + i];
			*_data |= (VO_U32(*_left_data & bCompare) << nForward);
			nForward -= 8;
		}
		_size -= *_left_len;
		for(i = 0; i < ((_size + 7) / 8 - 1); i++)
		{
			if(!_chunk->FRead(_left_data, 1))
				return VO_FALSE;
			*_data |= (VO_U32(*_left_data) << nForward);
			nForward -= 8;
		}
		_size -= (VO_U8)(i * 8);
		if(!_chunk->FRead(_left_data, 1))
			return VO_FALSE;
		*_left_len = 8 - _size;
		if(*_left_len)
			*_data |= *_left_data >> *_left_len;
		else
			*_data |= *_left_data;
	}

	return VO_TRUE;
}
#ifdef _VONAMESPACE
}
#endif



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define VO_AUDIOREADER_GETSAMPLEREADSIZE	0x2000		//8K


CFramePosChain::CFramePosChain(CMemPool* pMemPool)
	: m_pHead(VO_NULL)
	, m_pTail(VO_NULL)
	, m_dwCount(0)
	, m_pMemPool(pMemPool)
{
}

CFramePosChain::~CFramePosChain()
{
	Release();
}

VO_VOID CFramePosChain::Release()
{
	if(m_pHead)
	{
		PFramePos pCur = m_pHead;
		PFramePos pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->next;

			m_pMemPool->free(pDel, sizeof(FramePos));
		}
		m_pHead = VO_NULL;
	}

	m_pTail = VO_NULL;
	m_dwCount = 0;
}

VO_BOOL CFramePosChain::Add(VO_U32 dwFilePos, VO_U32 dwTimeStamp)
{
	PFramePos pNew = (PFramePos)m_pMemPool->alloc(sizeof(FramePos));
	if(!pNew)
		return VO_FALSE;

	pNew->file_pos = dwFilePos;
	pNew->media_time = dwTimeStamp;
	pNew->next = VO_NULL;

	if(!m_pTail)
		m_pHead = m_pTail = pNew;
	else
		m_pTail = m_pTail->next = pNew;

	m_dwCount++;

	return VO_TRUE;
}

PFramePos CFramePosChain::GetFrameByFilePos(VO_U32 dwFilePos, VO_BOOL bIsPrev /* = VO_TRUE */)
{
	PFramePos pTmp = m_pHead;
	PFramePos pPrev = VO_NULL;
	while(pTmp)
	{
		if(pTmp->file_pos >= dwFilePos)
			return (bIsPrev && pPrev) ? pPrev : pTmp;

		pPrev = pTmp;
		pTmp = pTmp->next;
	}

	return pPrev;
}

PFramePos CFramePosChain::GetFrameByTime(VO_U32 dwTimeStamp, VO_BOOL bIsPrev /* = VO_TRUE */)
{
	PFramePos pTmp = m_pHead;
	PFramePos pPrev = VO_NULL;
	while(pTmp)
	{
		if(pTmp->media_time > dwTimeStamp)
			return (bIsPrev && pPrev) ? pPrev : pTmp;

		pPrev = pTmp;
		pTmp = pTmp->next;
	}

	return pPrev;
}

PFramePos CFramePosChain::GetFrameByIndex(VO_U32 dwIndex)
{
	PFramePos pTmp = m_pHead;
	VO_U32 dwFrames = 0;
	while(pTmp)
	{
		if(dwFrames >= dwIndex)
			break;

		dwFrames++;
		if(dwFrames >= m_dwCount)
			break;

		pTmp = pTmp->next;
	}

	return pTmp;
}

PFramePos CFramePosChain::GetFrameByIndex(VO_U32 dwIndex, PFramePos pFrameRef, VO_U32 dwIndexRef)
{
	if(!pFrameRef || dwIndex < dwIndexRef)
		return GetFrameByIndex(dwIndex);

	if(dwIndex == dwIndexRef)
		return pFrameRef;
	else
	{
		PFramePos pTmp = pFrameRef;
		for(VO_U32 i = dwIndexRef; i < dwIndex && pTmp; i++)
			pTmp = pTmp->next;

		return pTmp;
	}
}

VO_U32 CFramePosChain::GetFramIndex(PFramePos pFrameRef)
{
	if(!pFrameRef)
		return 0;		//here will be same to first frame, East note

	PFramePos pTmp = m_pHead;
	VO_U32 dwFrames = 0;
	while(pTmp)
	{
		if(pTmp == pFrameRef)
			break;

		pTmp = pTmp->next;
		dwFrames++;
	}

	return dwFrames;
}

CBaseAudioReader::CBaseAudioReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_ullFileHeadSize(0)
	, m_dwDuration(0)
	, m_PlayMode(VO_SOURCE_PM_PLAY)
	, m_dwMaxSampleSize(0x800)		//2K
	, m_pBuf(VO_NULL)
	, m_dwBufLength(0)
	, m_dwBufReaded(0)
	, m_bEndOfStream(VO_FALSE)
	, m_memPool(pMemOp)
{
}

CBaseAudioReader::~CBaseAudioReader()
{
	SAFE_MEM_FREE(m_pBuf);
}

VO_U32 CBaseAudioReader::GetInfo(VO_SOURCE_INFO* pSourceInfo)
{
	if(!pSourceInfo)
		return VO_ERR_INVALID_ARG;

	pSourceInfo->Duration = GetDuration();
	pSourceInfo->Tracks = 1;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CBaseAudioReader::SkipID3V2Header()
{
	ID3v2Header header;
	read_pointer(&header, ID3V2_HEADER_LEN);

	//judge if it is ID3v2 header!!
	static const VO_CHAR byteID3[3] = {0x49, 0x44, 0x33};
	if(MemCompare(header.szTag, (VO_PTR)byteID3, 3))
	{
		m_chunk.FBack(ID3V2_HEADER_LEN);
		return VO_FALSE;
	}

	//calculate the size and skip it!!
	VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE(header.btSize);
	m_ullFileHeadSize += (ID3V2_HEADER_LEN + dwSize);
	skip(dwSize);

	return VO_TRUE;
}

VO_VOID CBaseAudioReader::NewReadSampleBuffer()
{
	SAFE_MEM_FREE(m_pBuf);
	m_pBuf = NEW_BUFFER(VO_AUDIOREADER_GETSAMPLEREADSIZE + m_dwMaxSampleSize);
	m_dwBufLength = m_dwBufReaded = 0;
}

VO_U32 CBaseAudioReader::ReadSampleContent(VO_SOURCE_SAMPLE* pSample)
{
	if(m_dwBufLength < m_dwMaxSampleSize + m_dwBufReaded)
	{
		//left buffer less than max sample size, we should read from file
		VO_BOOL bCopyBuf = VO_TRUE;
		if(m_dwBufLength > m_dwBufReaded)	//has left buffer, you should copy them
		{
			m_dwBufLength -= m_dwBufReaded;
			MemCopy(m_pBuf, m_pBuf + m_dwBufReaded, m_dwBufLength);
		}
		else
		{
			m_dwBufLength = 0;
			bCopyBuf = VO_FALSE;
		}
		m_dwBufReaded = 0;

		VO_S32 nRes = FileRead(m_hFile, m_pBuf + m_dwBufLength, VO_AUDIOREADER_GETSAMPLEREADSIZE);
		if(nRes > 0)
			m_dwBufLength += nRes;
		else if(-2 == nRes) {
			//VOLOGI("rodney8: nRes == -2"); 
			return VO_ERR_SOURCE_NEEDRETRY;
		}
		else	//read file fail!!
		{
			if(!bCopyBuf) {
				//VOLOGI("rodney9: nRes = 0x%08x, m_pBuf = 0x%llx, m_dwBufLength = 0x%llx", nRes, m_pBuf, m_dwBufLength); 
				return VO_ERR_SOURCE_END;
			}
		}
	}

	VO_U32 dwReaded = 0, dwSampleStart = 0, dwSampleEnd = 0;
	VO_PBYTE pBuffer = m_pBuf + m_dwBufReaded;
	if(!ReadSampleFromBuffer(pBuffer, m_dwBufLength - m_dwBufReaded, dwReaded, dwSampleStart, dwSampleEnd, VO_NULL))
	{
		//VOLOGI("rodney: source end");
		return VO_ERR_SOURCE_END;
	}

	pSample->Buffer = pBuffer + dwSampleStart;
	pSample->Size = dwSampleEnd - dwSampleStart;
	m_dwBufReaded += dwReaded;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseAudioReader::SetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseAudioReader::GetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VO_PID_COMMON_HEADDATA:
		return GetHeadData((VO_CODECBUFFER*)pParam);

	case VO_PID_SOURCE_MAXSAMPLESIZE:
		return GetMaxSampleSize((VO_U32*)pParam);

	case VO_PID_SOURCE_CODECCC:
		return GetCodecCC((VO_U32*)pParam);

	case VO_PID_SOURCE_FIRSTFRAME:
		return GetFirstFrame((VO_SOURCE_SAMPLE*)pParam);

	case VO_PID_AUDIO_FORMAT:
		return GetAudioFormat((VO_AUDIO_FORMAT*)pParam);

	case VO_PID_SOURCE_BITRATE:
		return GetBitrate((VO_U32*)pParam);

	case VO_PID_SOURCE_WAVEFORMATEX:
		return GetWaveFormatEx((VO_WAVEFORMATEX**)pParam);

	default:
		break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseAudioReader::GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo)
{
	if(!pTrackInfo)
		return VO_ERR_INVALID_ARG;

	pTrackInfo->Start = 0;
	pTrackInfo->Duration = GetDuration();
	pTrackInfo->Type = VO_SOURCE_TT_AUDIO;

	VO_U32 nRes = GetCodec(&pTrackInfo->Codec);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	VO_CODECBUFFER cb;
	nRes = GetHeadData(&cb);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	pTrackInfo->HeadData = cb.Buffer;
	pTrackInfo->HeadSize = cb.Length;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseAudioReader::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	*pdwMaxSampleSize = m_dwMaxSampleSize;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseAudioReader::GetCodec(VO_U32* pCodec)
{
	VO_U32 dwFcc = 0;
	VO_U32 nRes = GetCodecCC(&dwFcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	if(pCodec)
		*pCodec = fCodecGetAudioCodec((VO_U16)dwFcc);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseAudioReader::GetFileHeadSize(VO_U32* pdwFileHeadSize)
{
	*pdwFileHeadSize = (VO_U32)m_ullFileHeadSize;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseAudioReader::SetPlayMode(VO_SOURCE_PLAYMODE PlayMode)
{
	m_PlayMode = PlayMode;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseAudioReader::SetEndOfStream(VO_BOOL bEndOfStream)
{
	m_bEndOfStream = bEndOfStream;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CBaseAudioReader::PCM24216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign)
{
	if(!nChannels || !nBlockAlign)
		return VO_FALSE;

	VO_U16 wMinBlock = nChannels * 3;
	if(nBlockAlign % wMinBlock)
		return VO_FALSE;

	VO_S16* pDst = (VO_S16*)pBuffer;
	VO_PBYTE pSrc = pBuffer;
	VO_U32 dwNum = *pdwSize / wMinBlock;
	for(VO_U32 i = 0; i < dwNum; i++)
	{
		for(VO_U16 j = 0;  j < nChannels; j++)
		{
			*pDst++ = ((pSrc[2] << 8) | pSrc[1]);
			pSrc += 3;
		}
	}

	*pdwSize = 2 * nChannels * dwNum;
	return VO_TRUE;
}

VO_BOOL CBaseAudioReader::PCM32216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign)
{
	if(!nChannels || !nBlockAlign)
		return VO_FALSE;

	VO_U16 wMinBlock = nChannels * 4;
	if(nBlockAlign % wMinBlock)
		return VO_FALSE;

	VO_S16* pDst = (VO_S16*)pBuffer;
	VO_PBYTE pSrc = pBuffer;
	VO_U32 dwNum = *pdwSize / wMinBlock;
	for(VO_U32 i = 0; i < dwNum; i++)
	{
		for(VO_U16 j = 0;  j < nChannels; j++)
		{
			*pDst++ = ((pSrc[3] << 8) | pSrc[2]);
			pSrc += 4;
		}
	}

	*pdwSize = 2 * nChannels * dwNum;
	return VO_TRUE;
}
