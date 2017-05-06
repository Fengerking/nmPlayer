#include "CPerfTest.h"
#include "voOSFunc.h"
#include "voThread.h"
#include "cmnFile.h"

#ifdef _TEST_PERFORMANCE_

VO_PTR		  CPerfTest::m_hFile = NULL;
unsigned long CPerfTest::m_lUseTime = 0;

#endif




VO_PTR	CPerfTest::m_hLogFile = NULL;
unsigned long CPerfTest::m_dwLogCount = 0;
void CPerfTest::DumpLogEx(char *format, ...)
{
#ifdef _DUMP_LOG_
	

	if(NULL == m_hLogFile)
	{
#ifdef _WIN32
		m_hLogFile = cmnFileOpen(0, _T("c:\\reclog.txt"), VO_FILE_WRITE_ONLY);
#elif defined _LINUX
		m_hLogFile = cmnFileOpen(0, _T("/data/reclog.txt"), VO_FILE_WRITE_ONLY);
#endif
	}

#ifdef _WIN32
	VO_U32	dwWrite = 0;
	char	szInfo[256];

	SYSTEMTIME t;
	GetLocalTime(&t);
	sprintf(szInfo, "\r\n[%03d]  ", m_dwLogCount);
	cmnFileWrite(0, m_hLogFile, szInfo, strlen(szInfo));


	va_list arg_ptr;
	va_start(arg_ptr, format);
	int nWrittenBytes = vsprintf(szInfo, format, arg_ptr);
	va_end(arg_ptr);

	//sprintf(szInfo, format, ...);
	cmnFileWrite(0, m_hLogFile, szInfo, strlen(szInfo));
	m_dwLogCount++;

#elif defined LINUX

// 	VO_U32	dwWrite = 0;
// 	char	szInfo[256];
// 
// 	va_list arg_ptr;
// 	va_start(arg_ptr, format);
// 	int nWrittenBytes = printf(szInfo, format, arg_ptr);
// 	va_end(arg_ptr);
// 
// 	cmnFileWrite(0, m_hLogFile, szInfo, strlen(szInfo));
// 	m_dwLogCount++;

#endif


#endif
}

void CPerfTest::DumpLog(char * Log)
{
#ifdef _DUMP_LOG_
	if(NULL == m_hLogFile)
	{
#ifdef _WIN32
		m_hLogFile = cmnFileOpen(0, _T("c:\\reclog.txt"), VO_FILE_WRITE_ONLY);
#elif defined _LINUX
		m_hLogFile = cmnFileOpen(0, _T("/data/reclog.txt"), VO_FILE_WRITE_ONLY);
#endif
	}
		

	cmnFileWrite(0, m_hLogFile, Log, strlen(Log));
	//cmnFileFlush(0, m_hLogFile);
#endif
}

void CPerfTest::CloseLog()
{
#ifdef _DUMP_LOG_

	if(m_hLogFile)
	{
		cmnFileClose(0, m_hLogFile);
		m_hLogFile = NULL;
	}

#endif
}




CPerfTest::CPerfTest(void)
{
#ifdef _TEST_THREAD_PERFORMANCE_

	Reset();

	m_nThreadID = 0;

	VO_U32 ThreadID;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, CREATE_SUSPENDED, &ThreadID);

	if (m_hThread != NULL)
	{
		SetThreadPriority(m_hThread, THREAD_PRIORITY_IDLE);
		ResumeThread(m_hThread);
	}
#endif // 
}

CPerfTest::~CPerfTest(void)
{
#ifdef _TEST_THREAD_PERFORMANCE_
	DumpResult ();

	if (m_hThread != NULL)
	{
		m_bStop = true;
		ResumeThread(m_hThread);
		//WaitForSingleObject(m_hThread, 1000);
		voOS_Sleep(1000);
	}
#endif // 
}



#ifdef _TEST_THREAD_PERFORMANCE_

void CPerfTest::Reset()
{
	m_bStop = false;

	m_nIdleCPUStart = 0;
	m_nIdleCPUEnd = 0;
	m_nVideoEncCPUStart = 0;
	m_nVideoEncCPUEnd = 0;
	m_nAudioEncCPUStart = 0;
	m_nAudioEncCPUEnd = 0;
	m_nRenderCPUStart = 0;
	m_nRenderCPUEnd = 0;

	m_nVideoEncStartTime = 0;
	m_nVideoEncEndTime = 0;
	m_nAudioEncStartTime = 0;
	m_nAudioEncEndTime = 0;
	m_nRenderStartTime = 0;
	m_nRenderEndTime = 0;
}


void CPerfTest::StartVE(HANDLE hThread)
{
	if(!hThread)
		return;
	voCAutoLock lock(&m_cSharedState);

	m_nVideoEncStartTime = voOS_GetSysTime();
	GetThreadTime(hThread, 1);
	GetThreadTime(m_hThread, 7);
}

void CPerfTest::EndVE(HANDLE hThread)
{
	if(!hThread)
		return;
	voCAutoLock lock(&m_cSharedState);

	m_nVideoEncEndTime = voOS_GetSysTime();
	GetThreadTime(hThread, 2);
	GetThreadTime(m_hThread, 8);
}


void CPerfTest::StartAE(HANDLE hThread)
{
	if(!hThread)
		return;
	voCAutoLock lock(&m_cSharedState);

	m_nAudioEncStartTime = voOS_GetSysTime();
	GetThreadTime(hThread, 3);
	GetThreadTime(m_hThread, 7);
}

void CPerfTest::EndAE(HANDLE hThread)
{
	if(!hThread)
		return;
	voCAutoLock lock(&m_cSharedState);

	m_nAudioEncEndTime = voOS_GetSysTime();
	GetThreadTime(hThread, 4);
	GetThreadTime(m_hThread, 8);
}


void CPerfTest::StartRender(HANDLE hThread)
{
	if(!hThread)
		return;

	voCAutoLock lock(&m_cSharedState);

	m_nRenderStartTime = voOS_GetSysTime();
	GetThreadTime(hThread, 5);
	GetThreadTime(m_hThread, 7);
}


void CPerfTest::EndRender(HANDLE hThread)
{
	if(!hThread)
		return;

	voCAutoLock lock(&m_cSharedState);

	m_nRenderEndTime = voOS_GetSysTime();
	GetThreadTime(hThread, 6);
	GetThreadTime(m_hThread, 8);
}

int CPerfTest::GetThreadTime (HANDLE hThread, int nThreadID)
{
	if (hThread == NULL)
		return -1;

	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if (nThreadID >= 7)
		voThreadSuspend (hThread);

	VO_U32 dwError = 0;
	bool bRC = GetThreadTimes(hThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);

	LONGLONG llKernelTime = ftKernelTime.dwHighDateTime;
	llKernelTime = llKernelTime << 32;
	llKernelTime += ftKernelTime.dwLowDateTime;

	LONGLONG llUserTime = ftUserTime.dwHighDateTime;
	llUserTime = llUserTime << 32;
	llUserTime += ftUserTime.dwLowDateTime;

	if (nThreadID == 1 && m_nVideoEncCPUStart == 0)
		m_nVideoEncCPUStart = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 2 && m_nVideoEncCPUEnd == 0)
		m_nVideoEncCPUEnd = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 3 && m_nAudioEncCPUStart == 0)
		m_nAudioEncCPUStart = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 4 && m_nAudioEncCPUEnd == 0)
		m_nAudioEncCPUEnd = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 5 && m_nRenderCPUStart == 0)
		m_nRenderCPUStart = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 6 && m_nRenderCPUEnd == 0)
		m_nRenderCPUEnd = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 7 && m_nIdleCPUStart == 0)
		m_nIdleCPUStart = (int) ((llKernelTime + llUserTime) / 10000);
	else if (nThreadID == 8 && m_nIdleCPUEnd == 0)
		m_nIdleCPUEnd = (int) ((llKernelTime + llUserTime) / 10000);

	if (nThreadID >= 7)
		voThreadResume (hThread);

	return 0;
}

void CPerfTest::DumpResult (void)
{
	VO_PTR hFileTXT = NULL;

#ifdef _WIN32
	hFileTXT = cmnFileOpen(0, _T("c:\\rec_perf.txt"), VO_FILE_WRITE_ONLY);
	//hFileTXT = CreateFile(_T("c:\\rec_perf.txt"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (VO_U32) 0, NULL);
#elif defined LINUX
	hFileTXT = cmnFileOpen(0, _T("/data/rec_perf.txt"), VO_FILE_WRITE_ONLY);
	//hFileTXT = CreateFile(_T("/data/rec_perf.txt"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (VO_U32) 0, NULL);
#endif

	if(hFileTXT == NULL)
		return;

	char szInfo[260];
	VO_U32	dwWrite = 0;
	int nCpuUsage = 100;
	int nTotalTime = 0;
	int nTotalExcuteTime = 0;

	if(m_nVideoEncEndTime - m_nVideoEncStartTime > 0)
		nTotalTime = m_nVideoEncEndTime - m_nVideoEncStartTime;
	else if(m_nAudioEncEndTime - m_nAudioEncStartTime > 0)
		nTotalTime = m_nAudioEncEndTime - m_nAudioEncStartTime;
	else if(m_nRenderEndTime - m_nRenderStartTime > 0)
		nTotalTime = m_nRenderEndTime - m_nRenderStartTime;

	if(nTotalTime == 0)
	{
		cmnFileClose (0, hFileTXT);
		return;
	}
		

	//video
	nTotalExcuteTime += m_nVideoEncCPUEnd - m_nVideoEncCPUStart;
// 	if (m_nVideoEncEndTime - m_nVideoEncStartTime > 0)
// 		nCpuUsage = 100 - (m_nIdleCPUEnd - m_nIdleCPUStart) * 100 / (m_nVideoEncEndTime - m_nVideoEncStartTime);

	if (m_nVideoEncEndTime - m_nVideoEncStartTime > 0)
		sprintf (szInfo, "Video encode: CPU time:%08d,percent:%0.2f\r\n", (m_nVideoEncCPUEnd - m_nVideoEncCPUStart), (m_nVideoEncCPUEnd - m_nVideoEncCPUStart) * 100.0 / nTotalTime);
	else
		sprintf (szInfo, "Video encode: CPU time:%08d,percent:%0.2f\r\n", (m_nVideoEncCPUEnd - m_nVideoEncCPUStart), 0);

	//WriteFile (hFileTXT, szInfo, strlen (szInfo), &dwWrite, NULL);
	cmnFileWrite(0, hFileTXT, szInfo, strlen(szInfo));


	//audio
	nTotalExcuteTime += m_nAudioEncCPUEnd - m_nAudioEncCPUStart;
// 	if (m_nAudioEncEndTime - m_nAudioEncStartTime > 0)
// 		nCpuUsage = 100 - (m_nIdleCPUEnd - m_nIdleCPUStart) * 100 / (m_nAudioEncEndTime - m_nAudioEncStartTime);

	if (m_nAudioEncEndTime - m_nAudioEncStartTime > 0)
		sprintf (szInfo, "Audio encode: CPU time:%08d,percent:%02.2f\r\n", (m_nAudioEncCPUEnd - m_nAudioEncCPUStart), (m_nAudioEncCPUEnd - m_nAudioEncCPUStart) * 100.0 / nTotalTime);
	else
		sprintf (szInfo, "Audio encode: CPU time:%08d,percent:%02.2f\r\n", (m_nAudioEncCPUEnd - m_nAudioEncCPUStart), 0);

	cmnFileWrite(0, hFileTXT, szInfo, strlen(szInfo));
	
	
	//render
	nTotalExcuteTime += m_nRenderCPUEnd - m_nRenderCPUStart;
// 	if (m_nRenderEndTime - m_nRenderStartTime > 0)
// 		nCpuUsage = 100 - (m_nIdleCPUEnd - m_nIdleCPUStart) * 100 / (m_nRenderEndTime - m_nRenderStartTime);

	if (m_nRenderEndTime - m_nRenderStartTime > 0)
		sprintf (szInfo, "      Render: CPU time:%08d,percent:%02.2f\r\n", (m_nRenderCPUEnd - m_nRenderCPUStart), (m_nRenderCPUEnd - m_nRenderCPUStart) * 100.0 / nTotalTime);
	else
		sprintf (szInfo, "      Render: CPU time:%08d,percent:%02.2f\r\n", (m_nRenderCPUEnd - m_nRenderCPUStart), 0);

	cmnFileWrite(0, hFileTXT, szInfo, strlen(szInfo));

	//idle
// 	if (m_nRenderEndTime - m_nRenderStartTime > 0)
// 		nCpuUsage = 100 - (m_nIdleCPUEnd - m_nIdleCPUStart) * 100 / (m_nRenderEndTime - m_nRenderStartTime);

	if (m_nRenderEndTime - m_nRenderStartTime > 0)
		sprintf (szInfo, "        Idle: CPU time:%08d,percent:%02.2f\r\n", (m_nIdleCPUEnd - m_nIdleCPUStart), (m_nIdleCPUEnd - m_nIdleCPUStart) * 100.0 / nTotalTime);
	else
		sprintf (szInfo, "        Idle: CPU time:%08d,percent:%02.2f\r\n", (m_nIdleCPUEnd - m_nIdleCPUStart), 0);

	cmnFileWrite(0, hFileTXT, szInfo, strlen(szInfo));


	//general info
	nCpuUsage = 100;
	nCpuUsage = 100 - (m_nIdleCPUEnd - m_nIdleCPUStart) * 100 / nTotalTime;
	sprintf (szInfo, "Thread life time:%08d, Total thread excute time:%08d, percent:%.2f, CPU usage:%02d \r\n", nTotalTime, nTotalExcuteTime, (nTotalExcuteTime*100.0) / nTotalTime, nCpuUsage);
	cmnFileWrite(0, hFileTXT, szInfo, strlen(szInfo));

	cmnFileClose(0, hFileTXT);
}


VO_U32 CPerfTest::ThreadProc (LPVOID pParam)
{
	CPerfTest * pPerformance = (CPerfTest *)pParam;
	pPerformance->ThreadLoop ();

	return 0;
}

VO_U32 CPerfTest::ThreadLoop (void)
{
	while (!m_bStop)
	{
	}

	return 0;
}

#endif //



void CPerfTest::OpenFile()
{
#ifdef _TEST_PERFORMANCE_
	if(m_hFile)
		return;

#ifdef _WIN32
	m_hFile = cmnFileOpen(0, _T("c:\\recperformance.txt"), VO_FILE_WRITE_ONLY);
#elif defined LINUX
	m_hFile = cmnFileOpen(0, _T("/data/recperformance.txt"), VO_FILE_WRITE_ONLY);
#endif

	VO_U32	dwWrite = 0;
	char	szInfo[256];
	sprintf(szInfo, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	cmnFileWrite(0, m_hFile, szInfo, strlen(szInfo));

// 	SYSTEMTIME t;
// 	GetLocalTime(&t);
// 	sprintf(szInfo, "This file was created on %d/%02d/%02d %02d:%02d:%02d\r\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
// 	cmnFileWrite(0, m_hFile, szInfo, vostrlen(szInfo));
	
	sprintf(szInfo, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	cmnFileWrite(0, m_hFile, szInfo, strlen(szInfo));
#endif
}

void CPerfTest::CloseFile()
{
#ifdef _TEST_PERFORMANCE_
	if(m_hFile)
	{
		cmnFileClose(0, m_hFile);
	}
#endif
}

/*
void CPerfTest::WriteLog(char *format, ...)
{
#ifdef _TEST_PERFORMANCE_

	VO_U32	dwWrite = 0;
	char	szInfo[256];

// 	SYSTEMTIME t;
// 	GetLocalTime(&t);
// 	sprintf(szInfo, "\r\n[%03d]%02d:%02d:%02d  ", m_dwLogCount, t.wHour, t.wMinute, t.wSecond);
// 	cmnFileWrite(0, m_hFile, szInfo, vostrlen(szInfo));
	
// 	sprintf(szInfo, "\r\n ");
// 	cmnFileWrite(0, m_hFile, szInfo, strlen(szInfo));
// 
// 	va_list arg_ptr;
// 	va_start(arg_ptr, format);
// 	int nWrittenBytes = vsprintf(szInfo, format, arg_ptr);
// 	va_end(arg_ptr);
// 
// 	//sprintf(szInfo, format, ...);
// 	cmnFileWrite(0, m_hFile, szInfo, strlen(szInfo));
// 
// 	m_dwLogCount++;
#endif
}
*/

void CPerfTest::DumpData(char* pszFile, unsigned char* pData, int nDataLen)
{
#ifdef _TEST_PERFORMANCE_
// 	VO_PTR hFile = cmnFileOpen(0, pszFile, VO_FILE_WRITE_ONLY);
// 
// 	if(hFile)
// 	{
// 		cmnFileWrite(0, hFile, pData, nDataLen);
// 		cmnFileClose(hFile);
// 	}
#endif
}

