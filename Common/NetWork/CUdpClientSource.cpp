/*
 *  CUdpClientSource.cpp
 *
 *  Created by Lin Jun on 2011-08-25.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CUdpClientSource.h"
#include "CUdpClientRecv.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CUdpClientSource::CUdpClientSource(void* pUserData, SENDDATACB fSendData)
:m_fSendData(fSendData)
,m_pUserData(pUserData)
,m_hReadThread(NULL)
,m_pReadBuffer(NULL)
,m_nReadBufferSize(1024*516)
,m_bStop(true)
{
	m_pUdpClient = new CUdpClientRecv;
	m_pReadBuffer = new unsigned char[m_nReadBufferSize];
}

CUdpClientSource::~CUdpClientSource()
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

bool CUdpClientSource::Open(int nPort)
{
	return 0 == m_pUdpClient->Init(nPort);
}

bool CUdpClientSource::Close()
{
	m_pUdpClient->Uninit();
	return true;
}

bool CUdpClientSource::Start()
{
	m_bStop = false;
	
	int nID = 0;
	CreateReadThread(&m_hReadThread, &nID, CUdpClientSource::PushDataEntry, this, 0);
	return true;
}

bool CUdpClientSource::Pause()
{
	return false;
}

bool CUdpClientSource::Stop()
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
		if(nCount > 200)
			break;
	}
	return true;
}


int CUdpClientSource::PushDataEntry(void* pParam)
{
	CUdpClientSource* pInst = (CUdpClientSource*)pParam;
	pInst->PushData();
	return 0;
}

int CUdpClientSource::PushData()
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
			//printf("read error\n");
#ifdef _WIN32
			Sleep(100);
#else
			usleep(1*1000);
#endif
		}
	}
	m_hReadThread = 0;
	return 0;
}

void CUdpClientSource::CreateReadThread(void** pHandle, int* pID, READSOCKET fProc, void* pParam, int uFlag)
{
#ifdef _WIN32
	DWORD nID;
	*pHandle = ::CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) fProc, pParam, 0, (LPDWORD)&nID);
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