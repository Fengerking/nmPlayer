	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPSource.h

	Contains:	voCOMXRTSPSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXRTSPSource_H__
#define __voCOMXRTSPSource_H__

#include <OMX_ContentPipe.h>

#include <voOMX_Index.h>

#include <voCOMXCompSource.h>
#include "voCOMXRTSPAudioPort.h"
#include "voCOMXRTSPVideoPort.h"
#include <voCOMXPortClock.h>

#include "CFileSource.h"

class voCOMXRTSPSource : public voCOMXCompSource
{
public:
	voCOMXRTSPSource(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXRTSPSource(void);

	virtual OMX_ERRORTYPE GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE SendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_COMMANDTYPE Cmd,
										OMX_IN  OMX_U32 nParam1,
										OMX_IN  OMX_PTR pCmdData);

	virtual OMX_ERRORTYPE FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

protected:
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);

	virtual OMX_ERRORTYPE	LoadFile (OMX_PTR pSource, OMX_U32 nType);
	virtual OMX_ERRORTYPE	CloseFile (void);
	virtual OMX_ERRORTYPE	ForceCloseSource (void);


protected:
	voCOMXRTSPAudioPort *		m_pAudioPort;
	voCOMXRTSPVideoPort *		m_pVideoPort;
	voCOMXPortClock *			m_pClockPort;
	voCOMXThreadMutex			m_tmLoadSource;

	OMX_U32						m_nURISize;
	OMX_S8 *					m_pURI;
	OMX_S64						m_nOffset;
	OMX_S64						m_nLength;
	OMX_BOOL					m_bForceClose;

	OMX_TIME_SEEKMODETYPE		m_nSeekMode;
	OMX_S64						m_llStartPos;

	CFileSource *				m_pSource;
	OMX_S32						m_nAudioTrack;
	OMX_S32						m_nVideoTrack;

protected:
	static int				voomxFileSourceStreamCallBack (long EventCode, long * EventParam1, long * userData);
};

#endif //__voCOMXRTSPSource_H__
