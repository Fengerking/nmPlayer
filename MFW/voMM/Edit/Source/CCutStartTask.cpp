/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CCutStartTask.cpp

Contains:	CCutStartTask class file

Written by:	   Yu Wei 

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#include "CCutStartTask.h"

#include "voLog.h"
#include "../../../Android/jni/source/base/vojnilog.h"


#pragma warning (disable : 4996)


#define LOG_TAG "CCutStartTask"

CCutStartTask::CCutStartTask()
{	

}

CCutStartTask::~CCutStartTask ()
{

}


VO_U32 CCutStartTask::CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo)
{
	int nReturn = 0;
	int nTime   = pSourceSample->Time & 0x7FFFFFFF;


	if (nIsVideo){

		if (nTime > m_pMark->VideoTime) {
			m_bVideoFinished = VO_TRUE;

			nReturn = 1;

			if (CheckFinished() == VO_TRUE)
				nReturn |= 0x00000002;			

		}
	}
	/*
	if (nTime < m_pMark->)
	{
		if (m_nVideoFirstSample == 0 && nIsVideo == 1) 
			int aa = 3;
		else
			return 0;
	}


	*/


	return nReturn;
}