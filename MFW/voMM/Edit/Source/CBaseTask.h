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

#ifndef __CBaseTask_H__
#define __CBaseTask_H__

#include "voType.h"
#include "voSource.h"

typedef enum {
	VO_MARK_TYPE_UNUSED	= 0,	
	VO_MARK_TYPE_START,		
	VO_MARK_TYPE_END
} VO_MARK_TYPE;


typedef struct voeditmark
{	
	VO_U32          type;
	VO_S64			VideoTime;				
	VO_S64			AudioTime;		
} VOEDITMARK;


class CBaseTask 
{
protected:
	VOEDITMARK*    m_pMark; 

	VO_BOOL        m_bVideoFinished;
	VO_BOOL        m_bAudioFinished;

protected:
	virtual  VO_BOOL  CheckFinished();

public:	
	CBaseTask ();
	virtual  ~CBaseTask (void);

	virtual  void   SetMark(VO_PTR in_pMark);

	virtual  VO_U32 CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo);


};

#endif // __CBaseTask_H__
