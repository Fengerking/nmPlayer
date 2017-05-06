#ifndef __TASKSCHEDULERENGINE_H__
#define __TASKSCHEDULERENGINE_H__
#include "network.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define INVALID_TASK_TOKEN -1


//////////////////////////

typedef void TaskFunction(void * taskParam);

class CTask
{
	friend class CTaskQueue;

public:
	CTask(long time, TaskFunction * taskFunc, void * taskParam);
	~CTask();

public:
	long TaskToken() { return m_taskToken; };
	void HandleTask();
	void	UpdateLaunchTime(){m_launchTime=timeGetTime()+m_interval;}
	long	GetLaunchTime(){return m_launchTime;}
private:
	CTask			* m_next;
	CTask			* m_prev;
	long		     m_launchTime;
	long			 m_interval;
	static long       m_taskTokenGenerator;
	long              m_taskToken;

private:
	TaskFunction    * m_taskFunc;
	void            * m_taskParam;
};

////////////////////////

class CTaskQueue
{
public:
	CTaskQueue();
	~CTaskQueue();

public:
	void    AddTask(CTask * newTask);
	void    RemoveTask(CTask * task);
	CTask * RemoveTask(long taskToken);
	void    UpdateTask(CTask * task, long newTimeInterval);
	void    UpdateTask(long taskToken, long newTimeInterval);
	void    RemoveAllTask();
	
public:
	void                  HandleTask();
	CTask * SearchTaskByToken(long taskToken);

protected:
	void    Synchronize();
	
private:
	long	m_tvLastSyncTime;

private:
	CTask * m_head;
	CTask * m_tail;
	int m_taskNum;
};

//////////////////////////////

class CTaskSchedulerEngine
{
public:
	static CTaskSchedulerEngine * CreateTaskSchedulerEngine();
	static void DestroyTaskSchedulerEngine();

protected:
	static CTaskSchedulerEngine * m_taskSchedulerEngine;

protected:
	CTaskSchedulerEngine();
	virtual ~CTaskSchedulerEngine();

public:
	long ScheduleTask(long millionSeconds, TaskFunction * taskProc, void * taskParam,long taskToken=0);
	void UnscheduleTask(long taskToken);
	void UnscheduleAllTask();
	void UpdateTask(long taskToken,long newTime);//{m_taskQueue->UpdateTask(taskToken,newTimeInterval);}
public:
	void                  HandleTask();

private:
	CTaskQueue * m_taskQueue;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__TASKSCHEDULERENGINE_H__
