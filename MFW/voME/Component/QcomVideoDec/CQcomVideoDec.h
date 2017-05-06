	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CQcomVideoDec.h

	Contains:	CQcomVideoDec header file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18		East		Create file

*******************************************************************************/

#ifndef __CQcomVideoDec_H__
#define __CQcomVideoDec_H__

#include "voCMutex.h"
#include "CAdsp.h"
#include "pmem.h"

typedef enum
{
	QcomVdec_BufferOwner_HW, 
	QcomVdec_BufferOwner_APP, 
	QcomVdec_BufferOwner_VDEC_CORE, 
	QcomVdec_BufferOwner_APP_FLUSHED, 
} QcomVdec_BufferOwner;

#define QCOMVDEC_FRAMEFLAG_EOS				0x00000001
#define QCOMVDEC_FRAMEFLAG_FLUSHED			0x00000002
#define QCOMVDEC_FRAMEFLAG_FATALERROR		0x00000010

typedef struct
{
	VO_PBYTE				pBase;
	VO_U32					nPMemID;
	VO_U32					nPMemOffset;
	VO_U32					nSize;
	QcomVdec_BufferOwner	eOwner;
	VO_PTR					pUserData;
} QcomVdec_BufferInfo;

typedef struct
{
	VO_S64					llTimeStamp;	/* frame timestamp */
	VO_U32					nFlags;			/* holds frame flags properties */
	ADSP_VDEC_FRAMEINFO		sFrameInfo;		/* Details on decoded frame */
	QcomVdec_BufferInfo		sBuffer;
} QcomVdec_FrameInfo;

class CQcomVideoDec
{
public:
	CQcomVideoDec();
	virtual ~CQcomVideoDec();

public:
	virtual VO_BOOL		Init(VO_U32 nCoding, VO_U32 nFourcc, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_U32 nWidth, VO_U32 nHeight);
	virtual VO_VOID		Uninit();

	virtual VO_BOOL		Flush(ADSP_VDEC_PORTTYPE nPort);

	virtual VO_BOOL		AllocateInputBuffer(VO_U32 nSize, QcomVdec_BufferInfo* pBufferInfo, VO_BOOL bIsPMem);
	virtual VO_VOID		FreeInputBuffer(QcomVdec_BufferInfo* pBufferInfo, VO_BOOL bIsPMem);
	virtual VO_BOOL		PostInputBuffer(VO_PBYTE pData, VO_U32 nLen, VO_S64 llTimeStamp, VO_U32 nFlags, VO_PTR pUserData, VO_BOOL bIsPMem);

	virtual VO_BOOL		ReleaseFrame(QcomVdec_FrameInfo* pFrameInfo);

	virtual VO_BOOL		CommitMemory();

	virtual VO_VOID		OnFrameDone(QcomVdec_FrameInfo* pFrameInfo);
	virtual VO_VOID		OnBufferDone(VO_PTR pUserData);

protected:
	VO_U32						m_nCoding;
	VO_U32						m_nFourcc;

	VO_U32						m_nVideoWidth;
	VO_U32						m_nVideoHeight;

	voCMutex					m_mtxVDec;
	voCMutex					m_mtxInputBuffer;
	voCMutex					m_mtxOutputBuffer;

protected:
	CAdsp						m_Adsp;
	VO_PMEM						m_sPMem;

	VO_U32						m_nInputBuffers;
	QcomVdec_BufferInfo*		m_pInputBuffers;

	VO_U32						m_nOutputBufferAllocatedLength;
	VO_U32						m_nOutputBuffers;
	QcomVdec_FrameInfo*			m_pOutputBuffers;
};

#endif // __CQcomVideoDec_H__
