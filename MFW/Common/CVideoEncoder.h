	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoEncoder.h

	Contains:	CVideoEncoder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CVideoEncoder_H__
#define __CVideoEncoder_H__

#include "voVideo.h"
#include "CBaseNode.h"

class CVideoEncoder : public CBaseNode
{
public:
	CVideoEncoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CVideoEncoder (void);

public:
	virtual VO_U32		Init ();
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		Process (VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_VIDEO_ENCAPI				m_funEnc;
	VO_HANDLE					m_hEnc;

	VO_U32						m_nCoding;
};

#endif // __CVideoEncoder_H__
