     	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#include <cutils/properties.h>
#endif
enum{
	DMP_AUDIO=1,
	DMP_VIDEO=2
};
#include "stdlib.h"
#include "COMXMobiTVDataSourcePlayer.h"
#include "voLog.h"
#include "voaEngine.h"
#include "vomeplayer.h"


#include "voOMXThread.h"
#include "voOMXOSFun.h"
#include "MKBase/Result.h"
#include "MKCommon/Event.h"
#include "MKCommon/State.h"
#pragma warning (disable : 4996)
using namespace android; 

void BufferFunc(void* aObj, MK_Bool aOn, MK_U32 aFill);
void ErrorFunc(void* aObj, MK_Result aErr);
void EventFunc(void* aObj, MK_Event aEvt, const void* aArg);
void StateFunc(void* aObj, MK_State aWas, MK_State aIs);
static char* strVersion="2010.1223.1040";
/*buffer:the data  at the beginning of  NALU
1 : I frame
0 : P frame or other NALU
The input buffer excludes 0x01000000
*/
int IsH264IntraFrame(char * buffer,int size)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len,value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;
	
	if(naluType==5)
		return 1;
	if(naluType==1)//need continuous check
		buffer++;
	else//the nalu type is params info
		return 0;

	if(size<100)//TBD,for the sake,check the size
		return 0;

	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return 0;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf==2||inf==7)//I frame
		return 1;
	else if (inf!=0&&inf!=5)//P frame
	{
		return 0;
	}

	return 0;           
}
extern char vo_application_location[256];
COMXMobiTVDataSourcePlayer::COMXMobiTVDataSourcePlayer(void)
	: COMXEngine ()
	, m_pCompSrc (NULL)
	, m_pCompAD (NULL)
	, m_pCompVD (NULL)
	, m_pCompAR (NULL)
	, m_pCompVR (NULL)
	, m_pCompClock (NULL)
	, m_hThreadVideo (NULL)
	, m_hThreadAudio(NULL)
	, m_hThreadTrigger(NULL)
	, m_bVideoStop (false)
//	, lPlayer(NULL)
	,m_duration(0)
	,m_endTime(0x7fffffff)
	,m_mobiTVEOS(0)
	, m_pCallBackMsgHandler (NULL)
	, m_pUserData (NULL)
	,m_seeking(false)
	,m_currentPos(0)
	, m_nAudioDataFormat (0)
	, m_hAudioDataFile (NULL)
	, m_nVideoDataFormat (0)
	, m_hVideoDataFile (NULL)
	, m_nLogLevel (0)
	, m_bNetError (false)
	,m_vrType(VOME_C_RENDER)
	,m_arType(VOME_JAVA_RENDER)
	,m_bDisableInputVOME(false)
	,m_bIsBuffering(false)
{
    VOLOGI ("gMKPlayerFunc.lPlayer = 0x%x,version=%s", gMKPlayerFunc.lPlayer,strVersion);
	char szTmp[256];
	
	memset (szTmp, 0, sizeof (szTmp));
	property_get("ro.product.manufacturer", szTmp, "VisualOn");
	strcpy(m_deviceName,szTmp);
	strcat(m_deviceName,".");
	memset (szTmp, 0, sizeof (szTmp));
	property_get("ro.product.name", szTmp, "VisualOn");
	strcat(m_deviceName,szTmp);
	strcat(m_deviceName,".");
	memset (szTmp, 0, sizeof (szTmp));
	property_get("ro.product.model", szTmp, "VisualOn");
	strcat(m_deviceName,szTmp);
	VOLOGI("@@Device Property=%s",m_deviceName);
	m_pConfig = new voCOMXBaseConfig ();
	if (m_pConfig != NULL)
	{
		char szCfgFile[256];
		strcpy (szCfgFile, vo_application_location);
		strcat (szCfgFile, "vomeplay.cfg");

		VOLOGI ("Config File is %s", szCfgFile);

		if(!m_pConfig->Open (szCfgFile))
		{
			VOLOGI("open config file fail %s",strerror(errno));
		}

		char * pDumpFile = m_pConfig->GetItemText ("vomeMF", "DumpAudioDataFile");
		if (pDumpFile != NULL)
		{
			m_hAudioDataFile = voOMXFileOpen (pDumpFile, VOOMX_FILE_READ_WRITE);
			m_nAudioDataFormat =  m_pConfig->GetItemValue ("vomeMF", "DumpAudioDataFormat", 0);
			VOLOGI("Dump:audio file=%s,fmt=%d",pDumpFile,m_nAudioDataFormat);
		}

		pDumpFile = m_pConfig->GetItemText ("vomeMF", "DumpVideoDataFile");
		if (pDumpFile != NULL)
		{
			
			m_hVideoDataFile = voOMXFileOpen (pDumpFile, VOOMX_FILE_READ_WRITE);
			m_nVideoDataFormat =  m_pConfig->GetItemValue ("vomeMF", "DumpVideoDataFormat", 0);
			VOLOGI("Dump:video file=%s,fmt=%d",pDumpFile,m_nVideoDataFormat);
		}
		m_nLogLevel =  m_pConfig->GetItemValue ("vomeMF", "LogLevel", 1);
		VOLOGI("Loglevel=%d",m_nLogLevel);
		//pDumpFile = m_pConfig->GetItemText ("RenderType", "test");
		//if(pDumpFile) VOLOGI("test=%s",pDumpFile);
	
		m_vrType    = m_pConfig->GetItemValue ("vomeMF", "DefaultVideoRenderType",VOME_C_RENDER);
		char * javaVRList = m_pConfig->GetItemText ("vomeMF", "JavaVideoRenderDeviceList");
		if(m_vrType==VOME_C_RENDER&&javaVRList)
		{
			VOLOGI("Dump:javaVRList=%s",javaVRList);
			if (strstr(javaVRList,m_deviceName))
			{
				m_vrType = VOME_JAVA_RENDER;
				VOLOGI("match javaVRList:%s",javaVRList);
			}
		}
		int		isPlayDumpedFile =  m_pConfig->GetItemValue ("vomeMF", "TestEnable", 0);
		if(isPlayDumpedFile==1)
		{
			char*	audioDumpFile	 =  m_pConfig->GetItemText ("vomeMF", "TestAudioFile");
			char*	videoDumpFile	 =	m_pConfig->GetItemText ("vomeMF", "TestVideoFile");
			m_testDumpfile.Enable(true);
			m_testDumpfile.SetMediaFile(audioDumpFile,videoDumpFile);
			VOLOGI("test Dump file:%s and %s",audioDumpFile,videoDumpFile);
		}
		int	disableInputVOMEFlag = m_pConfig->GetItemValue ("vomeMF", "DisableInputVOME", 0);
		m_bDisableInputVOME = disableInputVOMEFlag==1;
		VOLOGI("DisableInputVOME=%d",disableInputVOMEFlag);

	}	

	if (NULL != gMKPlayerFunc.lPlayer)
	{
	  MK_Result lRes;
	    //lRes = gMKPlayerFunc.VO_MK_Player_SetBufferFunc(gMKPlayerFunc.lPlayer, BufferFunc, this);
	    //VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_SetBufferFunc lRes=0x%08x", lRes);
	  lRes = gMKPlayerFunc.VO_MK_Player_SetErrorFunc(gMKPlayerFunc.lPlayer, ErrorFunc, this);
	  VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_SetErrorFunc lRes=0x%08x", lRes);
	  lRes = gMKPlayerFunc.VO_MK_Player_SetEventFunc(gMKPlayerFunc.lPlayer, EventFunc, this);
	  VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_SetEventFunc lRes=0x%08x", lRes);
	    //lRes = gMKPlayerFunc.VO_MK_Player_SetStateFunc(gMKPlayerFunc.lPlayer, StateFunc, this);
	    //VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_SetStateFunc lRes=0x%08x", lRes);
	}

    VOLOGI ("<<<DaSoPl>>> leave");
}

COMXMobiTVDataSourcePlayer::~COMXMobiTVDataSourcePlayer ()
{
    VOLOGI ("<<<DaSoPl>>> enter");		
	if (m_hAudioDataFile != NULL)
		voOMXFileClose (m_hAudioDataFile);

	if (m_hVideoDataFile != NULL)
		voOMXFileClose (m_hVideoDataFile);

	delete m_pConfig;
    MK_Result lRes;

    if (NULL != gMKPlayerFunc.lPlayer) {
      lRes = gMKPlayerFunc.VO_MK_Player_SetBufferFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
      lRes = gMKPlayerFunc.VO_MK_Player_SetErrorFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
      lRes = gMKPlayerFunc.VO_MK_Player_SetEventFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
      lRes = gMKPlayerFunc.VO_MK_Player_SetStateFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
    }

    Close();
   
    VOLOGI ("<<<DaSoPl>>> leave");
}


const int sampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};
void COMXMobiTVDataSourcePlayer::WriteADTSHead(int sampFreq, int chanNum,int framelen,unsigned char *adtsbuf)
{
//    VOLOGI ("<<<DaSoPl>>> enter");
	int sampIdx ,object=1;//LC
	adtsbuf[0] = 0xFF; adtsbuf[1] = 0xF9;
	for (int idx = 0; idx < 12; idx++) {
		if (sampFreq == sampRateTab[idx]) {
			sampIdx = idx;
			break;
		}
	}
	adtsbuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

	framelen += 7;



	adtsbuf[3] = (chanNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
#if 0
	adtsbuf[5] = ((framelen & 7) << 5) ;//| 0x1F;
	adtsbuf[6] = 0x00;	//0xFC
#else
	adtsbuf[5] = ((framelen & 7) << 5) | 0x1F;
	adtsbuf[6] = 0xFC;
#endif
//    VOLOGI ("<<<DaSoPl>>> leave");
}
void COMXMobiTVDataSourcePlayer::UpdateADTSHead(int framelen,unsigned char *adtsbuf)
{
//    VOLOGI ("<<<DaSoPl>>> enter");
	framelen += 7;
	adtsbuf[3] = (m_channelNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
#if 1
	adtsbuf[5] = ((framelen & 7) << 5) ;//| 0x1F;
	adtsbuf[6] = 0x00;	//0xFC
#else
	adtsbuf[5] = ((framelen & 7) << 5) | 0x1F;
	adtsbuf[6] = 0xFC;
#endif
//    VOLOGI ("<<<DaSoPl>>> leave");
}
#define MOBITV_BUFFER_SRC 1120
#define MOBITV_ERROR_SRC 1121
#define MOBITV_EVENT_SRC 1122
#define MOBITV_STATE_SRC 1123
void BufferFunc(void* aObj, MK_Bool aOn, MK_U32 aFill)
{
	reinterpret_cast<COMXMobiTVDataSourcePlayer*>(aObj)->MyBufferFunc(aOn, aFill);
}
#define  VOLOGI3		
void COMXMobiTVDataSourcePlayer::MyBufferFunc(MK_Bool aOn, MK_U32 aFill)
{
  VOLOGI3 ("<<<DaSoPl>>> enter");
#if 0
  static int inIF = 0;
  if (aOn)
    {
      VOLOGI3("<MKPlayer:> Buffering %u percent", aFill);
      VOLOGI3("<MKPlayer:> Buffering: %p %p", m_pCallBackMsgHandler, m_pUserData);

      if (m_pCallBackMsgHandler!=NULL && m_pUserData!=NULL) {
	VOA_NOTIFYEVENT event;
	event.msg = VOME_BUFFERING_UPDATE;
	event.ext1 = aFill;
	event.ext2 = MOBITV_BUFFER_SRC;
	m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
      }

      inIF = 1;
    }
  else
    {
      if (inIF==1) {
	VOLOGI3("<MKPlayer:> Buffering complete", aFill);
	if (m_pCallBackMsgHandler!=NULL && m_pUserData!=NULL) {
	  VOA_NOTIFYEVENT event;
	  event.msg = VOME_BUFFERING_UPDATE;
	  event.ext1 = 100;
	  event.ext2 = MOBITV_BUFFER_SRC;
	  m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	inIF = 0;
      } else {
	VOLOGI3("<MKPlayer:> Not buffering", aFill);
      }
    }
#endif
  VOLOGI3 ("<<<DaSoPl>>> leave");
}


void ErrorFunc(void* aObj, MK_Result aErr)
{
	reinterpret_cast<COMXMobiTVDataSourcePlayer*>(aObj)->MyErrorFunc(aErr);
}

void COMXMobiTVDataSourcePlayer::MyErrorFunc(MK_Result aErr)
{
   VOLOGE("<MKPlayer:> errID=%08x", aErr);
   if (m_pCallBackMsgHandler!=NULL && m_pUserData!=NULL) {
	   VOA_NOTIFYEVENT event;
	   //event.msg = VOME_ERROR;
	   //event.ext1 = aErr; 
	   //event.ext2 = MOBITV_ERROR_SRC; 
	   event.msg = VOME_INFO;
	   event.ext1 = 0xdead;    
	   event.ext2 = aErr;   
	   m_bNetError = true;
	   m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
   }
}


void EventFunc(void* aObj, MK_Event aEvt, const void* aArg)
{
  reinterpret_cast<COMXMobiTVDataSourcePlayer*>(aObj)->MyEventFunc(aEvt, aArg);
}

void COMXMobiTVDataSourcePlayer::MyEventFunc(MK_Event aEvt, const void* aArg)
{
  MK_Result* lArg = (MK_Result*)aArg;
  //VOLOGI("<MKPlayer:> evtID=%d \"%s\" aArg=%p *aArg=0x%08x", aEvt, MK_Event_ToCStr(aEvt), aArg, aArg != NULL ? *aArg : 0);
  VOLOGI("<MKPlayer:> evtID=%d \"%s\" aArg=%p *aArg=0x%08x", aEvt, MK_Event_ToCStr(aEvt), lArg, lArg != NULL ? *lArg : 0);
  switch (aEvt)
    {
    case MK_kEvent_End:
		{
		   VOLOGI ("MK_kEvent_End, Arg 0x%08x", lArg ? *lArg : 0);
		   int value = lArg?*lArg:0;
		   m_mobiTVEOS = value;
		  if (m_mobiTVEOS==0)//it is impossible but just for safe check
		  {
			  m_mobiTVEOS = 1;
		  }
		  if(value)//error happens
		  {
			  VOA_NOTIFYEVENT event;
			  event.msg = VOME_INFO;
			  event.ext1 = 0xdead;   
			  event.ext2 = value;
			  m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		  }
		 
		}
      break;      
    case MK_kEvent_Reset:
      VOLOGI ("MK_kEvent_Reset");
      break;
    case MK_kEvent_Flush:
      VOLOGI ("MK_kEvent_Flush");
      break;
    case MK_kEvent_Start:
      {
	VOLOGI ("MK_kEvent_Start");
	// We use the getstarttime in setcurpos()
	//unsigned ms = MK_Time_Scale((MK_Time)(*aArg), 1000, 1, MK_FALSE);      
	//OMX_ERRORTYPE lRes = m_OMXEng.SetCurPos(m_hEngine, ms);
      }
      break;
    default:
      break;
    }
#if 0
  if (m_pCallBackMsgHandler!=NULL && m_pUserData!=NULL) {
    VOA_NOTIFYEVENT event;
    event.msg = VOME_INFO;
    event.ext1 = aEvt;
    event.ext2 = MOBITV_EVENT_SRC;
    m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
  }
#endif
}


void StateFunc(void* aObj, MK_State aWas, MK_State aIs)
{
	reinterpret_cast<COMXMobiTVDataSourcePlayer*>(aObj)->MyStateFunc(aWas, aIs);
}

void COMXMobiTVDataSourcePlayer::MyStateFunc(MK_State aWas, MK_State aIs)
{
	VOLOGI("<MKPlayer:> state changed from %d \"%s\" to %d \"%s\"", aWas, MK_State_ToCStr(aWas), aIs, MK_State_ToCStr(aIs));
	if (m_pCallBackMsgHandler!=NULL && m_pUserData!=NULL) 
	{
		VOA_NOTIFYEVENT event;
		event.msg = VOME_INFO;
		event.ext1 = (aWas<<16)|aIs;
		event.ext2 = MOBITV_STATE_SRC;
		m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
}


OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::SetMsgHandler(VOACallBack pFunc, void * pUserData)
{
  m_pCallBackMsgHandler = pFunc;  
  m_pUserData = pUserData;
  return OMX_ErrorNone;
}


int COMXMobiTVDataSourcePlayer::OpenURL(char* url)
{
  VOLOGI ("<<<DaSoPl>>> enter");

  m_bNetError = false;
  if (gMKPlayerFunc.lPlayer==NULL)
    {
      VOLOGI ("<<<DaSoPl>>> leave");
      return -1;
    }

  //VOLOGI("Turn of BA");
  //gMKPlayerFunc.VO_MK_Player_HTTP_SetUseBA(gMKPlayerFunc.lPlayer, MK_FALSE);
  VOLOGI("gonna gMKPlayerFunc.VO_MK_Player_open...%s",url);
  MK_Result lRes = gMKPlayerFunc.VO_MK_Player_Open(gMKPlayerFunc.lPlayer, url);
  VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_Open lRes=0x%08x", lRes);
  //gMKPlayerFunc.VO_MK_Player_HTTP_SetUseBA(gMKPlayerFunc.lPlayer, MK_FALSE);
  m_lRes_open = lRes;
  if (MK_OK(lRes))
    //    if (MK_OK(gMKPlayerFunc.VO_MK_Player_Open(gMKPlayerFunc.lPlayer, url)))
    {
      VOLOGI("gMKPlayerFunc.VO_MK_Player_Open done");
      int lTrackIdx;
      const MK_TrackInfo* lTrackInfo = 0;
      int trackCount=gMKPlayerFunc.VO_MK_Player_GetTrackCount(gMKPlayerFunc.lPlayer);
      VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_GetTrackCount");
      VOLOGI("trackCount: %d", trackCount);
      int duration=0;
      int endTime = 0;
      for (lTrackIdx = 0; lTrackIdx < trackCount; ++lTrackIdx)
	{ 
	  VOLOGI("track index: %d", lTrackIdx);
	  
	  lTrackInfo = gMKPlayerFunc.VO_MK_Player_GetTrackInfo(gMKPlayerFunc.lPlayer, lTrackIdx);
	  VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_GetTrackInfo");
	  if (lTrackInfo && kMediaFormat_Unknown != lTrackInfo->Format)
	    {
	      duration = MK_Time_Scale((lTrackInfo->EndTime-lTrackInfo->BeginTime), 1000, 1, MK_FALSE); 
	      endTime  = MK_Time_Scale((lTrackInfo->EndTime), 1000, 1, MK_FALSE); 
	      if (m_duration<duration)
		{
		  m_duration = duration;
		}
	      
	      if (m_endTime>endTime)
		{
		  m_endTime=endTime;
		}
	      // Get track information...Only informational.
	      if (kMediaFormat_H264 == lTrackInfo->Format)
		{
#define CONFIG_SIZE 100
		  //MK_U8   lSPS[CONFIG_SIZE];
		  MK_U32  lSPSLen = 0;
		  //MK_U8   lPPS[CONFIG_SIZE];
		  MK_U32  lPPSLen = 0;
		  m_width	=	MK_TrackInfo_GetVideoWidth(lTrackInfo);
		  m_height	=  MK_TrackInfo_GetVideoHeight(lTrackInfo);
		  VOLOGI("width=%d,height=%d",m_width,m_height);
		  unsigned char* head=m_h264HeadData;
		  m_h264HeadDataSize=0;
		  if (MK_TrackInfo_GetH264ConfSPSCount(lTrackInfo))
		    {
		      head[0]	=	head[1]	= head[2]	= 0;
		      head[3]	= 1;
		      head+=4;
		      lSPSLen = gMKPlayerFunc.VO_MK_TrackInfo_GetH264ConfSPS(lTrackInfo, 0, head, CONFIG_SIZE);
		      VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_TrackInfo_GetH264ConfSPS");
		      head+=lSPSLen;
		      m_h264HeadDataSize+=lSPSLen+4;
		    }
		  if (MK_TrackInfo_GetH264ConfPPSCount(lTrackInfo))
		    {
		      head[0]	=	head[1]	= head[2]	= 0;
		      head[3]	= 1;
		      head+=4;
		      lPPSLen = gMKPlayerFunc.VO_MK_TrackInfo_GetH264ConfPPS(lTrackInfo, 0, head, CONFIG_SIZE);
		      VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_TrackInfo_GetH264ConfPPS");
		      head+=lPPSLen;
		      m_h264HeadDataSize+=lPPSLen+4;
		    }
		  
		  lVideoTrackIdx = lTrackIdx;
		}
	      else if (kMediaFormat_MP4A == lTrackInfo->Format)
		{
		  
		  m_sampleRate = MK_TrackInfo_GetAudioSampleRate(lTrackInfo);
		  m_channelNum = MK_TrackInfo_GetAudioChannels(lTrackInfo);
		  VOLOGI("sampleRate=%d,chanNum=%d",m_sampleRate,m_channelNum);
		  WriteADTSHead(m_sampleRate,m_channelNum,0,m_adtsHead);
		  lAudioTrackIdx = lTrackIdx;
		}
	      else
		{
		  VOLOGI("unsupported codec,id=%d",lTrackInfo->Format);
		  VOLOGI ("<<<DaSoPl>>> leave");
		  return -100;//TBD
		}
	      // Enable all tracks that has got a known format, i.e. H264 and AAC.
	      // All tracks are disabled as default.
	      //gMKPlayerFunc.VO_MK_Player_SetTrackStateFunc(gMKPlayerFunc.lPlayer, lTrackIdx, &MyTrackStateFunc, NULL);
	      lRes = gMKPlayerFunc.VO_MK_Player_EnableTrack(gMKPlayerFunc.lPlayer, lTrackIdx);
	      VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_EnableTrack lRes=0x%08x", lRes);
	    }
	} 
      VOLOGI ("<<<DaSoPl>>> leave");
      return 0;
    }
  else
    {
      VOLOGI("gMKPlayerFunc.VO_MK_Player_Open failed lRes=0x%08x", lRes);
      //        DestroyMKSource();
      //        CreateMKSource();
      VOLOGI ("<<<DaSoPl>>> leave");
      return -1;
    }
  
  VOLOGI ("<<<DaSoPl>>> leave");
}
OMX_ERRORTYPE COMXMobiTVDataSourcePlayer:: GetParam (OMX_S32 nID, OMX_PTR pValue)
{
	VOLOGI ("<<<DaSoPl>>> enter, %X",nID);

	switch (nID)
	{
	case MOBITV_EOS_PARAM:
		*(int*)pValue = m_mobiTVEOS; 
		break;
	case MOBITV_VR_TYPE:
		*(int*)pValue = m_vrType; 
		break;
	case MOBITV_AR_TYPE:
		*(int*)pValue = m_arType; 
		break;
	default:
		return COMXEngine::GetParam(nID,pValue);
	}
	VOLOGI ("<<<DaSoPl>>> leave");
	return OMX_ErrorNone;
}
OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::SetParam (OMX_S32 nID, OMX_PTR pValue)
{
	VOLOGI ("<<<DaSoPl>>> enter, %X",nID);
	
	switch (nID)
	{
	case MOBITV_FUNCSET_ID:
		{
		  break;
		}
	case MOBITV_FORCE_STOP:
		return Stop();
	case MOBITV_BUFFERING:
		{
			m_bIsBuffering = *(bool*)pValue;
		}
		break;
	default:
		return COMXEngine::SetParam(nID,pValue);
	}
    VOLOGI ("<<<DaSoPl>>> leave");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	memcpy (&m_dspArea, pDisplay, sizeof (OMX_VO_DISPLAYAREATYPE));

    OMX_ERRORTYPE lRes = m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
    VOLOGI ("<<<DaSoPl>>> leave");
    return lRes;
//	return m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
}
#if 0//defined(_WIN32_XP)
#define VOLOGI2 VOLOGI
#else
#define VOLOGI2(...)
#endif
static char newlink[1024];
static char* GetTheFileFromCFG(int param,char* filename=NULL)
{
	if (1)//param)
	{
		newlink[0]='\0';
		if (filename==NULL)
#ifdef _WIN32
			filename = "c:/mobitv.txt";
#else
			filename = "/data/local/mobitv.txt";
#endif
		FILE* pSDPFile = fopen(filename, "rb");
		if(pSDPFile)
		{
			fseek( pSDPFile, 0L, SEEK_END );
			int fileSize = ftell(pSDPFile);
			if(fileSize<0)
				fileSize = 1024*10;
			char* pSDPData = new char[fileSize+1];
			fseek(pSDPFile,0,SEEK_SET);
			int sdpDataSize = fread(pSDPData, 1, fileSize, pSDPFile);
			pSDPData[sdpDataSize] = '\0';
			char * pRTSPLink = strstr(pSDPData, "file://");
			if(pRTSPLink)
			{
				sscanf(pRTSPLink, "%[^\r\n]", newlink);
				VOLOGI("the link is %s",newlink);
			}
			else
			{
				VOLOGE("no valid file");
			}
			delete[] pSDPData;
			fclose(pSDPFile);
		}
		else
		{
			VOLOGE("can not open file,%s",filename);
		}
		return newlink;
	}
	else
	{
		//VOLOGE("can not open file,%s\n",filename);
		return "file:///sdcard/video/BeeMovie_1.3gp";
	}

}

static char* GetTheLinkFromCFG(int param,char* filename=NULL)
{
	if (param)
	{
		newlink[0]='\0';
		if (filename==NULL)
#ifdef _WIN32
			filename = "c:/visualon/test2.vois";
#else
			filename = "/data/local/test2.vois";
#endif
		FILE* pSDPFile = fopen(filename, "rb");
		if(pSDPFile)
		{
			fseek( pSDPFile, 0L, SEEK_END );
			int fileSize = ftell(pSDPFile);
			if(fileSize<0)
				fileSize = 1024*10;
			char* pSDPData = new char[fileSize+1];
			fseek(pSDPFile,0,SEEK_SET);
			int sdpDataSize = fread(pSDPData, 1, fileSize, pSDPFile);
			pSDPData[sdpDataSize] = '\0';
			char * pRTSPLink = strstr(pSDPData, "rtsp://");
			if(pRTSPLink)
			{
				sscanf(pRTSPLink, "%[^\r\n]", newlink);
				VOLOGI("the link is %s",newlink);
			}
			delete[] pSDPData;
			fclose(pSDPFile);
		}
		else
		{
			VOLOGE("can not open url file,%s",filename);
		}
		return newlink;
	}
	else
	{
		return "rtsp://masds03.htc.com.tw/h264/H264_10f_64k_AAC_16k_5KF_qcif.3gp";
	}
	
}
static char* versionNum = "1.10.810.1835";
OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::Playback (VOME_SOURCECONTENTTYPE * pSource)
{
    VOLOGI ("versionNum=%s",versionNum);
	if (m_hEngine == NULL)
	{
		VOLOGE("the engine is not inited yet");
		return OMX_ErrorInvalidState;
	}
	//return m_OMXEng.Playback (m_hEngine, pSource);
	
	
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
#ifndef VORTSPIMP
	VOLOGI("source: %s", pSource->pSource);
#ifdef TEST_GET_FILE_FROM_CFG
	 char* url=GetTheFileFromCFG(0);
#else//GET_FILE_FROM_CFG
	char *url = (char *)(pSource->pSource);
#endif//GET_FILE_FROM_CFG
#else//VORTSPIMP
	char* url=GetTheLinkFromCFG(1);
#endif//VORTSPIMP	
	if (strlen(url)<7)
	{
		VOLOGE("Invalid URL");
		return OMX_ErrorUndefined;
	}
	VOLOGI("gonna open url....%s",url);
	errType=(OMX_ERRORTYPE)OpenURL(url);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("Open %s failed.", url);
		return errType;
	}
	VOLOGI("Before Add DataSource");
	errType = AddComponent ("OMX.VisualOn.DataSource", &m_pCompSrc);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("AddComponent OMX.VisualOn.DataSource failed,%X",errType);
		return errType;
	}
	// set the Audio format
	OMX_AUDIO_PARAM_PCMMODETYPE tpAudioFormat;
	memset (&tpAudioFormat, 0, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	tpAudioFormat.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("GetParameter Audio Foramt,%X",errType);
		return errType;
	}

	tpAudioFormat.nSamplingRate = m_sampleRate;
	tpAudioFormat.nChannels = m_channelNum;
	tpAudioFormat.nBitPerSample = 16;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("SetParameter Audio Foramt,%X",errType);
		return errType;
	}

	// set the video codec and size
	OMX_PARAM_PORTDEFINITIONTYPE portType;
	memset (&portType, 0, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	portType.nPortIndex = 1;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("GetParameter Video Foramt,%X",errType);
		return errType;
	}
	portType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	portType.format.video.nFrameWidth = m_width;
	portType.format.video.nFrameHeight = m_height;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("SetParameter video Foramt,%X",errType);
		return errType;
	}
	errType = AddComponent ("OMX.VisualOn.Clock.Time", &m_pCompClock);

	errType = AddComponent ("OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	errType = AddComponent ("OMX.VisualOn.Audio.Sink", &m_pCompAR);

	errType = AddComponent ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	errType = AddComponent ("OMX.VisualOn.Video.Sink", &m_pCompVR);
	
	VOLOGI("Before Audio Chain");
	
	errType = ConnectPorts (m_pCompSrc, 0, m_pCompAD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);

	VOLOGI("Before Video Chain");

	errType = ConnectPorts (m_pCompSrc, 1, m_pCompVD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);

	VOLOGI("Before Clock Chain");
	errType = ConnectPorts (m_pCompClock, 0, m_pCompSrc, 2, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);
	VOLOGI("Before OMX_IndexConfigTimeActiveRefClock");
	
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
//	clkRef.eClock = OMX_TIME_RefClockVideo;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);
	// SetDisplayArea (&m_dspArea);
	m_dspArea.nHeight = m_height;
	m_dspArea.nWidth  = m_width;
	VOLOGI("Before OMX_VO_IndexConfigDisplayArea(%d,%d)",m_dspArea.nWidth, m_dspArea.nHeight);
	
	if (m_pCompVR != NULL)
		m_pCompVR->SetConfig (m_pCompVR, (OMX_INDEXTYPE)OMX_VO_IndexConfigDisplayArea, &m_dspArea);
	
	//errType = Run ();
	MK_Result lRes = gMKPlayerFunc.VO_MK_Player_Play(gMKPlayerFunc.lPlayer);
	VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_Play lRes=0x%08x", lRes);
	int 	dwID = 0;
	HANDLE tmpHandle=0;
	if (m_pCompAD != NULL)
	{
		m_bAudioStop = false;
		if (m_hThreadAudio==NULL)
		{
			m_hThreadAudio = (HANDLE)1;
			voOMXThreadCreate(&tmpHandle, (OMX_U32*)&dwID, (voOMX_ThreadProc) ReadAudioDataProc, this, 0);
			if (errType!=OMX_ErrorNone)
			{
				m_hThreadAudio = NULL;
				VOLOGE("fail to create audio thread");
				return errType;
			}
		
		}
		else
		{
			VOLOGE("The audio thread is alive,%X",m_hThreadAudio);
			return OMX_ErrorInsufficientResources;
		}
	}
	
	if (m_pCompVD != NULL)
	{
		m_bVideoStop = false;
		//TBD:the voOMXThreadCreate has one bug, do no rely on the thread handle
		if (m_hThreadVideo == NULL)
		{
			m_hThreadVideo = (HANDLE)2;
			errType=voOMXThreadCreate (&tmpHandle, (OMX_U32*)&dwID, (voOMX_ThreadProc) ReadVideoDataProc, this, 0);
			if (errType!=OMX_ErrorNone)
			{
				m_hThreadVideo = NULL;
				VOLOGE("fail to create video thread");
				return errType;
			}
		
			
		}
		else
		{
			VOLOGE("The video thread is alive,%X",m_hThreadVideo);
			return OMX_ErrorInsufficientResources;
		}
	}
	
	if (m_hThreadTrigger==NULL)
	{
		m_hThreadTrigger = (HANDLE)3;		
		errType = 	voOMXThreadCreate (&tmpHandle, (OMX_U32*)&dwID, (voOMX_ThreadProc) TriggerProc, this, 0);
		if (errType!=OMX_ErrorNone)
		{
			m_hThreadTrigger = NULL;
			VOLOGE("fail to create trigger thread");
			return errType;
		}
	} 
	else
	{
		VOLOGE("The trigger thread is alive,%X",m_hThreadTrigger);
		return OMX_ErrorInsufficientResources;
	}
		

    VOLOGI ("<<<DaSoPl>>> leave");
	return errType;
}
 OMX_U32		COMXMobiTVDataSourcePlayer::Release (void)
 {
    VOLOGI ("<<<DaSoPl>>> enter");
	 Stop();
	 COMXEngine::Release();
//	 lPlayer=NULL;
    VOLOGI ("<<<DaSoPl>>> leave");
	 return 0;
 }
OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::Close (void)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL)
	{
		VOLOGI ("<<<DaSoPl>>> leave");
		return OMX_ErrorInvalidState;
	}

	Stop ();

    OMX_ERRORTYPE lRes = m_OMXEng.Close (m_hEngine);
    VOLOGI ("<<<DaSoPl>>> leave");
    return lRes;
//	return m_OMXEng.Close (m_hEngine);
}



OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::Run (void)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL || gMKPlayerFunc.lPlayer == NULL)
		return OMX_ErrorInvalidState;
	VOLOGI("@Run");
	OMX_ERRORTYPE errType = m_OMXEng.Run (m_hEngine);
	VOLOGI("@current state is paused?%d",IsPaused());
	MK_Result lRes = gMKPlayerFunc.VO_MK_Player_Play(gMKPlayerFunc.lPlayer);
	VOLOGI("@State: %s", MK_State_ToCStr(gMKPlayerFunc.VO_MK_Player_GetState(gMKPlayerFunc.lPlayer)));
    VOLOGI ("<<<DaSoPl>>> leave");
	return errType;
}

OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::Pause (void)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL || gMKPlayerFunc.lPlayer == NULL)
		return OMX_ErrorInvalidState;
	VOLOGI("@Pause");
	//gMKPlayerFunc.VO_MK_Player_Pause(gMKPlayerFunc.lPlayer);
    OMX_ERRORTYPE lRes = m_OMXEng.Pause (m_hEngine);
    VOLOGI ("<<<DaSoPl>>> leave");
    return lRes;
//	return m_OMXEng.Pause (m_hEngine);
}

OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::Stop (void)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	OMX_ERRORTYPE lRes = OMX_ErrorNone;
	
	if (gMKPlayerFunc.lPlayer)
	{
		MK_Result lRes = gMKPlayerFunc.VO_MK_Player_Stop(gMKPlayerFunc.lPlayer);
		VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_Stop lRes=0x%08x", lRes);
		
	}
	if (m_hEngine)
	{
		StopSourceThread ();
		OMX_S32 pos;GetCurPos(&pos);//just for check the last pos before stop
		lRes = m_OMXEng.Stop (m_hEngine);
	}

	VOLOGI ("<<<DaSoPl>>> leave");
    return lRes;
//	return m_OMXEng.Stop (m_hEngine);
}

OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::GetDuration (OMX_S32 * pDuration)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL || gMKPlayerFunc.lPlayer == NULL)
		return OMX_ErrorInvalidState;
	VOLOGI("duration=%d",m_duration);
	*pDuration=m_duration;
    VOLOGI ("<<<DaSoPl>>> leave");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::GetCurPos (OMX_S32 * pCurPos)
{
    //VOLOGI ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL || gMKPlayerFunc.lPlayer == NULL)
		return OMX_ErrorInvalidState;
	OMX_ERRORTYPE lRes=	OMX_ErrorNone;
	m_OMXEng.GetCurPos (m_hEngine, pCurPos);
	if(m_currentPos>1000&&*pCurPos==0)//has displayed and the pos=0 means it has been stopped or EOS,so the pos is reset 
		*pCurPos = m_duration;
	m_currentPos = *pCurPos;
    //lRes = m_OMXEng.GetCurPos (m_hEngine, pCurPos);
    VOLOGI ("<<<DaSoPl>>> leave,pos=%d",*pCurPos);
    return lRes;
//	return m_OMXEng.GetCurPos (m_hEngine, pCurPos);
}

OMX_ERRORTYPE COMXMobiTVDataSourcePlayer::SetCurPos (OMX_S32 nCurPos)
{
  OMX_ERRORTYPE lRes;
 
  VOLOGI ("<<<DaSoPl>>> enter");
  if (m_hEngine == NULL || gMKPlayerFunc.lPlayer == NULL)
    return OMX_ErrorInvalidState;
  MK_Time time1=MK_Time_Init(nCurPos, 1000, MK_FALSE);
  VOLOGI("@SetPos %d,time1=%d", nCurPos,(long)time1);
  VOLOGI("@State: %s", MK_State_ToCStr(gMKPlayerFunc.VO_MK_Player_GetState(gMKPlayerFunc.lPlayer)));

  if (gMKPlayerFunc.VO_MK_Player_GetState(gMKPlayerFunc.lPlayer)==MK_kState_Playing)
  {
    MK_Result lRes = gMKPlayerFunc.VO_MK_Player_Stop(gMKPlayerFunc.lPlayer);
    VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_Stop lRes=0x%08x", lRes);
  }

  MK_Result result = 0;//MK_OK;
  //time1=32400000000;
  m_seeking = true;
  result = gMKPlayerFunc.VO_MK_Player_Seek(gMKPlayerFunc.lPlayer, time1);

  VOLOGI("@seek: 0x%08x", result);
  if(result<0) {
    VOLOGE("MK_Player_Seek returns with 0x%08x", result);
    lRes = OMX_ErrorBadParameter;
  } else {
     gMKPlayerFunc.VO_MK_Player_Play(gMKPlayerFunc.lPlayer);
	 long value=121;
	 lRes = OMX_ErrorNone;
	 
    lRes = m_OMXEng.SetParam(m_hEngine,VOME_PID_Flush,&value);// SetCurPos(m_hEngine, ms);
	 //voOMXOS_Sleep (20);	 
	m_pCallBackMsgHandler(m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);
	VOLOGI("@seek13: (%d,%d)", (long)(time1>>32),long(time1));
	 
  }
  m_seeking = false;
  //OMX_ERRORTYPE lRes = m_OMXEng.SetCurPos (m_hEngine, nCurPos);
  VOLOGI ("<<<DaSoPl>>> leave");
  return lRes;
}

int	COMXMobiTVDataSourcePlayer::TriggerProc (OMX_PTR pParam)
{
	COMXMobiTVDataSourcePlayer * pPlayer = (COMXMobiTVDataSourcePlayer *) pParam;

	pPlayer->TriggerLoop ();

	return 0;
}

int COMXMobiTVDataSourcePlayer::ReadVideoDataProc (OMX_PTR pParam)
{
	COMXMobiTVDataSourcePlayer * pPlayer = (COMXMobiTVDataSourcePlayer *) pParam;

	pPlayer->ReadVideoDataLoop ();

	return 0;
}
static int GetNALUSize(unsigned char* lenBuf,int size)
{
	int length=0;
	switch(size)
	{
	case 4:
		length = (lenBuf[0]<<24)|(lenBuf[1]<<16)|(lenBuf[2]<<8)|lenBuf[3];
		break;
	case 3:
		length = (lenBuf[0]<<16)|(lenBuf[1]<<8)|(lenBuf[2]);//<<8)|lenBuf[3])
		break;
	case 2:
		length = (lenBuf[0]<<8)|(lenBuf[1]);//<<8)|(lenBuf[2]);
		break;
	case 1:
		length = lenBuf[0];
		break;
	default:
		break;
	}
	return length;
}
#ifdef VORTSPIMP
#define MK_Time_Scale(a,b,c,d) (a)
#endif//VORTSPIMP
#define WAITING_TIME 5
#define WAITING_TIME2 20

int	COMXMobiTVDataSourcePlayer::TriggerLoop()
{
    VOLOGI ("<<<DaSoPl>>> enter");
	VOLOGI("Trigger thread launch,%X",m_hThreadTrigger);
	while (gMKPlayerFunc.lPlayer && !m_bAudioStop) 
	{
		voOMXOS_Sleep (WAITING_TIME);
		if(IsPaused()&&!m_bIsBuffering)
			continue;
		gMKPlayerFunc.VO_MK_Player_Wait(gMKPlayerFunc.lPlayer, WAITING_TIME);
		
	} 
	
	VOLOGI("Trigger thread exit,%X",m_hThreadTrigger);
	m_hThreadTrigger=NULL;
	return 0;
}
#define MAX_WAIT_END_NUM 50
#define MAX_END_TIME_DELTA 2000
int COMXMobiTVDataSourcePlayer::TryToSendEOS(OMX_BUFFERHEADERTYPE* pBufHead)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(m_mobiTVEOS)
	{
			pBufHead->nFlags = OMX_BUFFERFLAG_EOS;
			pBufHead->nFilledLen = 0;
			VOLOGI("Send EOS,streamID=%d\n",pBufHead->nOutputPortIndex);
			do
			{
				if (m_bVideoStop)
					break;

				errType = Input2Vome(pBufHead,NULL);
				if (errType != OMX_ErrorNone)
				{
					VOLOGI("wait for process EOS streamID=%d ",pBufHead->nOutputPortIndex);
					voOMXOS_Sleep (5);
				}

			} while (errType != OMX_ErrorNone);
			VOLOGI("Send EOS,streamID=%d done\n",pBufHead->nOutputPortIndex);
			return 1;
	}
	return 0;
}
OMX_ERRORTYPE	COMXMobiTVDataSourcePlayer::Input2Vome(OMX_BUFFERHEADERTYPE *bufHead,TDeliverTime* preTime)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	if(!m_bDisableInputVOME)
	{
		ret = m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, bufHead);
	}
	else if(preTime)//just for test to verify if the VOME is involved into some memory issues 
	{
		unsigned long currLocalTime = voOS_GetSysTime();
		if(preTime->localTime!=0)
		{
			
			unsigned long diffLocalTime = currLocalTime - preTime->localTime;
			unsigned long diffTS		  = (unsigned long)bufHead->nTimeStamp-(unsigned long)preTime->timestamp;
			int sleepTime	  = diffTS - diffLocalTime-5;
			if(sleepTime>0&&sleepTime<1000)
				voOS_Sleep(sleepTime);
			else//if the result is invalid, try to sleep a default 20ms
				voOS_Sleep(20);
			if(m_nLogLevel)
				VOLOGI("DisableInput:sleep(%d)",sleepTime);
		}
		preTime->localTime = currLocalTime;
		preTime->timestamp = bufHead->nTimeStamp;
	}
	return ret;
}
int COMXMobiTVDataSourcePlayer::ReadVideoDataLoop (void)
{
  VOLOGI ("<<<DaSoPl>>> enter");
  VOLOGI("VideoThread Launch,%X",m_hThreadVideo);
  OMX_BUFFERHEADERTYPE bufHead;
  memset (&bufHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
  bufHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
  bufHead.nAllocLen = 0x1ffff;//128K
  bufHead.nOutputPortIndex = 1; // Video
  bufHead.nTickCount = 1;
  int frameCount=0;
  MK_Sample* lSample = 0;
  bool  isGettingIFrame = true;
  OMX_ERRORTYPE errType = OMX_ErrorNone;
  VOMP_BUFFERTYPE  bufHead2;
  TDeliverTime	preTime = {0};
  unsigned long rcvTime = voOS_GetSysTime();
  unsigned long prvTS	= 0;
    while (!m_bVideoStop)
    {
      
      //gMKPlayerFunc.VO_MK_Player_Wait(gMKPlayerFunc.lPlayer, WAITING_TIME);
      voOMXOS_Sleep (WAITING_TIME);
      if(IsPaused()&&!m_bIsBuffering)
			continue;
      if (m_bVideoStop)
			break;

	  if(m_testDumpfile.IsEnabled())
	  {

		  int ret = m_testDumpfile.ReadVideo(&bufHead2);
		  if(ret<0)
		  {
			  VOLOGE("Read Video fail %d",ret);
			  //MK_Result result=0;
			  //MyEventFunc(MK_kEvent_End,&result);
		  }
		  else
		  {
			  if(m_nLogLevel&DMP_VIDEO)
				  VOLOGI("DVideo:%u,%d,size=%d,time=%u",voOS_GetSysTime(),frameCount++,bufHead2.nSize,(unsigned long)bufHead2.llTime);

			  bufHead.nFilledLen		= bufHead2.nSize;
			  bufHead.nTimeStamp		= bufHead2.llTime;
			  bufHead.pBuffer			= bufHead2.pBuffer;
			  if (m_hVideoDataFile != NULL)//just for check
			  {
				  voOMXFileWrite (m_hVideoDataFile, bufHead.pBuffer, bufHead.nFilledLen);
			  }
			  do
			  {
				  if (m_bVideoStop)
					  break;

				  errType = Input2Vome(&bufHead,&preTime);//m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);


				  if (errType != OMX_ErrorNone)
				  {
					  if(m_nLogLevel&DMP_VIDEO)
						  VOLOGI("%d,Video:wait for process %X",frameCount,errType);

					  voOMXOS_Sleep (WAITING_TIME);
				  }

			  } while (errType != OMX_ErrorNone);
		  }

		  continue;
	  }
    if ((lSample = gMKPlayerFunc.VO_MK_Player_GetTrackSample(gMKPlayerFunc.lPlayer, lVideoTrackIdx)))
	{
	  m_mobiTVEOS = 0;
	  unsigned int localTime = voOS_GetSysTime();
	  if (frameCount++<1)
	    {
	      bufHead.pBuffer		= m_h264HeadData;
	      bufHead.nTimeStamp	= 0;
	      bufHead.nFilledLen	= m_h264HeadDataSize;
		  if (m_hVideoDataFile != NULL)
		  {
			  
			  if (m_nVideoDataFormat >= 1)
				  voOMXFileWrite (m_hVideoDataFile, (OMX_U8*)&bufHead.nFilledLen, 4);
			  if (m_nVideoDataFormat >= 2)
				  voOMXFileWrite (m_hVideoDataFile, (OMX_U8*)&bufHead.nTimeStamp, 4);
			  if (m_nVideoDataFormat >= 3)
				  voOMXFileWrite (m_hVideoDataFile, (OMX_U8*)&localTime, 4);
			  voOMXFileWrite (m_hVideoDataFile, bufHead.pBuffer, bufHead.nFilledLen);
		  }
	      do
		{
		  if (m_bVideoStop)
		    break;
		  
		  errType = Input2Vome(&bufHead,&preTime);//m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);
		  if (errType != OMX_ErrorNone)
		    {
		      VOLOGI2("Video:wait for process headData");
		      voOMXOS_Sleep (2);
		    }
		}while (errType != OMX_ErrorNone);
	    }
	  
	  
	  bufHead.pBuffer	= lSample->mData;
	  bufHead.nTimeStamp	= MK_Time_Scale(lSample->mTime, 1000, 1, MK_FALSE);

	  bool currIsIFrame = false;//IsH264IntraFrame((char*)bufHead.pBuffer+4);

	  errType = OMX_ErrorUndefined;
	  int leftSize=lSample->mDataLen;
	  bool isFirstSlice = true;
	  while (leftSize>4)
		{
			if(!currIsIFrame)
				currIsIFrame = IsH264IntraFrame((char*)bufHead.pBuffer+4,leftSize-4);
			
			if(currIsIFrame&&isFirstSlice)//only set the first slice
			{
				bufHead.nFlags = OMX_BUFFERFLAG_SYNCFRAME;
				isFirstSlice = false;
			}
			else
				bufHead.nFlags  = 0;

			int naluSize=GetNALUSize(bufHead.pBuffer,4)+4;
			if (naluSize>leftSize)
			{
				naluSize=leftSize;
			}
			bufHead.pBuffer[0]=bufHead.pBuffer[1]=bufHead.pBuffer[2]=0;
			bufHead.pBuffer[3]=1;
			leftSize-=naluSize;

			bufHead.nFilledLen  = naluSize;
			if (m_hVideoDataFile != NULL)
			{
				if (m_nVideoDataFormat >= 1)
					voOMXFileWrite (m_hVideoDataFile, (OMX_U8*)&bufHead.nFilledLen, 4);
				if (m_nVideoDataFormat >= 2)
					voOMXFileWrite (m_hVideoDataFile, (OMX_U8*)&bufHead.nTimeStamp, 4);
				if (m_nVideoDataFormat >= 3)
					voOMXFileWrite (m_hVideoDataFile, (OMX_U8*)&localTime, 4);
			}
			
	      bufHead.pBuffer	+= naluSize;
	    
	
	    } 
		bufHead.pBuffer = lSample->mData;
		bufHead.nFilledLen  = lSample->mDataLen;
		if(m_nLogLevel&DMP_VIDEO)
		{
			long diffRcv = voOS_GetSysTime()-rcvTime;
			long diffTS  = (unsigned long)bufHead.nTimeStamp-prvTS;
			long diff    = diffRcv - diffTS;
			VOLOGI("@#@Video:%d,size=%d,time=%u,I=%d,(%u-%u)=%d",frameCount,lSample->mDataLen,(unsigned long)bufHead.nTimeStamp,currIsIFrame,diffRcv,diffTS,diff);
			rcvTime = voOS_GetSysTime();
			prvTS	= (unsigned long)bufHead.nTimeStamp;
		}
	  do
	  {
		  if (m_bVideoStop)
			  break;

		  errType =Input2Vome(&bufHead,&preTime);// m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);

		  if (m_seeking)
		  {
			  isGettingIFrame = true;
			  break;
		  }

		  if (errType != OMX_ErrorNone)
		  {
			  if(m_nLogLevel&DMP_VIDEO)
				 VOLOGI("%d,Video:wait for process(%X,%X) %X",frameCount,bufHead.pBuffer[4],bufHead.pBuffer[5],errType);
			  voOMXOS_Sleep (WAITING_TIME);
		  }

	  }while (errType != OMX_ErrorNone);

		  gMKPlayerFunc.VO_MK_Sample_Destroy(lSample);				
		  lSample = NULL;
	  
		}
		else
		{
		  if(TryToSendEOS(&bufHead))
			break;
		  VOLOGI2("wait for next sample,cur=%d",frameCount);
		  voOMXOS_Sleep (WAITING_TIME2);
		}
    }
  
  VOLOGI("VideoThread Exit,%X",m_hThreadVideo);
  m_hThreadVideo = NULL;

  VOLOGI ("<<<DaSoPl>>> leave");
  return 0;
}


int COMXMobiTVDataSourcePlayer::ReadAudioDataProc (OMX_PTR pParam)
{
	COMXMobiTVDataSourcePlayer * pPlayer = (COMXMobiTVDataSourcePlayer *) pParam;

	pPlayer->ReadAudioDataLoop ();

	return 0;
}

int COMXMobiTVDataSourcePlayer::ReadAudioDataLoop (void)
{
  VOLOGI ("<<<DaSoPl>>> enter");
  VOLOGI("AudioThread Launch,%X",m_hThreadAudio);
  OMX_BUFFERHEADERTYPE bufHead;
  memset (&bufHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
  bufHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
  bufHead.nAllocLen = 0x0ffff;//64K
  if(!m_testDumpfile.IsEnabled())
	 bufHead.pBuffer = new unsigned char[bufHead.nAllocLen];
  bufHead.nOutputPortIndex = 0; // Audio
  bufHead.nTickCount = 1;
  MK_Sample* lSample = 0;
  OMX_ERRORTYPE errType = OMX_ErrorNone;
  int frameCount=0;
  VOMP_BUFFERTYPE  bufHead2;
  TDeliverTime		preTime={0};
  unsigned long		rcvTime=voOS_GetSysTime();
  unsigned long prvTS	= 0;
  while (!m_bAudioStop)
    {    
		
      //gMKPlayerFunc.VO_MK_Player_Wait(gMKPlayerFunc.lPlayer, WAITING_TIME);
      //VOLOGI("Audio:before get sample");
      voOMXOS_Sleep (WAITING_TIME);
      if(IsPaused()&&!m_bIsBuffering)
	{
		if(m_nLogLevel&DMP_AUDIO)
		 VOLOGI("Audio:is paused");
	  continue;
	}
      
      if (m_bAudioStop)
	break;
      
	  if(m_testDumpfile.IsEnabled())
	  {
		  
		  int ret = m_testDumpfile.ReadAudio(&bufHead2);
		  if(ret<0)
		  {
			  VOLOGE("Read Audio fail %d",ret);
			  //MK_Result result=0;
			  //MyEventFunc(MK_kEvent_End,&result);
		  }
		  else
		  {
			  if(m_nLogLevel&DMP_AUDIO)
				  VOLOGI("DAudio:%u,%d,size=%d,time=%u",voOS_GetSysTime(),frameCount++,bufHead2.nSize,(unsigned long)bufHead2.llTime);

			  bufHead.nFilledLen		= bufHead2.nSize;
			  bufHead.nTimeStamp		= bufHead2.llTime;
			  bufHead.pBuffer			= bufHead2.pBuffer;
			  if (m_hAudioDataFile != NULL)//just for check
			  {
				  voOMXFileWrite (m_hAudioDataFile, bufHead.pBuffer, bufHead.nFilledLen);
			  }
				  do
				  {
					  if (m_bAudioStop)
						  break;

					  errType = Input2Vome(&bufHead,&preTime);//m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);


					  if (errType != OMX_ErrorNone)
					  {
						  if(m_nLogLevel&DMP_AUDIO)
							VOLOGI("%d,Audio:wait for process %X",frameCount,errType);

						  voOMXOS_Sleep (WAITING_TIME);
					  }

				  } while (errType != OMX_ErrorNone);
		  }
		 
		  continue;
	  }

      if ((lSample = gMKPlayerFunc.VO_MK_Player_GetTrackSample(gMKPlayerFunc.lPlayer, lAudioTrackIdx)))
	{
	  m_mobiTVEOS = 0;
	  UpdateADTSHead(lSample->mDataLen,m_adtsHead);
	  memcpy(bufHead.pBuffer,m_adtsHead,7);
	  memcpy(bufHead.pBuffer+7,lSample->mData,lSample->mDataLen);
	  bufHead.nFilledLen		= lSample->mDataLen+7;
	  bufHead.nTimeStamp	= MK_Time_Scale(lSample->mTime, 1000, 1, MK_FALSE); 
	  frameCount++;
	  if(m_nLogLevel&DMP_AUDIO)
	  {
		  long diffRcv = voOS_GetSysTime()-rcvTime;
		  long diffTS  = (unsigned long)bufHead.nTimeStamp-prvTS;
		  long diff    = diffRcv - diffTS;
		  VOLOGI("@#@Audio:%d,size=%d,time=%u,(%u-%u)=%d",frameCount,lSample->mDataLen,(unsigned long)bufHead.nTimeStamp,diffRcv,diffTS,diff);
		  prvTS	= (unsigned long)bufHead.nTimeStamp;
	  }
	  rcvTime = voOS_GetSysTime();
	  if (m_hAudioDataFile != NULL)
	  {
		  unsigned long localTime = voOS_GetSysTime();
		  if (m_nAudioDataFormat >= 1)
			  voOMXFileWrite (m_hAudioDataFile, (OMX_U8*)&bufHead.nFilledLen, 4);
		  if (m_nAudioDataFormat >= 2)
			  voOMXFileWrite (m_hAudioDataFile, (OMX_U8*)&bufHead.nTimeStamp, 4);
		  if (m_nAudioDataFormat >= 3)
			  voOMXFileWrite (m_hAudioDataFile, (OMX_U8*)&localTime, 4);
		  voOMXFileWrite (m_hAudioDataFile, bufHead.pBuffer, bufHead.nFilledLen);
	  }
#define MIN_FRAMES_IN_BUF 10
	  if (frameCount==MIN_FRAMES_IN_BUF)
	    {
	      VOLOGI("audio buffer is ready");
	      //Run();
	    }
	  do
	    {
	      if (m_bAudioStop)
		break;
	      
	      errType = Input2Vome(&bufHead,&preTime);//m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);
	      
	      if (m_seeking) break;
	      
	      if (errType != OMX_ErrorNone)
		{
			if(m_nLogLevel&DMP_AUDIO)
				VOLOGI("%d,Audio:wait for process %X",frameCount,errType);
			if(errType == OMX_ErrorOverflow && m_bIsBuffering)	
			{
				VOLOGI("rebuffering is done");
				m_bIsBuffering = false;
			}
		     voOMXOS_Sleep (WAITING_TIME);
		}
	      
	    } while (errType != OMX_ErrorNone);
	  gMKPlayerFunc.VO_MK_Sample_Destroy(lSample);
	  lSample = NULL;
	}
      else
	{
	  

		
		{
			unsigned long curLocalTime = voOS_GetSysTime();
			long diffRcv = curLocalTime-rcvTime;
			if(diffRcv>1000)
			{
				if(m_nLogLevel&DMP_AUDIO)
				{
					VOLOGI("@$@audio:no data recieved from MKLIB, %d=(%u-%u)",diffRcv,curLocalTime,rcvTime);
					VOLOGI("@State: %s", MK_State_ToCStr(gMKPlayerFunc.VO_MK_Player_GetState(gMKPlayerFunc.lPlayer)));
				}
				rcvTime = curLocalTime;
				//if(IsPaused()&&m_bIsBuffering)//to keep polling data from MKLib in pause mode will cause crash
				//	m_bIsBuffering = false;
			}

		}
	  if(TryToSendEOS(&bufHead))
	    break;
	  
	  voOMXOS_Sleep (WAITING_TIME2);

	}
      
    }

  if(!m_testDumpfile.IsEnabled())
	 delete[] bufHead.pBuffer;
  VOLOGI("AudioThread Exit,%X",m_hThreadAudio);
  m_hThreadAudio = NULL;
  
  VOLOGI ("<<<DaSoPl>>> leave");
  return 0;
}

int COMXMobiTVDataSourcePlayer::StopSourceThread (void)
{
    VOLOGI ("<<<DaSoPl>>> enter");
	m_bVideoStop = true;
	m_bAudioStop = true;

	while (m_hThreadVideo != NULL || m_hThreadAudio != NULL||m_hThreadTrigger!=NULL)
		voOMXOS_Sleep (2);

    VOLOGI ("<<<DaSoPl>>> leave");
	return 0;
}
