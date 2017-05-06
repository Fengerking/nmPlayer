	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "cmnMemory.h"
#include "COutVideoRender.h"

COutVideoRender::COutVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
{
}

COutVideoRender::~COutVideoRender ()
{
}

VO_U32 COutVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if (m_fCallBack != NULL)
		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);

	return VO_ERR_NONE;
}

VO_U32 COutVideoRender::WaitDone (void)
{
	return VO_ERR_NONE;
}

VO_U32 COutVideoRender::Redraw (void)
{
	return VO_ERR_NONE;
}

bool COutVideoRender::CreateCCRRR (void)
{
	return true;
}

VO_U32 COutVideoRender::UpdateSize (void)
{
	CBaseVideoRender::UpdateSize ();

	return VO_ERR_NONE;
}


