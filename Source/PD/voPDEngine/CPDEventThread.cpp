#include <voPDPort.h>
#include <stdio.h>
#include "CPDEventThread.h"
#include "CNetGet.h"
#include "UFileReader.h"
extern int g_nLog;

CPDEventThread::CPDEventThread()
	: m_hThread(NULL)
	, m_bStop(false)
	, m_dwWatchTime(1000)
{
}

CPDEventThread::~CPDEventThread()
{
	Destroy();
}

bool CPDEventThread::Create(CNetGet2* pNetGet)
{
	Destroy();

	m_pNetGet = pNetGet;
	//m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, NULL);
	CREATE_THREAD(m_hThread);//=VOCPFactory::CreateOneThread();
	if(m_hThread)
	{
		m_bStop = false;
		m_hThread->Create(this);
		voLog(LL_TRACE,"DFInfo.txt","CPDEventThread::Create()\n");
		return true;
	}
	return false;
}

void CPDEventThread::Destroy()
{
	m_bStop = true;
	if(m_hThread)
	{
		//WaitForSingleObject(m_hThread, INFINITE);
		m_hThread->TryJoin();
		//CloseHandle(m_hThread);
		DELETE_THREAD(m_hThread);
		m_hThread = NULL;
		voLog(LL_TRACE,"DFInfo.txt","CPDEventThread::Destroy()\n");

	}
}

void CPDEventThread::Execute()
{
	ThreadProcB();
}

DWORD CPDEventThread::ThreadProcB()
{
	DWORD dwFilePos = 0;
	int loop=0;
	while(!m_bStop)
	{
		if (UFileReader::g_bCancel)
		{
			break;
		}
		CDFInfo* pdi = m_pNetGet->GetDFInfo();
		CDFInfo2* pdi2 = m_pNetGet->GetDFInfo2();
		if(pdi2)
		{
			m_pNetGet->UpdateDf2();
			if(pdi2->CanRestartDownloadThread(dwFilePos))
			{
			
				voLog(LL_TRACE,"DFInfo.txt","[DF2]CPDEventThread call StartDownloadB: %d\r\n", dwFilePos);
				if(m_pNetGet->StartDownloadB(dwFilePos, MAXDWORD, false))
					break;
			}
		}
		else if(pdi)
		{
			DWORD dwStart = 0, dwLen = 0;
			if(pdi->GetBlankFragment(dwStart, dwLen))
			{
				voLog(LL_TRACE,"DFInfo.txt","[DF1]CPDEventThread call StartDownloadB\r\n");
				if(m_pNetGet->StartDownloadB(dwStart, dwLen, false))
					break;
			}
		}
		else
			break;

		for(int i = 0; i < (m_dwWatchTime / 10); i++)
		{
			if(m_bStop)
				break;
			IVOThread::Sleep(10);
		}
		loop++;
#define  TEST_BUFFERING  0
#if TEST_BUFFERING
		if ((loop&15)==0)
		{
			m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_BEGIN,0);
			voLog(LL_TRACE,"event.txt","simulating datalack begin\r\n");
		}

		if ((loop&15)==5)
		{
			m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END,0);
			voLog(LL_TRACE,"event.txt","simulating datalack end\r\n");
		}
#endif//TEST_BUFFERING
	}
	return 0;
}