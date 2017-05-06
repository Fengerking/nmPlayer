	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioMockDecoder.h

	Contains:	CAudioMockDecoder header file

	Written by:	Lin Jun

	Change History (most recent first):
	2010-12-04		Lin Jun			Create file

*******************************************************************************/

#ifndef __CAudioMockDecoder_H__
#define __CAudioMockDecoder_H__

#include "CAudioDecoder.h"
#include "CAudioQueueService.h"

class CAudioMockDecoder : public CAudioDecoder
{
public:
	CAudioMockDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioMockDecoder (void);

	virtual void		SetFourCC (VO_U32 nFourCC) {m_nFourCC = nFourCC;}

	virtual VO_U32		Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		GetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	virtual VO_U32		UpdateParam (void);
	virtual VO_U32		SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_U32		HandleFirstSample (VO_CODECBUFFER * pInput);
	
protected:
	VO_PBYTE			m_pLastOutputBuf;
	VO_BOOL				m_bPause;
	
	CAudioQueueService*	m_pAQS;
};

#endif // __CAudioMockDecoder_H__
