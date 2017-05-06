	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataOutputPort.h

	Contains:	voCOMXDataOutputPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXDataOutputPort_H__
#define __voCOMXDataOutputPort_H__

#include "voCOMXPortSource.h"
#include "voCOMXDataBuffer.h"
#include "CBaseConfig.h"
#include "voLiveSource.h"

class voCOMXPortClock;
class voCOMXDataOutputPort : public voCOMXPortSource
{
public:
	voCOMXDataOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXDataOutputPort(void);

public:
	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual	void			SetConfigFile (CBaseConfig * pCfgFile);
	virtual OMX_ERRORTYPE	SetClockPort (voCOMXPortClock * pClock);

	virtual OMX_ERRORTYPE	GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);
	virtual OMX_ERRORTYPE	SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);

	virtual OMX_ERRORTYPE	ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE	SendBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_S32			GetBufferTime (void);

	virtual OMX_ERRORTYPE	SetTrackPos (OMX_S64 * pPos);

	virtual OMX_ERRORTYPE	Flush (void);
	virtual OMX_ERRORTYPE   FlushBuffer(void);

	virtual OMX_BOOL		IsEOS (void) {return m_bEOS;}
	virtual OMX_BOOL		IsDecOnly (void) {return m_bDecOnly;}
	virtual void			SetOtherPort (voCOMXDataOutputPort * pOtherPort) {m_pOtherPort = pOtherPort;}

	virtual OMX_ERRORTYPE   SetTrackInfo(VO_SOURCE_TRACKINFO* pti);

	virtual OMX_ERRORTYPE	GetSeekKeyFrame(OMX_TICKS * pnTimeStamp);
	virtual void			SetBufferFull(OMX_BOOL bBufferFull) {m_bBufferFull = bBufferFull;}
	virtual void			SetSeekPos(OMX_TICKS nSeekPos) {m_nSeekPos = nSeekPos;}

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual OMX_ERRORTYPE	ReconfigPort (VO_LIVESRC_TRACK_INFOEX * pInfoEx) {return OMX_ErrorNotImplemented;}

protected:
	voCOMXDataOutputPort *	m_pOtherPort;
	voCOMXPortClock *		m_pClockPort;
	OMX_TICKS				m_llLastClockTime;

	voCOMXDataBufferList *	m_pLstBuffer;
	OMX_S64					m_llMaxBufferTime;
	OMX_S64					m_llMinBufferTime;

	OMX_S32					m_nFillBufferCount;

	OMX_BOOL				m_bEOS;
	OMX_BOOL				m_bDecOnly;

	CBaseConfig *			m_pCfgComponent;
	OMX_PTR					m_hSourceFile;
	OMX_S32					m_nSourceFormat;
	OMX_S32					m_nSourceSize;
	OMX_U8 *				m_pSourceBuffer;
	OMX_U8 *				m_pCurrentBuffer;
	OMX_TICKS				m_llSourceSampleTime;
	OMX_U32					m_nSyncWord;

	OMX_PTR					m_hDumpFile;
	OMX_S32					m_nDumpFormat;
	OMX_U32					m_nLogLevel;
	OMX_U32					m_nLogSysStartTime;
	OMX_U32					m_nLogSysCurrentTime;

	VO_SOURCE_TRACKINFO		mTrackInfo;
	OMX_BOOL				m_bBufferFull;
	OMX_TICKS				m_nSeekPos;
};

#endif //__voCOMXDataOutputPort_H__
