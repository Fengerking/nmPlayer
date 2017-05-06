#include "fCC.h"
#include "CMpegReader.h"
#include "CMpegAVCVideoTrack.h"
#include "fVideoHeadDataInfo.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DEFINE_USE_MPEG_GLOBAL_VARIABLE(CMpegAVCVideoTrack)
CMpegAVCVideoTrack::CMpegAVCVideoTrack(VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader *pReader, VO_MEM_OPERATOR* pMemOp, VO_U8 btSubStreamID)
	: CMpegVideoTrack( btStreamID ,dwTimeOffset, dwDuration, pReader, pMemOp, btSubStreamID)
	, m_wCntByteSyncWord(0)
	, m_LeftData(0xFFFFFFFF)
	, m_bDelimiter(0)
{
}

CMpegAVCVideoTrack::~CMpegAVCVideoTrack()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	SAFE_MEM_FREE(m_pSeqHeader);
}

VO_VOID CMpegAVCVideoTrack::Flush()
{
	m_LeftData = 0xFFFFFFFF;

	CMpegVideoTrack::Flush();
}

VO_U32 CMpegAVCVideoTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = m_wSeqHeaderSize;
	pHeadData->Buffer = m_pSeqHeader;

	
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegAVCVideoTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = VO_U32(m_wWidth) * m_wHeight / 2;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegAVCVideoTrack::GetCodecCC(VO_U32* pCC)
{
	*pCC = FOURCC_H264;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegAVCVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	pVideoFormat->Width = m_wWidth;
	pVideoFormat->Height = m_wHeight;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegAVCVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(pdwFrameTime)
		*pdwFrameTime = m_wFrameRate;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMpegAVCVideoTrack::OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt)
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

	VO_PBYTE pEnd = pPESBuffer + wLen;

	VO_PBYTE pStartPos = pPESBuffer;
	VO_PBYTE pHeadDataPos = NULL;
	VO_PBYTE pFindPos = pPESBuffer;
	while(pStartPos < pEnd)
	{
		pFindPos = FindPESHeaderInBuffer(pFindPos, pEnd);
		if(NULL == pFindPos)
		{
			if (m_bDelimiter && m_pTempEntry)///<assemble frame by ACCESS_UNIT_DELIMITER_RBSP 
			{
				AddPayload(pStartPos, pEnd - pStartPos);
			}
			else
			{
				if (!m_bDelimiter)
				{
					if (!m_pTempEntry)
					{
						m_pTempEntry = m_pStreamFileIndex->NewEntry(CalcTrackTs(dwTimeStamp), -1, 0);
						if(!m_pTempEntry)
							return VO_FALSE;
					}
					AssemblePayloadToFrame(pStartPos,pEnd - pStartPos);
				}
				else
				{
					VOLOGE("MPEG Error:Error H264 Data")
				}
			}
			return VO_TRUE;
		}
		if (ACCESS_UNIT_DELIMITER_RBSP == ((*pFindPos)&0x1F))
		{
			m_bDelimiter = 1;
			if (pFindPos - m_wCntByteSyncWord < pStartPos)///<has syncword in last frame
			{
				if(m_pTempEntry)
					m_pTempEntry->Cut(pStartPos - (pFindPos - m_wCntByteSyncWord));
			}
			VO_S32 wCntLastFrame = (pFindPos - m_wCntByteSyncWord < pStartPos)? 0 : (pFindPos - m_wCntByteSyncWord - pStartPos);
			
			if(!ProcessPrevStartCode(pStartPos,wCntLastFrame))
				return VO_FALSE;

			if (m_pStreamFileIndex)
			{
				m_pTempEntry = m_pStreamFileIndex->NewEntry(CalcTrackTs(dwTimeStamp), -1, 0);
				if(!m_pTempEntry)
					return VO_FALSE;
			}
			pStartPos = pFindPos;
		}
		else if (!m_bSetExtData)
		{
			if ((SPS_RBSP == ((*pFindPos)&0x1F) || PPS_RBSP == ((*pFindPos)&0x1F))
				&& !pHeadDataPos)
			{
				pHeadDataPos = pFindPos - m_wCntByteSyncWord;
			}
			else if (pHeadDataPos
				&& SPS_RBSP != ((*pFindPos)&0x1F) && PPS_RBSP != ((*pFindPos)&0x1F))
			{
				if(SetExtData(pHeadDataPos, pFindPos - m_wCntByteSyncWord - pHeadDataPos))
				{
					Init(0);
					SetInUsed(VO_TRUE);
					Prepare();
					m_bSetExtData = VO_TRUE;
					if (!m_pTempEntry)
					{
						m_pTempEntry = m_pStreamFileIndex->NewEntry(CalcTrackTs(dwTimeStamp), -1, 0);
						if(!m_pTempEntry)
							return VO_FALSE;
					}
				}
			}
		}
	}

	return VO_TRUE;
}

VO_BOOL CMpegAVCVideoTrack::SetExtData(VO_PBYTE pData, VO_U16 wSize)
{
//	if(wSize <= 12)
//		return VO_FALSE;

	if(m_pSeqHeader)
		MemFree(m_pSeqHeader);
	m_wSeqHeaderSize = wSize;
	m_pSeqHeader = (VO_PBYTE)MemAlloc(m_wSeqHeaderSize);
	if(!m_pSeqHeader)
		return VO_FALSE;
	MemCopy(m_pSeqHeader, pData, m_wSeqHeaderSize);

	VO_CODECBUFFER codecbuf = {0};
	VO_VIDEO_HEADDATAINFO headdatainfo = {0};

	VO_U32 nRes = GetHeadData(&codecbuf);
	if (VO_ERR_SOURCE_OK == nRes && codecbuf.Length)
	{
		if (getResolution_H264(&codecbuf, &headdatainfo) == 0)
		{
			m_wWidth = headdatainfo.Width;
			m_wHeight = headdatainfo.Height;
		}
	}

	return VO_TRUE;
}

VO_BOOL CMpegAVCVideoTrack::ProcessPrevStartCode(VO_PBYTE pPESBuffer,VO_U16 wCurrPos)
{
	if(!m_pTempEntry)
		return VO_TRUE;

	if(wCurrPos > 0)
	{
		AddPayload(pPESBuffer, wCurrPos);
	}
	
	VO_SOURCE_SAMPLE tmpSample;
	if(2 == m_pGlobeBuffer->Peek(&tmpSample.Buffer, m_pSampleData, m_pTempEntry->pos_in_buffer, m_pTempEntry->size))
		tmpSample.Buffer = m_pSampleData;
	IsKeyFrame_H264(tmpSample.Buffer,m_pTempEntry->size,0)?m_pTempEntry->SetKeyFrame(VO_TRUE):m_pTempEntry->SetKeyFrame(VO_FALSE);

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

VO_BOOL CMpegAVCVideoTrack::AddPayload(VO_PBYTE pData, VO_U16 wSize)
{
	if(!m_pGlobeBuffer->HasIdleBuffer(wSize))
		return VO_FALSE;

	VO_U32 CntNALUHeadSyncWord = 4;

	VO_PBYTE pWrite = NULL;
	if (!m_pTempEntry->size)
	{
		wSize += CntNALUHeadSyncWord;
	}
	VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pWrite, wSize);
	if(VO_MAXU32 == dwPosInBuffer)
		return VO_FALSE;
	
	if (!m_pTempEntry->size)
	{
		m_pGlobeBuffer->DirectWrite2((VO_PBYTE)&MPEG_VIDEO_NALU_START_HEADER, pWrite, 0, CntNALUHeadSyncWord);
		m_pGlobeBuffer->DirectWrite2(pData, pWrite, CntNALUHeadSyncWord, wSize - CntNALUHeadSyncWord);
	}
	else
	{
		m_pGlobeBuffer->DirectWrite2(pData, pWrite, 0, wSize);
	}
 ///<we should set keyframe flag only whole frame data is ready 
	if(m_pTempEntry->pos_in_buffer == 0xFFFFFFFF)
	{
		m_pTempEntry->pos_in_buffer = dwPosInBuffer;
	}
	
	m_pTempEntry->Add(wSize);
	return VO_TRUE;
}
VO_PBYTE CMpegAVCVideoTrack::FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_PBYTE pBufferEnd)
{
	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBufferEnd;
	VO_U32 wMask = 0x00FFFFFF;
	const VO_U32 wAVCSyncWord = 0x00000001;///<or 0x000001
	while(pHead < pTail)
	{
		m_LeftData <<= 8;
		m_LeftData |= *pHead++;
		if ((m_LeftData & wMask) == wAVCSyncWord)
		{
			if (!m_wCntByteSyncWord)
			{
				if (m_LeftData & 0xFF000000)
				{
					m_wCntByteSyncWord = 3;///<0x000001
				}
				else
					m_wCntByteSyncWord = 4;///<0x00000001

			}
			return pHead;
		}
	}
	return NULL;
}
VO_BOOL CMpegAVCVideoTrack::AssemblePayloadToFrame(VO_PBYTE pData, VO_U16 wSize)
{
	if(!m_pTempEntry)
		return VO_TRUE;

	if(!m_pGlobeBuffer->HasIdleBuffer(wSize))
		return VO_FALSE;

	VO_PBYTE pWrite = NULL;
	VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pWrite, wSize);
	if(VO_MAXU32 == dwPosInBuffer)
		return VO_FALSE;

	m_pGlobeBuffer->DirectWrite2(pData, pWrite, 0, wSize);

	///<we should set keyframe flag only whole frame data is ready 
	if(m_pTempEntry->pos_in_buffer == 0xFFFFFFFF)
	{
		m_pTempEntry->pos_in_buffer = dwPosInBuffer;
	}

	m_pTempEntry->Add(wSize);

	VO_SOURCE_SAMPLE tmpSample;
	if(2 == m_pGlobeBuffer->Peek(&tmpSample.Buffer, m_pSampleData, m_pTempEntry->pos_in_buffer, m_pTempEntry->size))
		tmpSample.Buffer = m_pSampleData;
	IsKeyFrame_H264(tmpSample.Buffer,m_pTempEntry->size,0)?m_pTempEntry->SetKeyFrame(VO_TRUE):m_pTempEntry->SetKeyFrame(VO_FALSE);


	if (CalculateTimeStamp(m_pTempEntry))
	{
		PushTmpSampleToStreamFile(m_pTempEntry);
	}
	//m_pStreamFileIndex->Add(m_pTempEntry);
	m_pTempEntry = NULL;

	return VO_TRUE;
}