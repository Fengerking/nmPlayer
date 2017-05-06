// ConsoleAP.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include "voPD.h"
#include <stdlib.h>
#include <stdio.h>

#include <memory.h>
#include <string.h>
static TCHAR* testURL=_T("http://10.2.64.11/xietfeng_h263_aac_qcif.3gp");//_T("http://10.2.64.11/matrix12.mp4");//
int  VO_API CALLBACK_NOTIFY2 (long EventCode, long * EventParam1, long * userData)
{
	printf("Get callback Event %d\n",EventCode);
	return 0;
}
int main()
{
	VO_SOURCE_READAPI pdAPI;
	VO_SOURCE_OPENPARAM		openParam;
	StreamingNotifyEventFunc	notify={CALLBACK_NOTIFY2,0};
	if(voGetPDReadAPI (&pdAPI,0))
	{
		printf("loading pdReaderAPI fail\n");
		return -1;
	}
	//init param
	VOPDInitParam	 m_initParam;
	memset(&m_initParam,0,sizeof(m_initParam));
	//TO_DO:init params
	memset(&m_initParam,0,sizeof(m_initParam));
	m_initParam.nBufferTime = 5;			//<S>
	m_initParam.nPlayBufferTime = 90;		//<S>
	m_initParam.nHTTPDataTimeOut = 15000;	//<MS>
	m_initParam.nMaxBuffer = 5120;			//<KB>
	m_initParam.nTempFileLocation = 1;		//storage
	m_initParam.nPacketLength = 5;			//<KB>
	m_initParam.nHttpProtocol =  1;
	m_initParam.nSDKlogflag   = 2;
	strcpy((char*)m_initParam.mUserAgent, "VisualOn-PD Player");
	strcpy((char*)m_initParam.mProxyName, "Proxy test");
#ifdef _WIN32_WCE
	_tcscpy(m_initParam.mFilePath, _T("\\tmp.3gp"));
#else//_WIN32_WCE
	_tcscpy(m_initParam.mFilePath, _T("C:\\visualon\\VOPD\\PDSource.3gp"));
#endif//_WIN32_WCE

	VO_PTR* handle	= NULL;
	do 
	{
		int rc,i;
		
		VO_SOURCE_INFO				src_info={0};
		openParam.pSource				=  testURL;
		openParam.nReserve			=  (long)&m_initParam;
		openParam.pSourceOP			= &notify;//add notify 
		rc=pdAPI.Open((VO_PTR*)&handle,&openParam);
		if(rc)
		{
			printf("fail to  pdAPI.Open\n");
			break;
		}
		rc=pdAPI.SetSourceParam(handle,VOID_STREAMING_OPENURL,0);
		if(rc)
		{
			printf("fail to  VOID_STREAMING_OPENURL\n");
			break;
		}
		rc = pdAPI.GetSourceInfo(handle,&src_info);
		printf("pdAPI.GetSourceInfo:trackCount=%d,duration=%d\n",src_info.Tracks,src_info.Duration);
		VO_SOURCE_TRACKINFO trackInfos[2];
		for (i=0;i<src_info.Tracks;i++)
		{
			pdAPI.GetTrackInfo(handle,i,&trackInfos[i]);
			printf("GetTrackInfo_%d,codec=0x%X,duration=%d,headSize=%d\n",i,trackInfos[i].Codec,trackInfos[i].Duration,trackInfos[i].HeadSize);
		}
		//test play/pasue
		long params=0;
		rc =pdAPI.SetSourceParam(handle,VO_PD_PLAY,&params);
		//rc =pdAPI.SetSourceParam(handle,VO_PD_PAUSE,&params);
		//rc =pdAPI.SetSourceParam(handle,VO_PD_PLAY,&params);
		//receive data
#define MAX_AUDIO_FRAMES 500
#define MAX_VIDEO_FRAMES 500
#define MAX_FRAME_SIZE		100*1024
		int framesAudio=0;
		int framesVideo=0;
		VO_SOURCE_SAMPLE SampleInfos[2];
		SampleInfos[0].Buffer =new unsigned char[MAX_FRAME_SIZE];
		SampleInfos[1].Buffer =new unsigned char[MAX_FRAME_SIZE];
		SampleInfos[0].Time=0;
		SampleInfos[1].Time=0;
		do 
		{
			for (i=0;i<src_info.Tracks;i++)
			{
				rc=pdAPI.GetSample(handle,i, &SampleInfos[i]);
				if(rc==0)
				{
					if(trackInfos[i].Type==VO_SOURCE_TT_VIDEO)
					{
						framesVideo++;
						if(framesVideo==200)
						{
							
							VO_S64 pos=30000;
							for (i=0;i<src_info.Tracks;i++)
							{
								pdAPI.SetPos(handle,i,&pos);
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
		rc =pdAPI.SetSourceParam(handle,VO_PD_STOP,&params);
		
	} while(0);
	pdAPI.Close(handle);
	
	return 0;
}

