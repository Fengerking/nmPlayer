/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CCutEndTask.cpp

Contains:	CCutEndTask class file

Written by:	   Yu Wei 

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#include "CCutEndTask.h"

#include "voLog.h"
#include "../../../Android/jni/source/base/vojnilog.h"


#pragma warning (disable : 4996)


#define LOG_TAG "CCutEndTask"

CCutEndTask::CCutEndTask()
{	

}

CCutEndTask::~CCutEndTask ()
{

}

VO_U32 CCutEndTask::CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo)
{
	return 0;
}
