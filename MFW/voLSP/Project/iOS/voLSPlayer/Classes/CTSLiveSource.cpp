/*
 *  CTSLiveSource.cpp
 *
 *  Created by Lin Jun on 4/28/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CTSLiveSource.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#endif

CTSLiveSource::CTSLiveSource(void* pUserData, SENDDATACB fSendData)
:CSourceInterface(pUserData, fSendData)
,m_hReadThread(NULL)
,m_pReadBuffer(NULL)
,m_nReadBufferSize(1024*516)
,m_bStop(true)
{
	m_pUdpClient = new CUdpClientRecv;
	m_pReadBuffer = new unsigned char[m_nReadBufferSize];
}

CTSLiveSource::~CTSLiveSource()
{
	m_bStop = true;
	
	if(m_pReadBuffer)
	{
		delete m_pReadBuffer;
		m_nReadBufferSize = NULL;
	}
	
	if(m_pUdpClient)
	{
		delete m_pUdpClient;
		m_pUdpClient = NULL;
	}
}

bool CTSLiveSource::Open(unsigned char* url, int nPort)
{
	return 0 == m_pUdpClient->Init(nPort);
}

bool CTSLiveSource::Close()
{
	m_pUdpClient->Uninit();
	return true;
}

bool CTSLiveSource::Start()
{
	m_bStop = false;
	
	int nID = 0;
	CTSLiveSource::CreateThread(&m_hReadThread, &nID, CTSLiveSource::PushDataEntry, this, 0);
	return true;
}

bool CTSLiveSource::Pause()
{
	return false;
}

bool CTSLiveSource::Stop()
{
	m_bStop = true;
	
	int nCount = 0;
	while (m_hReadThread != NULL)
	{
		nCount++;
#ifdef _WIN32
		Sleep(2);
#else
		usleep(2*1000);
#endif
		if(nCount > 2000) {
			printf("CTSLiveSource::Stop error\n");
			break;
		}
	}
	return true;
}


int CTSLiveSource::PushDataEntry(void* pParam)
{
	CTSLiveSource* pInst = (CTSLiveSource*)pParam;
	pInst->PushData();
	return 0;
}

int CTSLiveSource::PushData()
{
	while (!m_bStop)
	{
		int nRealSize = 0;
		int nRet = m_pUdpClient->Read(188*7*50, &nRealSize, m_pReadBuffer);
		
		if(nRet==0 && nRealSize>0)
		{
			if(m_fSendData)
			{
				m_fSendData(m_pUserData, m_pReadBuffer, nRealSize);
			}
		}
		else
		{
			printf("read error\n");
#ifdef _WIN32
			Sleep(1);
#else
			usleep(1*1000);
#endif
		}
	}
	m_hReadThread = 0;
	return 0;
}

void CTSLiveSource::CreateThread(void** pHandle, int* pID, READSOCKET fProc, void* pParam, int uFlag)
{
#ifdef _WIN32
#else
	pthread_attr_t  attr;
    pthread_t       posixThreadID;
    int             returnVal;
	
    returnVal = pthread_attr_init(&attr);
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&posixThreadID, &attr, (void*(*)(void*))fProc, pParam);
	*pHandle	= (void*)posixThreadID;
	*pID		= (int)posixThreadID;
#endif

}