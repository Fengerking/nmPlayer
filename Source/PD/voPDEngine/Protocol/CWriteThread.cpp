#include "httpget.h"
#include "CWriteThread.h"
#include "UFileReader.h"
CWriteThread::CWriteThread()
	: m_hThread(NULL)
	, m_bStop(false)
	, m_pHttpGet(NULL)
{
	CREATE_MUTEX(m_cs);
}

CWriteThread::~CWriteThread()
{
	Destroy();

	DELETE_MUTEX(m_cs);
}

bool CWriteThread::Create(CHttpGet* pHttpGet, DWORD dwBuffer, int nThreadPriority)
{
	m_pHttpGet = pHttpGet;
	for(int i = 0; i < 2; i++)
	{
		m_Buffer[i].buf_ptr = (BYTE*)allocate(dwBuffer,MEM_CHECK);
		if(!m_Buffer[i].buf_ptr)
			return false;
		m_Buffer[i].buf_len = 0;
	}

	m_bStop = false;
	CREATE_THREAD(m_hThread);// = VOCPFactory::CreateOneThread();//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GettingEntry, this, 0, &dwThreadID);
	if(m_hThread == NULL)
		return false;
	m_hThread->Create(this);

	m_hThread->SetPriority((VOThreadPriority)nThreadPriority);

	return true;
}

void CWriteThread::Destroy()
{
	if(m_pHttpGet)
	{
		m_bStop = true;
		if(m_hThread)
		{
			//WaitForSingleObject(m_hThread, INFINITE);
			m_hThread->TryJoin();
			//CloseHandle(m_hThread);
			DELETE_THREAD(m_hThread);
			m_hThread = NULL;
		}

		for(int i = 0; i < 2; i++)
		{
			if(m_Buffer[i].buf_ptr)
			{
				VO_SAFE_DELETE(m_Buffer[i].buf_ptr);
				m_Buffer[i].buf_ptr = NULL;
			}
			m_Buffer[i].buf_len = 0;
		}

		m_pHttpGet = NULL;
	}
}

void CWriteThread::Execute()
{
	ThreadProcB();
}

UINT CWriteThread::ThreadProcB()
{
	BYTE* pWritableBuf = NULL;
	DWORD dwWritableBuf = 0;
	int writeSeq=0;
	int totalSize=0;
	while(!m_bStop)
	{
		if (UFileReader::g_bCancel)
		{
			break;
		}
		dwWritableBuf = GetWritableBuffer(&pWritableBuf);
		if(dwWritableBuf <= 0)
		{
			IVOThread::Sleep(1);
			continue;
		}

		//write to file
		voLog(LL_TRACE,"netDataStatus.txt","w:%d,size=%d,total=%d\n",writeSeq++,dwWritableBuf,totalSize+=dwWritableBuf);
		if(!Write(pWritableBuf, dwWritableBuf))
		{
			m_pHttpGet->m_stopGetting = true;
			return 0L;
		}
		SetBufferValid(false, pWritableBuf, dwWritableBuf);
	}

	//write left buffer to file
	dwWritableBuf = GetWritableBuffer(&pWritableBuf);
	while(dwWritableBuf > 0)
	{
		//write to file
		if(!Write(pWritableBuf, dwWritableBuf))
		{
			m_pHttpGet->m_stopGetting = true;
			return 0L;
		}
		SetBufferValid(false, pWritableBuf, dwWritableBuf);

		dwWritableBuf = GetWritableBuffer(&pWritableBuf);
	}
	return 0L;
}

bool CWriteThread::Write(BYTE* pBuffer, DWORD dwLength)
{
	return m_pHttpGet ? m_pHttpGet->SinkFile(pBuffer, dwLength) : false;
}

bool CWriteThread::GetIdleBuffer(BYTE** ppBuffer)
{
	CAutoLock	lock(m_cs);
	for(int i = 0; i < 2; i++)
	{
		if(m_Buffer[i].buf_len == 0)
		{
			*ppBuffer = m_Buffer[i].buf_ptr;
			return true;
		}
	}

	return false;
}

DWORD CWriteThread::GetWritableBuffer(BYTE** ppBuffer)
{
	CAutoLock	lock(m_cs);
	for(int i = 0; i < 2; i++)
	{
		if(m_Buffer[i].buf_len & 0x80000000)
		{
			*ppBuffer = m_Buffer[i].buf_ptr;
			return (m_Buffer[i].buf_len & 0x7FFFFFFF);
		}
	}

	return 0;
}

void CWriteThread::SetBufferValid(bool bValid, BYTE* pBuffer, DWORD dwLen)
{
	CAutoLock	lock(m_cs);
	for(int i = 0; i < 2; i++)
	{
		if(m_Buffer[i].buf_ptr == pBuffer)
		{
			if(bValid)
			{
				m_Buffer[i].buf_len = dwLen;
				if(m_Buffer[1 - i].buf_len == 0)
					m_Buffer[i].buf_len |= 0x80000000;
			}
			else
			{
				if(m_Buffer[i].buf_len & 0x80000000 && m_Buffer[1 - i].buf_len != 0)
					m_Buffer[1 - i].buf_len |= 0x80000000;
				m_Buffer[i].buf_len = 0;
			}
			break;
		}
	}

}
