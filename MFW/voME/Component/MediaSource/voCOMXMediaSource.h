	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaSource.h

	Contains:	voCOMXMediaSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXMediaSource_H__
#define __voCOMXMediaSource_H__

#include <OMX_ContentPipe.h>
#include <voOMX_Index.h>
#include <vomeFilePipe.h>

#include "voOMX_IMediaSource.h"
#include "voCOMXCompSource.h"

#include "CBaseConfig.h"

class voCOMXMediaAudioPort;
class voCOMXMediaVideoPort;
class voCOMXPortClock;

class voCOMXMediaSource : public voCOMXCompSource
{
public:
	voCOMXMediaSource(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXMediaSource(void);

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

	virtual OMX_ERRORTYPE FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
protected:
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_U32			MessageHandle (COMP_MESSAGE_TYPE * pTask);

	virtual OMX_ERRORTYPE	CloseSource (void);

protected:
	virtual OMX_U32			DumpPerformanceInfo (OMX_STRING pFile);

public: // IMediaSource
	virtual OMX_ERRORTYPE NewTrack (OMX_S32 nIndex, VO_SOURCE_TRACKINFO *pTrackInfo);
	virtual OMX_ERRORTYPE SendData (OMX_S32 nIndex, VO_SOURCE_SAMPLE *pSample);

protected:
	VOOMX_IMEDIASOURCE *        m_pIMediaSource;

protected:
	voCOMXMediaAudioPort *		m_pAudioPort;
	voCOMXMediaVideoPort *		m_pVideoPort;
	voCOMXPortClock *			m_pClockPort;

	OMX_VO_FILE_OPERATOR *		m_pPipe;
	//OMX_S64						m_nOffset;
	//OMX_S64						m_nLength;

	OMX_TIME_SEEKMODETYPE		m_nSeekMode;

protected:
	OMX_S32						m_nAudioTrack;
	OMX_S32						m_nVideoTrack;

	VO_MEM_OPERATOR *			m_pMemOP;
	VO_FILE_OPERATOR *			m_pFileOP;
	VO_FILE_OPERATOR			m_FileOP;

	CBaseConfig *				m_pCfgSource;

	OMX_BOOL					m_bBufferThread;

};

#endif //__voCOMXMediaSource_H__
