// ConsoleAP.cpp : Defines the entry point for the console application.
//

#include "voRTSPSDK.h"
#include "rtsptvparam.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
static char* testURL="rtsp://10.2.64.11/matrix12.mp4";
void CreateLogDirectory();
int  EventHandler(RTSPTV_EVENTTYPE eEventType, unsigned int nParam, void * pEventData)
{
	printf("Event:%d,%d\n",eEventType,nParam);
	return 0;
}

int PayloadReceived(unsigned char * pPayloadData, unsigned int nDataLength)
{
	return 0;
}
int FrameReceived(RTSPTV_FrameType nFrameType, unsigned char * pFrameData, unsigned int nDataLength, unsigned int nStartTime)
{
	printf("Frames:%d,time=%d\n",nFrameType,nStartTime);
	return 0;
}
int main()
{
	HVOFILEREAD	rtspSrc;
	HVOFILETRACK	track[2];
	VOSAMPLEINFO SampleInfo;
	int rc;
	do 
	{
		
		rc=voRTSPSessionCreate(&rtspSrc);
		if(rc)
		{
			printf("fail to  voRTSPSessionCreate\n");
			break;
		}
		CreateLogDirectory();
		VOStreamInitParam m_initParam;
		memset(&m_initParam,0,sizeof(m_initParam));
		m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
		m_initParam.nMinUdpPort		= DEFAULT_PORT_MIN;
		m_initParam.nRTPTimeOut		= DEFAULT_RTPTIMEOUT;
		m_initParam.nBufferTime		= DEFAULT_BUFFERTIME;
		m_initParam.nRTCPInterval	= 5000;
		m_initParam.nFirstIFrame	= 1;
		m_initParam.nExtraRTPPacket	= 1;
		m_initParam.nConnectTimeout	= DEFAULT_RTPTIMEOUT;
		m_initParam.nRTSPTimeout	= DEFAULT_RTPTIMEOUT;
		m_initParam.nSDKlogflag	=2;
		strcpy(m_initParam.szUserAgent,"VisualOn Streaming Player 2.2");

		rc =voRTSPSessionSetParameter(rtspSrc,VOID_STREAMING_INIT_PARAM,(LONG)&(m_initParam));
		RTSPTV_CALLBACK callBackFunc;
		callBackFunc.EventHandler=EventHandler;
		callBackFunc.PayloadReceived=PayloadReceived;
		callBackFunc.FrameReceived=FrameReceived;
		rc =voRTSPSessionSetParameter(rtspSrc,RTSPTV_ParamCallbacks,(LONG)&(callBackFunc));
		rc=voRTSPSessionOpenURL(rtspSrc,testURL,NULL);
		if(rc)
		{
			printf("fail to  voRTSPSessionOpenURL\n");
			break;
		}
		rc =voRTSPTrackOpen(&track[0],rtspSrc,0);
		rc|=voRTSPTrackOpen(&track[1],rtspSrc,1);
		if(rc)
		{
			printf("fail to  voRTSPTrackOpen\n");
			break;
		}
		rc =voRTSPSessionSetParameter(rtspSrc,VOID_FILEREAD_ACTION_START,1);
		rc =voRTSPSessionSetParameter(rtspSrc,VOID_FILEREAD_ACTION_RUN,1);
		//receive data
#define MAX_AUDIO_FRAMES 500
#define MAX_VIDEO_FRAMES 500
#define MAX_FRAME_SIZE		100*1024
		int framesAudio=0;
		int framesVideo=0;
		SampleInfo.pBuffer=new unsigned char[MAX_FRAME_SIZE];
		do 
		{
			SampleInfo.uIndex=framesAudio;
			rc=voRTSPTrackGetSampleByIndex(rtspSrc, track[0], &SampleInfo);
			if(rc==0)
			{
				framesAudio++;
			}
			SampleInfo.uIndex=framesVideo;
			voRTSPTrackGetSampleByIndex(rtspSrc, track[1], &SampleInfo);
			if(rc==0)
			{
				framesVideo++;
			}
			if(framesVideo>100)//test switch channel
				rc =voRTSPSessionSetParameter(rtspSrc,RTSPTV_ParamActionChangeChannel,1);

		} while(framesVideo<MAX_VIDEO_FRAMES&&framesAudio<MAX_AUDIO_FRAMES);
		delete []	SampleInfo.pBuffer;
	} while(0);
	voRTSPSessionSetParameter(rtspSrc,VOID_FILEREAD_ACTION_STOP,1);
	voRTSPTrackClose(rtspSrc,track[0]);
	voRTSPTrackClose(rtspSrc,track[1]);
	voRTSPSessionClose(rtspSrc);
	return 0;
}

