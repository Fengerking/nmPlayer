	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFLOGraph.h

	Contains:	voCOMXFLOGraph header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXFLOGraph_H__
#define __voCOMXFLOGraph_H__

#include "voFLOEngine.h"
#include "voCOMXBaseGraph.h"

class voCOMXFLOGraph : public voCOMXBaseGraph
{
	static voCOMXFLOGraph * g_pFLOGraph;

public:
    voCOMXFLOGraph(void);
    virtual ~voCOMXFLOGraph(void);

	virtual OMX_ERRORTYPE	Init (OMX_PTR hInst);

	virtual OMX_ERRORTYPE 	Playback (VOME_SOURCECONTENTTYPE * pSource);
	virtual OMX_ERRORTYPE 	Close (void);

	virtual OMX_ERRORTYPE 	Run (void);
	virtual OMX_ERRORTYPE 	Pause (void);
	virtual OMX_ERRORTYPE 	Stop (void);

	virtual OMX_ERRORTYPE 	GetDuration (OMX_S32 * pDuration);
	virtual OMX_ERRORTYPE 	GetCurPos (OMX_S32 * pCurPos);
	virtual OMX_ERRORTYPE 	SetCurPos (OMX_S32 nCurPos);

protected:
	virtual OMX_U32	voGraphMessageHandle (void);

protected:
	OMX_COMPONENTTYPE *		m_pCompDS;
	OMX_COMPONENTTYPE *		m_pCompVD;

protected:
	OMX_ERRORTYPE		CreateFLOEngine();
	OMX_ERRORTYPE		DestroyFLOEngine();

protected:
	static VO_S32 OnEvent(int nEventType, unsigned int nParam, void * pData);
	static VO_S32 OnFrame(VO_FLOENGINE_FRAME * pFrame);

private:
#ifdef _WIN32
	HMODULE		m_libFLOEngine;
#else
	OMX_PTR		m_libFLOEngine;
#endif // _WIN32

	VO_HANDLE           m_hFLOEngine;
	VO_FLOENGINE_API    m_FLOEngineAPI;

	VO_FLOENGINE_CALLBACK m_FLOEngineCallBack;

private:
	char    m_szSource[1024];
};

#endif //__voCOMXFLOGraph_H__