#include "filebase.h"
#include "network.h"
#include "macro.h"
#if !CP_SOCKET
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif//CP_SOCKET
#include "RTSPSource.h"
#include "RTSPTrack.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

IVOCPFactoryMethod* GetOneWindowsFactoryMethod();
int GetDefaultInitParam(VOStreamInitParam*);
//#include "voRTSPSdk.h"
//#include "voCrossPlatformWrapper.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	// return value:
	// 0	is ok.
	// <0	is Failed.  -1 Can't find the voVidDec.dll file
	//					-2 Can't get function from voVidDec.dll file
	//					-3 Can't create the buffer
	//					-4 Can't init in voVidDec.Dll.
	//					-5 The voVidDec.Dll is not available
	//					-6 Check license file failed.
	// >0	is limit time. (seconds)
	int	__cdecl		CheckLicenseFile (void * hInst);

#ifdef __cplusplus
}
#endif /* __cplusplus */
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionCreate(HVOFILEREAD * ppRTSPSession)
{
	CRTSPSource * pRTSPSource;
#ifdef LICENSE_CHECK
	int ret = CheckLicenseFile(NULL);
	if(ret<0)
		return VORC_FILEREAD_OPEN_FAILED;
#endif
	pRTSPSource = new CRTSPSource();TRACE_NEW("rtss_pRTSPSource",pRTSPSource);
	if(pRTSPSource == NULL)
		return VORC_FILEREAD_NULL_POINTER;
	*ppRTSPSession = pRTSPSource;
	return VORC_FILEREAD_OK;
}
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpenURL(HVOFILEREAD pRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp)
{
	if(pRTSPSession==NULL)
		return VORC_FILEREAD_NULL_POINTER;
	CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	IVOCPFactoryMethod* voFactoryMethod=GetOneWindowsFactoryMethod();
	voRTSPSessionSetParameter(pRTSPSource,VOID_STREAMING_PlatformFactoryMethod,(int)(voFactoryMethod));
	VOStreamInitParam m_initParam;
	m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
	m_initParam.nMinUdpPort		= DEFAULT_PORT_MIN;
	m_initParam.nRTPTimeOut		= DEFAULT_RTPTIMEOUT;
	m_initParam.nUseTCP			= 0;
	m_initParam.nBufferTime		= DEFAULT_BUFFERTIME;
	m_initParam.nSDKlogflag		= 0;
	GetDefaultInitParam(&m_initParam);
	m_initParam.bUseRTSPProxy	= 0;
	voRTSPSessionSetParameter(pRTSPSource,VOID_STREAMING_INIT_PARAM,(int)&m_initParam);
	if(pRTSPSource->Open(pRTSPLink))
	{
		return VORC_FILEREAD_OK;
	}
	else
	{
		return VORC_FILEREAD_OPEN_FAILED;
	}
}
//class IVOCPFactoryMethod;

VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpen(HVOFILEREAD * ppRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp)
{
#ifdef LICENSE_CHECK1
	int ret = CheckLicenseFile(NULL);
	if(ret<0)
	{
		//ShowWinInfo("voRTSPss:License Check failed\n");
		return VORC_FILEREAD_OPEN_FAILED;
	}
#endif
	CRTSPSource * pRTSPSource = new CRTSPSource();TRACE_NEW("rtss_pRTSPSource2",pRTSPSource);
	if(pRTSPSource == NULL)
		return VORC_FILEREAD_NULL_POINTER;
	
	IVOCPFactoryMethod* voFactoryMethod=GetOneWindowsFactoryMethod();
	voRTSPSessionSetParameter(pRTSPSource,VOID_STREAMING_PlatformFactoryMethod,(int)(voFactoryMethod));
	VOStreamInitParam m_initParam;
	m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
	m_initParam.nMinUdpPort		= DEFAULT_PORT_MIN;
	m_initParam.nRTPTimeOut		= DEFAULT_RTPTIMEOUT;
	m_initParam.nUseTCP			= 0;
	m_initParam.nBufferTime		= DEFAULT_BUFFERTIME;
	m_initParam.nSDKlogflag		= 0;
	GetDefaultInitParam(&m_initParam);
	m_initParam.bUseRTSPProxy	= 0;
	voRTSPSessionSetParameter(pRTSPSource,VOID_STREAMING_INIT_PARAM,(int)&m_initParam);
	if(pRTSPSource->Open(pRTSPLink))
	{
		*ppRTSPSession = pRTSPSource;
		return VORC_FILEREAD_OK;
	}
	else
	{
		SAFE_DELETE(pRTSPSource);
		return VORC_FILEREAD_OPEN_FAILED;
	}
}
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionClose(HVOFILEREAD pRTSPSession)
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	SAFE_DELETE(pRTSPSource);
	return VORC_FILEREAD_OK;
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionSetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG lValue) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	return pRTSPSource->SetParameter(lID, lValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionGetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG * plValue) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	return pRTSPSource->GetParameter(lID, plValue);
}



VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackOpen(HVOFILETRACK * ppRTSPTrack, HVOFILEREAD pRTSPSession, int nIndex) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	return pRTSPSource->OpenRTSPTrack(nIndex, (CRTSPTrack **)ppRTSPTrack);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackClose(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	return pRTSPSource->CloseRTSPTrack((CRTSPTrack *)pRTSPTrack);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackSetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG lValue) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->SetParameter(lID, lValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG * plValue) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetParameter(lID, plValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackInfo(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOTRACKINFO * pTrackInfo) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL || pTrackInfo == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetInfo(pTrackInfo);
}



VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetSampleByIndex(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOSAMPLEINFO * pSampleInfo) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL || pSampleInfo == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CRTSPSource * pRTSPSource = (CRTSPSource *)pRTSPSession;
	return pRTSPSource->GetRTSPTrackSampleByIndex((CRTSPTrack *)pRTSPTrack, pSampleInfo);

//	return ((CRTSPTrack *)pRTSPTrack)->GetSampleByIndex(pSampleInfo);
}

/*
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetSampleByTime(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOSAMPLEINFO * pSampleInfo) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL || pSampleInfo == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetSampleByTime(pSampleInfo);
}

bool VOFILEAPI voRTSPTrackIsKeyFrame(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, int nIndex)
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->IsKeyFrame(nIndex);
}
*/

int VOFILEAPI voRTSPTrackGetNextKeyFrame(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, int nIndex, int nDirectionFlag)
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetNextKeyFrame(nIndex, nDirectionFlag);
}

