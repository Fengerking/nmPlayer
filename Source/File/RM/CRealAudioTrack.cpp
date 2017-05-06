#include "CRealReader.h"
#include "CRealAudioTrack.h"
#include "voRealAudio.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define CHECK_VORA_INIT_PARAM	if(!m_pInitParam)\
	return VO_ERR_OUTOF_MEMORY;\
	VORA_INIT_PARAM* pInitParam = (VORA_INIT_PARAM*)m_pInitParam;

#define RA_VBR_MAXFRAMESINPACKET	4096

CRealAudioTrack::CRealAudioTrack(PRealTrackInfo pTrackInfo, CRealReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CRealTrack(VOTT_AUDIO, pTrackInfo, pReader, pMemOp)
	, m_wInterleaveFactor(0)
	, m_wInterleaveBlockSize(0)
	, m_wInterleaveFrameSize(0)
	, m_wInterleaveFramesPerBlock(0)
	, m_pwInterleavePattern(VO_NULL)
	, m_wInterleavePatternSize(0)
	, m_ppTempEntries(VO_NULL)
	, m_ppTempBuf(VO_NULL)
	, m_wCurrTmpEntry(0)
	, m_bSuperBlockSent(VO_FALSE)
	, m_pwFrameSizes(VO_NULL)
{
	ParseExtData(pTrackInfo->pPropBuffer, pTrackInfo->dwPropBufferSize);

	m_TmpBlock.ulDataFlags = 0xFFFFFFFF;
	if(1 == m_wInterleaveFactor)	//VBR && no interleave
		m_pwFrameSizes = NEW_OBJS(VO_U16, RA_VBR_MAXFRAMESINPACKET);
	else if(m_wInterleaveFactor>1)
	{
		m_ppTempBuf = NEW_OBJS(VO_PBYTE, m_wInterleaveFactor);
		m_TmpBlock.ulDataLen = m_wInterleaveBlockSize;
		m_ppTempEntries = NEW_OBJS(PBaseStreamMediaSampleIndexEntry, m_wInterleaveFactor);
		for(VO_U16 i = 0; i < m_wInterleaveFactor; i++)
			m_ppTempEntries[i] = VO_NULL;
	}
	else
	{
		m_ppTempBuf = VO_NULL;
		m_ppTempEntries = VO_NULL;
	}
}

CRealAudioTrack::~CRealAudioTrack()
{
	if(m_ppTempEntries)
	{
		for(VO_U16 i = 0; i < m_wInterleaveFactor; i++)
		{
			if(m_ppTempEntries[i])
			{
				m_pStreamFileIndex->DeleteEntry(m_ppTempEntries[i]);
				m_ppTempEntries[i] = VO_NULL;
			}
		}

		MemFree(m_ppTempEntries);
		m_ppTempEntries = VO_NULL;
	}

	SAFE_MEM_FREE(m_pwFrameSizes);
	SAFE_MEM_FREE(m_ppTempBuf);
	SAFE_MEM_FREE(m_pwInterleavePattern);
}

VO_VOID CRealAudioTrack::Flush()
{
	m_bSuperBlockSent = VO_FALSE;
	m_wCurrTmpEntry = 0;

	if(m_ppTempEntries)
	{
		for(VO_U16 i = 0; i < m_wInterleaveFactor; i++)
		{
			if(m_ppTempEntries[i])
			{
				m_pStreamFileIndex->DeleteEntry(m_ppTempEntries[i]);
				m_ppTempEntries[i] = VO_NULL;
			}
		}
	}
	
	CRealTrack::Flush();
}

VO_U32 CRealAudioTrack::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = CRealTrack::GetSample(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	ra_block* pBlock = (ra_block*)pSample->Buffer;
	pBlock->ulTimestamp = (VO_U32)pSample->Time;
	pBlock->pData = pSample->Buffer + sizeof(ra_block);

	pSample->Size = (sizeof(ra_block) + pBlock->ulDataLen) | (pSample->Size & 0x80000000);
	if(1 != m_wInterleaveFactor && !m_bSuperBlockSent)	//CBR and not send super block!!
	{
		if(pSample->Size & 0x80000000)	//super block
			m_bSuperBlockSent = VO_TRUE;
		else
			pSample->Size = 0;	//not send this frame!!
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealAudioTrack::SetPosN(VO_S64* pPos)
{
	m_pReader->OnTrackSetPosN(this, *pPos);

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime((1 != m_wInterleaveFactor) ? VO_TRUE : VO_FALSE, *pPos, &pGet);
	if(-1 == nRes)
		return VO_ERR_SOURCE_END;
	else if(1 == nRes)	//retry
	{
		VO_BOOL bParseEnd = VO_FALSE;
		while(1 == nRes && !bParseEnd)	//retry!!
		{
			if(m_pGlobeBuffer)
				m_pGlobeBuffer->RemoveTo2(pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size);
			m_pStreamFileIndex->RemoveAll();

			if(!TrackGenerateIndex())
				bParseEnd = VO_TRUE;

			nRes = m_pStreamFileIndex->GetEntryByTime((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, *pPos, &pGet);
			if(-1 == nRes)
				return VO_ERR_SOURCE_END;
		}

		if(0 != nRes)
			return VO_ERR_SOURCE_END;

		*pPos = pGet->time_stamp;
		m_pStreamFileIndex->RemoveUntil(pGet);
		TrackGenerateIndex();
	}
	else	//ok
	{
		*pPos = pGet->time_stamp;
		m_pStreamFileIndex->RemoveUntil(pGet);
		m_pStreamFileReader->FileGenerateIndex();
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealAudioTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	//CHECK_VORA_INIT_PARAM

	if(pdwMaxSampleSize)
	{
		if(1 == m_wInterleaveFactor)
			*pdwMaxSampleSize = m_dwBitrate;
		else if(m_wInterleaveFactor>1)	//block size
			*pdwMaxSampleSize = m_wInterleaveBlockSize;
		else
			*pdwMaxSampleSize = 0;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealAudioTrack::GetCodec(VO_U32* pCodec)
{
	*pCodec = VO_AUDIO_CodingRA;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealAudioTrack::GetCodecCC(VO_U32* pCC)
{
	CHECK_VORA_INIT_PARAM

	*pCC = pInitParam->ulFourCC;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealAudioTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	CHECK_VORA_INIT_PARAM

	pAudioFormat->Channels = pInitParam->format->usNumChannels;
	pAudioFormat->SampleBits = pInitParam->format->usBitsPerSample;
	pAudioFormat->SampleRate = pInitParam->format->ulActualRate ? pInitParam->format->ulActualRate : pInitParam->format->ulSampleRate;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CRealAudioTrack::ParseExtData(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize)
{
	VO_U32 dwValue;
	VO_U16 wValue;

	VO_PBYTE pSrc = pPropBuffer;

	use_big_endian_read

	read_fcc2(dwValue);

	VO_BOOL bRet = VO_FALSE;
	if(FOURCC_MLTI == dwValue)
	{
		//TODO: process multi-stream header
	}
	else if(FOURCC_RAID == dwValue)
	{
		//header_version
		read_word2(wValue);
		switch(wValue)
		{
		case 3:		//NOT implement
			{
				bRet = ParseExtData_AudioFormat3(pSrc, dwPropBufferSize - (pSrc - pPropBuffer));
			}
			break;

		case 4:
			{
				bRet = ParseExtData_AudioFormat4(pSrc, dwPropBufferSize - (pSrc - pPropBuffer));
			}
			break;

		case 5:
			{
				bRet = ParseExtData_AudioFormat5(pSrc, dwPropBufferSize - (pSrc - pPropBuffer));
			}
			break;
		default:
			{
				;
			}
			break;
		}
	}

	return bRet;
}

VO_BOOL CRealAudioTrack::ParseExtData_AudioFormat3(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize)
{
	//not implemented
	return VO_FALSE;
}

VO_BOOL CRealAudioTrack::ParseExtData_AudioFormat4(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize)
{
	if( pPropBuffer && dwPropBufferSize > 0 )
	{
		VO_PBYTE pSrc = pPropBuffer;

		use_big_endian_read

		VO_U8 btValue;
		VO_U16 wValue;
		// Skip first 10 bytes , 10 bytes reserved
		skip2(10);
		// Sanity check the version and revision
		//version
		read_word2(wValue);
		if(4 != wValue)
		{
			return VO_FALSE;
		}

		//revision
		read_word2(wValue);
		if(0 != wValue)
		{
			return VO_FALSE;
		}

		SAFE_MEM_FREE(m_pInitParam);

		m_dwInitParamSize = sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO);
		m_pInitParam = NEW_BUFFER(m_dwInitParamSize);
		if(!m_pInitParam)
		{
			return VO_FALSE;
		}

		MemSet(m_pInitParam, 0, m_dwInitParamSize);

		VORA_INIT_PARAM* pInitParam = (VORA_INIT_PARAM*)m_pInitParam;
		VORA_FORMAT_INFO* pFormat = (VORA_FORMAT_INFO*)(pInitParam + 1);
		pInitParam->format = pFormat;
		pFormat->usAudioQuality = 100;

		//skip usHeaderBytes
		skip2(2);
		read_word2(pFormat->usFlavorIndex);
		read_dword2(pFormat->ulGranularity);
		//skip ulTotalBytes
		skip2(4);
		read_dword2(m_dwBitrate);
		m_dwBitrate /= 60;
		//skip ulBytesPerMin2
		skip2(4);
		read_word2(m_wInterleaveFactor);
		read_word2(m_wInterleaveBlockSize);
		//skip ulUserData
		skip2(4);
		read_dword2(pFormat->ulSampleRate);
		pFormat->ulSampleRate >>= 16;
		read_word2(pFormat->usBitsPerSample);
		read_word2(pFormat->usNumChannels);

		read_byte2(btValue);
		if(btValue == 4)
		{
			//skip ulInterleaverID
			skip2(4);
		}

		//codec_id_length
		read_byte2(btValue);
		if(btValue == 4)
		{
			//codec_id
			read_dword2(pInitParam->ulFourCC);
		}

		//skip bIsInterleaved, bCopyByte, ucScatterType
		skip2(3);

		// If the interleave factor is 0, make it 1.
		if(!m_wInterleaveFactor)
		{
			m_wInterleaveFactor = 1;
		}

		//the following param has not been parsed:
		//title_string_length , title_string_byte[i] , author_string_length ,author_string_byte[i] , 
		//copyright_string_length , copyright_string_byte[i] , user_string_length , user_string_byte[i]

		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_FALSE;
}

VO_BOOL CRealAudioTrack::ParseExtData_AudioFormat5(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize)
{
	if(pPropBuffer && dwPropBufferSize >= 68)
	{
		VO_PBYTE pSrc = pPropBuffer;

		use_big_endian_read

		VO_U8 btValue;
		VO_U16 wValue;
		/* Skip first 10 bytes */
		skip2(10);
		/* Sanity check the version and revision */
		read_word2(wValue);
		if(5 != wValue)
		{
			return VO_FALSE;
		}

		read_word2(wValue);
		if(0 != wValue)
		{
			return VO_FALSE;
		}

		m_dwInitParamSize = sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO);
		m_pInitParam = NEW_BUFFER(m_dwInitParamSize);
		if(!m_pInitParam)
			return VO_FALSE;

		MemSet(m_pInitParam, 0, m_dwInitParamSize);

		VORA_INIT_PARAM* pInitParam = (VORA_INIT_PARAM*)m_pInitParam;
		VORA_FORMAT_INFO* pFormat = (VORA_FORMAT_INFO*)(pInitParam + 1);
		pInitParam->format = pFormat;
		pFormat->usAudioQuality = 100;

		//skip usHeaderBytes
		skip2(2);
		read_word2(pFormat->usFlavorIndex);
		read_dword2(pFormat->ulGranularity);
		//skip ulTotalBytes
		skip2(4);
		read_dword2(m_dwBitrate);
		m_dwBitrate /= 60;
		//skip ulBytesPerMin2
		skip2(4);
		read_word2(m_wInterleaveFactor);
		read_word2(m_wInterleaveBlockSize);
		read_word2(pFormat->ulBitsPerFrame);
		//skip ulUserData
		skip2(4);
		read_dword2(pFormat->ulSampleRate);
		pFormat->ulSampleRate >>= 16;
		read_dword2(pFormat->ulActualRate);
		pFormat->ulActualRate >>= 16;
		read_word2(pFormat->usBitsPerSample);
		read_word2(pFormat->usNumChannels);
		//skip ulInterleaverID
		skip2(4);
		read_dword2(pInitParam->ulFourCC);
		//skip bIsInterleaved, bCopyByte, ucStreamType
		skip2(3);

		//ucScatterType
		//has_interleave_pattern_flag
		read_byte2(btValue);

		SAFE_MEM_FREE(m_pwInterleavePattern);
		m_wInterleaveFrameSize = (VO_U16)pFormat->ulBitsPerFrame;
		if( m_wInterleaveFrameSize > 0 )
		{
			m_wInterleaveFramesPerBlock = m_wInterleaveBlockSize / m_wInterleaveFrameSize;
			m_wInterleavePatternSize = m_wInterleaveFactor * m_wInterleaveFramesPerBlock;
			m_pwInterleavePattern = NEW_OBJS(VO_U16, m_wInterleavePatternSize);
			if(!m_pwInterleavePattern)
			{
				return VO_FALSE;
			}
		}
		else
		{
			return VO_FALSE;
		}

		MemSet(m_pwInterleavePattern, 0, m_wInterleavePatternSize * sizeof(VO_U16));

		VO_BOOL bRet = VO_TRUE;
		if(btValue)
		{
			bRet = VO_FALSE;
			if(dwPropBufferSize - (pSrc - pPropBuffer) >= m_wInterleavePatternSize * sizeof(VO_U16))
			{
				for(VO_U16 i = 0; i < m_wInterleavePatternSize; i++)
				{
					read_word2(wValue);
//					m_pwInterleavePattern[i] = wValue;
					m_pwInterleavePattern[wValue] = i;
				}
				bRet = VO_TRUE;
			}
		}
		else
		{
			VO_U32 i = 0, b = 0, f = 0;
			VO_BOOL bEven = VO_TRUE;
			while(i < m_wInterleavePatternSize)
			{
//				m_pwInterleavePattern[i] = b * m_wInterleaveFramesPerBlock + f;
				VO_U32 idx = b * m_wInterleaveFramesPerBlock + f;
				if( idx <= (VO_U32)m_wInterleavePatternSize - 1)
				{
					m_pwInterleavePattern[idx] = (VO_U16)i;
				}
				else
				{
					break;
				}

				i++;
				b += 2;
				if(b >= m_wInterleaveFactor)
				{
					if(bEven)
					{
						bEven = VO_FALSE;
						b = 1;
					}
					else
					{
						bEven = VO_TRUE;
						b = 0;
						f++;
					}
				}
			}
		}

		if(bRet)
		{
			bRet = VO_FALSE;
			/* Make sure we have four bytes */
			if(dwPropBufferSize - (pSrc - pPropBuffer) >= 4)
			{
				read_dword2(pFormat->ulOpaqueDataSize);
				if(dwPropBufferSize - (pSrc - pPropBuffer) >= pFormat->ulOpaqueDataSize)
				{
					//re-alloc buffer
					VO_U32 dwNewInitParamSize = sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO) + pFormat->ulOpaqueDataSize;
					VO_PBYTE pNewInitParam = NEW_BUFFER(dwNewInitParamSize);
					if(!pNewInitParam)
						return VO_FALSE;

					MemCopy(pNewInitParam, m_pInitParam, m_dwInitParamSize);

					SAFE_MEM_FREE(m_pInitParam);

					m_pInitParam = pNewInitParam;
					m_dwInitParamSize = dwNewInitParamSize;

					pInitParam = (VORA_INIT_PARAM*)m_pInitParam;
					pFormat = (VORA_FORMAT_INFO*)(pInitParam + 1);
					pInitParam->format = pFormat;
					pFormat->pOpaqueData = VO_PBYTE(pFormat + 1);

					read_pointer2(pFormat->pOpaqueData, pFormat->ulOpaqueDataSize);
					bRet = VO_TRUE;

					/* If the interleave factor is 0, make it 1. */
					if(!m_wInterleaveFactor)
					{
						m_wInterleaveFactor = 1;
					}
				}
			}
		}

		return bRet;
	}

	return VO_FALSE;
}

VO_BOOL CRealAudioTrack::OnPacket(CGFileChunk* m_pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp)
{
	//if current content has been parsed, only skipped
	//East 2009/12/07
	if((VO_U64)-1 != m_ullCurrParseFilePos)
	{
		if(m_pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
			return m_pFileChunk->FSkip(wLen);
		else
			m_ullCurrParseFilePos = -1;
	}

	if(1 == m_wInterleaveFactor)	//VBR && no interleave
	{
		use_big_endian_read

		VO_U16 wFrames;
		ptr_read_word(wFrames);
		wFrames >>= 4;

		for(VO_U16 i = 0; i < wFrames; i++)
			ptr_read_word(m_pwFrameSizes[i]);

		for(VO_U16 i = 0; i < wFrames; i++)
		{
			if(!AddPayloadInfo(m_pFileChunk, btFlags, m_pwFrameSizes[i], dwTimeStamp))
				return VO_FALSE;

			dwTimeStamp += VO_U32(m_pwFrameSizes[i]) * 1000 / m_dwBitrate;
		}
	}
	else if(m_wInterleaveFactor>1)	//CBR
	{
		if(!AddPayloadInfo(m_pFileChunk, btFlags, wLen, dwTimeStamp))
			return VO_FALSE;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

/*
Data Structure:
ra_block
block data

Note:
for avoid data misalignment, block data must keep multiple of four!!
*/
VO_BOOL CRealAudioTrack::AddPayloadInfo(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp)
{
	if(!m_bInUsed)
		return pFileChunk->FSkip(wLen);

	if(1 == m_wInterleaveFactor)	//VBR && no interleave
	{
		VO_U32 dwSampleSize = sizeof(ra_block) + ((wLen + 0x3) & ~0x3);
		if(!m_pGlobeBuffer->HasIdleBuffer(dwSampleSize))	//buffer lack
			return VO_FALSE;

		VO_PBYTE pTempBuf = VO_NULL;
		VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pTempBuf, dwSampleSize);

		m_TmpBlock.ulDataLen = wLen;
		m_pGlobeBuffer->DirectWrite2((VO_PBYTE)&m_TmpBlock, pTempBuf, 0, sizeof(ra_block));

		if(!m_pGlobeBuffer->DirectWrite(pFileChunk, pTempBuf, sizeof(ra_block), wLen))
			return VO_FALSE;

		PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry(dwTimeStamp, dwPosInBuffer, dwSampleSize);
		if(!pNew)
			return VO_FALSE;

		m_pStreamFileIndex->Add(pNew);
	}
	else if(m_wInterleaveFactor>1)
	{
		if(btFlags & RM_KEYFRAME_FLAG)	//super block!!
		{
			//only do actual add when error data occur
			//generally do actual add when last block finish
			if(AddTmpEntriesToIndex())
				m_wCurrTmpEntry = 0;

			VO_U32 dwSampleSize = sizeof(ra_block) + ((m_wInterleaveBlockSize + 0x3) & ~0x3);
			if(!m_pGlobeBuffer->HasIdleBuffer(m_wInterleaveFactor * dwSampleSize))
				return VO_FALSE;

			for(VO_U32 i = 0; i < m_wInterleaveFactor; i++)
			{
				m_ppTempEntries[i] = m_pStreamFileIndex->NewEntry(dwTimeStamp + i * m_wInterleaveBlockSize * 1000 / m_dwBitrate, 
					m_pGlobeBuffer->GetForDirectWrite(&m_ppTempBuf[i], dwSampleSize), m_wInterleaveBlockSize);

				if(!m_ppTempEntries[i])
					return VO_FALSE;

				m_pGlobeBuffer->DirectWrite2((VO_PBYTE)&m_TmpBlock, m_ppTempBuf[i], 0, sizeof(ra_block));
			}
			//mark super block flag!!
			m_ppTempEntries[0]->pos_in_buffer |= 0x80000000;
		}
		else
		{
			//we should process first frame
			//make sure interleave mode, first frame must be key frame!!
			if(!m_ppTempEntries[0])
				return pFileChunk->FSkip(wLen);

			if(m_ppTempEntries[0] && m_ppTempEntries[0]->time_stamp != dwTimeStamp)
				m_ppTempEntries[m_wCurrTmpEntry]->time_stamp = dwTimeStamp;
		}

		VO_U16 wFrameIndex = m_wCurrTmpEntry * m_wInterleaveFramesPerBlock;	//frame index
		for(VO_U16 i = 0; i < m_wInterleaveFramesPerBlock; i++)
		{
			VO_U16 wDstFrameIndex = m_pwInterleavePattern[wFrameIndex + i];
			VO_U16 wDstBlockIndex = wDstFrameIndex / m_wInterleaveFramesPerBlock;
			wDstFrameIndex = wDstFrameIndex % m_wInterleaveFramesPerBlock;

			if(!m_pGlobeBuffer->DirectWrite(pFileChunk, m_ppTempBuf[wDstBlockIndex], sizeof(ra_block) + wDstFrameIndex * m_wInterleaveFrameSize, m_wInterleaveFrameSize))
				return VO_FALSE;
		}

		m_wCurrTmpEntry++;
		if(m_wCurrTmpEntry == m_wInterleaveFactor)
		{
			AddTmpEntriesToIndex();
			m_wCurrTmpEntry = 0;
		}
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CRealAudioTrack::AddTmpEntriesToIndex()
{
	if(m_ppTempEntries[0])
	{
		for(VO_U16 i = 0; i < m_wInterleaveFactor; i++)
		{
			m_pStreamFileIndex->Add(m_ppTempEntries[i]);
			m_ppTempEntries[i] = VO_NULL;
		}

		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL CRealAudioTrack::SetGlobalBufferExtSize(VO_U32 dwExtSize)
{
	if(1 == m_wInterleaveFactor)	//VBR && no interleave
		m_dwGBExtSize = sizeof(ra_block) * RA_VBR_MAXFRAMESINPACKET + ((dwExtSize + 0x3) & ~0x3);
	else if( m_wInterleaveFactor > 1 )
		m_dwGBExtSize = VO_U32(((m_wInterleaveBlockSize + 0x3) & ~0x3) + sizeof(ra_block)) * m_wInterleaveFactor;
	else
		m_dwGBExtSize = 0;

	return VO_TRUE;
}