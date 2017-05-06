	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioEncoder.h

	Contains:	CAudioEncoder header file

	Written by:	Bangfei Jin
	Modify by:    Leon Huang

	Change History (most recent first):
	2008-04-30		JBF			Create file
	2011-01-20     Leon         Add CallBack func

*******************************************************************************/

#ifndef __CAudioEncoder_H__
#define __CAudioEncoder_H__

#include <voAudio.h>
#include "CBaseNode.h"

#include "videoEditorType.h"
class CAudioEncoder : public CBaseNode
{
public:
	CAudioEncoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioEncoder (void);

	virtual VO_U32		Init ();
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioInfo);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

	VO_S32  SetEncParam(VOEDT_OUTPUT_AUDIO_PARAM *param);
protected:
	virtual VO_U32		UpdateParam (void);

protected:
	VO_AUDIO_CODECAPI	 m_funEnc;
	VO_HANDLE			m_hEnc;

	VO_U32				m_nCoding;

	VO_CODECBUFFER *	m_pInputData;
	VO_U32				m_nInputUsed;

	VO_AUDIO_OUTPUTINFO	m_OutputInfo;

	VOEDT_OUTPUT_AUDIO_PARAM m_outputParam;
};

#endif // __CAudioEncoder_H__
