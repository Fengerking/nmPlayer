/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseController.cpp

Contains:	CBaseController class file

Written by:	   Yu Wei 

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#include "CBaseController.h"

#include "voLog.h"
#include "../../../Android/jni/source/base/vojnilog.h"

#pragma warning (disable : 4996)


#define LOG_TAG "CBaseController"

CBaseController::CBaseController()
: m_pTask(NULL)
{	
	memset(&m_CutStart1, 0, sizeof(VOEDITMARK));
	memset(&m_CutEnd1  , 0, sizeof(VOEDITMARK));
	memset(&m_CutStart2, 0, sizeof(VOEDITMARK));
	memset(&m_CutEnd2  , 0, sizeof(VOEDITMARK));
	memset(&m_CutStart3, 0, sizeof(VOEDITMARK));
	memset(&m_CutEnd3  , 0, sizeof(VOEDITMARK));


	m_CutStart1.type      = VO_MARK_TYPE_START;
	m_CutStart1.AudioTime = 0;
	m_CutStart1.VideoTime = 0;

	m_CutEnd1.type      = VO_MARK_TYPE_END;
	m_CutEnd1.AudioTime = 3000;
	m_CutEnd1.VideoTime = 3000;

	m_CutStart2.type      = VO_MARK_TYPE_START;
	m_CutStart2.AudioTime = 7000;
	m_CutStart2.VideoTime = 7000;

	m_CutEnd2.type      = VO_MARK_TYPE_END;
	m_CutEnd2.AudioTime = 12000;
	m_CutEnd2.VideoTime = 12000;

	m_CutStart3.type      = VO_MARK_TYPE_START;
	m_CutStart3.AudioTime = 16000;
	m_CutStart3.VideoTime = 16000;

	m_CutEnd3.type      = VO_MARK_TYPE_END;
	m_CutEnd3.AudioTime = 99000;
	m_CutEnd3.VideoTime = 99000;


}

CBaseController::~CBaseController ()
{
	if (m_pTask)
		delete m_pTask;
}


CBaseTask* CBaseController::GetNextTask()
{
	VOEDITMARK* pMark = GetNextMark();

	if (pMark != NULL) {
		if (m_pTask) {
			delete m_pTask;
			m_pTask= NULL;
		}

		if (pMark->type == VO_MARK_TYPE_START) {

			CCutStartTask* pTask = new CCutStartTask();
			pTask->SetMark(pMark);
			return pTask;

		}
		else if (pMark->type == VO_MARK_TYPE_END) {

			CCutEndTask* pTask = new CCutEndTask();
			pTask->SetMark(pMark);
			return pTask;

		}	
	}

	return NULL;
}

VOEDITMARK* CBaseController::GetNextMark()
{
	// its just a test. it will be read from the project file.

	VOEDITMARK* pReturn = NULL;

	static int nStatus = 0;

	switch(nStatus)
	{
		case 0:
			pReturn = &m_CutStart1;
			break;

		case 1:
			pReturn = &m_CutEnd1;
			break;

		case 2:
			pReturn = &m_CutStart2;
			break;

		case 3:
			pReturn = &m_CutEnd2;
			break;

		case 4:
			pReturn = &m_CutStart3;
			break;

		case 5:
			pReturn = &m_CutEnd3;
			break;

		case 6:
			break;

	}


	nStatus++;

	return pReturn;
}

void   CBaseController::OnOneTaskFinished()
{
	m_pTask = GetNextTask();

	if (m_pTask == NULL)
		OnAllTaskFinsihed();
	else
	{


	}
}

void   CBaseController::OnAllTaskFinsihed()
{
	JNILOGE("all the tasks has been finished.");
}


VO_U32 CBaseController::CheckSample(VO_SOURCE_SAMPLE* pSourceSample, VO_VIDEO_FRAMETYPE frametype, VO_U32 nIsVideo)
{
	if (m_pTask == NULL) {
		m_pTask = GetNextTask();
		if (m_pTask == NULL)
			return 0;
	}

	int nReturn = 0;
	nReturn = m_pTask->CheckSample(pSourceSample, frametype, nIsVideo);

	if (nReturn & 0x00000002)
		OnOneTaskFinished();


    return nReturn & 0x00000001;
}