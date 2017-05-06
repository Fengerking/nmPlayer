	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileSink.h

	Contains:	voCOMXFileSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXFileSink_H__
#define __voCOMXFileSink_H__

#include "voCOMXBaseComponent.h"
#include "CFileSink.h"

class voCOMXFileSink : public voCOMXBaseComponent
{
public:
	voCOMXFileSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXFileSink(void);

public:
	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	FreeBuffer (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_U32 nPortIndex, 
										OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32 nPort);
	virtual OMX_ERRORTYPE	Disable (OMX_U32 nPort);

	inline	void			ReturnKeptBuffers (void);

protected:
	CFileSink *					m_pFileSink;
	OMX_VO_FILE_FORMATTYPE		m_nFileType;
	OMX_U32						m_nURISize;
	OMX_S8 *					m_pURI;		   /* m_Source.pSource, m_Source.nFlag = VO_FILE_TYPE_NAME; m_Source.nReserve */
	VO_FILE_SOURCE				m_Source;
	VO_MEM_OPERATOR *			m_pMemOP;
	VO_FILE_OPERATOR *			m_pFileOP;
	VO_FILE_OPERATOR			m_FileOP;

	OMX_S64                     m_nMaxDuration;
	OMX_U32                     m_Start_ms;
	OMX_S64                     m_nMaxSize;
	OMX_S64                     m_nWriteSize;
	OMX_BOOL					m_bFinished;

	OMX_U32						m_nMaxAVDiffTime;		// <ms>

	// audio port
	voCOMXBasePort *			m_pAudioPort;
	OMX_AUDIO_CODINGTYPE		m_nAudioCoding;
	OMX_BOOL					m_bAudioEOS;
	OMX_U32						m_nAudioExtSize;
	OMX_S8 *					m_pAudioExtData;
	OMX_TICKS					m_nCurrAudioTimeStamp;
	OMX_BUFFERHEADERTYPE *		m_pAudioKeptBuffer;

	OMX_AUDIO_PARAM_PCMMODETYPE	m_pcmType;

	// video port
	voCOMXBasePort *			m_pVideoPort;
	OMX_VIDEO_CODINGTYPE		m_nVideoCoding;
	OMX_BOOL					m_bVideoEOS;
	OMX_U32						m_nVideoExtSize;
	OMX_S8 *					m_pVideoExtData;
	OMX_TICKS					m_nCurrVideoTimeStamp;
	OMX_BUFFERHEADERTYPE *		m_pVideoKeptBuffer;

	OMX_BOOL					mbIntraDetectIF;

	voCOMXThreadMutex			m_tmKeptBuffer;
};

#endif //__voCOMXFileSink_H__
