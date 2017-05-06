#include "utility.h"
#include "network.h"
#include "TaskSchedulerEngine.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const long MILLION = 1000000;

//-------------------------- TaskFunction

long CTask::m_taskTokenGenerator = 0;

CTask::CTask(long timeInterval, TaskFunction * taskFunc, void * taskParam)
:m_interval(timeInterval)
,m_taskFunc(taskFunc)
, m_taskParam(taskParam)
{
	m_next = NULL;
	m_prev = NULL;
	m_taskToken = ++m_taskTokenGenerator;
	m_launchTime=timeGetTime()+m_interval;


}
	
CTask::~CTask()
{
}

void CTask::HandleTask()
{
	(*m_taskFunc)(m_taskParam);
}

// -----------------------  CTaskQueue

CTaskQueue::CTaskQueue()
{
	m_head = NULL;
	m_tail = NULL;
	m_taskNum = 0;
	m_tvLastSyncTime = timeGetTime();
}

CTaskQueue::~CTaskQueue()
{
	RemoveAllTask();
}

void CTaskQueue::RemoveAllTask()
{
	CTask * task = NULL;
	while(m_head != NULL)
	{
		task = m_head;
		m_head = m_head->m_next;
		SAFE_DELETE(task);
	}

	m_head = NULL;
	m_tail = NULL;
	m_tvLastSyncTime = timeGetTime();
	CTask::m_taskTokenGenerator = 0;
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","RemoveAllTask()\n");
}

void CTaskQueue::AddTask(CTask * newTask)
{
	if(m_head == NULL)
	{
		m_head = newTask;
		m_tail = newTask;
	}
	else
	{
		m_tail->m_next = newTask;
		newTask->m_prev = m_tail;
		newTask->m_next = NULL;
		m_tail = newTask;
	}
	m_taskNum++;
	sprintf(CLog::formatString,"new Task=%X\n",(unsigned int)newTask);
	CLog::Log.MakeLog(LL_RTSP_ERR,"task.txt",CLog::formatString);
}

void CTaskQueue::RemoveTask(CTask * task)
{
	if(task == NULL)
		return;

	if(task == m_head && task == m_tail)
	{
		m_head = NULL;
		m_tail = NULL;
	}
	else if(task == m_head)
	{
		task->m_next->m_prev = NULL;
		m_head = task->m_next;
	}
	else if(task == m_tail)
	{
		task->m_prev->m_next = NULL;
		m_tail = task->m_prev;
	}
	else
	{
		task->m_prev->m_next = task->m_next;
		task->m_next->m_prev = task->m_prev;
	}

	task->m_prev = NULL;
	task->m_next = NULL;
	m_taskNum--;

	sprintf(CLog::formatString,"remove Task=%X\n",(unsigned int)task);
	CLog::Log.MakeLog(LL_RTSP_ERR,"task.txt",CLog::formatString);
}

CTask * CTaskQueue::RemoveTask(long taskToken)
{
	CTask * task = SearchTaskByToken(taskToken);
	RemoveTask(task);
	return task;
}

void CTaskQueue::UpdateTask(CTask * task, long newTimeInterval)
{
	if(task == NULL) 
		return;
	task->m_launchTime = timeGetTime()+newTimeInterval;
	task->m_interval   = newTimeInterval;
}

void CTaskQueue::UpdateTask(long taskToken, long newTimeInterval)
{
	CTask * task = SearchTaskByToken(taskToken);
	UpdateTask(task, newTimeInterval);
}

void CTaskQueue::HandleTask()
{
	if(m_head == NULL)
		return;
	Synchronize();
	/*
	if(m_head->m_launchTime != ZERO_INTERVAL) 
	{
		Synchronize();
	}

	if(m_head->m_launchTime == ZERO_INTERVAL)
	{
		CTask * task = m_head;
		//RemoveTask(task);
		task->HandleTask();
		//SAFE_DELETE(task);
		long timeInterval(3600, 0);
		UpdateTask(task,timeInterval);
	}
	*/
}

void CTaskQueue::Synchronize()
{
	if(m_head == NULL)
		return;

	long nowTime = timeGetTime();
	m_tvLastSyncTime = nowTime;

	CTask * task = m_head;
	while(task != NULL )
	{
		if(nowTime >= task->GetLaunchTime())
		{
			sprintf(CLog::formatString,"task=%ld,launch=%ld,go?=%d\n",task->m_taskToken,task->m_launchTime,nowTime >= task->GetLaunchTime());	
			CLog::Log.MakeLog(LL_RTSP_ERR,"task.txt",CLog::formatString);
		}
		
		if(nowTime >= task->GetLaunchTime())
		{

			task->HandleTask();
			task->UpdateLaunchTime();//update to next with interval
		}
		task = task->m_next;
	}
}

CTask * CTaskQueue::SearchTaskByToken(long taskToken)
{
	CTask * task = m_head;
	while(task != NULL)
	{
		if(task->TaskToken() == taskToken)
			break;

		task = task->m_next;
	}

	return task;
}

// ------------------    CTaskSchedulerEngine

CTaskSchedulerEngine * CTaskSchedulerEngine::m_taskSchedulerEngine = NULL;

CTaskSchedulerEngine * CTaskSchedulerEngine::CreateTaskSchedulerEngine()
{
	if(m_taskSchedulerEngine == NULL)
	{
		m_taskSchedulerEngine = new CTaskSchedulerEngine();TRACE_NEW("rtss_m_taskSchedulerEngine",m_taskSchedulerEngine);
		sprintf(CLog::formatString,"&m_taskSchedulerEngine =%X,value=%X\n",(unsigned int)&m_taskSchedulerEngine,(unsigned int)m_taskSchedulerEngine);
		CLog::Log.MakeLog(LL_RTSP_ERR,"task.txt",CLog::formatString);
	}

	return m_taskSchedulerEngine;
}

void CTaskSchedulerEngine::DestroyTaskSchedulerEngine()
{
	if(m_taskSchedulerEngine != NULL)
	{
		SAFE_DELETE(m_taskSchedulerEngine);
		m_taskSchedulerEngine = NULL;
	}
}

CTaskSchedulerEngine::CTaskSchedulerEngine()
{
	m_taskQueue = new CTaskQueue();TRACE_NEW("rtss_m_taskQueue",m_taskQueue);
}

CTaskSchedulerEngine::~CTaskSchedulerEngine()
{
	if(m_taskQueue != NULL)
	{
		SAFE_DELETE(m_taskQueue);
		m_taskQueue = NULL;
	}
}
void CTaskSchedulerEngine::UpdateTask(long taskToken,long millionSeconds)
{
	if(millionSeconds < 0) 
		millionSeconds = 0;
	m_taskQueue->UpdateTask(taskToken,millionSeconds);
}

long CTaskSchedulerEngine::ScheduleTask(long millionSeconds, TaskFunction * taskProc, void * taskParam,long taskToken)
{
	if(millionSeconds < 0) 
		millionSeconds = 0;

	CTask * newTask = NULL;
	if(taskToken>0)
	{
		newTask = m_taskQueue->SearchTaskByToken(taskToken);
	}

	if(newTask==NULL)
	{
		CTask * newTask = new CTask(millionSeconds, taskProc, taskParam);TRACE_NEW("rtss_CTask",newTask);
		m_taskQueue->AddTask(newTask);
		return newTask->TaskToken();
	}
	else
	{
		m_taskQueue->UpdateTask(newTask,millionSeconds);
		return taskToken;
	}
	
}

void CTaskSchedulerEngine::UnscheduleTask(long taskToken)
{
	CTask * task = m_taskQueue->RemoveTask(taskToken);
	SAFE_DELETE(task);
}

void CTaskSchedulerEngine::UnscheduleAllTask()
{
	m_taskQueue->RemoveAllTask();
}

void CTaskSchedulerEngine::HandleTask()
{
	m_taskQueue->HandleTask();
}
