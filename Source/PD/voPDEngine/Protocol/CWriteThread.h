#pragma once

typedef struct tagVOBuffer 
{
	BYTE*	buf_ptr;
	DWORD	buf_len;	//first bit indicate this buffer if writable

	tagVOBuffer()
		: buf_ptr(NULL)
		, buf_len(0)
	{
	}
} VOBuffer, *PVOBuffer;

class CHttpGet;
class CWriteThread:public IVOCommand,MEM_MANAGER
{
public:
	CWriteThread();
	virtual ~CWriteThread();
	void Execute();
public:
	//static UINT	ThreadProc(LPVOID lParam);

public:
	bool		Create(CHttpGet* pHttpGet, DWORD dwBuffer, int nThreadPriority);
	void		Destroy();

	bool		GetIdleBuffer(BYTE** ppBuffer);
	void		SetBufferValid(bool bValid, BYTE* pBuffer, DWORD dwLen);

protected:
	UINT		ThreadProcB();

	//if more than 0, get successfully
	DWORD		GetWritableBuffer(BYTE** ppBuffer);

	bool		Write(BYTE* pBuffer, DWORD dwLength);

protected:
	IVOThread*	m_hThread;
	bool					m_bStop;

	VOBuffer				m_Buffer[2];

	IVOMutex*				m_cs;

	CHttpGet*				m_pHttpGet;
};
