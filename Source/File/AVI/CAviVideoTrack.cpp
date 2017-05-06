/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		CAviTrack.h

Contains:	The wrapper for avi video track  

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#include "CAviReader.h"
#include "CAviVideoTrack.h"
#include "fCC.h"
#include "voLog.h"
#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define AVI_VIDEO_INDEX_ENTRY_COUNT		60

//////////////////////////////////////////////////////////////////////////
//MPEG4 ID
const VO_U32 SEQ_HEADER_ID	= 0XB0010000;
const VO_U32 GOP_HEADER_ID	= 0XB3010000;
const VO_U32 VOP_HEADER_ID	= 0XB6010000;
const VO_U32 VOL_HEADER_ID	= 0X20010000;
const VO_U32 I_FRAME_ID		= 0XC0;
//////////////////////////////////////////////////////////////////////////
//DIVX3 ID, first two bits
#define DIVX_I_FRAME_FLAG	0
#define DIVX_P_FRAME_FLAG	1
#define DIVX_B_FRAME_FLAG	2
////////////////////////////////////////////////////////////////////////

CAviVideoTrack::CAviVideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CAviTrack(VOTT_VIDEO, btStreamNum, dwDuration, pReader, pMemOp)
	, m_ullAvgTimePerFrame(400000)
	, m_bNoIndex(VO_FALSE)
	, m_pList(VO_NULL)
	, m_pHead(VO_NULL)
	, m_pTail(VO_NULL)
	, m_dwCount(0)
	, m_pFrameHeader(VO_NULL)
	, m_dwCodec(VO_VIDEO_CodingUnused)
	, m_bHasDdChunk(VO_FALSE)
	, m_pTempEntry(VO_NULL)
	, m_dwExtData(0)
	, m_pExtData(VO_NULL)
{
}

CAviVideoTrack::~CAviVideoTrack()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	SAFE_MEM_FREE(m_pExtData);
}

VO_U32 CAviVideoTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	VO_BOOL bDdFrame = VO_FALSE;
	if(m_pIndex)
	{
		if(!IndexBuffering_Fill())
			return VO_ERR_SOURCE_END;

		//process notify!!
		VO_U32 dwOptSkip = static_cast<VO_U32>(VO_U64(pSample->Time) * 10000 / m_ullAvgTimePerFrame);
		VO_U32 dwActSkip = 0;
		PAviIndexBufferingEntry pGet = m_pHead;
		if(dwOptSkip > m_dwCurrIndex)
		{
			dwOptSkip -= m_dwCurrIndex;
			if(dwOptSkip > m_dwCount)
				dwOptSkip = m_dwCount;

			PAviIndexBufferingEntry pTmp = m_pHead;
			for(VO_U32 i = 0; i < dwOptSkip; i++)
			{
				if(IS_KEY_FRAME(pTmp->size))
				{
					dwActSkip = i;
					pGet = pTmp;
				}
				pTmp = pTmp->next;
			}
		}

		if(dwActSkip > 0)
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;

		VO_S32 nRes = 0;
		if(IS_DD_FRAME(pGet->size))
			bDdFrame = VO_TRUE;
		if(bDdFrame)
		{
			nRes = ReadFileContent(&pSample->Buffer, pGet->dd_pos_in_file, pGet->dd_size);

			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;

			MemCopy(m_ddInfo, pSample->Buffer, sizeof(m_ddInfo));
		}

		pSample->Size = GET_SIZE(pGet->size);
		pSample->Time = (m_dwCurrIndex + dwActSkip) * m_ullAvgTimePerFrame / 10000;

		if(pSample->Size > 0)
		{
			if (pSample->Size > m_dwMaxSampleSize)
			{
				pSample->Size = 0;
			} 
			else
			{
				nRes = ReadFileContent(&pSample->Buffer, pGet->pos_in_file, pSample->Size);

				if(0 == nRes)
					return VO_ERR_SOURCE_END;
				else if(2 == nRes)
					return VO_ERR_SOURCE_NEEDRETRY;
			}
		}

		if(IS_KEY_FRAME(pGet->size))
			pSample->Size |= 0x80000000;

		pSample->Duration = 1;

		IndexBuffering_Remove(dwActSkip + 1);
		m_dwCurrIndex += (dwActSkip + 1);
	}
	else
	{
		VO_U32 rc = CAviTrack::GetSampleN(pSample);
		if(VO_ERR_SOURCE_OK != rc)
			return rc;

		if(pSample->Size > 0 && 0xFF == *pSample->Buffer)
			bDdFrame = VO_TRUE;
		if(bDdFrame)
		{
			//encrypt data
			MemCopy(m_ddInfo, pSample->Buffer + 1, sizeof(m_ddInfo));

			pSample->Buffer += (VO_DIVXDRM_DD_INFO_LENGTH + 1);
			pSample->Size = ((pSample->Size & VO_MAXS32) - (VO_DIVXDRM_DD_INFO_LENGTH + 1)) | (pSample->Size & 0x80000000);
		}
	}

	if(m_pReader->IsDivXDRM() && bDdFrame)
	{
		VO_PBYTE	pReturn = VO_NULL;
		VO_U32		iRetLen = 0;

		if(m_pReader->DRMData(VO_DRMDATATYPE_VIDEOSAMPLE, pSample->Buffer, pSample->Size & VO_MAXS32, &pReturn, &iRetLen, m_ddInfo) != VO_ERR_DRM_OK)
			return VO_ERR_SOURCE_ERRORDATA;

		if (iRetLen)
		{
			pSample->Size = (pSample->Size & 0x80000000) + iRetLen;
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_DivXDRM;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::SetPosN(VO_S64* pPos)
{
	if(!m_pIndex)
		return VO_ERR_SOURCE_SEEKFAIL;

	m_dwCurrIndex = 0;

	VO_U32 dwOptIndex = static_cast<VO_U32>(double(*pPos) * 10000 / m_ullAvgTimePerFrame + 0.5);
	if ( dwOptIndex > m_pIndex->GetCount() )
		dwOptIndex = m_pIndex->GetCount();

	VO_U32 dwSize = 0;
	for(VO_U32 i = 0; i <= dwOptIndex; i++)
	{
		if(!m_pIndex->GetEntryByIndex(i, VO_NULL, &dwSize))
			return VO_ERR_SOURCE_END;

		if(IS_KEY_FRAME(dwSize))
			m_dwCurrIndex = i;
	}

	m_dwCount = 0;
	m_pHead = m_pTail = m_pList;
	m_bNoIndex = VO_FALSE;

	*pPos = m_dwCurrIndex * m_ullAvgTimePerFrame / 10000;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetSampleK(VO_SOURCE_SAMPLE* pSample)
{
	if(!m_pIndex)
		return VO_ERR_SOURCE_END;

	VO_U64 ullPosInFile = 0, ullDdPosInFile = 0;
	VO_U32 dwSize = 0x80000000, dwDdSize = 0;
	while(!IS_KEY_FRAME(dwSize))
	{
		if(!m_pIndex->GetEntryByIndex(m_dwCurrIndex, &ullPosInFile, &dwSize, &ullDdPosInFile, &dwDdSize))
			return VO_ERR_SOURCE_END;

		if(VO_SOURCE_PM_FF == m_PlayMode)
			m_dwCurrIndex++;
		else
			m_dwCurrIndex--;
	}

	VO_S32 nRes = 0;
	VO_BOOL bDdFrame = IS_DD_FRAME(dwSize) ? VO_TRUE : VO_FALSE;
	if(bDdFrame)
	{
		nRes = m_pReader->ReadBuffer(m_pSampleData, ullDdPosInFile, dwDdSize);
		if(0 == nRes)
			return VO_ERR_SOURCE_END;
		else if(2 == nRes)
			return VO_ERR_SOURCE_NEEDRETRY;

		MemCopy(m_ddInfo, m_pSampleData, sizeof(m_ddInfo));
	}

	pSample->Size = GET_SIZE(dwSize);
	pSample->Time = m_dwCurrIndex * m_ullAvgTimePerFrame / 10000;

	if(pSample->Size > 0)
	{
		nRes = m_pReader->ReadBuffer(m_pSampleData, ullPosInFile, pSample->Size);
		if(0 == nRes)
			return VO_ERR_SOURCE_END;
		else if(2 == nRes)
			return VO_ERR_SOURCE_NEEDRETRY;

		pSample->Buffer = m_pSampleData;
	}

	pSample->Size |= 0x80000000;
	pSample->Duration = 1;

	if(VO_SOURCE_PM_FF == m_PlayMode)
		m_dwCurrIndex++;
	else
		m_dwCurrIndex--;

	if(m_pReader->IsDivXDRM() && bDdFrame)
	{
		VO_PBYTE	pReturn = VO_NULL;
		VO_U32		iRetLen = 0;

		if(m_pReader->DRMData(VO_DRMDATATYPE_VIDEOSAMPLE, pSample->Buffer, pSample->Size & VO_MAXS32, &pReturn, &iRetLen, m_ddInfo) != VO_ERR_DRM_OK)
			return VO_ERR_SOURCE_ERRORDATA;

		if (iRetLen)
		{
			pSample->Size = (pSample->Size & 0x80000000) + iRetLen;
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_DivXDRM;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::SetPosK(VO_S64* pPos)
{
	return SetPosN(pPos);
}

VO_U32 CAviVideoTrack::Prepare()
{
	if(m_pIndex)
	{
		m_pList = (PAviIndexBufferingEntry)MemAlloc(AVI_VIDEO_INDEX_ENTRY_COUNT * sizeof(AviIndexBufferingEntry));
		if(!m_pList)
			return VO_ERR_OUTOF_MEMORY;

		for(VO_U32 i = 0; i < AVI_VIDEO_INDEX_ENTRY_COUNT; i++)
			m_pList[i].next = m_pList + i + 1;

		m_pHead = m_pTail = m_pList[AVI_VIDEO_INDEX_ENTRY_COUNT - 1].next = m_pList;
		m_dwCount = 0;
	}
	else
	{
		if(VO_VIDEO_CodingMPEG4 == m_dwCodec)
		{
			m_pFrameHeader = NEW_BUFFER(8);
			if(!m_pFrameHeader)
				return VO_ERR_OUTOF_MEMORY;
		}
	}

	return CAviTrack::Prepare();
}

VO_U32 CAviVideoTrack::Unprepare()
{
	CAviTrack::Unprepare();

	SAFE_MEM_FREE(m_pFrameHeader);
	m_dwCodec = VO_VIDEO_CodingUnused;

	SAFE_MEM_FREE(m_pList);
	m_pHead = m_pTail = VO_NULL;
	m_dwCount = 0;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CAviVideoTrack::IndexBuffering_Fill()
{
	if(!m_bNoIndex)
	{
		while(m_dwCount < AVI_VIDEO_INDEX_ENTRY_COUNT)
		{
			if(!m_pIndex->GetEntryByIndex(m_dwCurrIndex + m_dwCount, &m_pTail->pos_in_file, &m_pTail->size, &m_pTail->dd_pos_in_file, &m_pTail->dd_size))
			{
				m_bNoIndex = VO_TRUE;
				break;
			}

			m_pTail = m_pTail->next;
			m_dwCount++;
		}
	}

	return (m_dwCount > 0) ? VO_TRUE : VO_FALSE;
}

VO_VOID CAviVideoTrack::IndexBuffering_Remove(VO_U32 dwRemoveCount)
{
	for(VO_U32 i = 0; i < dwRemoveCount; i++)
	{
		m_pHead = m_pHead->next;
		m_dwCount--;
	}
}

VO_U32 CAviVideoTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	VO_BITMAPINFOHEADER* pbih = (VO_BITMAPINFOHEADER*)m_pPropBuffer;
	pHeadData->Length = pbih->biSize - sizeof(VO_BITMAPINFOHEADER);
	pHeadData->Buffer = m_pPropBuffer + sizeof(VO_BITMAPINFOHEADER);

	if(pHeadData->Length == 0)
	{
		if(m_pExtData == VO_NULL)
		{
			VO_SOURCE_SAMPLE sample;
			MemSet(&sample, 0, sizeof(VO_SOURCE_SAMPLE));
			if(VO_ERR_SOURCE_OK == GetFirstFrame(&sample))
			{
				VO_U32 dwCodec = 0;
				GetCodec(&dwCodec);

				VO_U32 dwSeqHeadPos = VO_MAXU32;
				VO_U32 dwSeqHeadSize = 0;

				if(VO_TRUE == GetVideoHeadDataFromBuffer(dwCodec, sample.Buffer, sample.Size & VO_MAXS32, &dwSeqHeadPos, &dwSeqHeadSize))
				{
					if(dwSeqHeadPos != VO_MAXU32)
					{
						m_dwExtData = dwSeqHeadSize;
						m_pExtData = NEW_BUFFER(m_dwExtData);
						if(m_pExtData)
							MemCopy(m_pExtData, sample.Buffer + dwSeqHeadPos, m_dwExtData);
					}
				}
			}
		}

		if(m_pExtData && m_dwExtData > 0)
		{
			pHeadData->Length = m_dwExtData;
			pHeadData->Buffer = m_pExtData;
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
	{
		if(m_dwMaxSampleSize)
		{
			VO_BITMAPINFOHEADER* pbih = (VO_BITMAPINFOHEADER*)m_pPropBuffer;
			*pdwMaxSampleSize = pbih->biWidth * pbih->biHeight * 3 / 2;

			VO_U32 dwCodec = 0;
			if (0 == GetCodec(&dwCodec) && VO_VIDEO_CodingUnused == dwCodec)
			{
				if(m_dwMaxSampleSize > *pdwMaxSampleSize)
					*pdwMaxSampleSize = m_dwMaxSampleSize;
			}
			else
			{
				if(m_dwMaxSampleSize < *pdwMaxSampleSize)
					*pdwMaxSampleSize = m_dwMaxSampleSize;
			}

		}
		else
		{
			VO_BITMAPINFOHEADER* pbih = (VO_BITMAPINFOHEADER*)m_pPropBuffer;
			if(pbih->biWidth * pbih->biHeight < 76800)	//320X240
				*pdwMaxSampleSize = pbih->biWidth * pbih->biHeight * 3 / 2;
			else
				*pdwMaxSampleSize = pbih->biWidth * pbih->biHeight * 3 / 4;
		}
	}

	*pdwMaxSampleSize += 256;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetCodecCC(VO_U32* pCC)
{
	*pCC = ((VO_BITMAPINFOHEADER*)m_pPropBuffer)->biCompression;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetFirstFrame(VO_SOURCE_SAMPLE* pSample)
{
	VO_BOOL bDdFrame = VO_FALSE;
	if(m_pIndex)
	{
		VO_U64 ullPosInFile = 0, ullDdPosInFile = 0;
		VO_U32 dwSize = 0, dwDdSize = 0, dwIndex = 0;
		while(GET_SIZE(dwSize) == 0)
		{
			if(!m_pIndex->GetEntryByIndex(dwIndex++, &ullPosInFile, &dwSize, &ullDdPosInFile, &dwDdSize))
				return VO_ERR_SOURCE_END;
		}

		VO_S32 nRes = 0;
		if(IS_DD_FRAME(dwSize))
			bDdFrame = VO_TRUE;
		if(bDdFrame)
		{
			nRes = m_pReader->ReadBuffer(m_pSampleData, ullDdPosInFile, dwDdSize);
			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;

			MemCopy(m_ddInfo, m_pSampleData, sizeof(m_ddInfo));
		}

		pSample->Size = GET_SIZE(dwSize);
		pSample->Time = 0;

		if(pSample->Size > 0)
		{
			nRes = m_pReader->ReadBuffer(m_pSampleData, ullPosInFile, pSample->Size);
			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;

			pSample->Buffer = m_pSampleData;
		}
		pSample->Duration = 1;
	}
	else
	{
		PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
		if(!m_pStreamFileIndex || !m_pStreamFileIndex->GetEntry(VO_FALSE, 0, &pGet, NULL))
			return VO_ERR_SOURCE_END;

		while(pGet->size == 0 && pGet->next)
			pGet = pGet->next;

		if(2 == m_pGlobeBuffer->Peek(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & VO_MAXS32, pGet->size))
			pSample->Buffer = m_pSampleData;

		pSample->Time = 0;
		pSample->Duration = 1;
		pSample->Size = pGet->size;

		if(0xFF == *pSample->Buffer)
			bDdFrame = VO_TRUE;
		if(bDdFrame)	//encrypt data
		{
			//confirm not modify the data of m_pGlobeBuffer!!
			if(pSample->Buffer != m_pSampleData)
			{
				MemCopy(m_pSampleData, pSample->Buffer, pGet->size);
				pSample->Buffer = m_pSampleData;
			}

			MemCopy(m_ddInfo, pSample->Buffer + 1, sizeof(m_ddInfo));

			pSample->Buffer += (VO_DIVXDRM_DD_INFO_LENGTH + 1);
			pSample->Size -= (VO_DIVXDRM_DD_INFO_LENGTH + 1);
		}
	}

	//if(bDdFrame)
	//{
	//	VO_PBYTE	pReturn = VO_NULL;
	//	VO_U32		iRetLen = 0;

	//	if(m_pReader->DRMData(VO_DRMDATATYPE_VIDEOSAMPLE, pSample->Buffer, pSample->Size & VO_MAXS32, &pReturn, &iRetLen, m_ddInfo) != VO_ERR_DRM_OK)
	//		return VO_ERR_SOURCE_ERRORDATA;

	//	if (iRetLen)
	//	{
	//		pSample->Size = (pSample->Size & 0x80000000) + iRetLen;
	//		pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_DivXDRM;
	//	}
	//}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample)
{
	if(!m_pIndex)
		return CAviTrack::GetNextKeyFrame(pSample);

	PAviIndexBufferingEntry pGet = VO_NULL;
	PAviIndexBufferingEntry pTmp = m_pHead;
	VO_U32 i = 0;
	for(i = 0; i < m_dwCount; i++)
	{
		if(IS_KEY_FRAME(pTmp->size))
		{
			pGet = pTmp;
			break;
		}
		pTmp = pTmp->next;
	}

	if(!pGet)
		return VO_ERR_SOURCE_NEEDRETRY;

	pSample->Size = GET_SIZE(pGet->size);
	pSample->Size |= 0x80000000;

	pSample->Time = (m_dwCurrIndex + i) * m_ullAvgTimePerFrame / 10000;
	pSample->Duration = 1;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	if(!m_pIndex)
		return VO_ERR_NOT_IMPLEMENT;

	if(llTime < 0)
		return VO_ERR_INVALID_ARG;

	VO_U32 dwIndex = static_cast<VO_U32>(llTime * 10000 / m_ullAvgTimePerFrame);

	VO_U32 dwPrevKeyframeIndex = m_pIndex->GetEntryPrevKeyFrame(dwIndex);
	*pllPreviousKeyframeTime = (VO_MAXU32 == dwPrevKeyframeIndex) ? VO_MAXS64 : (dwPrevKeyframeIndex * m_ullAvgTimePerFrame / 10000);

	VO_U32 dwNextKeyframeIndex = m_pIndex->GetEntryNextKeyFrame(dwIndex);
	*pllNextKeyframeTime = (VO_MAXU32 == dwNextKeyframeIndex) ? VO_MAXS64 : (dwNextKeyframeIndex * m_ullAvgTimePerFrame / 10000);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	VO_BITMAPINFOHEADER* pbih = (VO_BITMAPINFOHEADER*)m_pPropBuffer;
	pVideoFormat->Width = pbih->biWidth;
	pVideoFormat->Height = pbih->biHeight;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader)
{
	*ppBitmapInfoHeader = (VO_BITMAPINFOHEADER*)m_pPropBuffer;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
	{
		if(0 == m_dwDuration)
			*pdwBitrate = 0;
		else
		{
			VO_U64 ullAudioDataSize = 0;
			VO_U32 i = 0;
			CBaseTrack* pTrack = VO_NULL;
			while(true)
			{
				pTrack = m_pReader->GetTrackByIndex(i);
				if(!pTrack)
					break;
				i++;

				if(VOTT_AUDIO != pTrack->GetType())
					continue;

				VO_U32 dwAudioBitrate = 0;
				pTrack->GetBitrate(&dwAudioBitrate);

				ullAudioDataSize += (VO_U64(pTrack->GetDuration()) * dwAudioBitrate / 1000);
			}

			VO_U64 ullTotalSize = m_pReader->GetMediaDataSize() - ullAudioDataSize;
			*pdwBitrate = static_cast<VO_U32>(1000 * ullTotalSize / m_dwDuration);
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetFrameNum(VO_U32* pdwFrameNum)
{
	if(!m_pIndex)
		return VO_ERR_NOT_IMPLEMENT;

	if(pdwFrameNum)
		*pdwFrameNum = m_pIndex->GetCount();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(pdwFrameTime)
		*pdwFrameTime = static_cast<VO_U32>(m_ullAvgTimePerFrame / 100);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CAviVideoTrack::AddFrame(CGFileChunk* pFileChunk, VO_U32 dwLen)
{
	if(dwLen & 0x80000000)
	{
		//'dd' chunk
		dwLen &= VO_MAXS32;

		if(!m_bInUsed || !m_pGlobeBuffer->HasIdleBuffer(dwLen + 1))
			return pFileChunk->FSkip(dwLen);

		VO_PBYTE pTempBuf;
		VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pTempBuf, dwLen + 1);
		*pTempBuf = 0xFF;

		if(!m_pGlobeBuffer->DirectWrite(pFileChunk, pTempBuf, 1, dwLen))
			return VO_FALSE;

		m_pTempEntry = m_pStreamFileIndex->NewEntry(static_cast<VO_U32>(m_dwLastIndex * m_ullAvgTimePerFrame / 10000), dwPosInBuffer, dwLen + 1);
		if(!m_pTempEntry)
			return VO_FALSE;
	}
	else
	{
		//actual data
		if(!m_bInUsed || !m_pGlobeBuffer->HasIdleBuffer(dwLen))
			return pFileChunk->FSkip(dwLen);

		VO_U32 dwPosInBuffer = m_pGlobeBuffer->Add(pFileChunk, dwLen);
		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		VO_BOOL bKeyFrame = VO_FALSE;
		if(VO_VIDEO_CodingMPEG4 == m_dwCodec)
		{
			VO_PBYTE pFrameHeader = VO_NULL;
			VO_S32 nRes = m_pGlobeBuffer->Peek(&pFrameHeader, m_pFrameHeader, dwPosInBuffer, 8);
			if(1 == nRes)
				bKeyFrame = IsKeyFrame_MPEG4(pFrameHeader);
			else if(2 == nRes)
				bKeyFrame = IsKeyFrame_MPEG4(m_pFrameHeader);
		}
		else if(VO_VIDEO_CodingDIVX == m_dwCodec)
		{
			VO_PBYTE pFrameHeader = VO_NULL;
			m_pGlobeBuffer->Peek(&pFrameHeader, VO_NULL, dwPosInBuffer, 1);
			bKeyFrame = IsKeyFrame_DIVX(pFrameHeader);
		}

		if(m_pTempEntry)
		{
			m_pTempEntry->Add(dwLen);
			if(bKeyFrame)
				m_pTempEntry->SetKeyFrame(VO_TRUE);

			m_pStreamFileIndex->Add(m_pTempEntry);
			m_pTempEntry = VO_NULL;
		}
		else
		{
			PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry(static_cast<VO_U32>(m_dwLastIndex * m_ullAvgTimePerFrame / 10000), 
				(bKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), dwLen);
			if(!pNew)
				return VO_FALSE;

			m_pStreamFileIndex->Add(pNew);
		}
		m_dwLastIndex++;
	}

	return VO_TRUE;
}

VO_BOOL CAviVideoTrack::IsKeyFrame_MPEG4(VO_PBYTE pMpeg4Header)
{
	if(!pMpeg4Header)
		return VO_FALSE;

	if(!MemCompare(pMpeg4Header, (VO_PTR)&SEQ_HEADER_ID, 4) || 
		!MemCompare(pMpeg4Header, (VO_PTR)&GOP_HEADER_ID, 4) || 
		!MemCompare(pMpeg4Header, (VO_PTR)&VOL_HEADER_ID, 4))
		return VO_TRUE;

	if(!MemCompare(pMpeg4Header, (VO_PTR)&VOP_HEADER_ID, 4) && !(pMpeg4Header[4] & I_FRAME_ID))
		return VO_TRUE;

	if(!MemCompare(pMpeg4Header + 4, (VO_PTR)&VOL_HEADER_ID, 4))
		return VO_TRUE;

	return VO_FALSE; 
}

VO_BOOL CAviVideoTrack::IsKeyFrame_DIVX(VO_PBYTE pDivxHeader)
{
	return (DIVX_I_FRAME_FLAG == (*pDivxHeader & 0xC0)) ? VO_TRUE : VO_FALSE;
}

VO_VOID CAviVideoTrack::Flush()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	return CAviTrack::Flush();
}

VO_VOID CAviVideoTrack::OnIdx1(VO_U32 dwCurrIndexNum, PAviOriginalIndexEntry pEntry)
{
	CAviTrack::OnIdx1(dwCurrIndexNum, pEntry);

	if(!m_bHasDdChunk && m_pIndex)
		((CAviIdx1Index*)m_pIndex)->AddCount(dwCurrIndexNum);

	m_bHasDdChunk = (TWOCC_dd == (pEntry->dwChunkId >> 16)) ? VO_TRUE : VO_FALSE;
}
VO_U32 CAviVideoTrack::GetThumbnailSyncSampleInfo(VO_PTR pParam)
{
	VOLOGR("+GetThumbnailSyncSampleInfo");
	if(!m_pIndex)
		return VO_ERR_SOURCE_END;

	VO_SOURCE_THUMBNAILINFO* pThumbNailInfo = (VO_SOURCE_THUMBNAILINFO *)pParam;

	VO_U64 ullPosInFile = 0, ullDdPosInFile = 0;
	VO_U32 dwSize = 0x80000000, dwDdSize = 0;
	while(!IS_KEY_FRAME(dwSize))
	{
		if(!m_pIndex->GetEntryByIndex(m_dwCurrIndex, &ullPosInFile, &dwSize, &ullDdPosInFile, &dwDdSize))
			return VO_ERR_SOURCE_END;
		m_dwCurrIndex++;
	}

	VO_S32 nRes = 0;
	VO_BOOL bDdFrame = IS_DD_FRAME(dwSize) ? VO_TRUE : VO_FALSE;
	if(bDdFrame)
	{
		nRes = m_pReader->ReadBuffer(m_pSampleData, ullDdPosInFile, dwDdSize);
		if(0 == nRes)
			return VO_ERR_SOURCE_END;
		else if(2 == nRes)
			return VO_ERR_SOURCE_NEEDRETRY;

		MemCopy(m_ddInfo, m_pSampleData, sizeof(m_ddInfo));
	}

	pThumbNailInfo->uSampleSize = GET_SIZE(dwSize);
	pThumbNailInfo->ullTimeStamp = m_dwCurrIndex * m_ullAvgTimePerFrame / 10000;
	pThumbNailInfo->ullFilePos = ullPosInFile;
	pThumbNailInfo->uPrivateData = 0;

	if(VO_SOURCE_THUMBNAILMODE_DATA == pThumbNailInfo->uFlag)
	{
		if(pThumbNailInfo->uSampleSize > 0)
		{
			nRes = m_pReader->ReadBuffer(m_pSampleData, ullPosInFile, pThumbNailInfo->uSampleSize);
			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;

			pThumbNailInfo->pSampleData = m_pSampleData;
		}
		if(m_pReader->IsDivXDRM() && bDdFrame)
		{
			VO_PBYTE	pReturn = VO_NULL;
			VO_U32		iRetLen = 0;

			if(m_pReader->DRMData(VO_DRMDATATYPE_VIDEOSAMPLE, pThumbNailInfo->pSampleData, pThumbNailInfo->uSampleSize & VO_MAXS32, &pReturn, &iRetLen, m_ddInfo) != VO_ERR_DRM_OK)
				return VO_ERR_SOURCE_ERRORDATA;

			if (iRetLen)
			{
				pThumbNailInfo->uSampleSize = iRetLen;
				//pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_DivXDRM;
			}
		}
	}

	m_dwCurrIndex++;
 VOLOGR("-GetThumbnailSyncSampleInfo");
	return VO_ERR_SOURCE_OK;
}

VO_U32 	CAviVideoTrack::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	VOLOGR("+GetThumbNail");
	if(VO_SOURCE_THUMBNAILMODE_INFOONLY == pThumbNailInfo->uFlag)
	{
		
		GetThumbnailSyncSampleInfo(pThumbNailInfo);
	}
	else if(VO_SOURCE_THUMBNAILMODE_DATA == pThumbNailInfo->uFlag)
	{
		SetPosN(&pThumbNailInfo->ullTimeStamp);
		GetThumbnailSyncSampleInfo(pThumbNailInfo);
	}
	VOLOGR("-GetThumbNail");
	return VO_ERR_NONE;	
}

VO_U32 CAviVideoTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_VIDEO_UPSIDEDOWN:
		{
			VO_U32 fourcc = 0;
			VO_U32 nRes = GetCodecCC(&fourcc);
			if(VO_ERR_SOURCE_OK != nRes)
				return nRes;

			if (FOURCC_VP60 == fourcc || FOURCC_VP61 == fourcc || FOURCC_VP6F == fourcc || FOURCC_VP62 == fourcc)
				*( (VO_BOOL*)pParam ) = VO_TRUE;
			else
				*( (VO_BOOL*)pParam ) = VO_FALSE;

			return VO_ERR_SOURCE_OK;
		}
		break;

	case VO_PID_SOURCE_GETTHUMBNAIL:
		{
			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;

			VO_SOURCE_THUMBNAILINFO* pThumbNailInfo = (VO_SOURCE_THUMBNAILINFO*)pParam;
			GetThumbNail(pThumbNailInfo);
			return VO_ERR_SOURCE_OK;
		}
		break;

	}

	return CAviTrack::GetParameter(uID, pParam);
}
