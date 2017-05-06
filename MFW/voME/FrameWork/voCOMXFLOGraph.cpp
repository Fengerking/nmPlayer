	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFLOGraph.cpp

	Contains:	voCOMXFLOGraph class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <string.h>

#if defined(_WIN32)
#include <tchar.h>
#include <windows.h>
#elif defined(_LINUX)
#include <dlfcn.h>
#include "voString.h"
#endif

#include "voOMX_Index.h"
#include "voOMXOSFun.h"
#include "voCOMXFLOGraph.h"

#define LOG_TAG "voCOMXFLOGraph"
#include "voLog.h"

 
#define VOME_CONFIG_CORE_FILE _T("vomeCore.cfg")
#define VOME_CONFIG_COMP_FILE _T("vomeComp.cfg")

voCOMXFLOGraph * voCOMXFLOGraph::g_pFLOGraph = NULL;

voCOMXFLOGraph::voCOMXFLOGraph()
: m_pCompDS(NULL)
, m_pCompVD(NULL)
, m_libFLOEngine(NULL)
, m_hFLOEngine(NULL)
{
	VOLOGF ();

	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);

	g_pFLOGraph = this;

	memset(&m_FLOEngineAPI, 0, sizeof(VO_FLOENGINE_API));
	memset(&m_FLOEngineCallBack, 0, sizeof(VO_FLOENGINE_CALLBACK));
}

voCOMXFLOGraph::~voCOMXFLOGraph()
{
	VOLOGF ();

	g_pFLOGraph = NULL;

	DestroyFLOEngine();
}

OMX_ERRORTYPE	voCOMXFLOGraph::Init (OMX_PTR hInst)
{
	VOLOGF ();

	voCOMXBaseGraph::Init (hInst);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXFLOGraph::Playback (VOME_SOURCECONTENTTYPE * pSource)
{
	VOLOGF ();
/*
	if (!m_bInit)
		Init (NULL);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	if (m_pChain->m_pLibOP == NULL && m_pWorkPath != NULL)
		SetParam (OMX_VO_IndexLibraryOperator, &m_libOP);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = Stop ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from Stop!", errType);
		return errType;
	}

	voCOMXAutoLock lock (&m_tmState);

	errType = m_pChain->ReleaseBoxes ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from m_pChain->ReleaseBoxes!", errType);
		return errType;
	}


	errType = AddComponent ("OMX.VisualOn.DataSource", &m_pCompDS);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Clock.Time", &m_pCompClock);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Audio.Sink", &m_pCompAR);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Video.Sink", &m_pCompVR);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompDS, 0, m_pCompAD, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompDS, 1, m_pCompVD, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompClock, 0, m_pCompDS, 2, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	OMX_U32 nTimeStampOwner = 1; 
	m_pCompVD->SetParameter(m_pCompVD, (OMX_INDEXTYPE)OMX_VO_IndexTimeStampOwner, &nTimeStampOwner);

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
//	clkRef.eClock = OMX_TIME_RefClockVideo;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);
*/

	OMX_ERRORTYPE errType = voCOMXBaseGraph::Playback (pSource);
	if (errType != OMX_ErrorNone)
		return errType;

	m_nVideoWidth = 320;
	m_nVideoHeight = 240;

	GetComponentByName ("OMX.VisualOn.DataSource", &m_pCompDS);
	if (m_pCompDS != NULL)
	{
		CreateFLOEngine();
		if(m_hFLOEngine == NULL)
			return OMX_ErrorUndefined;

#ifdef _WIN32
		WideCharToMultiByte (CP_ACP, 0, (LPWSTR) pSource->pSource, -1, m_szSource, 1024, NULL, NULL);
		strcpy(m_szSource, "rtsp://10.2.64.104/wtcc.pcap");
		if(_strnicmp(m_szSource, "rtsp://", 7) != 0)
			return OMX_ErrorNotImplemented;
#elif defined(_LINUX)
		strcpy(m_szSource, (OMX_STRING)pSource->pSource); 
	//	strcpy(m_szSource, "rtsp://10.2.64.104/wtcc.pcap");
		if(strncasecmp(m_szSource, "rtsp://", 7) != 0)
			return OMX_ErrorNotImplemented;
#endif // _WIN32

		m_FLOEngineCallBack.OnEvent = voCOMXFLOGraph::OnEvent;
		m_FLOEngineCallBack.OnFrame = voCOMXFLOGraph::OnFrame;
		m_FLOEngineAPI.SetParam(m_hFLOEngine, VO_PID_FLOENGINE_CALLBACK, &m_FLOEngineCallBack);

		VO_U32 ret = m_FLOEngineAPI.Open(m_hFLOEngine, m_szSource);
		if(ret != VO_ERR_FLOENGINE_OK)
		{
			VOLOGE("FLOEngine Open Failure");
			return OMX_ErrorUndefined;
		}

		ret = m_FLOEngineAPI.Start(m_hFLOEngine);
		if(ret != VO_ERR_FLOENGINE_OK)
		{
			VOLOGE("FLOEngine Start Failure");
			return OMX_ErrorUndefined;
		}

		GetComponentByName ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
		if (m_pCompVD != NULL)
		{
			OMX_U32 nTimeStampOwner = 1; 
			m_pCompVD->SetParameter(m_pCompVD, (OMX_INDEXTYPE)OMX_VO_IndexTimeStampOwner, &nTimeStampOwner);
		}
	}

	m_sStatus = VOME_STATUS_Loaded;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFLOGraph::Close (void)
{
	VOLOGF ();

	Stop();

	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	voCOMXAutoLock lock (&m_tmState);

	if(m_hFLOEngine != NULL)
	{
		m_FLOEngineAPI.Close(m_hFLOEngine);
		m_hFLOEngine = NULL;
	}

	OMX_ERRORTYPE errType = m_pChain->ReleaseBoxes ();

	return errType;
}

OMX_ERRORTYPE  voCOMXFLOGraph::Run (void)
{
	VOLOGF ();

	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	errType = m_pChain->Run ();
	if (errType == OMX_ErrorNone)
		m_sStatus = VOME_STATUS_Running;

	m_nStartRunTime = voOMXOS_GetSysTime ();

	//if(m_hFLOEngine != NULL)
	//{
	//	m_FLOEngineAPI.Start(m_hFLOEngine);
	//}

	return errType;
}

OMX_ERRORTYPE  voCOMXFLOGraph::Pause (void)
{
	VOLOGF ();

	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	errType = m_pChain->Pause ();
	if (errType == OMX_ErrorNone)
		m_sStatus = VOME_STATUS_Paused;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXFLOGraph::Stop (void)
{
	VOLOGF ();

	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	if(m_hFLOEngine != NULL)
	{
		m_FLOEngineAPI.Stop(m_hFLOEngine);
	}

	errType = m_pChain->Stop ();
	if (errType == OMX_ErrorNone)
		m_sStatus = VOME_STATUS_Stopped;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXFLOGraph::GetDuration (OMX_S32 * pDuration)
{
	VOLOGF ();
	voCOMXAutoLock lock (&m_tmState);

	*pDuration = 0;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXFLOGraph::GetCurPos (OMX_S32 * pCurPos)
{
	VOLOGF ();

	return OMX_ErrorUndefined;
}

OMX_ERRORTYPE  voCOMXFLOGraph::SetCurPos (OMX_S32 nCurPos)
{
	VOLOGF ();

	return OMX_ErrorNotImplemented;
}

OMX_U32 voCOMXFLOGraph::voGraphMessageHandle (void)
{
	while (!m_bMsgThreadStop)
	{
		m_tsMessage.Down ();

		GRAPH_MESSAGE_TYPE * pMsg = (GRAPH_MESSAGE_TYPE *)m_tqMessage.Remove ();
		if (pMsg != NULL)
		{
			if (pMsg->uType == GRAPH_MESSAGE_TYPE_CALLBACK)
			{
				if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_FINISHED)
					VOLOGI ("VOME_CID_PLAY_FINISHED!");

				if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_BUFFERING || pMsg->sInfo.callback.uType == VOME_CID_PLAY_BUFFERSTART || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_BUFFERSTOP || pMsg->sInfo.callback.uType == VOME_CID_PLAY_DOWNLOADPOS)
				{
					if (m_pCallBack != NULL)
					{
						OMX_U32 nBuffer = (OMX_U32)pMsg->sInfo.callback.pData1;
						m_pCallBack (m_pUserData, pMsg->sInfo.callback.uType, &nBuffer, 0);
					}
				}
				else if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_CODECPASSABLEERR || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_MEDIACONTENTTYPE)
				{
					if (m_pCallBack)
						m_pCallBack (m_pUserData, pMsg->sInfo.callback.uType, pMsg->sInfo.callback.pData1, 0);
				}
				else	//default no additional parameter
				{
					if (m_pCallBack)
						m_pCallBack (m_pUserData,pMsg->sInfo.callback.uType, 0, 0);
				}

			}
			else if (pMsg->uType == GRAPH_MESSAGE_TYPE_RECONFIGPORT)
			{
				if(pMsg->sInfo.reconfig_port.pBox)
					pMsg->sInfo.reconfig_port.pBox->ReconfigPort(pMsg->sInfo.reconfig_port.nPort);
			}
			
			voOMXMemFree (pMsg);
		}
	}

	m_hMsgThread = NULL;

	return 0;
}


OMX_ERRORTYPE voCOMXFLOGraph::CreateFLOEngine()
{
	if(m_libFLOEngine != NULL)
		return OMX_ErrorNone;

#ifdef _WIN32
	m_libFLOEngine = (HMODULE)m_libOP.LoadLib ( m_libOP.pUserData, "voFLOEngine.Dll", 0);
#else 
	m_libFLOEngine = (OMX_PTR)m_libOP.LoadLib ( m_libOP.pUserData, "libvoFLOEngine.so", RTLD_NOW);
#endif // _WIN32

	if(m_libFLOEngine == NULL)
	{
		VOLOGE ("lib voFLOEngine Load failure");
		return OMX_ErrorUndefined;
	}

	PVOGETFLOENGINEAPI pAPI = (PVOGETFLOENGINEAPI) m_libOP.GetAddress(m_libOP.pUserData, m_libFLOEngine, ("voGetFLOEngineAPI"), 0);
	if (pAPI == NULL)
		return OMX_ErrorUndefined;

	pAPI (&m_FLOEngineAPI);

	m_FLOEngineAPI.Create(&m_hFLOEngine);

	VOLOGI ("Create voFLOEngine Success");

	if (m_pWorkPath != NULL)
	{
		char szPathLib[256];
		strcpy (szPathLib, m_pWorkPath);
		strcat (szPathLib, "lib");
		m_FLOEngineAPI.SetParam (m_hFLOEngine, VO_PID_FLOENGINE_LIBPATH, szPathLib);
		VOLOGI ("Lib path is %s", szPathLib);

		m_FLOEngineAPI.SetParam (m_hFLOEngine, VO_PID_FLOENGINE_DATAPATH, m_pWorkPath);
	}
	if (m_pConfig != NULL)
	{
		int nDrmEnable = m_pConfig->GetItemValue ("vomeMF", "DrmEnable", 0);
		int nDumpLevel = m_pConfig->GetItemValue ("vomeMF", "DumpLevel", 0);
		m_FLOEngineAPI.SetParam (m_hFLOEngine, VO_PID_FLOENGINE_DXDRM, (VO_PTR)nDrmEnable);
		m_FLOEngineAPI.SetParam (m_hFLOEngine, VO_PID_FLOENGINE_LOGLEVEL, (VO_PTR)nDumpLevel);

		VOLOGI ("Dra Enable %d. DumpLevel %d", nDrmEnable, nDumpLevel);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFLOGraph::DestroyFLOEngine()
{
	if(m_hFLOEngine != NULL)
	{
		m_FLOEngineAPI.Destroy(m_hFLOEngine);
		m_hFLOEngine = NULL;
	}

	if(m_libFLOEngine != NULL)
	{
		m_libOP.FreeLib(m_libOP.pUserData, m_libFLOEngine, 0);
		m_libFLOEngine = NULL;
	}

	return OMX_ErrorNone;
}

VO_S32 voCOMXFLOGraph::OnEvent(int nEventType, unsigned int nParam, void * pData)
{
	if(g_pFLOGraph == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	if(g_pFLOGraph->m_pCallBack == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;


	VOLOGI("com.flotv.events:%d", nEventType);
	VOLOGI("com.flotv.events.reason:%d", nParam);

	g_pFLOGraph->m_pCallBack(g_pFLOGraph->m_pUserData, nEventType, &nParam, NULL);

	return 0;
}

VO_S32 voCOMXFLOGraph::OnFrame(VO_FLOENGINE_FRAME * pFrame)
{
	if(g_pFLOGraph == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	OMX_BUFFERHEADERTYPE bufHead;
	memset (&bufHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
	bufHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
	bufHead.nAllocLen = pFrame->nSize;

	if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_AUDIO)
		bufHead.nOutputPortIndex = 0; // 0 Audio  1 Video
	else if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_VIDEO)
		bufHead.nOutputPortIndex = 1; // 0 Audio  1 Video

	bufHead.nTickCount = 1;

	bufHead.nFilledLen = pFrame->nSize;
	bufHead.pBuffer = pFrame->pData;
	bufHead.nTimeStamp = pFrame->nStartTime;

	OMX_ERRORTYPE errType = OMX_ErrorUndefined;
	while(errType != OMX_ErrorNone)
	{
		errType = g_pFLOGraph->m_pCompDS->SetParameter(g_pFLOGraph->m_pCompDS, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);
		if(errType != OMX_ErrorNone)
			voOMXOS_Sleep(2);

/*
		if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_AUDIO)
		{
			VOLOGI("Audio: SysTime = %u    StartTime = %llu    Size = %d    errType = %u \n", voOMXOS_GetSysTime(), pFrame->nStartTime, pFrame->nSize, errType);
		}
		else if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_VIDEO)
		{
			VOLOGI("Video: SysTime = %u    StartTime = %llu    Size = %d    errType = %u \n", voOMXOS_GetSysTime(), pFrame->nStartTime, pFrame->nSize, errType);
		}
*/
	}

	return VO_ERR_FLOENGINE_OK;
}
