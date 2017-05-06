	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioDec.h

	Contains:	voCOMXAudioDec header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXAudioDec_H__
#define __voCOMXAudioDec_H__

#include "voOMX_Index.h"
#include "CAudioDecoder.h"
#include "voCOMXCompFilter.h"

class voCOMXAudioDec : public voCOMXCompFilter
{
public:
	voCOMXAudioDec(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioDec(void);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	GetConfig (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE	SetConfig (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);
	virtual VO_U32			GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat, VO_U32 nStart);

protected:
	OMX_AUDIO_CODINGTYPE			m_nCoding;
	OMX_PTR							m_pFormatData;
	OMX_U32							m_nExtSize;
	OMX_S8 *						m_pExtData;

	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nBits;
	OMX_U32							m_nStereoMode;

	OMX_BOOL						m_bSetThreadPriority;

	OMX_U32							m_nPassableError;
	OMX_U32							mnSourceType;

protected:
	CAudioDecoder *					m_pAudioDec;
	VO_MEM_OPERATOR *				m_pMemOP;
	VO_U32							m_nCodec;

	VO_CODECBUFFER					m_inBuffer;
	VO_CODECBUFFER					m_outBuffer;

	VO_U32							m_nStepSize;
	VO_S64							m_nOutputSize;
	VO_BOOL							m_bNofityError;

	CBaseConfig *					m_pCfgCodec;
};

#endif //__voCOMXAudioDec_H__
