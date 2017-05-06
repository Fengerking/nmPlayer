#include "fCC.h"
#include "CMpegReader.h"
#include "CMpegVideoTrack.h"
#include "fVideoHeadDataInfo.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DEFINE_USE_MPEG_GLOBAL_VARIABLE(CMpegVideoTrack)
CMpegVideoTrack::CMpegVideoTrack(VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader *pReader, VO_MEM_OPERATOR* pMemOp, VO_U8 btSubStreamID)
	: CMpegTrack(VOTT_VIDEO, btStreamID ,dwTimeOffset, dwDuration, pReader, pMemOp, btSubStreamID)
	, m_pSeqHeader(NULL)
	, m_wSeqHeaderSize(0)
	, m_wWidth(0)
	, m_wHeight(0)
	, m_wFrameRate(0)
	, m_bMPEG2(VO_FALSE)
	, m_btLeftBytes(0)
	, m_pTempEntry(VO_NULL)
	, m_uThumbnailCnt(0)
{
}

CMpegVideoTrack::~CMpegVideoTrack()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	SAFE_MEM_FREE(m_pSeqHeader);
}

VO_VOID CMpegVideoTrack::Flush()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}
	m_btLeftBytes = 0;

	CMpegTrack::Flush();
}

VO_U32 CMpegVideoTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = m_wSeqHeaderSize;
	pHeadData->Buffer = m_pSeqHeader;

	
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegVideoTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = VO_U32(m_wWidth) * m_wHeight * 2;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegVideoTrack::GetCodecCC(VO_U32* pCC)
{
	*pCC = m_bMPEG2 ? FOURCC_MPG2 : FOURCC_MPG1;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	pVideoFormat->Width = m_wWidth;
	pVideoFormat->Height = m_wHeight;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(pdwFrameTime)
		*pdwFrameTime = m_wFrameRate;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMpegVideoTrack::OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt)
{
	if (m_bSetExtData && !m_bInUsed)
	{
		return VO_TRUE;
	}
	if(MPEG_MAX_VALUE != m_ullCurrParseFilePos)
	{
		if(pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
		{
			pFileChunk->FSkip(wLen);
			return VO_TRUE;
		}
		else
			m_ullCurrParseFilePos = -1;
	}
	VO_PBYTE pPESBuffer = pFileChunk->FGetFileContentPtr(wLen);
	if(!pPESBuffer)
	{
		if(!CheckPESBuffer())
			return VO_FALSE;

		if(!pFileChunk->FRead(m_pActPESBuffer, wLen))
			return VO_FALSE;

		pPESBuffer = m_pActPESBuffer;
	}
	if (beEncrpt)
		m_pReader->DRMData_Widevine_PESData(pPESBuffer,wLen,NULL,NULL);
	CodeInfo iStartCode;
	VO_U16 i = 0;
	//0-no, 1-picture, 2-sequence
	CODE_TYPE ct = UNKNOWN_CODE;
	if(m_btLeftBytes == 1)		//0x00
	{
		if(0x00 == pPESBuffer[0] && 0x01 == pPESBuffer[1])
		{
			i = 3;
			ct = MPEG_CODES[pPESBuffer[2]];
		}
	}
	else if(m_btLeftBytes == 2)	//0x00, 0x00
	{
		if(0x01 == pPESBuffer[0])
		{
			i = 2;
			ct = MPEG_CODES[pPESBuffer[1]];
		}
	}
	else if(m_btLeftBytes == 3)	//0x00, 0x00, 0x01
	{
		i = 1;
		ct = MPEG_CODES[pPESBuffer[0]];
	}

	if(PICTURE_START_CODE == ct)
	{
		if(m_pTempEntry)
		{
			if (m_pTempEntry->size > m_btLeftBytes)
			{
				m_pTempEntry->Cut(m_btLeftBytes);
			}			
		}
		if(!ProcessPrevStartCode(pPESBuffer, iStartCode, 0))
			return VO_FALSE;

		if(!m_bSetExtData && SEQUENCE_HEADER_CODE == iStartCode.type)
		{
			if(SetExtData(pPESBuffer + iStartCode.pos, i - iStartCode.pos))
			{
				Init(0);
				SetInUsed(VO_TRUE);
				Prepare();

				m_bSetExtData = VO_TRUE;
			}
		}
		if (!m_bSetExtData)
		{
			return VO_TRUE;
		}

		m_pTempEntry = m_pStreamFileIndex->NewEntry(CalcTrackTs(dwTimeStamp), -1, 0);
		if(!m_pTempEntry)
			return VO_FALSE;

		iStartCode.type = PICTURE_START_CODE;
		iStartCode.pos = i;
		i += 4;
	}
	else if(SEQUENCE_HEADER_CODE == ct)
	{
		if(m_pTempEntry)
			m_pTempEntry->Cut(m_btLeftBytes);
		if(!ProcessPrevStartCode(pPESBuffer, iStartCode, 0))
			return VO_FALSE;
		i += 8;
	}

	m_btLeftBytes = 0;
	VO_U16 wPESHeaderPos = VO_MAXU16;
	while(i < wLen - 3)
	{
		wPESHeaderPos = FindPESHeaderInBuffer(pPESBuffer + i, wLen - i);
		if(VO_MAXU16 == wPESHeaderPos)
			break;

		i += wPESHeaderPos;
		if((VO_U32)(i + 3) == wLen)
		{
			m_btLeftBytes = 3;
			break;
		}

		if(0x00 == pPESBuffer[i + 3])
		{
			if(!ProcessPrevStartCode(pPESBuffer, iStartCode, i))
				return VO_FALSE;

			if(!m_bSetExtData && SEQUENCE_HEADER_CODE == iStartCode.type)
			{
				if(SetExtData(pPESBuffer + iStartCode.pos, i - iStartCode.pos))
				{
					Init(0);
					SetInUsed(VO_TRUE);
					Prepare();

					m_bSetExtData = VO_TRUE;
				}
			}
			if (!m_bSetExtData)
			{
				return VO_TRUE;
			}
			m_pTempEntry = m_pStreamFileIndex->NewEntry(CalcTrackTs(dwTimeStamp), -1, 0);
			if(!m_pTempEntry)
				return VO_FALSE;

			iStartCode.type = PICTURE_START_CODE;
			iStartCode.pos = i + 4;
			i += 8;
		}
		else if(0xb3 == pPESBuffer[i + 3])
		{
			if(!ProcessPrevStartCode(pPESBuffer, iStartCode, i))
				return VO_FALSE;

			iStartCode.type = SEQUENCE_HEADER_CODE;
			iStartCode.pos = i;
			i += 12;
		}
		else if(0xb8 == pPESBuffer[i + 3])
			i += 8;
		else
			i += 4;
	}

	if((PICTURE_START_CODE == iStartCode.type || UNKNOWN_CODE == iStartCode.type) && m_pTempEntry)
		AddPayload(pPESBuffer + iStartCode.pos, wLen - iStartCode.pos);

	//0x00 0x00 0x01	m_bLeftBytes = 3;
	//0xXX 0x00 0x00	m_bLeftBytes = 2;
	//0xXX 0xXX 0x00	m_bLeftBytes = 1;
	//else				m_bLeftBytes = 0;
	if(!m_btLeftBytes)
	{
		VO_PBYTE pLeftBytes = pPESBuffer + wLen - 3;
		if(0x00 == pLeftBytes[2])
		{
			if(0x00 == pLeftBytes[1])
				m_btLeftBytes = 2;
			else
				m_btLeftBytes = 1;
		}
		else if(0x01 == pLeftBytes[2])
		{
			if(0x00 == pLeftBytes[1] && 0x00 == pLeftBytes[0])
				m_btLeftBytes = 3;
		}
	}

	return VO_TRUE;
}

VO_BOOL CMpegVideoTrack::SetExtData(VO_PBYTE pData, VO_U16 wSize)
{
	VO_PBYTE pOutBuf = NULL;
	VO_U32 nOutBufSize = 0;
	VO_U32 ret = GetMpeg2SequenceHead(pData,wSize,&pOutBuf,&nOutBufSize);
	if (ret == VO_ERR_NONE)
	{
		if(m_pSeqHeader)
			MemFree(m_pSeqHeader);
		m_wSeqHeaderSize = nOutBufSize;
		m_pSeqHeader = (VO_PBYTE)MemAlloc(m_wSeqHeaderSize);
		if(!m_pSeqHeader || m_wSeqHeaderSize < 11)
		{
			VOLOGE("MPEG ERROR:Video No Sequence Head");
			return VO_FALSE;
		}
		MemCopy(m_pSeqHeader, pOutBuf, m_wSeqHeaderSize);
		VO_PBYTE pHead = m_pSeqHeader;
		
		m_wWidth = VO_U16(pHead[4]) << 4 | pHead[5] >> 4;
		m_wHeight = VO_U16(pHead[5] & 0xF) << 8 | pHead[6];

		m_wFrameRate = s_wFrameRate[pHead[7] & 0xF];
		if(0xFF == pHead[8] && 0xFF == pHead[9])	//variable bit-rate
			m_dwBitrate = m_pReader->GetBitrate();	//we get the value of m_dwBitrate equal to file bitrate
		else	//const bit-rate
			m_dwBitrate = ((VO_U32(pHead[8]) << 10) | (VO_U32(pHead[9]) << 2) | ((pHead[10] & 0xc0) >> 6)) * 50;

		m_bMPEG2 = VO_FALSE;
		for(VO_U16 i = 12; i < wSize; i++)
		{
			if(!memcmp(pHead + i, &MPEG_VIDEO_EXTENSION_START_HEADER, 4))
				m_bMPEG2 = VO_TRUE;
		}
		ReleaseMpeg2SequenceHead(pOutBuf);
		return VO_TRUE;
	}
	ReleaseMpeg2SequenceHead(pOutBuf);
	return VO_FALSE;
}

VO_BOOL CMpegVideoTrack::ProcessPrevStartCode(VO_PBYTE pPESBuffer, CodeInfo iStartCode, VO_U16 wCurrPos)
{
	if(!m_pTempEntry || SEQUENCE_HEADER_CODE == iStartCode.type)
		return VO_TRUE;

	if(wCurrPos > 0)
	{
		if(UNKNOWN_CODE == iStartCode.type)
			AddPayload(pPESBuffer, wCurrPos);
		else if(PICTURE_START_CODE == iStartCode.type)
			AddPayload(pPESBuffer + iStartCode.pos, wCurrPos - iStartCode.pos);
	}

	if (m_pTempEntry->size > 0)
	{
		VO_PBYTE pSampleBuf = VO_NULL;
		if(2 == m_pGlobeBuffer->Peek(&pSampleBuf, m_pSampleData, m_pTempEntry->pos_in_buffer, m_pTempEntry->size))
			pSampleBuf = m_pSampleData;

		(pSampleBuf[5] >> 3 & 0x7)==1?m_pTempEntry->SetKeyFrame(VO_TRUE):m_pTempEntry->SetKeyFrame(VO_FALSE);
	}
	CMpegDataParser *pDataParser = (CMpegDataParser *)m_pStreamFileReader->GetFileDataParserPtr();
	if (pDataParser && pDataParser->GetParserMode() == VO_MPEG_PARSER_MODE_THUMBNAIL)
	{
		if (!m_pTempEntry->IsKeyFrame())
		{
			m_pGlobeBuffer->RemoveFrom(m_pTempEntry->pos_in_buffer);
			m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		}
		else
		{
			PushTmpSampleToStreamFile(m_pTempEntry);
		}
	}
	else
	{
		if (CalculateTimeStamp(m_pTempEntry))
		{
			PushTmpSampleToStreamFile(m_pTempEntry);
		}
	}
	
	//m_pStreamFileIndex->Add(m_pTempEntry);
	m_pTempEntry = NULL;
	
	return VO_TRUE;
}

VO_BOOL CMpegVideoTrack::AddPayload(VO_PBYTE pData, VO_U16 wSize)
{
	if(!m_pGlobeBuffer->HasIdleBuffer(wSize))
		return VO_FALSE;

	VO_U32 CntPicHeadSyncWord = 4;

	VO_PBYTE pWrite = NULL;
	if (!m_pTempEntry->size)
	{
		wSize += CntPicHeadSyncWord;
	}
	VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pWrite, wSize);
	if(VO_MAXU32 == dwPosInBuffer)
		return VO_FALSE;
	
	if (!m_pTempEntry->size)
	{
		m_pGlobeBuffer->DirectWrite2((VO_PBYTE)&MPEG_VIDEO_PICTURE_START_HEADER, pWrite, 0, CntPicHeadSyncWord);
		m_pGlobeBuffer->DirectWrite2(pData, pWrite, CntPicHeadSyncWord, wSize - CntPicHeadSyncWord);
	}
	else
	{
		m_pGlobeBuffer->DirectWrite2(pData, pWrite, 0, wSize);
	}

	if(m_pTempEntry->pos_in_buffer == 0xFFFFFFFF)
		m_pTempEntry->pos_in_buffer = dwPosInBuffer;

	m_pTempEntry->Add(wSize);
	return VO_TRUE;
}
VO_U32 CMpegVideoTrack::FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_U32 dwSize)
{
	if(dwSize < 3)
		return -1;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwSize - 2;
	while(pHead < pTail)
	{
		if(pHead[0])
		{
			pHead += 2;
			continue;
		}

		if(pHead == pBuffer || *(pHead - 1))	//previous byte is not 0x00
		{
			//0x00 found
			if(pHead[1])
			{
				pHead += 2;
				continue;
			}

			//0x0000 found
			pHead++;	//point to second 0x00
		}

		while(!pHead[1] && pHead < pTail)
			pHead++;

		if(1 != pHead[1])
		{
			pHead += 2;
			continue;
		}

		return (pHead - pBuffer - 1);
	}

	if(pHead > pTail)
		return -1;

	if(*(pHead - 1))
		return -1;

	if(pHead[0])
		return -1;

	if(1 != pHead[1])
		return -1;

	return (pHead - pBuffer - 1);
}

VO_U32 CMpegVideoTrack::CheckHeadDataBuf(VO_PBYTE pBuf,VO_U32 nSize)
{
	if(!m_bSetExtData && SetExtData(pBuf,nSize))
	{
		Init(0);
		SetInUsed(VO_TRUE);
		Prepare();

		m_bSetExtData = VO_TRUE;
	}
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegVideoTrack::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	if (pThumbNailInfo == NULL || m_pStreamFileIndex == NULL)
	{
		return VO_ERR_SOURCE_END;
	}
	if(pThumbNailInfo->uFlag == VO_SOURCE_THUMBNAILMODE_INFOONLY)
	{
		m_uThumbnailCnt++;
		VO_S64 ullTime = VO_MAXS64; 
		CMpegDataParser* pDataParser = (CMpegDataParser*)m_pStreamFileReader->GetFileDataParserPtr();
		if (pDataParser == NULL)
		{
			return VO_ERR_SOURCE_END;
		}
		while (1)
		{
			if(!IsCannotGenerateIndex(pDataParser))
			{
				if(!pDataParser->Step())
					pDataParser->SetParseEnd(VO_TRUE);

				
				if (m_pStreamFileIndex->GetCurrEntryCount()>= m_uThumbnailCnt)
				{
					PBaseStreamMediaSampleIndexEntry pEntry;
					m_pStreamFileIndex->GetTail(&pEntry);
					if (pEntry)
					{
						if (!pEntry->IsKeyFrame())
						{
							VOLOGE("pEntry is not Keyframe:%d",m_uThumbnailCnt);
							return VO_ERR_SOURCE_END;
						}
						pThumbNailInfo->ullFilePos = pDataParser->GetCurrParseFilePos();
						pThumbNailInfo->ullTimeStamp = pEntry->time_stamp;
						pThumbNailInfo->uSampleSize = pEntry->size;
						pThumbNailInfo->uPrivateData = m_uThumbnailCnt;
						return VO_ERR_SOURCE_OK;
					}
				}
			}
			else
			{
				VOLOGE("can not generate more key frame:%d",m_uThumbnailCnt);
				return VO_ERR_SOURCE_END;
			}
		}
	}
	else if(pThumbNailInfo->uFlag == VO_SOURCE_THUMBNAILMODE_DATA)
	{
		PBaseStreamMediaSampleIndexEntry pEntry;
		CMpegFileIndex* pIndex = (CMpegFileIndex*)m_pStreamFileIndex;
		if (VO_ERR_SOURCE_OK == pIndex->GetEntryByCount(pThumbNailInfo->uPrivateData - 1,&pEntry))
		{
			if (pEntry)
			{
				if(2 == m_pGlobeBuffer->Get(&pThumbNailInfo->pSampleData, m_pSampleData, pEntry->pos_in_buffer & 0x7FFFFFFF, pEntry->size))
					pThumbNailInfo->pSampleData = m_pSampleData;

				return VO_ERR_SOURCE_OK;
			}
			else
			{
				VOLOGE("GpEntry is null:uPrivateData==%d",pThumbNailInfo->uPrivateData);
			}
		}
		else
		{
			VOLOGE("GetEntryByCount error:uPrivateData==%d",pThumbNailInfo->uPrivateData);
		}
	}
	return VO_ERR_SOURCE_END;
}