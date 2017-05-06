/*
 *  CUdpClientRecv.h
 *
 *  Created by Lin Jun on 8/24/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

class CUdpClientRecv
{
public:
	CUdpClientRecv();
	virtual ~CUdpClientRecv(void);
	
public:
	int Init(int nPort);
	int Uninit();
	int Read(int nWantRead, int* outRealRead, unsigned char* outBuffer);
	
private:
	int		m_hSocket;
	int		m_nMaxRecvBuffSize;
};