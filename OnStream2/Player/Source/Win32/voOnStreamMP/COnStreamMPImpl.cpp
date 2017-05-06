/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#include "COnStreamMPImpl.h"
#include "OnStreamEngine.h"
#include "OnStreamSrc.h"
#include "voOSFunc.h"
#include "voXMLLoad.h"
#include "VOMPTYPE.h"
#include "voLog.h"

COnStreamMPImpl::COnStreamMPImpl(int nPlayerType, void* pInitParam, int nInitParamFlag)
: mnInitParamFlag(nInitParamFlag)
, mnPlayerType(nPlayerType)
, mpSource(NULL)
, mpEngine(NULL)
, mhWnd(NULL)
, mnColorType(VOOSMP_COLOR_YUV_PLANAR420)
, mEnableInnerCloseCaption(true)
, m_ccMan(NULL)
, m_aspectRatio(VOOSMP_RATIO_AUTO)
, m_nVideoWidth(0)
, m_nVideoHeight(0)
, m_nProxyport(-1)
, m_pAapterDRMObject(NULL)
, m_renderType(VOOSMP_RENDER_TYPE_DDRAW)
, m_bID3Picture(false)
, m_bDrawVideoDirectly(false)
{
	ZeroMemory(&mInitParam , sizeof(mInitParam));
	VOOSMP_INIT_PARAM *pTempInitParam = (VOOSMP_INIT_PARAM *)pInitParam;
	if(pTempInitParam != NULL)
	{
		mInitParam.pLibOP = pTempInitParam->pLibOP;
		mInitParam.llFileSize = pTempInitParam->llFileSize;
		if(pTempInitParam->pszIOApiName != NULL)
		{
			int nStrLen = strlen(pTempInitParam->pszIOApiName);
			mInitParam.pszIOApiName = new char[nStrLen + 1];
			ZeroMemory(mInitParam.pszIOApiName , nStrLen + 1);
			strcpy(mInitParam.pszIOApiName , pTempInitParam->pszIOApiName);
		}

		if(pTempInitParam->pszIOFileName != NULL)
		{
			int nStrLen = strlen(pTempInitParam->pszIOFileName);
			mInitParam.pszIOFileName = new char[nStrLen + 1];
			ZeroMemory(mInitParam.pszIOFileName , nStrLen + 1);
			strcpy(mInitParam.pszIOFileName , pTempInitParam->pszIOFileName);
		}

		if(pTempInitParam->pWorkingPath != NULL)
		{
			int nStrLen = vostrlen((VO_TCHAR *)pTempInitParam->pWorkingPath);
			mInitParam.pWorkingPath = new VO_TCHAR[nStrLen + 1];
			memset(mInitParam.pWorkingPath , 0, (nStrLen + 1)*sizeof(VO_TCHAR));
			vostrcpy((VO_TCHAR *)mInitParam.pWorkingPath , (VO_TCHAR *)pTempInitParam->pWorkingPath);
		}
	}
	

	memset(&mCallbackFunc , 0 , sizeof(mCallbackFunc));
	memset (&mrcDraw, 0, sizeof (mrcDraw));

	m_pProxyhost[0] = 0;
	m_pUseragent[0] = 0;
	m_pLiscenseText[0] = 0;
	m_pLiscenseFilePath[0] = 0;

	m_szAdapterDRMFile[0] = 0;
	m_szAdapterDRMAPI[0] = 0;

	memset (&m_sHttpHeader, 0, sizeof (VOOSMP_SRC_HTTP_HEADER));
	memset (&m_sHttpProxy, 0, sizeof (VOOSMP_SRC_HTTP_PROXY));

	m_sHttpProxy.pszProxyHost = (unsigned char*)m_pProxyhost;
	m_sHttpProxy.nProxyPort = 0;

	memset (&m_OSCapData, 0, sizeof (VOOSMP_PERF_DATA));

	Init();
}

COnStreamMPImpl::~COnStreamMPImpl(void)
{
	DeleteSubtitleManager();
	if(mpEngine != NULL)
	{
		delete mpEngine;
		mpEngine = NULL;
	}

	if(mpSource != NULL)
	{
		delete mpSource ;
		mpSource = NULL;
	}

	if(mInitParam.pszIOFileName != NULL)
	{
		delete mInitParam.pszIOFileName;
		mInitParam.pszIOFileName = NULL;
	}

	if(mInitParam.pszIOApiName != NULL)
	{
		delete mInitParam.pszIOApiName;
		mInitParam.pszIOApiName = NULL;
	}

	if(mInitParam.pWorkingPath != NULL)
	{
		delete mInitParam.pWorkingPath;
		mInitParam.pWorkingPath = NULL;
	}
}
void COnStreamMPImpl::DeleteSubtitleManager()
{
	if(m_ccMan)
	{
		//CloseCloseCaptionTimer();
		delete m_ccMan;
	}
	m_ccMan = NULL;

}
int COnStreamMPImpl::SetView( void* pView)
{
	mhWnd = (HWND)pView;

	GetClientRect (mhWnd, (LPRECT)&mrcDraw);


	if(mpEngine != NULL)
	{
		mpEngine->SetView(mhWnd);
		return mpEngine->SetParam (VOOSMP_PID_DRAW_RECT, &mrcDraw);
	}

	return VOOSMP_ERR_None;
}

int COnStreamMPImpl::Open(void* pSource, int nFlag, int nSourceType)
{
	m_bID3Picture = false;
	m_aspectRatio=VOOSMP_RATIO_00;
	m_nVideoWidth=0;
	m_nVideoHeight=0;

	if(mpSource == NULL || mpEngine == NULL)
		return VOOSMP_ERR_Pointer;
	CloseCloseCaptionTimer();
	//DeleteSubtitleManager();

	int nRC = mpSource->Init(pSource , nFlag , nSourceType , &mInitParam , mnInitParamFlag);
	if(nRC != VOOSMP_ERR_None)
		return nRC;

	if(_tcslen(m_szDRMFile) > 0)
	{
		mpSource->SetParam(VOOSMP_SRC_PID_DRM_FILE_NAME , (void *)m_szDRMFile);
	}

	if(_tcslen(m_szDRMAPI) > 0)
	{
		mpSource->SetParam(VOOSMP_SRC_PID_DRM_API_NAME , (void *)m_szDRMAPI);
	}

	if(_tcslen(m_szAdapterDRMFile) > 0)
	{
		mpSource->SetParam(VOOSMP_SRC_PID_DRM_ADAPTER_FILE_NAME , (void *)m_szAdapterDRMFile);
	}

	if(_tcslen(m_szAdapterDRMAPI) > 0)
	{
		mpSource->SetParam(VOOSMP_SRC_PID_DRM_ADAPTER_API_NAME , (void *)m_szAdapterDRMAPI);
	}

	if(m_pAapterDRMObject)
	{
		mpSource->SetParam(VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT , m_pAapterDRMObject);
	}

	mpSource->SetParam(VOOSMP_PID_LISTENER, &m_sListener);

	if (strlen(m_pUseragent) > 0)
	{
		mpSource->SetParam(VOOSMP_SRC_PID_HTTP_HEADER, &m_sHttpHeader);
	}

	mpSource->SetParam(VOOSMP_SRC_PID_HTTP_PROXY_INFO, &m_sHttpProxy);

	if(strlen(m_pLiscenseText)>0)
	{
		mpSource->SetParam(VOOSMP_PID_LICENSE_TEXT,&m_pLiscenseText);
		mpEngine->SetParam(VOOSMP_PID_LICENSE_TEXT,&m_pLiscenseText);
	}
	else
	{
		VO_PCHAR strErrorLiscense = "Illegal Liscense";
		mpEngine->SetParam(VOOSMP_PID_LICENSE_TEXT, (void*)strErrorLiscense);
		mpSource->SetParam(VOOSMP_PID_LICENSE_TEXT, (void*)strErrorLiscense);
	}

	if(vostrlen(m_pLiscenseFilePath)>0)
	{
		mpSource->SetParam(VOOSMP_PID_LICENSE_FILE_PATH,&m_pLiscenseFilePath);
		mpEngine->SetParam(VOOSMP_PID_LICENSE_FILE_PATH,&m_pLiscenseFilePath);
	}

	nRC = CapPerfDataExtractor();
	if (nRC == VOOSMP_ERR_None)
	{
		VOLOGI("@@@###  SetCap bitRate = %d", m_OSCapData.nBitRate);
		mpSource->SetParam(VOOSMP_PID_PERFORMANCE_DATA, &m_OSCapData);
	}
	else
	{
		VOLOGE("@@@### No exactly cap config to set Controller!...");
	}

	nRC = mpSource->Open();
	if(nRC != VOOSMP_ERR_None)
		return nRC;

	void *pReadBufPtr = NULL;
	nRC = mpSource->GetParam(VOOSMP_SRC_PID_FUNC_READ_BUF, &pReadBufPtr);
	if(nRC != VOOSMP_ERR_None)
		return nRC;

	int nRtn = mpEngine->SetDataSource(pReadBufPtr , VOOSMP_FLAG_SOURCE_READBUFFER);
	mpEngine->SetParam(VOOSMP_PID_DRAW_VIDEO_DIRECTLY , &m_bDrawVideoDirectly);

	return nRtn;

}

int COnStreamMPImpl::GetProgramCount( int* pProgramCount)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->GetProgramCount(pProgramCount);
}

int COnStreamMPImpl::GetProgramInfo(int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->GetProgramInfo(nProgramIndex , ppProgramInfo);
}

int COnStreamMPImpl::GetCurTrackInfo( int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo )
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->GetCurTrackInfo(nTrackType , ppTrackInfo);
}

int COnStreamMPImpl::GetSample( int nTrackType, void* pSample)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	int n = mpSource->GetSample(nTrackType , pSample);
	if(mpSource->IsSubtitlePathSet())
		return n;
	//if(VOOSMP_ERR_None != n)
	//{
	//	if(this->mpEngine == NULL)
	//		return VOOSMP_ERR_Pointer;
	//	n = mpEngine->GetSubtitleSample(NULL,(voSubtitleInfo*)pSample);
	//}
	return n;
}

int COnStreamMPImpl::SelectProgram( int nProgram)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->SelectProgram(nProgram);
}

int COnStreamMPImpl::SelectStream( int nStream)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->SelectStream(nStream);
}

int COnStreamMPImpl::SelectTrack( int nTrack)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->SelectTrack(nTrack);
}


int COnStreamMPImpl::SelectLanguage( int nIndex)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->SelectLanguage(nIndex);
}

int COnStreamMPImpl::GetLanguage(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->GetLanguage(ppLangInfo);
}

int COnStreamMPImpl::Close()
{
	if(mpEngine == NULL || mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	mpEngine->Stop();
	mpSource->Stop();
	mpSource->Close();

	return VOOSMP_ERR_Implement;
}

int COnStreamMPImpl::Run()
{
	int nRC = VOOSMP_ERR_Pointer;

	if(mpEngine != NULL)
	{
		nRC = mpEngine->Run();
		if(nRC != VOOSMP_ERR_None)
			return nRC;
	}	

	if(mpSource != NULL)	
	{
		nRC = mpSource->Run();	
	}

    VO_BOOL b = VO_TRUE;
    SetParam(VOOSMP_PID_VR_USERCALLBACK,&b);

	if(nRC == VOOSMP_ERR_None && m_ccMan!=NULL && mEnableInnerCloseCaption)
    {
		m_ccMan->Show(true);
	}

	return nRC;
}

int COnStreamMPImpl::Pause()
{
	int nRC = VOOSMP_ERR_Pointer;

	if(mpEngine != NULL)
	{
		nRC = mpEngine->Pause();
		if(nRC != VOOSMP_ERR_None)
			return nRC;
	}	

	if(mpSource != NULL)	
	{
		nRC = mpSource->Pause();	
	}

	return nRC;
}


int COnStreamMPImpl::Stop()
{
	int nRC = VOOSMP_ERR_Pointer;
	CloseCloseCaptionTimer();
	//DeleteSubtitleManager();

    VO_BOOL b = VO_FALSE;
    SetParam(VOOSMP_PID_VR_USERCALLBACK,&b);

	if(mpEngine != NULL)
	{
		nRC = mpEngine->Stop();
		if(nRC != VOOSMP_ERR_None)
			return nRC;
	}	

	if(mpSource != NULL)	
	{
		nRC = mpSource->Stop();
	}

	return nRC;
}

int COnStreamMPImpl::GetPos()
{
	int nCurPos = 0;
	if(mpEngine != NULL)
	{
		mpEngine->GetCurPos(&nCurPos);
	}

	return nCurPos;
}

int COnStreamMPImpl::SetPos( int nPos)
{
   VOOSMP_STATUS oldStatus = VOOSMP_STATUS_MAX;
   if (mpEngine!=NULL)
   {
       mpEngine->GetStatus(&oldStatus);
   }
 
	if(this->m_ccMan)
		m_ccMan->EnableDraw(false);
	if(oldStatus == VOOSMP_STATUS_RUNNING)
		Pause();

	long long lCusPos = nPos;
	if(mpSource != NULL)	
	{
		nPos = mpSource->SetPos(&lCusPos);
	}

	if(mpEngine != NULL)
	{
		mpEngine->SetCurPos((int)lCusPos);
	}

	if(oldStatus == VOOSMP_STATUS_RUNNING)
		Run();
	if(this->m_ccMan)
		m_ccMan->EnableDraw(true);

	return (int)lCusPos;
}

int COnStreamMPImpl::GetDuration( long long* pDuration)
{
	if(mpSource == NULL)
		return VOOSMP_ERR_Pointer;

	return mpSource->GetDuration(pDuration);
}

int COnStreamMPImpl::GetParam( int nParamID, void* pValue)
{
	if(nParamID == VOOSMP_PID_CLOSED_CAPTION_SETTINGS)
	{
		if(m_ccMan==NULL || pValue ==NULL)
			return VOOSMP_ERR_Pointer;
		*((voSubTitleFormatSetting**)pValue) = (voSubTitleFormatSetting*)m_ccMan->GetSettings();
		return VOOSMP_ERR_None;
	}
	else if (nParamID == VOOSMP_PID_AUDIO_DEC_BITRATE)
	{
		if(mpEngine != NULL)
			return mpEngine->GetParam(nParamID , pValue);
	}
	else if (nParamID == VOOSMP_PID_VIDEO_DEC_BITRATE)
	{
		if(mpEngine != NULL)
			return mpEngine->GetParam(nParamID , pValue);
	}
	if((nParamID & 0x03000000) == 0x03000000)
	{
		if(mpSource != NULL)
		{
			return mpSource->GetParam(nParamID , pValue);
		}
	}
	else
	{
		if(mpEngine != NULL)
		{
			int nRC =  mpEngine->GetParam(nParamID , pValue);
			if(nRC == VOOSMP_ERR_None)
				return nRC ;
		}

		if(mpSource != NULL)
		{
			return mpSource->GetParam(nParamID , pValue);
		}
	}

	return VOOSMP_ERR_Implement;
}
void COnStreamMPImpl::HandleTimerEvent(int idEvent)
{
	if(m_ccMan==NULL)
		return;
	if(m_ccMan->GetRenderWindow() == NULL)
		return;
	VOOSMP_STATUS pStatus;
	memset(&pStatus,0,sizeof(pStatus));
	if(GetStatus (&pStatus) ==0)
	//if(idEvent == _TIMER_ID_SUBTITLE)
	{
		if(pStatus != VOOSMP_STATUS_RUNNING && pStatus != VOOSMP_STATUS_PAUSED)
		{
			//m_ccMan->Show(false);
			return;
		}
		int nCurr = this->GetPos();
		voSubtitleInfo info;
		memset(&info, 0, sizeof(voSubtitleInfo));
		info.nTimeStamp = nCurr;
		int nRtn = GetSample(VOOSMP_SS_SUBTITLE,&info);
		if(nRtn==VOOSMP_ERR_None)
		{
			//return;
			VOOSMP_VIDEO_FORMAT sVideoFormat;

			memset (&sVideoFormat, 0, sizeof (VOOSMP_VIDEO_FORMAT));

			GetParam (VOOSMP_PID_VIDEO_FORMAT, &sVideoFormat);

			if(sVideoFormat.Height>0 && sVideoFormat.Width>0 &&(this->m_nVideoHeight != sVideoFormat.Height || m_nVideoWidth != sVideoFormat.Width))
			{
				m_nVideoHeight = sVideoFormat.Height;
				m_nVideoWidth = sVideoFormat.Width;
				//if(m_nVideoHeight>0 && m_nVideoWidth>0)// && m_ccMan
					this->ComputeXYRate();
					//m_ccMan->SetXYRate(m_nVideoWidth/(float)m_nVideoHeight);
			}
			if(m_ccMan)
				m_ccMan->SetData(&info, true);
		}
		if(m_ccMan)
			m_ccMan->CheckViewShowStatus(nCurr);
	}
            	
}

void COnStreamMPImpl::StartCloseCaptionTimer()
{
	if(m_ccMan==NULL)
		return;
	m_ccMan->Start();
	//VO_BOOL b = VO_TRUE;
	//if(b)
	//{
	//	b = b;
	//}
	//SetParam(VOOSMP_PID_VR_USERCALLBACK,&b);

}
void COnStreamMPImpl::CloseCloseCaptionTimer()
{
	if(m_ccMan==NULL)
		return;

    //now, buffering need this callback event too
	//VO_BOOL b = VO_FALSE;
	//SetParam(VOOSMP_PID_VR_USERCALLBACK,&b);

	m_ccMan->Stop();
	m_ccMan->Show(false);

	//delete m_ccMan;
	//m_ccMan = NULL;
}

void COnStreamMPImpl::CreateCloseCaptionUI()
{
	//if(!IsInnerCloseCaptionEnable())
	//	return;
	if(m_ccMan!=NULL){
		m_ccMan->Show(true);
   		StartCloseCaptionTimer();

        //now, buffering need this callback event too
		//VO_BOOL b = VO_TRUE;
		//SetParam(VOOSMP_PID_VR_USERCALLBACK,&b);
	}
	if(m_ccMan!=NULL || mhWnd == NULL)
		return;
   	m_ccMan = new voSubTitleManager();
	if (m_ccMan == NULL)
		return;
	m_ccMan->SetParentWnd(mhWnd);
	m_ccMan->SetParam(VOOSMP_PID_LISTENER, &m_sListener);
	m_ccMan->SetDisplayRect(mrcDraw);
	m_ccMan->SetParam (VOOSMP_PID_VIDEO_RENDER_TYPE, &m_renderType);

	//m_ccMan->
   	StartCloseCaptionTimer();
}

int COnStreamMPImpl::SetParam(int nParamID, void* pValue)
{	
	if(nParamID == VOOSMP_PID_DRAW_RECT)
	{
		mrcDraw = *((VOOSMP_RECT*)pValue);
		if(mpEngine != NULL)
		{
			mpEngine->SetParam (VOOSMP_PID_DRAW_RECT, &mrcDraw);
		}

		if(m_ccMan != NULL)
			m_ccMan->SetDisplayRect(mrcDraw);
		return VOOSMP_ERR_None;
	}
	if (nParamID == VOOSMP_PID_COMMON_CCPARSER)
	{
		int n = 0;
		//if(pValue != NULL)
		//	n = mpEngine->SetParam(nParamID, pValue);

		if(pValue!=NULL && *((int*)pValue) == 1)
		{
			mEnableInnerCloseCaption = true;
			CreateCloseCaptionUI();
		}
		else
		{
			if(!m_bID3Picture){
				mEnableInnerCloseCaption = false;
				CloseCloseCaptionTimer();
			}
		}
		return VOOSMP_ERR_None;//setCommonCCParser(id,param);
	}
	if(nParamID == VOOSMP_PID_LISTENER)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		VOOSMP_LISTENERINFO *pFunc = (VOOSMP_LISTENERINFO *)pValue;
	
		mCallbackFunc.pListener = pFunc->pListener;
		mCallbackFunc.pUserData = pFunc->pUserData;
		if(m_ccMan!=NULL){
			m_ccMan->SetParam(VOOSMP_PID_LISTENER, &m_sListener);
		}

		return VOOSMP_ERR_None;
	}
	else if(nParamID == VOOSMP_PID_VIEW_ACTIVE)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		bool bValue = *((bool *)pValue);

		if(mpEngine != NULL)
		{
			if(bValue == true)
			{
				return mpEngine->SetView(mhWnd);
			}
			else
			{
				return mpEngine->SetView(NULL);
			}
		}
		else
		{
			return VOOSMP_ERR_Implement;
		}
		
	}
	else if(nParamID == VOOSMP_PID_LICENSE_TEXT)
	{
		if(pValue!=NULL)
			strcpy(m_pLiscenseText,(char*)pValue);
	}
	else if (nParamID == VOOSMP_PID_LICENSE_FILE_PATH)
	{
		if(pValue!=NULL)
			vostrcpy(m_pLiscenseFilePath, (TCHAR*)pValue);	
	}
	else if (nParamID == VOOSMP_SRC_PID_HTTP_HEADER)
	{
		VOOSMP_SRC_HTTP_HEADER* pHeader = (VOOSMP_SRC_HTTP_HEADER*)pValue;
		if (!strcmp((char*)pHeader->pszHeaderName, "User-Agent"))
		{
			strcpy(m_pUseragent , (char*)pHeader->pszHeaderValue);
			m_sHttpHeader.pszHeaderName = (unsigned char*)"User-Agent";
			m_sHttpHeader.pszHeaderValue = (unsigned char*)m_pUseragent;
		}
	}
	else if (nParamID == VOOSMP_SRC_PID_HTTP_PROXY_INFO)
	{
		if (pValue)
		{
			VOOSMP_SRC_HTTP_PROXY* pHeader = (VOOSMP_SRC_HTTP_PROXY*)pValue;
			if (strlen( (char*)pHeader->pszProxyHost ) > 0)
				strcpy( m_pProxyhost, (char*)pHeader->pszProxyHost);
			else
				m_pProxyhost[0] = 0;
			m_nProxyport = pHeader->nProxyPort;

			m_sHttpProxy.pszProxyHost = (unsigned char*)m_pProxyhost;
			m_sHttpProxy.nProxyPort = m_nProxyport;
		}
		else
		{
			strcpy( m_pProxyhost, "");
			m_sHttpProxy.pszProxyHost = (unsigned char*)m_pProxyhost;
			m_sHttpProxy.nProxyPort = 0;
		}

		if (mpSource)
			mpSource->SetParam(VOOSMP_SRC_PID_HTTP_PROXY_INFO, &m_sHttpProxy);
	}
	else if (nParamID == VOOSMP_PID_DRAW_VIDEO_DIRECTLY)
	{
		int nRC = VOOSMP_ERR_None;
		m_bDrawVideoDirectly = *(bool*)pValue;
		if (mpEngine)
			nRC = mpEngine->SetParam(nParamID , &m_bDrawVideoDirectly);
		return nRC;
	}
	else if (nParamID == VOOSMP_PID_AUDIO_PLAYBACK_SPEED)
	{
		if (mpSource)
			mpSource->SetParam(nParamID , pValue);
		if (mpEngine)
			mpEngine->SetParam(nParamID , pValue);

		return VOOSMP_ERR_None;
	}
	else if((nParamID & 0x03000000) == 0x03000000)
	{
		if(mpSource != NULL && mpSource->IsInit())
		{
			return mpSource->SetParam(nParamID , pValue);
		}
		else if(nParamID == VOOSMP_SRC_PID_DRM_FILE_NAME)
		{
			memset(m_szDRMFile , 0 , sizeof(TCHAR) * 1024);
			_tcscpy(m_szDRMFile , (TCHAR *)pValue);
		}
		else if(nParamID == VOOSMP_SRC_PID_DRM_API_NAME)
		{
			memset(m_szDRMAPI , 0 , sizeof(TCHAR) * 256);
			_tcscpy(m_szDRMAPI , (TCHAR *)pValue);
		}
		else if (nParamID == VOOSMP_SRC_PID_DRM_ADAPTER_FILE_NAME)
		{
			memset(m_szAdapterDRMFile , 0 , sizeof(TCHAR) * 1024);
			_tcscpy(m_szDRMAPI , (TCHAR *)pValue);
		}
		else if (nParamID == VOOSMP_SRC_PID_DRM_ADAPTER_API_NAME)
		{
			memset(m_szAdapterDRMAPI , 0 , sizeof(TCHAR) * 256);
			_tcscpy(m_szDRMAPI , (TCHAR *)pValue);
		}
		else if (nParamID == VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT)
		{
			m_pAapterDRMObject = pValue;
		}
		else 
		{
			return VOOSMP_ERR_Implement;
		}
	}
	else if(nParamID == VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF || nParamID == VOOSMP_PID_VIDEO_PERFORMANCE_OPTION)
	{
		int nRC = VOOSMP_ERR_None;

		nRC = nRC | mpEngine->SetParam(nParamID , pValue);

		return nRC;
	}
	else
	{
		int nRC = VOOSMP_ERR_None;
		if(nParamID == 787911)
		{
			int kk = 0;
			kk++;
		}
		if(mpSource != NULL && mpSource->IsInit())
		{
			if (nParamID != VOOSMP_PID_VIDEO_RENDER_TYPE)
			{
				/* OSSource no need to process this param ID, this for OSEng */
				nRC = mpSource->SetParam(nParamID , pValue);
			}
		}
		else if(nParamID == VOOSMP_SRC_PID_DRM_FILE_NAME)
		{
			memset(m_szDRMFile , 0 , sizeof(TCHAR) * 1024);
			_tcscpy(m_szDRMFile , (TCHAR *)pValue);
		}
		else if(nParamID == VOOSMP_SRC_PID_DRM_API_NAME)
		{
			memset(m_szDRMAPI , 0 , sizeof(TCHAR) * 256);
			_tcscpy(m_szDRMAPI , (TCHAR *)pValue);
		}
		else if (nParamID == VOOSMP_SRC_PID_DRM_ADAPTER_FILE_NAME)
		{
			memset(m_szAdapterDRMFile , 0 , sizeof(TCHAR) * 1024);
			_tcscpy(m_szDRMAPI , (TCHAR *)pValue);
		}
		else if (nParamID == VOOSMP_SRC_PID_DRM_ADAPTER_API_NAME)
		{
			memset(m_szAdapterDRMAPI , 0 , sizeof(TCHAR) * 256);
			_tcscpy(m_szDRMAPI , (TCHAR *)pValue);
		}
		else if (nParamID == VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT)
		{
			m_pAapterDRMObject = pValue;
		}

		if(nParamID == VOOSMP_PID_VIDEO_RENDER_TYPE)
		{
			this->m_renderType = *(VOOSMP_RENDER_TYPE*)pValue;
		}
		if(mpEngine != NULL)
        {
            if(nRC == VOOSMP_ERR_Implement)
                nRC = mpEngine->SetParam(nParamID , pValue);
            else
                nRC = nRC | mpEngine->SetParam(nParamID , pValue);
        }
		if(m_ccMan != NULL)
		{
			if(VOOSMP_PID_VIDEO_ASPECT_RATIO == nParamID)
			{
				m_aspectRatio = *(VOOSMP_ASPECT_RATIO*)pValue;
				this->ComputeXYRate();
			}
			m_ccMan->SetParam (nParamID, pValue);
		}
		return nRC;
	}

	return VOOSMP_ERR_Implement;
}

int COnStreamMPImpl::Init()
{
	CloseCloseCaptionTimer();

	if(mpEngine == NULL)
	{
		mpEngine = new OnStreamEngine(mnPlayerType , &mInitParam, mnInitParamFlag);
		if(mpEngine == NULL)
			return VOOSMP_ERR_Pointer;
	}

	if(mpSource == NULL)
	{
		mpSource = new OnStreamSrc(mnPlayerType , &mInitParam, mnInitParamFlag);
		if(mpSource == NULL)
			return VOOSMP_ERR_Pointer;
	}

	mpEngine->Init(mnPlayerType , &mInitParam, mnInitParamFlag);

	VOLOGINIT((VO_TCHAR*) mInitParam.pWorkingPath);

	m_sListener.pUserData = this;
	m_sListener.pListener = OnListener;

	//VO_U32 nValue = 20111111;
	//mpEngine->SetParam(VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF , &nValue);

	mpEngine->SetParam(VOOSMP_PID_LISTENER, &m_sListener);
	mpEngine->SetParam(VOOSMP_PID_DRAW_COLOR, &mnColorType);

	memset(m_szDRMFile , 0 , sizeof(TCHAR) * 1024);
	memset(m_szDRMAPI , 0 , sizeof(TCHAR) * 256);

	return VOOSMP_ERR_None;
}

int COnStreamMPImpl::Uninit()
{
	if(m_ccMan)
	{
		delete m_ccMan;
		m_ccMan = NULL;
	}
	if(mpEngine != NULL)
	{
		mpEngine->Uninit();
	}

	if(mpSource != NULL)
	{
		mpSource->Uninit();
	}

	VOLOGUNINIT();
	return VOOSMP_ERR_None;
}
float COnStreamMPImpl::ComputeXYRate()
{
	float fRate = 1.333f;
	int n = m_aspectRatio;//*(int*)pParam1;
	//m_aspectRatio = (VOOSMP_ASPECT_RATIO)n;
	switch(n)
	{
	case VOOSMP_RATIO_00:
		{
			if(m_nVideoHeight != 0)
				fRate = (m_nVideoWidth/(float)m_nVideoHeight);
		}
		
		break;
	case VOOSMP_RATIO_11:
		fRate = 1;
		break;
	case VOOSMP_RATIO_43:
		fRate = 4.0f/3;
		break;
	case VOOSMP_RATIO_169:
		fRate = 16.0f/9;
		break;
	case VOOSMP_RATIO_21:
		fRate = 2.0f;
		break;
	case VOOSMP_RATIO_2331:
		fRate = 233/100.0f;
		break;
	case VOOSMP_RATIO_AUTO:
		if(m_nVideoHeight != 0)
			fRate = (m_nVideoWidth/(float)m_nVideoHeight);
		break;
	default:
		if((m_aspectRatio&0xFFFF)!=0)
			fRate = (m_aspectRatio >> 16)/(float) (m_aspectRatio&0xFFFF);
		break;
	}
	if(m_ccMan)
		m_ccMan->SetXYRate(fRate);
	return fRate;
}
int	COnStreamMPImpl::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	switch(nID)
	{
	case VOOSMP_CB_Video_Render_Complete:
	case VOOSMP_CB_VR_USERCALLBACK:
		{
			if(m_ccMan)
			{
				//for thread safe
				VOOSMP_VR_USERCALLBACK_TYPE* pVr = (VOOSMP_VR_USERCALLBACK_TYPE*)pParam1;
				if(pVr)
					m_ccMan->Draw((HDC)pVr->pDC, (void*)(pParam1));//(NULL);//(*(HDC*)pParam1)
			}
		}
		break;
	case VOOSMP_CB_VideoAspectRatio:
		{
			if(pParam1!=NULL)
			{
				float fRate = 1.333f;
				int n = *(int*)pParam1;
				m_aspectRatio = (VOOSMP_ASPECT_RATIO)n;
				ComputeXYRate();
			}
		}
		break;
	case VOOSMP_CB_VideoSizeChanged:
		{
			if(pParam1 == NULL || pParam2 == NULL)
				return VOOSMP_ERR_Pointer;

			m_nVideoWidth = *(int*)pParam1;
			m_nVideoHeight = *(int*)pParam2;
			if(m_ccMan)
			{
				if(m_aspectRatio == VOOSMP_RATIO_00)
					m_ccMan->SetXYRate(m_nVideoWidth/(float)m_nVideoHeight);
			}
		}
		break;
	case VOOSMP_CB_VideoStartBuff:
		{
			if(m_ccMan)
			{
				//for thread safe
				if(m_ccMan->GetRenderWindow())
					::PostMessage(m_ccMan->GetRenderWindow(),WM_SUBTITLE_SHOW,0,0);
			}
		}
		break;
	case VOOSMP_CB_VideoStopBuff:
		{
			if(m_ccMan && this->mEnableInnerCloseCaption)
			{
				if(m_ccMan->GetRenderWindow())
					::PostMessage(m_ccMan->GetRenderWindow(),WM_SUBTITLE_SHOW,1,0);
			}
		}
		break;
	}

	if(mCallbackFunc.pListener != NULL)
	{
		return mCallbackFunc.pListener(mCallbackFunc.pUserData , nID , pParam1 , pParam2);
	}


	return VOOSMP_ERR_Implement;
}
int COnStreamMPImpl::GetStatus (VOOSMP_STATUS * pStatus)
{
	if (this->mpEngine== NULL)
		return VOOSMP_ERR_Pointer;

	return mpEngine->GetStatus(pStatus);
}

int COnStreamMPImpl::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	COnStreamMPImpl * pPlayer = (COnStreamMPImpl *)pUserData;
	if(nID == VOOSMP_PID_CLOSED_CAPTION_NOTIFY_EVENT)
	{
		int nParam1 = *(int*)pParam1;
		int nParam2 = *(int*)pParam2;
		if(VOOSMP_FLAG_SUBTITLE_VR_USERCALLBACK  == nParam1)
		{
			VO_BOOL b = nParam2==0?VO_FALSE:VO_TRUE;
			if(b)
			{
				b = b;
			}
			pPlayer->SetParam(VOOSMP_PID_VR_USERCALLBACK,&b);
		}
		else
			pPlayer->HandleTimerEvent(0);
		return VOOSMP_ERR_None;
	}
	if(nID == VOOSMP_CB_PlayComplete)
	{
		if(pPlayer->m_ccMan)
		{
			//for thread safe
			//if(pPlayer->m_ccMan->GetRenderWindow())
			//	::PostMessage(pPlayer->m_ccMan->GetRenderWindow(),WM_SUBTITLE_SHOW,0,0);
			pPlayer->m_ccMan->Show(false);
		}
	}
	if(nID == VOOSMP_CB_Metadata_Arrive )
	{
		int nParam1 = *(int*)pParam1;
		if(nParam1 == VOOSMP_SS_IMAGE)
		{
			int iTemp = 1;
			if(pPlayer->m_ccMan == NULL)
				pPlayer->SetParam(VOOSMP_PID_COMMON_CCPARSER, &iTemp);

			if(pPlayer->m_ccMan)
			{
				pPlayer->m_bID3Picture = true;
				pPlayer->m_ccMan->SetData(*((voSubtitleInfo**)pParam2),false);

				//to notify parent to redraw
				//pPlayer->HandleEvent (VOMP_CB_VideoReadyToRender, NULL, NULL);
				
			}
		}
	}

	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int COnStreamMPImpl::CapPerfDataExtractor()
{
	bool  bFoundCfg = false;
	FILE* pFile = NULL;
	char  pCapFilePath[256] = {0};
	char* pFileBuff = NULL;
	int   nFileBuffLength = 0;
	int   nCapCoreNum = voOS_GetCPUNum();
	VO_U64   nCapFrequency = voOS_GetCPUFrequency() * 1000;

	VOLOGI("@@@### CoreNum = %d, CPUFeq = %lu", nCapCoreNum, nCapFrequency);
	WideCharToMultiByte(CP_ACP, 0, (TCHAR*)mInitParam.pWorkingPath, -1,	pCapFilePath, 256, NULL, NULL);
	strcat (pCapFilePath, "cap.xml");

	int nCapItemsCount = 0;
	VO_U64 szFrequency[256];
	VOOSMP_PERF_DATA szCapItems[256];

	pFile = fopen (pCapFilePath, "r");
	if (pFile)
	{
		fseek(pFile, 0 ,SEEK_END);
		nFileBuffLength = ftell(pFile) + 1;
		fseek(pFile, 0, SEEK_SET);

		pFileBuff = new char[nFileBuffLength];
		memset (pFileBuff, 0, nFileBuffLength);
		fread(pFileBuff, 1, nFileBuffLength - 1, pFile);

		fclose (pFile);
	}
	else
		return VOOSMP_ERR_Unknown;
	
	if ( pFileBuff )
	{
		CXMLLoad *pXmlLoad = new CXMLLoad;
		if ( pXmlLoad )
		{
			pXmlLoad->SetLibOperator( (VO_LIB_OPERATOR*)mInitParam.pLibOP);
			pXmlLoad->SetWorkPath( (TCHAR*)mInitParam.pWorkingPath);
			if (pXmlLoad->OpenParser())
			{
				delete pXmlLoad;
				delete []pFileBuff;
				return VOOSMP_ERR_Unknown;
			}
			pXmlLoad->LoadXML(pFileBuff, nFileBuffLength, voXML_FLAG_SOURCE_BUFFER);

			int size = 0;
			void *pNode = NULL;
			void *pItemNode = NULL;
			pXmlLoad->GetFirstChild(NULL,&pNode);
			pXmlLoad->GetFirstChild(pNode,&pItemNode);
			while (pItemNode)
			{
				int nCoreNum = 0;
				VO_U64 nFrequency = 0;
				int   nAttributeVal = 0;
				void* pAttributeNode = NULL;
				char* pAttributeName = NULL;
				char* pAttributeVal = NULL;
				VOOSMP_PERF_DATA szTmpCapItems;
				memset (&szTmpCapItems, 0, sizeof (VOOSMP_PERF_DATA));

				pXmlLoad->GetFirstChild(pItemNode,&pAttributeNode);
				while (pAttributeNode)
				{
					pXmlLoad->GetTagValue(pAttributeNode, &pAttributeVal,size);
					pXmlLoad->GetTagName(pAttributeNode, &pAttributeName,size);
					if (strcmp (pAttributeName, "Core") == 0)
					{
						nCoreNum = atoi( pAttributeVal );
					}
					else if (strcmp (pAttributeName, "Frequency") == 0)
					{
						int nAtrrValLen = strlen (pAttributeVal);
						if (nAtrrValLen > 6)
						{
							pAttributeVal [nAtrrValLen - 3] = 0;
						}
						nFrequency = atoi( pAttributeVal );
						nFrequency = nFrequency * 1000;
					}
					else if (strcmp (pAttributeName, "CodecType") == 0)
					{
						nAttributeVal = atoi( pAttributeVal );
						szTmpCapItems.nCodecType = nAttributeVal;
					}
					else if (strcmp (pAttributeName, "BitRate") == 0)
					{
						nAttributeVal = atoi( pAttributeVal );
						szTmpCapItems.nBitRate = nAttributeVal;
					}
					else if (strcmp (pAttributeName, "VideoWidth") == 0)
					{
						nAttributeVal = atoi( pAttributeVal );
						szTmpCapItems.nVideoWidth = nAttributeVal;
					}
					else if (strcmp (pAttributeName, "VideoHeight") == 0)
					{
						nAttributeVal = atoi( pAttributeVal );
						szTmpCapItems.nVideoHeight = nAttributeVal;
					}
					else if (strcmp (pAttributeName, "ProfileLevel") == 0)
					{
						nAttributeVal = atoi( pAttributeVal );
						szTmpCapItems.nProfileLevel = nAttributeVal;
					}
					else if (strcmp (pAttributeName, "FPS") == 0)
					{
						nAttributeVal = atoi( pAttributeVal );
						szTmpCapItems.nFPS = nAttributeVal;
					}

					void* pTmpNode = pAttributeNode;
					pXmlLoad->GetNextSibling(pTmpNode,&pAttributeNode);
				}

				if (nCapCoreNum == nCoreNum)
				{
					szFrequency[nCapItemsCount] = nFrequency;
					memcpy (szCapItems + nCapItemsCount, &szTmpCapItems, sizeof (VOOSMP_PERF_DATA));
					nCapItemsCount++;
				}

				void* pTempNode = pItemNode;
				pXmlLoad->GetNextSibling(pTempNode,&pItemNode);
			}

			pXmlLoad->CloseParser();
			delete pXmlLoad;
		}
		else
			return VOOSMP_ERR_Unknown;

		if (pFileBuff) delete []pFileBuff;
	}
	else
		return VOOSMP_ERR_Unknown;

	if (nCapItemsCount > 0)
	{
		for (int i = 0; i < nCapItemsCount; ++i)
		{
			for (int j = 0; j < nCapItemsCount; ++j)
			{
				if (szFrequency[i] < szFrequency[j])
				{
					VO_U64 uTmp = szFrequency[i];
					szFrequency[i] = szFrequency[j];
					szFrequency[j] = uTmp;

					VOOSMP_PERF_DATA szCapTmpItem;
					memcpy (&szCapTmpItem, szCapItems+i, sizeof (VOOSMP_PERF_DATA));
					memcpy (szCapItems + i, szCapItems+j, sizeof (VOOSMP_PERF_DATA));
					memcpy (szCapItems + j, &szCapTmpItem, sizeof (VOOSMP_PERF_DATA));
				}
			}
		}

		int nFindPos;
		for (nFindPos = 0; nFindPos < nCapItemsCount; ++nFindPos)
		{
			if (nCapFrequency < szFrequency[nFindPos])
			{
				VOLOGI("@@@### Choosed cap itme: DeviceFeq = %lu, CfgFeq = %lu", nCapFrequency, szFrequency[nFindPos]);
				bFoundCfg = true;
				memcpy (&m_OSCapData, szCapItems+nFindPos, sizeof (VOOSMP_PERF_DATA));
				break;
			}
		}
	}

	if (!bFoundCfg)
	{
		VOLOGE("@@@### can not find exactly config cap intem!...");
		return VOOSMP_ERR_Unknown;
	}

	return VOOSMP_ERR_None;
}