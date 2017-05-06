	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CAmrReader.cpp

Contains:	CAmrReader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CAmrReader.h"
#include "fCC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DEFINE_USE_AMR_GLOBAL_VARIABLE(CAmrReader)

CAmrFramePosChain::CAmrFramePosChain(CMemPool* pMemPool)
	: m_pHead(VO_NULL)
	, m_pTail(VO_NULL)
	, m_dwCount(0)
	, m_pMemPool(pMemPool)
{
}

CAmrFramePosChain::~CAmrFramePosChain()
{
	Release();
}

VO_VOID CAmrFramePosChain::Release()
{
	if(m_pHead)
	{
		PAmrFramePos pCur = m_pHead;
		PAmrFramePos pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->next;

			m_pMemPool->free(pDel, sizeof(AmrFramePos));
		}
		m_pHead = VO_NULL;
	}

	m_pTail = VO_NULL;
	m_dwCount = 0;
}

VO_BOOL CAmrFramePosChain::Add(VO_U8 btPacketSize)
{
	if(m_pTail && m_pTail->GetPacketSize() == btPacketSize)
		m_pTail->AddRef();
	else
	{
		PAmrFramePos pNew = (PAmrFramePos)m_pMemPool->alloc(sizeof(AmrFramePos));
		if(!pNew)
			return VO_FALSE;

		pNew->frame_num = btPacketSize << 26 | 0x1;
		pNew->next = VO_NULL;

		if(!m_pTail)
			m_pHead = m_pTail = pNew;
		else
			m_pTail = m_pTail->next = pNew;
	}

	m_dwCount++;
	return VO_TRUE;
}

VO_BOOL CAmrFramePosChain::GetFrameByIndex(VO_U32 dwIndex, VO_U32& dwPos, VO_U32& dwSize)
{
	PAmrFramePos pCur = m_pHead;
	VO_U32 dwFrameNum = 0;
	dwPos = 0;
	while(pCur)
	{
		if(dwFrameNum + pCur->GetRefCount() > dwIndex)
		{
			dwPos += (dwIndex - dwFrameNum) * pCur->GetPacketSize();
			dwSize = pCur->GetPacketSize();

			return VO_TRUE;
		}
		else
		{
			dwPos += pCur->GetTotalPacketSize();
			dwFrameNum += pCur->GetRefCount();
		}

		pCur = pCur->next;
	}

	return VO_FALSE;
}

VO_U32 CAmrFramePosChain::GetFramIndex(VO_U32 dwPos)
{
	PAmrFramePos pCur = m_pHead;
	VO_U32 dwIndex = 0;
	VO_U32 dwTotalPos = 0;
	while(pCur)
	{
		if(dwTotalPos + pCur->GetTotalPacketSize() > dwPos)
		{
			dwIndex += (dwPos - dwTotalPos) / pCur->GetPacketSize();

			return dwIndex;
		}
		else
		{
			dwTotalPos += pCur->GetTotalPacketSize();
			dwIndex += pCur->GetRefCount();
		}

		pCur = pCur->next;
	}

	return 0;
}

CAmrReader::CAmrReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_bWB(VO_FALSE)
	, m_bOnStream(VO_FALSE)
	, m_dwFrameSize(0)
	, m_fpc(&m_memPool)
	, m_dwCurrIndex(0)
{
	MemSet(&m_wPacketSizes, 0, sizeof(m_wPacketSizes));
}

CAmrReader::~CAmrReader()
{
	m_fpc.Release();
}

VO_U32 CAmrReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
	
	VO_BYTE btHeader[AWB_TAG_SIZE];
	if(!m_chunk.FRead(btHeader, AWB_TAG_SIZE))
		return VO_ERR_SOURCE_OPENFAIL;

	if(!MemCompare((VO_PTR)AMR_TAG_STR, btHeader, AMR_TAG_SIZE))
	{
		m_bWB = VO_FALSE;
		m_ullFileHeadSize = AMR_TAG_SIZE;
		m_dwMaxSampleSize = AMR_MAXPACKETSIZE;
		MemCopy(m_wPacketSizes, (VO_PTR)s_wAMRPacketSizes, sizeof(m_wPacketSizes));
	}
	else if(!MemCompare((VO_PTR)AWB_TAG_STR, btHeader, AWB_TAG_SIZE))
	{
		m_bWB = VO_TRUE;
		m_ullFileHeadSize = AWB_TAG_SIZE;
		m_dwMaxSampleSize = AWB_MAXPACKETSIZE;
		MemCopy(m_wPacketSizes, (VO_PTR)s_wAWBPacketSizes, sizeof(m_wPacketSizes));
	}
	else
		return VO_ERR_SOURCE_OPENFAIL;

 	if(VO_SOURCE_OPENPARAM_FLAG_OPENPD == (nSourceOpenFlags & 0xFF))
 	{
		m_bOnStream = VO_TRUE;
 		DoScanFile(3000 / AMR_SAMPLETIME);
 
 		VO_U32 i = 0, dwPos = 0, dwSize = 0;
 		while(m_fpc.GetFrameByIndex(i, dwPos, dwSize))
 		{
 			m_dwFrameSize += dwSize;
 			i++;
 		}
 		m_dwFrameSize /= i;
 
 		m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * AMR_SAMPLETIME / m_dwFrameSize);
 	}
 	else
	{
		m_bOnStream = VO_FALSE;
		DoScanFile();
		m_dwDuration = m_fpc.GetCount() * AMR_SAMPLETIME;
	}

	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;

	NewReadSampleBuffer();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAmrReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = ReadSampleContent(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	pSample->Time = m_dwCurrIndex * AMR_SAMPLETIME;
	pSample->Duration = 1;

	m_dwCurrIndex++;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAmrReader::SetPos(VO_S64* pPos)
{
	m_dwCurrIndex = (VO_U32)(*pPos / AMR_SAMPLETIME);
	m_dwBufLength = m_dwBufReaded = 0;

	VO_U32 dwPos = 0;
	if (m_bOnStream) {
		dwPos = m_dwCurrIndex * m_dwFrameSize;
	} else {
		VO_U32 dwSize = 0;
		if(!m_fpc.GetFrameByIndex(m_dwCurrIndex, dwPos, dwSize))
			return VO_ERR_SOURCE_END;
	}

	VO_S64 llRes = FileSeek(m_hFile, m_ullFileHeadSize + dwPos, VO_FILE_BEGIN);
	if(llRes < 0)
		return (-2 == llRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;

	*pPos = m_dwCurrIndex * AMR_SAMPLETIME;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAmrReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = VO_NULL;
	pHeadData->Length = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAmrReader::GetCodecCC(VO_U32* pCC)
{
	if(m_bWB)
		*pCC = AudioFlag_AMR_WB;
	else
		*pCC = AudioFlag_AMR_NB;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAmrReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = 1;
	pAudioFormat->SampleBits = 16;
	pAudioFormat->SampleRate = m_bWB ? 16000 : 8000;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAmrReader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwDuration);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CAmrReader::ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync)
{
	if(dwLen < 1)
		return VO_FALSE;

	VO_U8 btPacketSize = (VO_U8)(m_wPacketSizes[(pBuffer[0] & 0x78) >> 3]);
	if(btPacketSize > dwLen)
		return VO_FALSE;

	dwSampleStart = 0;
	dwReaded = dwSampleEnd = btPacketSize + 1;

	return VO_TRUE;
}

VO_BOOL CAmrReader::DoScanFile(VO_U32 dwFrameLimit /* = VO_MAXU32 */)
{
	m_chunk.FLocate(m_ullFileHeadSize);

	VO_BYTE btHeader;
	VO_U8 btPacketSize;
	while(m_chunk.FRead(&btHeader, 1) && m_fpc.GetCount() < dwFrameLimit)
	{
		btPacketSize = (VO_U8)(m_wPacketSizes[(btHeader & 0x78) >> 3]);
		if(!m_chunk.FSkip(btPacketSize))
			break;

		//add this frame
		if(!m_fpc.Add(btPacketSize + 1))
			return VO_FALSE;
	}

	return VO_TRUE;
}

VO_U32 CAmrReader::IsNeedScan(VO_BOOL* pIsNeedScan)
{
	*pIsNeedScan = VO_TRUE;

	return VO_ERR_SOURCE_OK;
}
