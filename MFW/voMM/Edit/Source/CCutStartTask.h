/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseTask.h

Contains:	CBaseTask header file

Written by:	 Yu Wei

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#ifndef __CCutStartTask_H__
#define __CCutStartTask_H__


#include "CTask.h"

class CCutStartTask : public CTask
{
protected:


public:	
	CCutStartTask ();
	virtual ~CCutStartTask (void);

	virtual  VO_U32 CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo);


};

#endif // __CCutStartTask_H__
