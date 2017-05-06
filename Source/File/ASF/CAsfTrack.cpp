#include "CAsfReader.h"
#include "CAsfTrack.h"
#include "fCodec.h"
#include "voPlayReady.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAsfTrack::CAsfTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CAsfReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CBaseStreamFileTrack(nType, btStreamNum, dwDuration, pReader, pMemOp)
	, m_pReader(pReader)
	, m_dwPropBufferSize(0)
	, m_pPropBuffer(VO_NULL)
	, m_dwBitrate(0)
	, m_dwFrameRate(0)
	, m_lMaxSampleSize(0)
	, m_dwTimeStampInSeek(VO_MAXU32)
	, m_dwPrevTimeStampInSeek(VO_MAXU32)
	, m_pTempEntry(VO_NULL)
	, m_dwSizeCurrentKeyFrame(0)
{
}

CAsfTrack::~CAsfTrack()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	SAFE_MEM_FREE(m_pPropBuffer);
}

VO_BOOL CAsfTrack::AddPayloadInfo(CGFileChunk* pFileChunk, VO_BOOL bKeyFrame, VO_U32 dwPosInMediaObj, VO_U32 dwPayloadLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp, VO_PBYTE pIV/* = NULL*/)
{
	if(!m_bInUsed)
		return pFileChunk->FSkip(dwPayloadLen);

	//if current content has been parsed, only skipped
	//East 2009/12/07
	if(VO_MAXU64 != m_ullCurrParseFilePos)
	{
		if(pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
			return pFileChunk->FSkip(dwPayloadLen);
		else
			m_ullCurrParseFilePos = VO_MAXU64;
	}

	//we should process first frame
	//1, if the new frame not from start position, we will ignore it!!
	//2, make sure video first frame must be key frame!!
	if(m_pStreamFileIndex->IsNull() && !m_pTempEntry && !m_pDataParser && (dwPosInMediaObj > 0 || ((VOTT_VIDEO == m_nType) && !bKeyFrame)))
		return pFileChunk->FSkip(dwPayloadLen);

	if(!m_pGlobeBuffer->HasIdleBuffer(dwPayloadLen))	//buffer lack
		return VO_FALSE;

	VO_U32 dwPosInBuffer = m_pGlobeBuffer->Add(pFileChunk, dwPayloadLen);
	if(VO_MAXU32 == dwPosInBuffer)
		return VO_FALSE;

	if ( m_pReader->IsWMDRM() )
	{
		VO_PBYTE pPayload = VO_NULL;
		VO_PBYTE pTmpPayload = m_pReader->GetTmpPayload();
		if(!pTmpPayload)
			return VO_FALSE;

		if(2 == m_pGlobeBuffer->Peek(&pPayload, pTmpPayload, dwPosInBuffer, dwPayloadLen))
		{
			if(VO_ERR_DRM_OK != m_pReader->DRMData(VO_DRMDATATYPE_PACKETDATA, pTmpPayload, dwPayloadLen))
				return VO_FALSE;

			m_pGlobeBuffer->RemoveFrom(dwPosInBuffer);
			m_pGlobeBuffer->GetForDirectWrite(&pPayload, dwPayloadLen);
			m_pGlobeBuffer->DirectWrite2(pTmpPayload, pPayload, 0, dwPayloadLen);
		}
		else
		{
			if(VO_ERR_DRM_OK != m_pReader->DRMData(VO_DRMDATATYPE_PACKETDATA, pPayload, dwPayloadLen))
				return VO_FALSE;
		}
	}

	if(!m_pTempEntry || !dwPosInMediaObj)		//new sample
	{
		if(m_pTempEntry)			//normally, here m_pTempEntry must be VO_NULL, but we must process packets lost!!
			m_pStreamFileIndex->DeleteEntry(m_pTempEntry);

		m_pTempEntry = m_pStreamFileIndex->NewEntry(dwTimeStamp, (bKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), dwMediaObjSize);
		if(!m_pTempEntry)
			return VO_FALSE;
	}

	if(dwPosInMediaObj + dwPayloadLen == m_pTempEntry->size)	//last payload
	{
		if ( m_pReader->IsPlayReady() )
		{
			VO_PBYTE pPayload = VO_NULL;
			VO_PBYTE pTmpPayload = m_pReader->GetTmpPayload();
			if(!pTmpPayload)
				return VO_FALSE;

			VO_PLAYREADY_AESCTR_INFO aesctr;
			memset( &aesctr, 0, sizeof(VO_PLAYREADY_AESCTR_INFO) );

			memcpy(aesctr.btInitializationVector, pIV, 8);
			aesctr.nBlockOffset				= dwPosInMediaObj / 16;
			aesctr.btByteOffset				= dwPosInMediaObj % 16;

			if(2 == m_pGlobeBuffer->Peek(&pPayload, pTmpPayload, m_pTempEntry->pos_in_buffer & 0x7FFFFFFF, m_pTempEntry->size))
			{
				if(VO_ERR_DRM_OK != m_pReader->DRMData(VO_DRMDATATYPE_PACKETDATA, pTmpPayload, m_pTempEntry->size, NULL, NULL, &aesctr))
					return VO_FALSE;

				m_pGlobeBuffer->RemoveFrom(m_pTempEntry->pos_in_buffer & 0x7FFFFFFF);
				m_pGlobeBuffer->GetForDirectWrite(&pPayload, m_pTempEntry->size);
				m_pGlobeBuffer->DirectWrite2(pTmpPayload, pPayload, 0, m_pTempEntry->size);
			}
			else
			{
				if(VO_ERR_DRM_OK != m_pReader->DRMData(VO_DRMDATATYPE_PACKETDATA, pPayload, m_pTempEntry->size, NULL, NULL, &aesctr))
					return VO_FALSE;
			}
		}

		m_pStreamFileIndex->Add(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	return VO_TRUE;
}

VO_VOID CAsfTrack::Flush()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	CBaseStreamFileTrack::Flush();
}

VO_U32 CAsfTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = m_dwBitrate;

	return VO_ERR_SOURCE_OK;
}

VO_U8 CAsfTrack::IsCannotGenerateIndex(CvoFileDataParser* pDataParser)
{
	if(pDataParser->IsParseEnd())
		return 2;

	if(m_bParseForSelf)
	{
		if(IsGlobalBufferFull())
			return 1;
	}
	else
	{
		if(m_pReader->IsTrackBufferFull() || m_pReader->IsTrackBufferHasEnoughFrame(VO_MAXS32))
		{	
			return 1;
		}
	}

	return 0;
}
VO_U32 CAsfTrack::GetTrackBufDuration()
{
	if (!m_pStreamFileIndex || (m_pStreamFileIndex->GetCurrEndTime() < m_pStreamFileIndex->GetCurrStartTime()))
	{
		return 0;
	}
	//VOLOGI("Time1==%d,Time2==%d",m_pStreamFileIndex->GetCurrEndTime(),m_pStreamFileIndex->GetCurrStartTime());
	return m_pStreamFileIndex->GetCurrEndTime() - m_pStreamFileIndex->GetCurrStartTime();
}