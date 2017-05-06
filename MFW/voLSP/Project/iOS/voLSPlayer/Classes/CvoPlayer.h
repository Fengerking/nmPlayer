/*
 *  CvoPlayer.h
 *
 *  Created by Lin Jun on 4/19/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _C_VO_PLAYER_H_
#define _C_VO_PLAYER_H_

#include "volspAPI.h"
#include "vompType.h"
#include "CReadSource.h"
#import "CEventDelegate.h"

typedef enum IOS_DEVICE_TYPE
{
	IPHONE_1G		= 0,
	IPHONE_3G,
	IPHONE_4,
	IPHONE_3GS,
	IPOD_TOUCH_1G,
	IPOD_TOUCH_2G,
	IPAD_1G,
	IPAD_2G
}IOS_DEVICE_TYPE;


class CvoPlayer
{
public:
	CvoPlayer(void* hWnd);
	virtual ~CvoPlayer();

public:
	int	Open();
	int	Close();
	int SetDataSource(void* pSource);
	int Run(bool bFile);
	int Stop();
	int SwitchProgram(int nIndex);
	int GetParam(int nParamID, void* pParam);
	int SetParam(int nParamID, void* pParam);
	int SetDrawArea(int nLeft, int nTop, int nRight, int nBottom);
	int GetStatus(VOMP_STATUS* pStatus);
	
	int GetDeviceType();
	
	static VO_U32 PushBufferCallback(void* pUserData, VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);
	
	static int VOLSPListener(void * pUserData, int nID, void * pParam1, void * pParam2);
	int ProcessListener(int nID, void * pParam1, void * pParam2);
	
	void SetPlayerListener(id <CEventDelegate> cDelegate);
	
private:
	int OpenSource(bool bFile);
	int CloseSource();
	
private:
	void*			m_hDrawWnd;
	VO_PLAYER_API	m_PlayerApi;
	VO_HANDLE		m_hPlayer;
	CReadSource*	m_pSource;
	
	int				m_nAudioCodecType;
	int				m_nVideoCodecType;
	bool			m_bStop;
	
	id <CEventDelegate>	m_cDelegate;
};

#endif

