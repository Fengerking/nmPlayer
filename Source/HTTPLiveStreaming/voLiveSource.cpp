#include "voLog.h"
#include "voLiveSource.h"
#include "vompType.h"

#ifdef _CMMB_FILESIMULATE
#include "CLiveSrcCMMBFile.h"
#define SOURCE_CLASS CLiveSrcCMMBFile
#endif //_FILESIMULATE

#ifdef _CMMB_SOURCE_
#include "CLiveSrcCMMB.h"
#define SOURCE_CLASS CLiveSrcCMMB
#endif

#ifdef _HLS_SOURCE_
#include "CLiveSrcHLS.h"
#define SOURCE_CLASS CLiveSrcHLS

#endif

#ifdef _LIVE_TS_SOURCE_
#include "CLiveSrcTS.h"
#define SOURCE_CLASS CLiveSrcTS
#endif

#ifdef _ISS_SOURCE_
#include "CLiveSrcISS.h"
#define SOURCE_CLASS CLiveSrcISS
#endif

 #ifdef _DASH_SOURCE_
 #include "CLiveSrcDASH.h"
 #define SOURCE_CLASS CLiveSrcDASH
 #endif


#ifdef _VONAMESPACE
 using namespace _VONAMESPACE;
#endif


#define SOUORCE_OBJ ((SOURCE_CLASS*)hLiveSrc)

 #ifdef _IOS
 	#ifdef _ISS_SOURCE_
 	#define MODULENAME ISS
 	#elif defined _HLS_SOURCE_
 	#define MODULENAME HLS
 	#endif
 #else
#define MODULENAME 
#endif

#define MODULEVOSRCOPEN(name) vosrcOpen##name
#define vosrcOpen(name) MODULEVOSRCOPEN(name)

#define MODULEVOSRCCLOSE(name) vosrcClose##name
#define vosrcClose(name) MODULEVOSRCCLOSE(name)

#define MODULEVOSRCSCAN(name) vosrcScan##name
#define vosrcScan(name) MODULEVOSRCSCAN(name)

#define MODULEVOSRCCANCEL(name) vosrcCancel##name
#define vosrcCancel(name) MODULEVOSRCCANCEL(name)

#define MODULEVOSRCSTART(name) vosrcStart##name
#define vosrcStart(name) MODULEVOSRCSTART(name)

#define MODULEVOSRCPAUSE(name) vosrcPause##name
#define vosrcPause(name) MODULEVOSRCPAUSE(name)

#define MODULEVOSRCSTOP(name) vosrcStop##name
#define vosrcStop(name) MODULEVOSRCSTOP(name)

#define MODULEVOSRCSENDBUFFER(name) vosrcSendBuffer##name
#define vosrcSendBuffer(name) MODULEVOSRCSENDBUFFER(name)

#define MODULEVOSRCSETDATASOURCE(name) vosrcSetDataSource##name
#define vosrcSetDataSource(name) MODULEVOSRCSETDATASOURCE(name)

#define MODULEVOSRCSEEK(name) vosrcSeek##name
#define vosrcSeek(name) MODULEVOSRCSEEK(name)

#define MODULEVOSRCGETCHANNEL(name) vosrcGetChannel##name
#define vosrcGetChannel(name) MODULEVOSRCGETCHANNEL(name)

#define MODULEVOSRCSETCHANNEL(name) vosrcSetChannel##name
#define vosrcSetChannel(name) MODULEVOSRCSETCHANNEL(name)

#define MODULEVOSRCGETESG(name) vosrcGetESG##name
#define vosrcGetESG(name) MODULEVOSRCGETESG(name)

#define MODULEVOSRCGETPARAM(name) vosrcGetParam##name
#define vosrcGetParam(name) MODULEVOSRCGETPARAM(name)

#define MODULEVOSRCSETPARAM(name) vosrcSetParam##name
#define vosrcSetParam(name) MODULEVOSRCSETPARAM(name)

#if defined __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	return TRUE;
}
#endif	//_WIN32


VO_U32 vosrcOpen(MODULENAME) (VO_HANDLE * phLiveSrc, VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	SOURCE_CLASS * pSrc = new SOURCE_CLASS();
	if (pSrc == NULL)
		return VO_ERR_LIVESRC_OUTOFMEMORY;

	pSrc->Open(pUserData, fStatus, fSendData);

	*phLiveSrc = (VO_HANDLE)pSrc;

	return VO_ERR_LIVESRC_OK;
}

VO_U32 vosrcClose(MODULENAME) (VO_HANDLE hLiveSrc)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	VO_U32 ret = pSrc->Close();
	delete pSrc;

	return ret;
}

VO_U32 vosrcScan(MODULENAME) (VO_HANDLE hLiveSrc, VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->Scan (pParam);
}

VO_U32 vosrcCancel(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 nID)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->Cancel (nID);
}


VO_U32 vosrcStart(MODULENAME) (VO_HANDLE hLiveSrc)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->Start ();
}


VO_U32 vosrcPause(MODULENAME) (VO_HANDLE hLiveSrc)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->Pause ();
}


VO_U32 vosrcStop(MODULENAME) (VO_HANDLE hLiveSrc)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->Stop ();
}

VO_U32 vosrcSendBuffer(MODULENAME) (VO_HANDLE hLiveSrc, VOMP_BUFFERTYPE * pBuffer)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;
	
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->SendBuffer((VOMP_BUFFERTYPE*)pBuffer);
}
	
/**
* Set data source type
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pSource [in] The source. Normally it should be URL for HTTP Live Streaming.
* \param nFlag [in]. It can indicate the source type..
*/
VO_U32 vosrcSetDataSource(MODULENAME)(VO_HANDLE hLiveSrc, VO_PTR pSource, VO_U32 nFlag)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->SetDataSource (pSource, nFlag);
}


/**
* Data source seek, Normally it should be used by HTTP Live Streaming
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pSource [in] The source.
* \param nTimeStamp [in]. Seek to this timestamp.
*/
VO_U32 vosrcSeek(MODULENAME)(VO_HANDLE hLiveSrc, VO_U64 nTimeStamp)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->Seek (nTimeStamp);
}

VO_U32 vosrcGetChannel(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->GetChannel (pCount, ppChannelInfo);
}

VO_U32 vosrcSetChannel(MODULENAME) (VO_HANDLE hLiveSrc, VO_S32 nChannelID)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->SetChannel (nChannelID);
}

VO_U32 vosrcGetESG(MODULENAME) (VO_HANDLE hLiveSrc, VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->GetESG (nChannelID, ppESGInfo);
}

VO_U32 vosrcGetParam(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pValue)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	return pSrc->GetParam(nParamID, pValue);
}

VO_U32 vosrcSetParam(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pValue)
{

	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	return pSrc->SetParam(nParamID, pValue);
}

#ifdef _HLS_SOURCE_
VO_S32 VO_API voGetHLSLiveSrcAPI(VO_LIVESRC_API* pHandle)
#elif defined  _ISS_SOURCE_
VO_S32 VO_API voGetISSLiveSrcAPI(VO_LIVESRC_API* pHandle)   
#elif defined _LIVE_TS_SOURCE_
VO_S32 VO_API voGetTSLiveSrcAPI(VO_LIVESRC_API* pHandle)
#elif defined _CMMB_SOURCE_	
VO_S32 VO_API voGetCMMBLiveSrcAPI(VO_LIVESRC_API* pHandle)
 #elif defined _DASH_SOURCE_
 VO_S32 VO_API voGetDASHLiveSrcAPI(VO_LIVESRC_API* pHandle)
#else 
VO_S32 VO_API voGetLiveSrcAPI(VO_LIVESRC_API* pHandle)
#endif // _HLS_SOURCE_

{
	pHandle->Open		= vosrcOpen(MODULENAME) ;
	pHandle->Close		= vosrcClose(MODULENAME) ;
	pHandle->Scan		= vosrcScan(MODULENAME) ;
	pHandle->Cancel		= vosrcCancel(MODULENAME) ;
	pHandle->Start		= vosrcStart(MODULENAME) ;
	pHandle->Pause		= vosrcPause(MODULENAME) ;
	pHandle->Stop		= vosrcStop(MODULENAME) ;
	pHandle->SendBuffer	= vosrcSendBuffer(MODULENAME) ;
	pHandle->SetDataSource	= vosrcSetDataSource(MODULENAME) ;
	pHandle->Seek		= vosrcSeek(MODULENAME) ;
	pHandle->GetChannel	= vosrcGetChannel(MODULENAME) ;
	pHandle->SetChannel	= vosrcSetChannel(MODULENAME) ;
	pHandle->GetESG		= vosrcGetESG(MODULENAME) ;
	pHandle->GetParam	= vosrcGetParam(MODULENAME) ;
	pHandle->SetParam	= vosrcSetParam(MODULENAME) ;

	return VO_ERR_LIVESRC_OK;
} 

#if defined __cplusplus
}
#endif
