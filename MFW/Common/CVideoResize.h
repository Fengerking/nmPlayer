	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoResize.h

	Contains:	CVideoResize header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CVideoResize_H__
#define __CVideoResize_H__

#include "CBaseNode.h"
#include "voColorConversion.h"

class CVideoResize : public CBaseNode
{
public:
	CVideoResize(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CVideoResize();

public:
	virtual VO_U32		Init();
	virtual VO_U32		Uninit();

	virtual VO_U32		Process(VO_VIDEO_FORMAT * pInputFormat, VO_VIDEO_BUFFER * pInput, VO_VIDEO_FORMAT * pOutputFormat, VO_VIDEO_BUFFER * pOutput);

	virtual VO_U32		SetParam(VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam(VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib(VO_HANDLE hInst);

protected:
	VO_CLRCONV_DECAPI		m_funResize;
	VO_HANDLE				m_hResize;

	ClrConvData				m_ccData;
};

#endif // __CVideoResize_H__
