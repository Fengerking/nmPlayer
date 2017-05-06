/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseController.h

Contains:	CBaseController header file

Written by:	 Yu Wei

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#ifndef __CBaseController_H__
#define __CBaseController_H__

#include "CCutStartTask.h"
#include "CCutEndTask.h"


class CBaseController 
{
protected:
	VOEDITMARK    m_CutStart1;
	VOEDITMARK    m_CutEnd1;

	VOEDITMARK    m_CutStart2;
	VOEDITMARK    m_CutEnd2;

	VOEDITMARK    m_CutStart3;
	VOEDITMARK    m_CutEnd3;

	CBaseTask *   m_pTask;

public:	
	CBaseController ();
	virtual ~CBaseController (void);

	virtual VO_U32 CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo);

protected:
	virtual CBaseTask*  GetNextTask();
	virtual VOEDITMARK* GetNextMark();

	virtual void        OnOneTaskFinished();
	virtual void        OnAllTaskFinsihed();
};

#endif // __CBaseController_H__
