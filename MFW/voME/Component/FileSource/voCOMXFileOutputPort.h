	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileOutputPort.h

	Contains:	voCOMXFileOutputPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXFileOutputPort_H__
#define __voCOMXFileOutputPort_H__

#include "voCOMXPortSource.h"
#include "CBaseConfig.h"
#include "CBaseSource.h"

class voCOMXFileOutputPort : public voCOMXPortSource
{
public:
	voCOMXFileOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXFileOutputPort(void);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);

	virtual OMX_ERRORTYPE	SetTrack (CBaseSource * pSource, OMX_S32 nTrackIndex);

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
	virtual void			SetVideoPort (voCOMXFileOutputPort * pVideoPort) {m_pVideoPort = pVideoPort;}
	virtual void			SetBuffering (OMX_BOOL bBuffering) {m_bHadBuffering = bBuffering;}
	virtual void			SetLoadMutex (voCOMXThreadMutex * pMutex) {m_pLoadMutex = pMutex;}
	virtual void			SetSeekMode (OMX_TIME_SEEKMODETYPE nSeekMode) {m_nSeekMode = nSeekMode;}

	virtual OMX_ERRORTYPE	FillBufferOne (OMX_BUFFERHEADERTYPE * pBuffer);

	virtual void			SetSourceConfig (CBaseConfig * pConfig);
	virtual void			DumpLog (void);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	CBaseSource *			m_pSource;
	voCOMXThreadMutex *		m_pLoadMutex;
	voCOMXFileOutputPort *	m_pVideoPort;
	OMX_S32					m_nTrackIndex;
	VO_SOURCE_TRACKINFO		m_trkInfo;
	OMX_U32					m_nFourCC;

	OMX_TIME_SEEKMODETYPE	m_nSeekMode;
	OMX_S64					m_nFirstPos;
	OMX_S64					m_nStartPos;
	OMX_S64					m_nCurrentPos;
	OMX_BOOL				m_bEOS;
	OMX_BOOL				m_bDecOnly;
	OMX_BOOL				m_bHadBuffering;
	
	VO_SOURCE_SAMPLE		m_Sample;
	OMX_S64					m_nSeekPos;

	CBaseConfig *			m_pConfig;
	VO_PTR					m_hDumpFile;
	OMX_U32					m_nLogLevel;
	OMX_U32					m_nLogSysStartTime;
	OMX_U32					m_nLogSysCurrentTime;

	OMX_U32					m_nLogFrameIndex;
	OMX_U32					m_nLogFrameSize;
	OMX_U32	*				m_pLogMediaTime;
	OMX_U32	*				m_pLogSystemTime;
	OMX_U32	*				m_pLogFrameSize;
	OMX_U32	*				m_pLogFrameSpeed;

	OMX_PTR					m_hDataSourceFile;

};

#endif //__voCOMXFileOutputPort_H__
