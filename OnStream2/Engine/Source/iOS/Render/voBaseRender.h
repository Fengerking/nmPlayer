/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
*																		*
************************************************************************/
/*******************************************************************************
File:		voBaseRender.h

Contains:	voBaseRender header file

Written by:	Jim

Change History (most recent first):
2011-11-15		Jim Lin			Create file

*******************************************************************************/

#ifndef __voBaseRender_h__
#define __voBaseRender_h__

#include "vompType.h"
#include "voIVCommon.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef enum 
{
	QT_GET_AUDIO_FMT	= 0,
	QT_GET_VIDEO_FMT,
	QT_GET_ADUIO_BUFFER,
	QT_GET_VIDEO_BUFFER,
	QT_RETURN_VIDEO_BUFFER,
	QT_SET_AUDIO_RENDER_BUF_TIME,
    QT_NOTIFY_VIDEO_FORMAT_CHANGE,
    QT_NOTIFY_VIDEO_DISPLAY_CHANGE,
    QT_NOTIFY_AUDIO_RENDER_READY,
}QueryType;

typedef int (* RenderQueryCallback) (void * pUserData, QueryType nID, void * pParam1, void * pParam2);

typedef enum{
	VO_THREAD_INIT				= 0,	/*!<The status is init */
	VO_THREAD_STOPPED			= 1,	/*!<The status is stopped */
	VO_THREAD_RUNNING			= 2,	/*!<The status is running */
	VO_THREAD_PAUSED			= 3,	/*!<The status is paused */
}VO_THREAD_STATUS;

class voBaseRender
{
public:
	voBaseRender(void* pUserData, RenderQueryCallback pQuery);
	virtual	~voBaseRender(void);

	virtual int			Start (void);
	virtual int			Pause (void);
	virtual int			Stop (void);

	static	int			RenderThreadProc (void * pParam);
	virtual	int			RenderThreadLoop (void);
	virtual	void		RenderThreadStart (void);
	virtual	void		RenderThreadExit (void);

    virtual void        SetWorkPath (VO_TCHAR * pWorkPath) {m_pWorkPath = pWorkPath;}
    
protected:
    virtual int			Pause (bool wait);
	virtual int			Stop (bool wait);
    
protected:
	RenderQueryCallback		m_pQuery;
	void*					m_pUserData;

	char					m_szThreadName[128];
	VO_THREAD_STATUS		m_bThreadStatus;
	bool					m_bThreadExit;
    bool					m_bPauseDone;
    
	VO_TCHAR *				m_pWorkPath;
    voCMutex                m_cMutex;
    
#ifdef _VOLOG_INFO
	int                     m_nSample;
    int                     m_nLastTime;
#endif
};


#endif //#define __vome2CBaseRender_h__

