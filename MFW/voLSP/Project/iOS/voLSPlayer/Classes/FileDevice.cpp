#include "FileDevice.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#include "vompType.h"

const int	TS_SEND_SLEEP_TIME = 200*1000;
const int	TS_READ_COUNT	= 50;
const int	TS_PACKET_SIZE	= 188;
const char	TS_SYNC_BYTE	= 0x47;
const int MAX_PACKET_SIZE = 512 << 10;

unsigned long GetSystemTime()
{
	unsigned long nTime = 0;
	struct timeval tval;
	gettimeofday(&tval, NULL);
	unsigned long long second = tval.tv_sec;
	second = second*1000 + tval.tv_usec/1000;
	nTime = second & 0x000000007FFFFFFF;
	
	return nTime;
}

void CreateThread(void** pHandle, int* pID, PUSHDATA fProc, void* pParam, int uFlag)
{
	pthread_attr_t  attr;
    pthread_t       posixThreadID;
    int             returnVal;
	
    returnVal = pthread_attr_init(&attr);
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int     threadError = pthread_create(&posixThreadID, &attr, (void*(*)(void*))fProc, pParam);
	*pHandle	= (void*)posixThreadID;
	*pID		= (int)posixThreadID;
}

unsigned char* FindTSPacketHeader(unsigned char* pData, int cbData, int packetSize)
{
	unsigned char* p = pData;
	unsigned char* p2 = pData + cbData - packetSize;
	while (p < p2)
	{
		if ( (*p == TS_SYNC_BYTE) && (*(p + packetSize) == TS_SYNC_BYTE) )
			return p;
		++p;
	}
	return 0;
}

unsigned char* FindTSPacketHeader2(unsigned char* pData, int cbData, int packetSize)
{
	unsigned char* p = FindTSPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		unsigned char* p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
			if (*p2 == TS_SYNC_BYTE)
				return p;
	}
	return 0;
}

int CheckTSPacketSize(unsigned char* pData, int cbData)
{
	printf("cbData = %d\n", cbData);
	if (FindTSPacketHeader2(pData, cbData, 188))
		return 188;
	if (FindTSPacketHeader2(pData, cbData, 204))
		return 204;
	if (FindTSPacketHeader2(pData, cbData, 192))
		return 192;
	return 0;
}


CFileDevice::CFileDevice(void * pUserData, SENDDATACB fSendData)
:CSourceInterface(pUserData, fSendData)
, m_pBuffer(NULL)
, m_bLoop(false)
, m_bMTPDumpFile (false)
, m_nFileLen(0)
, m_hFile(NULL)
, m_hPushThread(NULL)
, m_nByteRate(0)
, m_nPacketSize(0)
, m_bStopCmd(false)
, m_nSyncOffset(0)
, m_nStartTime(0)
, m_cSent(0)
, m_nStatus(0)
, m_nFileDataLeftLen(0)
{
	SetStatusClose();
}

CFileDevice::~CFileDevice(void)
{
	if (m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	
	Close();
}

int CFileDevice::PushDataEntry(void* pParam)
{
	CFileDevice * pInst = (CFileDevice*) pParam;
	int nRC = pInst->PushData();
	return nRC;
}


void CFileDevice::SetPacketSize(int size)
{
	if (m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
		m_nPacketSize = 0;
	}

	if (size > 0)
	{
		m_pBuffer = new unsigned char[size > 0 ? size : MAX_PACKET_SIZE];
		m_nPacketSize = size;
	}
}

bool CFileDevice::SyncTSHeader()
{
	const int TEST_SIZE = TS_PACKET_SIZE * 16;
	unsigned char buffer[TEST_SIZE];
	long long dwRead = 0;
	dwRead = fread(buffer, TEST_SIZE, 1, m_hFile);
	
	int packetSize = CheckTSPacketSize(buffer, TEST_SIZE);
	if (packetSize <= 0)
		return false;
	unsigned char* p = FindTSPacketHeader(buffer, TEST_SIZE, packetSize);
	if (!p)
		return false;
	m_nSyncOffset = p - buffer;
	fseek(m_hFile, m_nSyncOffset, SEEK_SET);
	
	SetPacketSize(packetSize*TS_READ_COUNT);
	//
	int lSize = 0;
	int lHigh = 0;

	lSize = GetFileSize ();

	m_nFileLen = lHigh;
	m_nFileLen = (m_nFileLen << 32) + lSize;
	m_nFileDataLeftLen = m_nFileLen;

	return true;
}

long long CFileDevice::GetFileSize()
{
	long long r0 = 0, r = 0;
	r0 = ftell (m_hFile);
	
	r = fseek (m_hFile, 0LL, SEEK_END);
	if (-1 == r)
	{
		return -1;
	}
	
	r = ftell (m_hFile);
	if (-1 == r)
	{
		return -1;
	}
	
	fseek(m_hFile, r0, SEEK_SET);
	
	return r;
}

bool CFileDevice::Open(unsigned char* url, int nPort)
{
	Close();
	
	m_hFile = fopen((const char*)url, "rb");
	if (m_hFile == NULL)
	{
		return false;
	}

	if (!SyncTSHeader())
		return false;

	SetStatusOpen();
	return true;
}

bool CFileDevice::Close()
{
	if (m_hFile)
	{
		fclose (m_hFile);
		SetStatusClose();
		m_hFile = NULL;
	}
	
	m_hPushThread = NULL;
	
	return true;
}

int CFileDevice::GetLeftDataLen()
{
	return m_nFileDataLeftLen;
}

int CFileDevice::PushData()
{
	if(!m_hFile)
	{
		printf("File not open...\n");
		m_hPushThread = NULL;
		return 0;
	}
	
	unsigned long long total_time = GetSystemTime();
	
	while (!m_bStopCmd)
	{
		int dwRead = 0;
		int dwSizetoRead = m_nPacketSize;
		dwRead = fread(m_pBuffer, 1, dwSizetoRead, m_hFile);
		
		if (dwRead <= 0)
		{
			
			if (m_bLoop)
			{
				m_nFileDataLeftLen = m_nFileLen;
				fseek(m_hFile, m_nSyncOffset, SEEK_SET);
				continue;
			}
			else 
			{
				printf("read failed!!!\n");
				break;
			}
		}
		
		if (dwRead < dwSizetoRead)
		{
			if(dwRead > 0)
			{
				if (m_fSendData)
				{
					m_nFileDataLeftLen -= dwRead;
					//printf("send data 1...\n");
					int nRet = m_fSendData(m_pUserData, m_pBuffer, dwRead);
					
					while(nRet == VOMP_ERR_Retry)
					{
						printf("overflow, need resend buffer 1\n");
						usleep(TS_SEND_SLEEP_TIME);

						nRet = m_fSendData(m_pUserData, m_pBuffer, dwRead);
						if(m_bStopCmd)
							break;
					}
				}
				m_cSent += dwRead;
			}

			if (m_bLoop)
			{
				m_nFileDataLeftLen = m_nFileLen;
				fseek(m_hFile, m_nSyncOffset, SEEK_SET);
			}
			else
			{
				printf("read file end!!!\n");
				break;
			}
		}
		else
		{
			if (m_fSendData)
			{
				//printf("send data 2...\n");
				m_nFileDataLeftLen -= dwRead;
				int nRet = m_fSendData(m_pUserData, m_pBuffer, dwRead);
				
				while(nRet == VOMP_ERR_Retry)
				{
					printf("overflow, need resend buffer 2\n");
					usleep(TS_SEND_SLEEP_TIME);
					// need resend 
					nRet = m_fSendData(m_pUserData, m_pBuffer, dwRead);
					if(m_bStopCmd)
						break;
				}
			}

			m_cSent += dwRead;
		}
		
		while(IsStatusPaused())
		{
			usleep(5*1000);
			if(m_bStopCmd)
				break;
		}
	}

	// notify file is end
	m_fSendData(m_pUserData, NULL, 0);
	
	fseek(m_hFile, 0, SEEK_SET);

	SetStatusStopped();
	m_hPushThread = NULL;
		
	total_time = GetSystemTime() - total_time;
	printf("Read file thread exit, total time = %qu!!!!!!!!\n", total_time);
	
	return 0;
}


bool CFileDevice::Start()
{
	m_bStopCmd = false;
	
	if(IsStatusPaused())
	{
		SetStatusRunning();
		return true;
	}
	
	if(m_hFile)
	{
		fseek(m_hFile, 0, SEEK_SET);
	}
	
	int dwThreadID;
	CreateThread(&m_hPushThread, &dwThreadID, PushDataEntry, this, 0);
	if (m_hPushThread == NULL)
		return false;
	
	SetStatusRunning();
	return true;
}

bool CFileDevice::Pause()
{
	SetStatusPaused();
	return true;
}

bool CFileDevice::Stop()
{
	m_bStopCmd = true;
	
	int nSleepTime	= 2*1000;
	int nWaitTime	= 0;
 	while (m_hPushThread)
 	{
 		usleep(nSleepTime);
		
		nWaitTime += nSleepTime;
		if(nWaitTime >= 3000*1000)
			break;
 	}

	SetStatusStopped();

	return true;
}

void CFileDevice::SetByteRate(int rate) 
{ 
	m_nByteRate = rate; 
	m_nStartTime = GetSystemTime();
	m_cSent = 0;
}

int CFileDevice::GetDelay()
{
	long long totalBytes = m_cSent;
	int totalTime = (int)(totalBytes * 1000 / m_nByteRate);
	int usedTime = GetSystemTime() - m_nStartTime;
	int delay = totalTime - usedTime;
	if (delay < 0)
		delay = 0;
	printf("delay time = %d\n", delay);
	return delay;
}

void CFileDevice::BitrateControl(bool cancel)
{
	int nDelay = GetDelay ();
	if (nDelay > 2)
	{
		usleep(nDelay*1000);
	}
}