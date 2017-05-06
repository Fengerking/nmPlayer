	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CQcomVideoDec.cpp

	Contains:	CQcomVideoDec class file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18		East		Create file

*******************************************************************************/
#include "CQcomVideoDec.h"
#include "OMX_Video.h"
#include "voOMXMemory.h"
#include "OMX_QCOMExtns.h"

#define LOG_TAG "CQcomVideoDec"
#include "voLog.h"

#define VO_MAKEALIGN(v, align)		(((v) + (align) - 1) & (~((align) - 1)))

#define Q6_VDEC_PAGE_SIZE  0x1000
#define Q6_VDEC_PAGE_ALIGN(addr)	VO_MAKEALIGN(addr, Q6_VDEC_PAGE_SIZE)

static inline VO_S32 GetExtraDataSize()
{
	return ((OMX_EXTRADATA_HEADER_SIZE + sizeof(OMX_QCOM_EXTRADATA_FRAMEINFO)+3) & (~3)) + ((OMX_EXTRADATA_HEADER_SIZE+sizeof(OMX_QCOM_EXTRADATA_CODEC_DATA)+3) & (~3)) + ((OMX_EXTRADATA_HEADER_SIZE+sizeof(OMX_QCOM_EXTRADATA_FRAMEDIMENSION)+3) & (~3))+(OMX_EXTRADATA_HEADER_SIZE + 4);
}

CQcomVideoDec::CQcomVideoDec()
	: m_nCoding(OMX_VIDEO_CodingUnused)
	, m_nFourcc(0)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
	, m_nInputBuffers(0)
	, m_pInputBuffers(NULL)
	, m_nOutputBufferAllocatedLength(0)
	, m_nOutputBuffers(0)
	, m_pOutputBuffers(NULL)
{
	memset(&m_sPMem, 0, sizeof(m_sPMem));
	m_sPMem.nFD = -1;
}

CQcomVideoDec::~CQcomVideoDec()
{
	Uninit ();
}

VO_BOOL CQcomVideoDec::Init(VO_U32 nCoding, VO_U32 nFourcc, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_U32 nWidth, VO_U32 nHeight)
{
	Uninit ();

	m_nCoding = nCoding;
	m_nFourcc = nFourcc;

	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;

	//do initialize...
	if(VO_FALSE == m_Adsp.Open())
		return VO_FALSE;

	if(VO_FALSE == m_Adsp.Start(nCoding, nFourcc, pHeadData, nHeadSize, nWidth, nHeight))
		return VO_FALSE;

	return VO_TRUE;
}

VO_VOID CQcomVideoDec::Uninit()
{
	//do uninitialize...
	m_Adsp.Stop();
	m_Adsp.Close();

	pmem_free(&m_sPMem);

	if(m_pInputBuffers)
	{
		delete [] m_pInputBuffers;
		m_pInputBuffers = NULL;
	}

	if(m_pOutputBuffers)
	{
		delete [] m_pOutputBuffers;
		m_pOutputBuffers = NULL;
	}
}

VO_BOOL CQcomVideoDec::Flush(ADSP_VDEC_PORTTYPE nPort)
{
	voCAutoLock lock (&m_mtxVDec);

	//do flush...
	m_Adsp.Flush(nPort);

	//We should wait flush done here...
	VO_U32 i = 0;
	if(m_pInputBuffers)
	{
		for(i = 0; i < m_nInputBuffers; i++)
		{
			if(m_pInputBuffers[i].eOwner == QcomVdec_BufferOwner_HW)
			{
				m_mtxInputBuffer.Lock();
				m_pInputBuffers[i].eOwner = QcomVdec_BufferOwner_VDEC_CORE;
				m_mtxInputBuffer.Unlock();

				OnBufferDone(m_pInputBuffers[i].pUserData);
			}
		}
	}

	if(m_pOutputBuffers)
	{
		for(i = 0; i < m_nOutputBuffers; i++)
		{
			if(m_pOutputBuffers[i].sBuffer.eOwner == QcomVdec_BufferOwner_HW)
			{
				m_mtxOutputBuffer.Lock();
				m_pOutputBuffers[i].sBuffer.eOwner = QcomVdec_BufferOwner_APP_FLUSHED;
				m_mtxOutputBuffer.Unlock();

				m_pOutputBuffers[i].nFlags = QCOMVDEC_FRAMEFLAG_FLUSHED;
				OnFrameDone(m_pOutputBuffers + i);
			}
		}
	}
	return VO_TRUE;
}

VO_BOOL CQcomVideoDec::AllocateInputBuffer(VO_U32 nSize, QcomVdec_BufferInfo* pBufferInfo, VO_BOOL bIsPMem)
{
	if(nSize <= 0 || pBufferInfo == NULL)
	{
		VOLOGE("Invalid argument allocate input buffer");

		return VO_FALSE;
	}

	if(bIsPMem)
	{
		VO_S32 nPageSize = pmem_getpagesize();

		VO_PMEM sPMem;
		sPMem.nFD = -1;
		sPMem.nSize = VO_MAKEALIGN(nSize, nPageSize);
		if(-1 == pmem_alloc(&sPMem, sPMem.nSize))
		{
			VOLOGE("pmem allocation failed. size %d, page size %d", nSize, nPageSize);

			return VO_FALSE;
		}

		pBufferInfo->pBase = (VO_PBYTE)sPMem.pData;
		pBufferInfo->nPMemID = sPMem.nFD;
		pBufferInfo->nPMemOffset = 0;
		pBufferInfo->nSize = sPMem.nSize;
	}
	else
	{
		VO_PBYTE pData = (VO_PBYTE)voOMXMemAlloc(nSize);
		if(pData == NULL)
		{
			VOLOGE("heap allocation failed. size %d", nSize);

			return VO_FALSE;
		}

		pBufferInfo->pBase = pData;
		pBufferInfo->nSize = nSize;
	}
	pBufferInfo->eOwner = QcomVdec_BufferOwner_APP;

	return VO_TRUE;
}

VO_VOID CQcomVideoDec::FreeInputBuffer(QcomVdec_BufferInfo* pBufferInfo, VO_BOOL bIsPMem)
{
	if(pBufferInfo && pBufferInfo->pBase)
	{
		if(bIsPMem)
		{
			VO_PMEM sPMem;
			sPMem.pData = pBufferInfo->pBase;
			sPMem.nFD = pBufferInfo->nPMemID;
			sPMem.nSize = pBufferInfo->nSize;
			pmem_free(&sPMem);
		}
		else
			voOMXMemFree(pBufferInfo->pBase);
	}
}

VO_BOOL CQcomVideoDec::PostInputBuffer(VO_PBYTE pData, VO_U32 nLen, VO_S64 llTimeStamp, VO_U32 nFlags, VO_PTR pUserData, VO_BOOL bIsPMem)
{
	VOLOGR("data addr 0x%08X, len %d, time %lld, flag 0x%08X", pData, nLen, llTimeStamp, nFlags);

	if(NULL == pData)
		return VO_FALSE;

	VO_U32 i = 0;
	if(m_pOutputBuffers)
	{
		for(i = 0; i < m_nOutputBuffers; i++)
		{
			if(m_pOutputBuffers[i].sBuffer.eOwner == QcomVdec_BufferOwner_HW)
			{
				VOLOGR("available free output buffer %d", i);

				break;
			}
		}

		if(i >= m_nOutputBuffers)
		{
			VOLOGW("output buffer not avilable for decode");

			return VO_FALSE;
		}
	}

	if(VO_FALSE == bIsPMem)
	{
		for(i = 0; i < m_nInputBuffers; i++)
		{
			if(m_pInputBuffers[i].eOwner == QcomVdec_BufferOwner_VDEC_CORE)
				break;
		}
	}
	else
	{
		for(i = 0; i < m_nInputBuffers; i++)
		{
			if((pData >= m_pInputBuffers[i].pBase) && (pData < m_pInputBuffers[i].pBase + m_pInputBuffers[i].nSize))
				break;
		}
	}

	if(i >= m_nInputBuffers)
	{
		VOLOGW("Wrong Input buffer and not able to get the buffer Index");

		return VO_FALSE;
	}

	m_mtxInputBuffer.Lock();
	m_pInputBuffers[i].eOwner = QcomVdec_BufferOwner_HW;
	m_mtxInputBuffer.Unlock();

	VOLOGR("inputBuffer addr 0x%08X, size %d", m_pInputBuffers[i].pBase, m_pInputBuffers[i].nSize);

	VO_U32 nSize = nLen;
	VO_U32 nOffset = 0;
	if(VO_FALSE == bIsPMem)
	{
		if(m_pInputBuffers[i].nSize < nSize)
			nSize = m_pInputBuffers[i].nSize;

		memcpy(m_pInputBuffers[i].pBase, pData, nSize);
		nOffset = m_pInputBuffers[i].nPMemOffset;
	}
	else
		nOffset = pData - m_pInputBuffers[i].pBase;

	m_pInputBuffers[i].pUserData = pUserData;

	if((nSize == 0) && (nFlags & QCOMVDEC_FRAMEFLAG_EOS))
	{
		m_mtxInputBuffer.Lock();
		m_pInputBuffers[i].eOwner = QcomVdec_BufferOwner_VDEC_CORE;
		m_mtxInputBuffer.Unlock();

		if(VO_FALSE == m_Adsp.PostInputBuffer(m_pInputBuffers[i].nPMemID, nOffset, nSize, llTimeStamp, nFlags, VO_TRUE, pUserData))
		{
			VOLOGE("Post Input Buffer Failed");
			
			return VO_FALSE;
		}

		OnBufferDone(pUserData);

		return VO_TRUE;
	}

	if(VO_FALSE == m_Adsp.PostInputBuffer(m_pInputBuffers[i].nPMemID, nOffset, nSize, llTimeStamp, nFlags, VO_FALSE, pUserData))
	{
		VOLOGE("Post Input Buffer Failed");

		m_mtxInputBuffer.Lock();
		m_pInputBuffers[i].eOwner = QcomVdec_BufferOwner_VDEC_CORE;
		m_mtxInputBuffer.Unlock();

		OnBufferDone(pUserData);

		return VO_FALSE;
	}

	if(nFlags & QCOMVDEC_FRAMEFLAG_EOS)
	{
		if(VO_FALSE == m_Adsp.PostInputBuffer(m_pInputBuffers[i].nPMemID, nOffset, nSize, llTimeStamp, nFlags, VO_TRUE, pUserData))
		{
			VOLOGE("Post Input Buffer EOS Failed");

			return VO_FALSE;
		}
	}

	return VO_TRUE;
}

VO_BOOL CQcomVideoDec::ReleaseFrame(QcomVdec_FrameInfo* pFrameInfo)
{
	if(NULL == pFrameInfo)
		return VO_FALSE;

	VO_U32 i = 0;
	for(i = 0; i < m_nOutputBuffers; i++)
	{
		if(m_pOutputBuffers[i].sBuffer.pBase == pFrameInfo->sBuffer.pBase)
			break;
	}

	if(i >= m_nOutputBuffers)
	{
		VOLOGE("Wrong Output buffer and not able to get the buffer Index");

		return VO_FALSE;
	}

	m_mtxOutputBuffer.Lock();
	m_pOutputBuffers[i].sBuffer.eOwner = QcomVdec_BufferOwner_HW;
	m_mtxOutputBuffer.Unlock();

	if(m_pOutputBuffers[i].sBuffer.eOwner == QcomVdec_BufferOwner_APP)
	{
		if(VO_FALSE == m_Adsp.ReleaseFrame(pFrameInfo->sFrameInfo.nUserData1))
		{
			VOLOGE("Adsp release frame failed");

			return VO_FALSE;
		}
	}

	return VO_TRUE;
}

VO_BOOL CQcomVideoDec::CommitMemory()
{
	VO_BOOL bAllocInputBuffers = VO_TRUE;
	if(m_pInputBuffers)
	{
		VOLOGI("we need not allocate input buffers");
		bAllocInputBuffers = VO_FALSE;
	}

	//get buffer request information for ADSP module
	VO_BUFFER_REQUEST* pBufReqInfo = m_Adsp.GetBufferRequestInformation();

	//////////////////////////////////////////////////////////////////////////
	//Create Input Buffer
	if(NULL == m_pInputBuffers)
	{
		m_nInputBuffers = pBufReqInfo->sInputBufferInfo.nMinNum;
		m_pInputBuffers = new QcomVdec_BufferInfo[m_nInputBuffers];
		if(NULL == m_pInputBuffers)
		{
			VOLOGE("failed to create input buffers");

			return VO_FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Create Output Buffer
	m_nOutputBuffers = pBufReqInfo->sOutputBufferInfo.nMinNum;
	m_pOutputBuffers = new QcomVdec_FrameInfo[m_nOutputBuffers];
	if(NULL == m_pOutputBuffers)
	{
		VOLOGE("failed to allocate output buffers");

		return VO_FALSE;
	}

	VO_S32 nExtraSize = GetExtraDataSize();
	m_nOutputBufferAllocatedLength = pBufReqInfo->sOutputBufferInfo.nSize + nExtraSize;

	//////////////////////////////////////////////////////////////////////////
	//Allocate Physical Memory
	VO_S32 nPageSize = pmem_getpagesize();

	VO_U32 nInBufSize = 0;
	if(bAllocInputBuffers)
		nInBufSize = m_nInputBuffers * pBufReqInfo->sInputBufferInfo.nSize;

	VO_U32 nOutBufSize = Q6_VDEC_PAGE_ALIGN(m_nOutputBufferAllocatedLength);
	VO_U32 nTotalOutBufSize = Q6_VDEC_PAGE_ALIGN(nOutBufSize * m_nOutputBuffers);

	VO_U32 nDec1BufSize = 0;
	if(pBufReqInfo->sDecReq1BufferInfo.nMinNum)
		nDec1BufSize = pBufReqInfo->sDecReq1BufferInfo.nMinNum * pBufReqInfo->sDecReq1BufferInfo.nSize;

	VO_U32 nDec2BufSize = 0;
	if(pBufReqInfo->sDecReq2BufferInfo.nMinNum)
		nDec2BufSize = pBufReqInfo->sDecReq2BufferInfo.nMinNum * pBufReqInfo->sDecReq2BufferInfo.nSize;

	VO_U32 nOutOffset = Q6_VDEC_PAGE_ALIGN(nInBufSize);
	VO_U32 nDec1Offset = Q6_VDEC_PAGE_ALIGN(nOutOffset + nTotalOutBufSize);
	VO_U32 nDec2Offset = Q6_VDEC_PAGE_ALIGN(nDec1Offset + nDec1BufSize);
	VO_U32 nTotalSize = VO_MAKEALIGN(nDec2Offset + nDec2BufSize, nPageSize);
	if(pmem_alloc(&m_sPMem, nTotalSize))
	{
		VOLOGE("failed to allocate pmem arena (%d bytes)", nTotalSize);

		return VO_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//Record Input Buffer
	VO_U32 nOffset = 0;
	if(VO_TRUE == bAllocInputBuffers)
	{
		for(VO_U32 i = 0; i < m_nInputBuffers; i++)
		{
			m_pInputBuffers[i].nPMemID = m_sPMem.nFD;
			m_pInputBuffers[i].nPMemOffset = nOffset;
			m_pInputBuffers[i].pBase = (VO_PBYTE)m_sPMem.pData + nOffset;
			m_pInputBuffers[i].nSize = pBufReqInfo->sInputBufferInfo.nSize;
			m_pInputBuffers[i].eOwner = QcomVdec_BufferOwner_VDEC_CORE;

			nOffset += pBufReqInfo->sInputBufferInfo.nSize;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Record Output Buffer
	nOffset = nOutOffset;
	for(VO_U32 i = 0; i < m_nOutputBuffers; i++)
	{
		m_pOutputBuffers[i].sBuffer.nPMemID = m_sPMem.nFD;
		m_pOutputBuffers[i].sBuffer.nPMemOffset = nOffset;
		m_pOutputBuffers[i].sBuffer.pBase = (VO_PBYTE)m_sPMem.pData + nOffset;
//		m_pOutputBuffers[i].sBuffer.nSize = pBufReqInfo->sOutputBufferInfo.nSize;//nOutBufSize;
		m_pOutputBuffers[i].sBuffer.eOwner = QcomVdec_BufferOwner_APP_FLUSHED;

		nOffset += nOutBufSize;
	}

	//////////////////////////////////////////////////////////////////////////
	//Set Input Buffer
	if(VO_TRUE == bAllocInputBuffers)
	{
		if(VO_FALSE == m_Adsp.SetBuffers(m_sPMem.nFD, ADSP_BUFFER_TYPE_INPUT, m_nInputBuffers, VO_TRUE, 0, nInBufSize))
		{
			VOLOGE("failed to set adsp buffers(%d)", m_nInputBuffers);

			return VO_FALSE;
		}
	}
	else
	{
		for(VO_U32 i = 0; i < m_nInputBuffers; i++)
		{
			if(VO_FALSE == m_Adsp.SetBuffers(m_pInputBuffers[i].nPMemID, ADSP_BUFFER_TYPE_INPUT, 1, (i == (m_nInputBuffers - 1)) ? VO_TRUE : VO_FALSE, m_pInputBuffers[i].nPMemOffset, m_pInputBuffers[i].nSize))
			{
				VOLOGE("failed to set adsp buffers(%d / %d)", i, m_nInputBuffers);

				return VO_FALSE;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Set Output Buffer
	for(VO_U32 i = 0; i < m_nOutputBuffers; i++)
	{
		if(VO_FALSE == m_Adsp.SetBuffers(m_sPMem.nFD, ADSP_BUFFER_TYPE_OUTPUT, 1, (i == (m_nOutputBuffers - 1)) ? VO_TRUE : VO_FALSE, m_pOutputBuffers[i].sBuffer.nPMemOffset, pBufReqInfo->sOutputBufferInfo.nSize))
		{
			VOLOGE("failed to set adsp buffers(output)");

			return VO_FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Set Dec1 Buffer
	if(pBufReqInfo->sDecReq1BufferInfo.nMinNum)
	{
		if(VO_FALSE == m_Adsp.SetBuffers(m_sPMem.nFD, ADSP_BUFFER_TYPE_INTERNAL1, pBufReqInfo->sDecReq1BufferInfo.nMinNum, VO_TRUE, nDec1Offset, nDec1BufSize))
		{
			VOLOGE("failed to set adsp buffers(dec1)");

			return VO_FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Set Dec2 Buffer
	if(pBufReqInfo->sDecReq2BufferInfo.nMinNum)
	{
		if(VO_FALSE == m_Adsp.SetBuffers(m_sPMem.nFD, ADSP_BUFFER_TYPE_INTERNAL2, pBufReqInfo->sDecReq2BufferInfo.nMinNum, VO_TRUE, nDec2Offset, nDec2BufSize))
		{
			VOLOGE("failed to set adsp buffers(dec2)");

			return VO_FALSE;
		}
	}

	return VO_TRUE;
}

VO_VOID CQcomVideoDec::OnFrameDone(QcomVdec_FrameInfo* pFrameInfo)
{
}

VO_VOID CQcomVideoDec::OnBufferDone(VO_PTR pUserData)
{
}