#include "COSPDMSSource.h"
#include "voHLS2.h"
#include "voStreaming.h"

#define  LOG_TAG    "COSPDMSSource"
#include "voLog.h"

COSPDMSSource::COSPDMSSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: COSBaseSource(pLibop)	
{	
	memset(m_szFileDll, 0, 256);
	memset(m_szFileAPI, 0, 256);
}

COSPDMSSource::~COSPDMSSource ()
{
	unLoadDll();
}

int	COSPDMSSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = CheckLinkType(pSource, nFlag);
	if(nRC) return nRC;

	nRC = LoadDll();
	if(nRC) return nRC;
	
	nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	return nRC;
}

int	COSPDMSSource::Uninit()
{
	int nRC = COSBaseSource::Uninit();
	return nRC;
}

int COSPDMSSource::GetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSPDMSSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSPDMSSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	if (VO_EVENT_BUFFERING_BEGIN == nID)
	{
		VOLOGI("@@@@### PD Start Bufferring....");
		m_bBuffering = true;
	}
	else if (VO_EVENT_BUFFERING_END == nID)
	{
		VOLOGI("@@@### PD Stop Buffering....");
		m_bBuffering = false;
	}

	int nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
	if(nRC) return nRC;

	nRC = callBackEvent ();
	return nRC;
}

int COSPDMSSource::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, m_szFileDll, 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library %s failed", m_szFileDll);
		return -1;
	}

	VOGETSRCFRAPI pGetAPI = (VOGETSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, m_szFileAPI, 0);
    
	if (pGetAPI == NULL)
	{
		VOLOGI("get API %s failed", m_szFileAPI);
		return -1;
	}

	pGetAPI(&m_SourceAPI);

	if(m_SourceAPI.Init == NULL)
	{
		VOLOGE ("The Init function could not be found!");
		return -1;
	}

	return 0;
}

int COSPDMSSource::CheckLinkType(void * pSource, int nFlag)
{
	if(m_pLibop == NULL)
		return -1;

	void * hDll = NULL;
    
	hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voPreHTTP", 0);
	if (hDll == NULL)
	{
		strcpy (m_szFileDll, "voSrcPD");
		strcpy (m_szFileAPI, "voGetPD2ReadAPI");
		VOLOGW("No preHTTP lib to check the link type, take PD as default");
		return 0;
	}

	pGetPreHTTPAPI pGetAPI = (pGetPreHTTPAPI) m_pLibop->GetAddress (m_pLibop->pUserData, hDll, "voGetPreHTTPAPI", 0);
    
	if (pGetAPI == NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, hDll, 0);
		strcpy (m_szFileDll, "voSrcPD");
		strcpy (m_szFileAPI, "voGetPD2ReadAPI");
		VOLOGW("No preHTTP API to check the link type, take PD as default");
		return 0;
	}

	VO_PREHTTP_API apiPreHttp = {0};
	void * handlePreHttp = NULL;

	pGetAPI(&apiPreHttp, 0);

	if(apiPreHttp.voPreHTTPInit == NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, hDll, 0);
		strcpy (m_szFileDll, "voSrcPD");
		strcpy (m_szFileAPI, "voGetPD2ReadAPI");
		VOLOGW("No preHTTP API to check the link type, take PD as default");
		return 0;
	}

	if ( 0 != apiPreHttp.voPreHTTPInit(&handlePreHttp) ) 
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, hDll, 0);
		strcpy (m_szFileDll, "voSrcPD");
		strcpy (m_szFileAPI, "voGetPD2ReadAPI");
		VOLOGW("No preHTTP API Init Err to check the link type, take PD as default");
		return 0;
	}

	int nTryTime = 0;

	VO_LIVESRC_FORMATTYPE nType = VO_LIVESRC_FT_MAX;

	do {
		nType = VO_LIVESRC_FT_MAX;

		if ( 0 != apiPreHttp.voPreHTTPSetSrcURL(handlePreHttp, (char *)pSource) ) 
		{
			apiPreHttp.voPreHTTPUninit(handlePreHttp);
			m_pLibop->FreeLib(m_pLibop->pUserData, hDll, 0);
			strcpy (m_szFileDll, "voSrcPD");
			strcpy (m_szFileAPI, "voGetPD2ReadAPI");
			return 0;
		}

		char *pURL = NULL;
		int iRet = 0;
		int mTryTimes = 0;

		do {
			iRet = apiPreHttp.voPreHTTPGetDesInfo(handlePreHttp, &nType, &pURL);

			mTryTimes++;
			if(mTryTimes > 1000)
				break;			
		} while ((unsigned int)iRet == 0xfffffffe);

		nTryTime++;
		if(nTryTime > 1000)
			break;

	} while (nType == VO_LIVESRC_FT_FILE);

	apiPreHttp.voPreHTTPUninit(handlePreHttp);

	m_pLibop->FreeLib(m_pLibop->pUserData, hDll, 0); 

	if(nType == VO_LIVESRC_FT_PD)
	{
		strcpy (m_szFileDll, "voSrcPD");
		strcpy (m_szFileAPI, "voGetPD2ReadAPI");
	}
	else if(nType == VO_LIVESRC_FT_WMS)
	{
		strcpy (m_szFileDll, "voSrcWMSP");
		strcpy (m_szFileAPI, "voGetWMSP2ReadAPI");
	}
	else
	{
		strcpy (m_szFileDll, "voSrcPD");
		strcpy (m_szFileAPI, "voGetPD2ReadAPI");
	}

	return 0;
}