/*
 *  CTSLiveSource.h
 *
 *  Created by Lin Jun on 2011-08-25.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CUdpClientSource_H_
#define _CUdpClientSource_H_

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
	
class CUdpClientRecv;

typedef int (* READSOCKET) (void* pParam);
typedef int (* SENDDATACB) (void * pUserData, unsigned char* pData, int nSize);

class CUdpClientSource
{
public:
	CUdpClientSource(void * pUserData, SENDDATACB fSendData);
	virtual ~CUdpClientSource(void);
	
public:
	virtual bool Open(int nPort);
	virtual bool Close();
	virtual bool Start();
	virtual bool Pause();
	virtual bool Stop();
	
	static int PushDataEntry(void* pParam);
	int PushData();
	
private:
	void CreateReadThread(void** pHandle, int* pID, READSOCKET fProc, void* pParam, int uFlag);
	
private:
	CUdpClientRecv*		m_pUdpClient;
	void*				m_hReadThread;

	SENDDATACB			m_fSendData;
	void*				m_pUserData;
	
	unsigned char*		m_pReadBuffer;
	int					m_nReadBufferSize;
	bool				m_bStop;
};
	
#ifdef _VONAMESPACE
}
#endif

#endif //_CUdpClientSource_H_