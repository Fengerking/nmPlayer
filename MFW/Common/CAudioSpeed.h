	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2013		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioSpeed.h

	Contains:	CAudioSpeed header file

	Written by:	Rogine Xu

	Change History (most recent first):
	2013-05-21		Rogine Xu		Create file

*******************************************************************************/

#ifndef __CAudioSpeed_H__
#define __CAudioSpeed_H__

#include "voAudioSpeed.h"
#include "CBaseNode.h"

#define AUDIO_SPEED_UNIT_SZ (40*1024)

class CAudioSpeed : public CBaseNode
{
public:
	CAudioSpeed (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioSpeed(void);

	virtual VO_U32		Init(VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit();	

	virtual VO_U32		Convert (const VO_U8 *pSrc, VO_U32 nInLen, VO_U8 *pOut, VO_U32 *pOutLen);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

protected:
	virtual VO_U32		LoadLib(VO_HANDLE hInst);
	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);	
	virtual VO_U32		GetOutputData (VO_CODECBUFFER *pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat);

protected:
	VO_AUDIO_CODECAPI	m_funSpeed;
	VO_HANDLE			m_hSpeed;

	VO_CODECBUFFER *	m_pInputData;
	VO_U32				m_nInputUsed;

	VO_AUDIO_FORMAT		m_fmtAudio;
	VO_AUDIO_OUTPUTINFO	m_OutputInfo;

	//VO_U8				m_OutputBuf[AUDIO_SPEED_UNIT_SZ];
	
	float				m_nCurrSpeed;
};

#endif //__CAudioSpeed_H__


