	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBasePort.h

	Contains:	CBasePort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __CBasePort_H__
#define __CBasePort_H__

#include <OMX_Component.h>

#include "voVideo.h"
#include "voAudio.h"

#include "voCOMXThreadMutex.h"
#include "voLog.h"

class CBaseComp;

class CBufferItem
{
public:
	CBufferItem (void)
	{
		m_pBufferHead = NULL;
		m_pNext = NULL;
	}
	virtual ~CBufferItem (void){}

public:
	OMX_BUFFERHEADERTYPE *	m_pBufferHead;
	CBufferItem *			m_pNext;
};

class CBufferList
{
public:
	CBufferList (void)
	{
		m_pFirst = NULL;
		m_pLast = NULL;
		m_nCount = 0;
	}

	virtual ~CBufferList (void)
	{
		ResetItems ();
	}

	void ResetItems (void)
	{
		CBufferItem * pNextItem = m_pFirst;
		CBufferItem * pTempItem = m_pFirst;
		while (pNextItem != NULL)
		{
			pTempItem = pNextItem;
			pNextItem = pNextItem->m_pNext;
			delete pTempItem;
			m_nCount--;
		}

		m_pFirst = NULL;
		m_pLast = NULL;
		m_nCount = 0;

		if (m_nCount != 0)
			VOLOGE ("The count is %d after reset.", m_nCount);
	}

	void AddBufferItem (CBufferItem * pItem)
	{
		pItem->m_pNext = NULL;
		if (m_pLast == NULL)
		{
			m_pFirst = pItem;
			m_pLast = m_pFirst;
		}
		else
		{
			m_pLast->m_pNext = pItem;
			m_pLast = pItem;
		}

		m_nCount++;
	}

	void AddBufferHead (OMX_BUFFERHEADERTYPE *	pBufferHead)
	{
		CBufferItem * pItem = new CBufferItem ();
		pItem->m_pBufferHead = pBufferHead;
		AddBufferItem (pItem);
	}

	CBufferItem * RemoveBufferHead (OMX_BUFFERHEADERTYPE *	pBufferHead)
	{
		CBufferItem * pBuffItem = NULL;
		if (m_pFirst == NULL)
			return NULL;

		m_nCount--;
		if (m_pFirst->m_pBufferHead == pBufferHead)
		{
			pBuffItem = m_pFirst;
			m_pFirst = m_pFirst->m_pNext;
			if (m_pFirst == NULL)
				m_pLast = NULL;
			pBuffItem->m_pNext = NULL;
			return pBuffItem;
		}

		CBufferItem * pPrevItem = m_pFirst;
		CBufferItem * pNextItem = m_pFirst;
		while (pNextItem != NULL)
		{
			if (pNextItem->m_pBufferHead == pBufferHead)
			{
				pPrevItem->m_pNext = pNextItem->m_pNext;
				pBuffItem = pNextItem;
				break;
			}

			pPrevItem = pNextItem;
			pNextItem = pNextItem->m_pNext;
		}

		if (m_pLast == pNextItem)
			m_pLast = pPrevItem;

		pBuffItem->m_pNext = NULL;
		return pBuffItem;
	}

	CBufferItem * RemoveBufferItem (void)
	{
		if (m_pFirst == NULL)
			return NULL;

		CBufferItem * pItem = m_pFirst;
		m_pFirst = m_pFirst->m_pNext;

		if (m_pFirst == NULL)
			m_pLast = NULL;

		m_nCount--;

		pItem->m_pNext = NULL;
		return pItem;
	}

	int GetCount (void)
	{
		int nCount = 0;
		CBufferItem * pNextItem = m_pFirst;
		while (pNextItem != NULL)
		{
			nCount++;
			pNextItem = pNextItem->m_pNext;
		}

		if (m_nCount != nCount)
			VOLOGE ("m_nCount %d, nCount  %d was not same!", m_nCount, nCount)

		return nCount;
	}

	CBufferItem* GetHead()
	{
		return m_pFirst;
	}

protected:
	CBufferItem *		m_pFirst;
	CBufferItem *		m_pLast;
	int					m_nCount;
};

class CBasePort
{
public:
    CBasePort(CBaseComp * pComp, OMX_U32 nIndex);
    virtual ~CBasePort(void);

	virtual VO_U32				SetVideoInputData(VO_CODECBUFFER * pInput, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_BOOL bHeader);
	virtual VO_U32				GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);
	virtual VO_U32				FillOutputBuffer();

	virtual VO_U32				SetAudioInputData(VO_CODECBUFFER * pInput);
	virtual VO_U32				GetAudioOutputData(VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);

	virtual VO_U32				SetVideoRenderInput(VO_VIDEO_BUFFER * pInput);

	virtual OMX_ERRORTYPE		AllocBuffer(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL);
	virtual OMX_ERRORTYPE		FreeBuffer(void);
	virtual OMX_ERRORTYPE		StartBuffer(void);
	virtual OMX_ERRORTYPE		EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE		FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE		Stop (void);
	virtual OMX_ERRORTYPE		SetVideoInfo (VO_VIDEO_CODINGTYPE nCoding, VO_IV_COLORTYPE nColorType, OMX_U32  nWidth, OMX_U32 nHeight);
	virtual OMX_ERRORTYPE		SetAudioInfo (VO_AUDIO_CODINGTYPE nCoding, VO_U32 nSampleRate, VO_U32 nChannels, VO_U32 nSampleBits);
	virtual OMX_ERRORTYPE		ReturnAllBuffers(void) { return OMX_ErrorNone;}
	virtual void				SetCrop(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL) {};
	virtual void				SetFlag(OMX_U32 flags = 0) {};

	OMX_ERRORTYPE				GetDefinition(OMX_PTR pParam = NULL);
	OMX_ERRORTYPE				SetDefinition(OMX_PTR pParam = NULL);
	VO_U32						GetVideoSize (OMX_U32 & nWidth, OMX_U32 & nHeight) {nWidth = m_nVideoWidth; nHeight = m_nVideoHeight; return 0;}
	VO_U32						GetVideoDecDimension (void* pdeminfo);
	VO_U32						GetEmptyNum (void) {return m_lstFree.GetCount ();}
	VO_U32						GetFullNum (void) {return m_lstFull.GetCount ();}

protected:
	virtual OMX_ERRORTYPE		FillPortType(OMX_PARAM_PORTDEFINITIONTYPE* pdef = NULL);
	virtual OMX_ERRORTYPE		CreateBufferHead (void);

protected:
	CBaseComp *									m_pComp;
	OMX_U32										m_nIndex;
	OMX_COMPONENTTYPE *							m_pComponent;

	OMX_PARAM_PORTDEFINITIONTYPE				m_portType;
	OMX_BOOL									m_bSupplier;

	voCOMXThreadMutex							m_muxBuffer;
	OMX_U32										m_nBuffCount;
	OMX_BUFFERHEADERTYPE **						m_ppBuffHead;
	OMX_U8 **									m_ppDataBuffer;
	CBufferItem *								m_pOutputItem;
	CBufferList									m_lstFull;
	CBufferList									m_lstFree;

	OMX_U32										m_nVideoWidth;
	OMX_U32										m_nVideoHeight;
	OMX_U32										m_nPadWidth;
	OMX_U32										m_nPadHeight;
	VO_IV_COLORTYPE								m_nColorType;
	VO_VIDEO_DECDIMENSION						m_diminfo;

	OMX_U32										m_nSampleRate;
	OMX_U32										m_nChannels;
	OMX_U32										m_nSampleBits;

	OMX_U32										m_nInputNum;
	OMX_U32										m_nOutputNum;
	OMX_TICKS									m_nLastBufferTime;
};

#endif //__CBasePort_H__
