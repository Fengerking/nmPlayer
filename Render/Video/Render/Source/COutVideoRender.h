	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COutVideoRender.h

	Contains:	COutVideoRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __COutVideoRender_H__
#define __COutVideoRender_H__

#include "CBaseVideoRender.h"

class COutVideoRender : public CBaseVideoRender
{
public:
	// Used to control the image drawing
	COutVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~COutVideoRender (void);

	virtual VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 	WaitDone (void);
	virtual VO_U32 	Redraw (void);

protected:
	virtual VO_U32	UpdateSize (void);
	virtual bool	CreateCCRRR (void);
};

#endif // __COutVideoRender_H__
