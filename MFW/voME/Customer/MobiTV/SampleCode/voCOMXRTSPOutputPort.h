	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPOutputPort.h

	Contains:	voCOMXRTSPOutputPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXRTSPOutputPort_H__
#define __voCOMXRTSPOutputPort_H__

#include <voCOMXPortSource.h>
#include "CFileSource.h"

class voCOMXFileSource;

class voCOMXRTSPOutputPort : public voCOMXPortSource
{
public:
	voCOMXRTSPOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXRTSPOutputPort(void);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);

	virtual OMX_ERRORTYPE	SetTrack (CFileSource * pSource, OMX_S32 nTrackIndex);

	virtual OMX_ERRORTYPE	GetStreams (OMX_U32 * pStreams);
	virtual OMX_ERRORTYPE	SetStream (OMX_U32 nStream);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);
	virtual OMX_ERRORTYPE	SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);

	virtual OMX_ERRORTYPE	ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	SetTrackPos (OMX_S64 * pPos);

	virtual OMX_S64			GetCurrentPos (void) {return m_nCurrentPos;}
	virtual OMX_BOOL		IsEOS (void) {return m_bEOS;}
	virtual OMX_BOOL		IsDecOnly (void) {return m_bDecOnly;}
	virtual void			SetVideoPort (voCOMXRTSPOutputPort * pVideoPort) {m_pVideoPort = pVideoPort;}
	virtual void			SetBuffering (OMX_BOOL bBuffering) {m_bHadBuffering = bBuffering;}
	virtual void			SetLoadMutex (voCOMXThreadMutex * pMutex) {m_pLoadMutex = pMutex;}

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	CFileSource *			m_pSource;
	voCOMXThreadMutex *		m_pLoadMutex;
	voCOMXRTSPOutputPort *	m_pVideoPort;
	OMX_S32					m_nTrackIndex;
	VO_SOURCE_TRACKINFO		m_trkInfo;
	OMX_U32					m_nFourCC;

	OMX_S64					m_nStartPos;
	OMX_S64					m_nCurrentPos;
	OMX_BOOL				m_bEOS;
	OMX_BOOL				m_bDecOnly;
	OMX_BOOL				m_bHadBuffering;
	
	VO_SOURCE_SAMPLE		m_Sample;
};

#endif //__voCOMXRTSPOutputPort_H__
