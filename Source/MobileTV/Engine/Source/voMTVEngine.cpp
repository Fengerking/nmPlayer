
#include "voLog.h"

#ifdef _INNOFIDEI
#include "./Innofidei/CEngineCMMBino.h"
#define ENGINE_CLASS CEngineCMMBino
#endif //_INNOFIDEI

#ifdef _TELEPATH
#include "./Telepath/CEngineCMMBtp.h"
#define ENGINE_CLASS CEngineCMMBtp
#endif //_TELEPATH

#ifdef _SIANO
#include "./Siano/CEngineCMMBsms.h"
#define ENGINE_CLASS CEngineCMMBsms
#endif //_SIANO

#ifdef _LENOVO
#include "./Lenovo/CEngineCMMBlnv.h"
#define ENGINE_CLASS CEngineCMMBlnv
#endif //_LENOVO

#ifdef _MAXSCEND
#include "./Maxscend/CEngineCMMBmax.h"
#define ENGINE_CLASS CEngineCMMBmax
#endif //_MAXSCEND

#ifdef _FILESIMULATE
#include "CEngineCMMBFile.h"
#define ENGINE_CLASS CEngineCMMBFile
#endif //_FILESIMULATE

#ifdef _DVBH_
#include "CEngineDVBH.h"
#define ENGINE_CLASS CEngineDVBH
#endif // _DVBH_

#define ENGINE_OBJ ((ENGINE_CLASS*)hMTEng)

//#pragma warning (disable : 4251)

#ifdef _CA_BIGCARD
extern TCHAR g_sCardName[];
#endif

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


VO_U32 voengOpen (VO_HANDLE * phMTEng, VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	ENGINE_CLASS * pEngine = new ENGINE_CLASS();
	if (pEngine == NULL)
		return VO_ERR_LIVESRC_OUTOFMEMORY;

	VO_U32 ret = pEngine->Open(pUserData, fStatus, fSendData);
// 	ret = pEngine->OpenDevice();
// 	if (ret != VO_ERR_LIVESRC_OK)
// 		return ret;

	*phMTEng = (VO_HANDLE)pEngine;

	return pEngine->GetChannelNum ();
}

VO_U32 voengClose (VO_HANDLE hMTEng)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;

	VO_U32 ret = pEngine->CloseDevice();
	delete pEngine;

	return ret;
}

VO_U32 voengScan (VO_HANDLE hMTEng, VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->Scan (pParam);
}

VO_U32 voengCancel (VO_HANDLE hMTEng, VO_U32 nID)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->Cancel (nID);
}



VO_U32 voengStart (VO_HANDLE hMTEng)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->Start ();
}

VO_U32 voengStop (VO_HANDLE hMTEng)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->Stop ();
}

/**
* Set data source type
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pSource [in] The source. Normally it should be URL for HTTP Live Streaming.
* \param nFlag [in]. It can indicate the source type..
*/
VO_U32 voengSetDataSource(VO_HANDLE hMTEng, VO_PTR pSource, VO_U32 nFlag)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->SetDataSource (pSource, nFlag);
}


/**
* Data source seek, Normally it should be used by HTTP Live Streaming
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pSource [in] The source.
* \param nTimeStamp [in]. Seek to this timestamp.
*/
VO_U32 vomtpSeek(VO_HANDLE hMTEng, VO_U64 nTimeStamp)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->Seek (nTimeStamp);
}

VO_U32 voengGetChannel (VO_HANDLE hMTEng, VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->GetChannel ((int *)pCount, ppChannelInfo);
}

VO_U32 voengSetChannel (VO_HANDLE hMTEng, VO_S32 nChannelID)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->SetChannel (nChannelID);
}

VO_U32 voengGetESG (VO_HANDLE hMTEng, VO_S32 nChannelID, VO_LIVESRC_ESG_INFO* pESGInfo)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	return pEngine->GetESG (nChannelID, pESGInfo);
}

VO_U32 voengGetParam (VO_HANDLE hMTEng, VO_U32 nParamID, VO_PTR pValue)
{
	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;

	if (nParamID == VO_PID_LIVESRC_SIGNAL)
	{
		*((VO_S32*)pValue) = pEngine->GetSignalStrength ();
		return VO_ERR_LIVESRC_OK;
	}
	/*else if (nParamID == VO_PID_LIVESRC_PROGRAMGUIDE)
	{
		return VO_ERR_LIVESRC_OK;
	}
	else if (nParamID == VO_PID_LIVESRC_EB)
	{
		return VO_ERR_LIVESRC_OK;
	}
	else if (nParamID == VO_PID_LIVESRC_CAID)
	{
//		return pEngine->GetCAID((TCHAR *)pValue);
	}
	*/
// 	else if (nParamID == VOENG_PID_GET_KDAVERSION)
// 	{
// 		return pEngine->GetKDAVersion((TCHAR *)pValue);
// 	}

	return VO_ERR_LIVESRC_INVALIDARG;
}

VO_U32 voengSetParam (VO_HANDLE hMTEng, VO_U32 nParamID, VO_PTR nValue)
{
/*	if (nParamID == VO_PID_LIVESRC_CACARDNAME)
	{
#ifdef _CA_BIGCARD
		_tcscpy(g_sCardName, (TCHAR *)nValue);
#endif
		return VO_ERR_LIVESRC_OK;
	}
	*/

	if (hMTEng == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	ENGINE_CLASS * pEngine = (ENGINE_CLASS *)hMTEng;
	if (nParamID == VO_PID_LIVESRC_CHDATASAVEPATH)
	{
		return VO_ERR_LIVESRC_INVALIDARG;
	}
/*	else if (nParamID == VO_PID_LIVESRC_CACARDINSERTED)
	{
		return VO_ERR_LIVESRC_INVALIDARG;
	}
	else if (nParamID == VO_PID_LIVESRC_CACARDREMOVED)
	{
		return VO_ERR_LIVESRC_INVALIDARG;
	}
	*/
	//selse if (nParamID == VO_PID_LIVESRC_DATAEXCALLBACK)
	{
//		return pEngine->SetSendDataExCallback((VOSRCSENDDATAEX)nValue);
	}
	/*else if (nParamID == VO_PID_LIVESRC_EBQUERY)
	{
//		return pEngine->EBQuery();
	}*/

	return VO_ERR_LIVESRC_INVALIDARG;
}

VO_S32 VO_API voGetLiveSrcAPI(VO_LIVESRC_API* pHandle)
{
	pHandle->Open		= voengOpen;
	pHandle->Close		= voengClose;
	pHandle->Scan		= voengScan;
	pHandle->Cancel		= voengCancel;
	pHandle->Start		= voengStart;
	pHandle->Stop		= voengStop;
	pHandle->GetChannel	= voengGetChannel;
	pHandle->SetChannel	= voengSetChannel;
	pHandle->GetESG		= voengGetESG;
	pHandle->GetParam	= voengGetParam;
	pHandle->SetParam	= voengSetParam;

	return VO_ERR_LIVESRC_OK;
}

#if defined __cplusplus
}
#endif
