#pragma once
#include <stdio.h>

#include "CSourceInterface.h"

typedef int (* PUSHDATA) (void* pParam);

class CFileDevice : public CSourceInterface
{
public:
	CFileDevice(void * pUserData, SENDDATACB fSendData);
	virtual ~CFileDevice(void);

	virtual bool Open(unsigned char* url, int nPort);
	virtual bool Close();
	virtual bool Start();
	virtual bool Pause();
	virtual bool Stop();
	
	virtual bool IsStatusClose()  { return m_nStatus == 0; }
	virtual bool IsStatusOpen()  { return m_nStatus == 1; }
	virtual bool IsStatusRunning()  { return m_nStatus == 2; }
	virtual bool IsStatusStopped()  { return m_nStatus == 1; }
	virtual bool IsStatusPaused()  { return m_nStatus == 3; }
	
private:
	void SetByteRate(int rate);
	void SetPacketSize(int size);
	bool SyncTSHeader();
	
	int GetChannelCount() const { return 1; }
	int GetReqBufSize() const { return 0; }

	void SetLoop(bool b) { m_bLoop = b; }

	int GetLeftDataLen();

protected:
	static int PushDataEntry(void* pParam);
	int PushData();
	int GetDelay();
	void BitrateControl(bool cancel);
	long long GetFileSize();

private:
	int		m_nByteRate;
	int		m_nPacketSize;

	bool	m_bLoop;
	bool	m_bStopCmd;

	unsigned char*	m_pBuffer;
	FILE*	m_hFile;
	void*	m_hPushThread;
	int		m_nSyncOffset;
	bool	m_bMTPDumpFile;

	int		m_nStartTime; //the time when start count sent
	long long m_cSent; //how many bytes already sent

protected:
	void SetStatusClose() { m_nStatus = 0; }
	void SetStatusOpen() { m_nStatus = 1; }
	void SetStatusRunning() { m_nStatus = 2; }
	void SetStatusStopped() { m_nStatus = 1; }
	void SetStatusPaused(){m_nStatus = 3;};

private:
	int			m_nStatus;
	long long	m_nFileLen;
	long long	m_nFileDataLeftLen;
};
