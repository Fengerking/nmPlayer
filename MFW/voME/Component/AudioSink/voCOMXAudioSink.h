	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioSink.h

	Contains:	voCOMXAudioSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXAudioSink_H__
#define __voCOMXAudioSink_H__

#include <voOMX_Index.h>

#include <voCOMXCompSink.h>
#include <voCOMXPortClock.h>

#include "CBaseConfig.h"
#include "CAudioRender.h"

class voCOMXAudioSink : public voCOMXCompSink
{
public:
	voCOMXAudioSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioSink(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);
	virtual OMX_ERRORTYPE	CreateRender (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	voCOMXPortClock *				m_pClockPort;

	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nBits;

	OMX_BOOL						m_bSetThreadPriority;
	OMX_S32							m_nOffsetTime;

	OMX_U32							m_nCopyBuffer;
	OMX_BUFFERHEADERTYPE			m_buffRender;
	OMX_U8 *						m_pDataBuffer;
	OMX_U32							m_nDataSize;
	OMX_U32							m_nDataFill;
	OMX_U32							m_nDataStep;
	OMX_U32							m_nDataTime;

	OMX_S64							m_llRenderBufferTime;
	OMX_S64							m_llRenderSystemTime;
	OMX_S32							m_nRenderAdjustTime;
	OMX_S32							m_nRenderDelay;

	OMX_PTR							m_pDumpInputFile;

protected:
	OMX_VO_CHECKRENDERBUFFERTYPE	m_cbRenderBuffer;
	CAudioRender *					m_pAudioRender;
	VO_MEM_OPERATOR *				m_pMemOP;

	VO_U32							m_nBufferTime;

	CBaseConfig *					m_pCfgVOME;
	OMX_BOOL						m_bThread;
	OMX_BOOL						m_bFixedSampleRate;
	OMX_BOOL						m_bVideoFirstOutput;
	OMX_BOOL						m_bSupportMultiChannel;

	//Rogine add for MFW Self-test
	OMX_S32*						m_pARTimeStamp;
};

#endif //__voCOMXAudioSink_H__
