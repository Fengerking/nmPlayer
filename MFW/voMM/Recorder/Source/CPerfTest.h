#pragma once
# include "CSampleData.h"
#include "voCMutex.h"
#include "cmnFile.h"

//#define _TEST_THREAD_PERFORMANCE_
//#define _TEST_PERFORMANCE_

//#define _DUMP_LOG_

class CPerfTest
{
public:
	CPerfTest(void);
	~CPerfTest(void);

#ifdef _TEST_THREAD_PERFORMANCE_

public:
	void Reset();

	void StartVE(HANDLE hThread);
	void EndVE(HANDLE hThread);

	void StartAE(HANDLE hThread);
	void EndAE(HANDLE hThread);

	void StartRender(HANDLE hThread);
	void EndRender(HANDLE hThread);

	void DumpResult();

public:
	static	VO_U32		ThreadProc (LPVOID pParam);
	virtual VO_U32		ThreadLoop (void);
	virtual int			GetThreadTime (HANDLE hThread, int nThreadID);

private:

protected:
	HANDLE		m_hThread;
	bool		m_bStop;
	int			m_nThreadID;

	voCMutex	m_cSharedState;

	int			m_nIdleCPUStart;
	int			m_nIdleCPUEnd;

	int			m_nVideoEncCPUStart;
	int			m_nVideoEncCPUEnd;
	int			m_nVideoEncStartTime;
	int			m_nVideoEncEndTime;

	int			m_nAudioEncCPUStart;
	int			m_nAudioEncCPUEnd;
	int			m_nAudioEncStartTime;
	int			m_nAudioEncEndTime;

	int			m_nRenderCPUStart;
	int			m_nRenderCPUEnd;
	int			m_nRenderStartTime;
	int			m_nRenderEndTime;

#endif


public:
	static void OpenFile();
	static void CloseFile();

	//general
	static unsigned long BeginCountTime();
	static unsigned long EndCountTime();
	//static void WriteLog(char *format, ...);
	static void DumpData(char* pszFile, unsigned char* pData, int nDataLen);


	// new dump 
	static void DumpLogEx(char *format, ...);
	static void DumpLog(char * Log);
	static void CloseLog();
	static VO_PTR	m_hLogFile;
	static unsigned long m_dwLogCount;



public:
#ifdef _TEST_PERFORMANCE_
	static VO_PTR m_hFile;
	static unsigned long m_lUseTime;

#endif

};
