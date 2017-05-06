/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		vome2CBaseRender.h

Contains:	vome2CBaseRender header file

Written by:	Tom Yu Wei 

Change History (most recent first):
2010-08-31		Tom Yu			Create file

*******************************************************************************/

#ifndef __vome2CBaseRender_h__
#define __vome2CBaseRender_h__

//#include <sys/types.h>

#include "vompCEngine.h"


typedef enum{
	VOME2_THREAD_INIT				= 0,	/*!<The status is init */
	VOME2_THREAD_STOPPED			= 1,	/*!<The status is stopped */
	VOME2_THREAD_RUNNING			= 2,	/*!<The status is running */
	VOME2_THREAD_PAUSED				= 3,	/*!<The status is paused */
}VOME2_THREAD_STATUS;

class vome2CBaseRender
{
public:
	vome2CBaseRender(vompCEngine * pEngine);
	virtual	~vome2CBaseRender(void);

	virtual int			Start (void);
	virtual int			Pause (void);
	virtual int			Stop (void);

	virtual int			Flush (void);

	virtual int			SetPos (int nPos);
    
    virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);

	static	int			vome2RenderThreadProc (void * pParam);
	virtual	int			vome2RenderThreadLoop (void);
	virtual	void		vome2RenderThreadStart (void);
	virtual	void		vome2RenderThreadExit (void);

	virtual int			SetSurface (void* hWnd){return -1;};
	virtual int			SetColorType (VOMP_COLORTYPE nColor){return -1;};
	virtual int			SetDrawRect(int nLeft, int nTop, int nRight, int nBottom){return -1;};
	
protected:
	vompCEngine *			m_pEngine;

	char					m_szThreadName[128];
	VOME2_THREAD_STATUS		m_bThreadStatus;
	bool					m_bThreadExit;
};


#endif //#define __vome2CBaseRender_h__

