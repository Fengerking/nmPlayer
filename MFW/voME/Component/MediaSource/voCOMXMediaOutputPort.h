	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaOutputPort.h

	Contains:	voCOMXMediaOutputPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXMediaOutputPort_H__
#define __voCOMXMediaOutputPort_H__

#include <voCOMXPortSource.h>
#include "CBaseConfig.h"

#include "SampleStore.h"

using namespace mtp;

class voCOMXMediaOutputPort : public voCOMXPortSource
{
public:
	voCOMXMediaOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXMediaOutputPort(void);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);

	virtual OMX_ERRORTYPE	SetTrackInfo (OMX_S32 nTrackIndex, VO_SOURCE_TRACKINFO *pTrackInfo);

	virtual OMX_ERRORTYPE	GetStreams (OMX_U32 * pStreams);
	virtual OMX_ERRORTYPE	SetStream (OMX_U32 nStream);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);
	virtual OMX_ERRORTYPE	SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);

	virtual OMX_ERRORTYPE	ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	FillBufferOne (OMX_BUFFERHEADERTYPE * pBuffer);

	virtual void			SetSourceConfig (CBaseConfig * pConfig);
	virtual void			DumpLog (void);

public:
	virtual OMX_ERRORTYPE   SendSample(VO_SOURCE_SAMPLE *pSample);	
	OMX_S32					GetTrackIndex() { return m_nTrackIndex; }

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_S32				m_nTrackIndex;
	VO_SOURCE_TRACKINFO	m_trkInfo;
	OMX_U32				m_nFourCC;

	OMX_S64				m_nFirstPos;
	OMX_S64				m_nStartPos;
	OMX_S64				m_nCurrentPos;
	OMX_BOOL			m_bEOS;

	VO_SOURCE_SAMPLE	m_Sample;

	CSampleStore      * m_pSampleStore;

protected:
	CBaseConfig *		m_pConfig;
	VO_PTR				m_hDumpFile;

	OMX_U32				m_nLogFrameIndex;
	OMX_U32				m_nLogFrameSize;
	OMX_U32	*			m_pLogMediaTime;
	OMX_U32	*			m_pLogSystemTime;
	OMX_U32	*			m_pLogFrameSize;
	OMX_U32	*			m_pLogFrameSpeed;
};

#endif //__voCOMXMediaOutputPort_H__