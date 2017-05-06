// ConsoleAP.cpp : Defines the entry point for the console application.
//

#include "voRTSP.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifndef UNDER_CE
#include <crtdbg.h>
#endif //UNDER_CE
#endif

#ifdef _DEBUG
#ifndef UNDER_CE	
#define  new   new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif //UNDER_CE	//doncy
#endif

static char* testURL="rtsp://10.2.64.11/source/3gp/mpeg4amrnb/mpeg4_qvga_10fps_amrnb_12kbps_8khz.3gp";
int  VO_API CALLBACK_NOTIFY2 (long EventCode, long * EventParam1, long * userData)
{
	printf("Get callback Event %d\n",EventCode);
	return 0;
}

int main()
{
//	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)  |  _CRTDBG_LEAK_CHECK_DF);		//conmented by doncy 0813

#ifndef UNDER_CE
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)  |  _CRTDBG_LEAK_CHECK_DF);
#endif //UNDER_CE
	//_CrtSetBreakAlloc(71);
	//_CrtSetBreakAlloc(62);

	VO_SOURCE_READAPI rtspAPI;
	VO_SOURCE_OPENPARAM		openParam;
	StreamingNotifyEventFunc	notify={CALLBACK_NOTIFY2,0};
	if(voGetRTSPReadAPI (&rtspAPI,0))
	{
		printf("loading RTSPReaderAPI fail\n");
		return -1;
	}
	//init param
	VOStreamInitParam m_initParam;
	memset(&m_initParam,0,sizeof(m_initParam));
	m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
	m_initParam.nMinUdpPort			= DEFAULT_PORT_MIN;
	m_initParam.nRTPTimeOut		= DEFAULT_RTPTIMEOUT;
	m_initParam.nBufferTime			= DEFAULT_BUFFERTIME;
	m_initParam.nRTCPInterval		= 5000;
	m_initParam.nFirstIFrame			= 1;
	m_initParam.nExtraRTPPacket	= 1;
	m_initParam.nConnectTimeout	= DEFAULT_RTPTIMEOUT;
	m_initParam.nRTSPTimeout		= DEFAULT_RTPTIMEOUT;
	m_initParam.nSDKlogflag				=2;//for test
	strcpy(m_initParam.szUserAgent,"VisualOn Streaming Player 2.2");

	VO_PTR* handle	= NULL;
	do 
	{
		int rc,i;
		VO_SOURCE_INFO				src_info={0};
		openParam.pSource				=  testURL;
		openParam.nReserve			=  (long)&m_initParam;
		openParam.pSourceOP			= &notify;//add notify 
		rc=rtspAPI.Open((VO_PTR*)&handle,&openParam);
		if(rc)
		{
			printf("fail to  rtspAPI.Open\n");
			break;
		}
		rc=rtspAPI.SetSourceParam(handle,VOID_STREAMING_OPENURL, NULL);

		rc = rtspAPI.GetSourceInfo(handle,&src_info);
		printf("rtspAPI.GetSourceInfo:trackCount=%d,duration=%d\n",src_info.Tracks,src_info.Duration);
		VO_SOURCE_TRACKINFO trackInfos[2];
		for (i=0;i<src_info.Tracks;i++)
		{
			rtspAPI.GetTrackInfo(handle,i,&trackInfos[i]);
			printf("GetTrackInfo_%d,codec=0x%X,duration=%d,headSize=%d\n",i,trackInfos[i].Codec,trackInfos[i].Duration,trackInfos[i].HeadSize);
		}
		//test play/pasue
		long params=0;
		rc =rtspAPI.SetSourceParam(handle,VO_RTSP_PLAY,&params);
		rc =rtspAPI.SetSourceParam(handle,VO_RTSP_PAUSE,&params);
		rc =rtspAPI.SetSourceParam(handle,VO_RTSP_PLAY,&params);
		//receive data
#define MAX_AUDIO_FRAMES 100
#define MAX_VIDEO_FRAMES 100
#define MAX_FRAME_SIZE		100*1024
		int framesAudio=0;
		int framesVideo=0;
		VO_SOURCE_SAMPLE SampleInfos[2];
		SampleInfos[0].Buffer =new unsigned char[MAX_FRAME_SIZE];
		SampleInfos[1].Buffer =new unsigned char[MAX_FRAME_SIZE];
		SampleInfos[0].Time=0;
		SampleInfos[1].Time=0;
		char *test = new char[200];

		do 
		{
			for (i=0;i<src_info.Tracks;i++)
			{
				rc=rtspAPI.GetSample(handle,i, &SampleInfos[i]);
				if(rc==0)
				{
					if(trackInfos[i].Type==VO_SOURCE_TT_RTSP_VIDEO)
					{
						framesVideo++;
						if(framesVideo==200)
						{
							
							VO_S64 pos=30000;
							for (i=0;i<src_info.Tracks;i++)
							{
								rtspAPI.SetPos(handle,i,&pos);
							}
						}
					}
					else
						framesAudio++;
				}
			}
			
			
		} while(framesVideo<MAX_VIDEO_FRAMES&&framesAudio<MAX_AUDIO_FRAMES);

		delete []	SampleInfos[0].Buffer;
		delete []   SampleInfos[1].Buffer;
		rc =rtspAPI.SetSourceParam(handle,VO_RTSP_STOP,&params);
		
	} while(0);
	rtspAPI.Close(handle);
	//_CrtDumpMemoryLeaks();
	return 0;
}

