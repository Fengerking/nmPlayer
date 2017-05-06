/*
 *  CvoPlayer.mm
 *
 *  Created by Lin Jun on 4/19/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CvoPlayer.h"

#include <sys/types.h>
#include <sys/sysctl.h>

CvoPlayer::CvoPlayer(void* hWnd)
:m_hPlayer(NULL)
,m_hDrawWnd(hWnd)
,m_pSource(NULL)
,m_nAudioCodecType(VOMP_AUDIO_CodingMP3)
,m_nVideoCodecType(VOMP_VIDEO_CodingMPEG2)
,m_bStop(true)
{
	memset(&m_PlayerApi, 0, sizeof(VO_PLAYER_API));
	voGetLSPAPI(&m_PlayerApi);
}

CvoPlayer::~CvoPlayer()
{
	if(m_pSource)
	{
		m_pSource->Stop();
		delete m_pSource;
		m_pSource = NULL;
	}
	
	if(m_hPlayer)
	{
		Stop();
		Close();
		m_hPlayer = NULL;
	}
}

void CvoPlayer::SetPlayerListener(id <CEventDelegate> cDelegate)
{
	m_cDelegate = cDelegate;
}

int CvoPlayer::VOLSPListener(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	CvoPlayer* pPlayer = (CvoPlayer*)pUserData;
	if(pPlayer)
	{
		return pPlayer->ProcessListener(nID, pParam1, pParam2);
	}
	
	return -1;
}

int CvoPlayer::ProcessListener(int nID, void * pParam1, void * pParam2)
{
	return [m_cDelegate handleEvent:nID withParam1:pParam1 withParam2:pParam2];
}

int	CvoPlayer::Open()
{
	if(m_hPlayer)
		return 0;
	
	VO_PLAYER_OPEN_PARAM initParam;
	memset(&initParam, 0, sizeof(VO_PLAYER_OPEN_PARAM));
	initParam.hDrawWnd  = m_hDrawWnd;
	initParam.pListener = CvoPlayer::VOLSPListener;
	initParam.pUserData	= this;
	
	
	NSBundle *bundle = [NSBundle mainBundle];
	if (bundle) 
	{
	    const char* path = [[bundle pathForResource:@"vompPlay" ofType:@"cfg"] UTF8String];
		
		if(path)
			strcpy(initParam.szCfgFilePath, path);
	}
	 
	
	m_bStop = true;
	
	return m_PlayerApi.voplayerOpen(&m_hPlayer, &initParam);
}

int CvoPlayer::OpenSource(bool bFile)
{
	if (NULL != m_pSource) {
		return 0;
	}
	
	m_pSource = new CReadSource(this, PushBufferCallback);

	unsigned char* pUrl = NULL;
	
/*	// open source and start
#if TARGET_IPHONE_SIMULATOR

	pUrl = (unsigned char*)"/Users/huangjiafa/0007#008_MPEGTS_MPEGV2'M@M_MPEGA'L2_2641K'34s200ms'720x576'25f'2c'48KHz.ts";
	pUrl = (unsigned char*)"/Users/huangjiafa/0007#009_MPEGTS_MPEGV2'M@M_MPEGA'L2_3112K'39s253ms'704x480'30f'2c'48KHz.ts";
	pUrl = (unsigned char*)"/Users/huangjiafa/0007#007_MPEGTS_MPEGV2'M@M_MPEGA'L2_3561K'18s800ms'720x576'25f'2c'48KHz.ts";
	pUrl = (unsigned char*)"/Users/huangjiafa/10023355_MPEGTS_MPEGV2'MM_841K'2m57s'320x240'25f.ts";
	
#else	
	NSString *filePath = nil;
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/0007#008_MPEGTS_MPEGV2'M@M_MPEGA'L2_2641K'34s200ms'720x576'25f'2c'48KHz.ts"];
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/0007#009_MPEGTS_MPEGV2'M@M_MPEGA'L2_3112K'39s253ms'704x480'30f'2c'48KHz.ts"];
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/0007#007_MPEGTS_MPEGV2'M@M_MPEGA'L2_3561K'18s800ms'720x576'25f'2c'48KHz.ts"];
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/10023355_MPEGTS_MPEGV2'MM_841K'2m57s'320x240'25f.ts"];
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/5.ts"];
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/0007#003_MPEGTS_MPEGV2_MPEGA'L2_1361K'40s692ms'352x288'25f'2c'44KHz.ts"];
	
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/ts.ts"];
	pUrl = (unsigned char*)[filePath UTF8String];
#endif*/
	NSString *filePath = nil;
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/ts.ts"];
	pUrl = (unsigned char*)[filePath UTF8String];
	
	m_pSource->SetStream(pUrl, 1234, bFile);

	return 0;
}

int CvoPlayer::CloseSource()
{
	if(m_pSource)
	{
		m_pSource->Stop();
		delete m_pSource;
		m_pSource = NULL;
	}
	
	return 0;
}


int	CvoPlayer::Close()
{
	CloseSource();
	
	if(m_hPlayer)
	{
		int nRet = m_PlayerApi.voplayerClose(m_hPlayer);
		m_hPlayer = NULL;
		return nRet;
	}
	
	m_bStop = true;
	
	return -1;
}

int CvoPlayer::SetDataSource(void* pSource)
{
	if(m_hPlayer)
	{
		return m_PlayerApi.voplayerSetDataSource(m_hPlayer, pSource);
	}		
	
	return -1;
}

int CvoPlayer::Run(bool bFile)
{
	if(m_hPlayer)
	{
		int nRet = m_PlayerApi.voplayerRun(m_hPlayer);
		
		m_bStop = false;
		OpenSource(bFile);
		
		if(m_pSource)
		{
			m_pSource->Start();
		}
		
		return nRet;
	}
	
	return -1;
}

int CvoPlayer::Stop()
{
	if(m_hPlayer)
	{
		if(m_pSource) {
			m_pSource->Stop();
		}
		
		CloseSource();
		
		m_bStop = true;
		return m_PlayerApi.voplayerStop(m_hPlayer);
	}
	
	return -1;
}

int CvoPlayer::SwitchProgram(int nIndex)
{
	// change streaming data
//	m_pSource->SetStream(pUrl);
	
	return -1;
}

int CvoPlayer::GetParam(int nParamID, void* pParam)
{
	if(m_hPlayer)
	{
		return m_PlayerApi.voplayerGetParam(m_hPlayer, nParamID, pParam);
	}
	
	return 0;
}

int CvoPlayer::SetParam(int nParamID, void* pParam)
{
	if(m_hPlayer)
	{
		return m_PlayerApi.voplayerSetParam(m_hPlayer, nParamID, pParam);
	}
	
	return 0;
}

int CvoPlayer::SetDrawArea(int nLeft, int nTop, int nRight, int nBottom)
{
	if(m_hPlayer)
	{
		return m_PlayerApi.voplayerSetDrawArea(m_hPlayer, nLeft, nTop, nRight, nBottom);
	}
	
	return 0;
}

int CvoPlayer::GetStatus(VOMP_STATUS* pStatus)
{
	if(m_hPlayer)
	{
		return m_PlayerApi.voplayerGetStatus(m_hPlayer, pStatus);
	}
	
	return -1;
}

int CvoPlayer::GetDeviceType()
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    /*
     Possible values:
     "iPhone1,1" = iPhone 1G
     "iPhone1,2" = iPhone 3G
     "iPhone3,1" = iPhone 4
	 "iPhone2,1" = iPhone 3GS
     "iPod1,1"   = iPod touch 1G
     "iPod2,1"   = iPod touch 2G
     */
	
	int type = IPHONE_4;
	
	if (!strcmp(machine, "iPhone1,1")) 
	{
		type = IPHONE_1G;
	}
	else if(!strcmp(machine, "iPhone1,2"))
	{
		type = IPHONE_3G;
	}
	else if(!strcmp(machine, "iPhone3,1"))
	{
		type = IPHONE_4;
	}
	else if(!strcmp(machine, "iPhone2,1"))
	{
		type = IPHONE_3GS;
	}
	else if(!strcmp(machine, "iPod1,1"))
	{
		type = IPOD_TOUCH_1G;
	}
	else if(!strcmp(machine, "iPod2,1"))
	{
		type = IPOD_TOUCH_2G;
	}
	else if(!strcmp(machine, "iPad1,1"))
	{
		type = IPAD_1G;
	}
	else if(!strcmp(machine, "iPad2,1"))
	{
		type = IPAD_2G;
	}
	
	printf("MACHINE %s\n", machine);
	
	free(machine);
	
    return type;
}

VO_U32 CvoPlayer::PushBufferCallback(void* pUserData, VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer)
{
	if(pUserData)
	{
		CvoPlayer* pPlayer = (CvoPlayer*)pUserData;
		
		return pPlayer->m_PlayerApi.voplayerSendBuffer((VO_HANDLE)pPlayer->m_hPlayer, eInputType, pBuffer);
	}
	return -1;
}

