/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		vome2CVideoRender.h

	Contains:	vome2CVideoRender header file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2010-09-25		Bang			Create file

*******************************************************************************/

#ifndef __vome2CVideoRender_h__
#define __vome2CVideoRender_h__

//#include <sys/types.h>
//#include <utils/Errors.h>

#include "CBaseVideoRender.h"
#include "vome2CBaseRender.h"
#include "voCMutex.h"

// ----------------------------------------------------------------------------
class vome2CVideoRender : public vome2CBaseRender
{
public:
	vome2CVideoRender(vompCEngine * pEngine);
	virtual	~vome2CVideoRender(void);

	virtual int			SetSurface (void* hWnd);

	virtual int			Start (void);
    virtual int			Stop (void);
    
	virtual int			SetPos (int nPos);

	virtual int			SetColorType (VOMP_COLORTYPE nColor);
	virtual int			SetDrawRect(int nLeft, int nTop, int nRight, int nBottom);
	
    virtual int 		GetParam (int nID, void * pValue);
    
	virtual	int			vome2RenderThreadLoop (void);
	virtual	void		vome2RenderThreadStart (void);
	virtual	void		vome2RenderThreadExit (void);

protected:
	voCMutex				m_mtStatus;
    //Mutex					m_mtStatus;

	void*					m_pSurface;
	VOMP_VIDEO_FORMAT		m_sVideoFormat;
	int						m_nColorType;
	VOMP_RECT				m_rcDraw;

	bool					m_bSetPos;

//	VOMP_BUFFERTYPE *		m_pVideoBuffer;
	bool					m_bSurfaceNeedChange;
	CBaseVideoRender*		m_pVideoRender;
};

#endif //#define __vome2CVideoRender_h__

