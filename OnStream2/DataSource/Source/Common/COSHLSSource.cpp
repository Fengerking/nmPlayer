#include "COSHLSSource.h"
#include "voHLS2.h"

#define  LOG_TAG    "COSHLSSource"
#include "voLog.h"

COSHLSSource::COSHLSSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: COSBaseSource(pLibop)	
{	
	LoadDll();
}

COSHLSSource::~COSHLSSource ()
{
	unLoadDll();
}

int	COSHLSSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	return nRC;
}

int	COSHLSSource::Uninit()
{
	int nRC = COSBaseSource::Uninit();
	return nRC;
}


int COSHLSSource::GetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSHLSSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSHLSSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
	if(nRC) return nRC;

	nRC = callBackEvent ();
	return nRC;
}

int COSHLSSource::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voLiveSrcHLS", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voLiveSrcHLS failed");
		return -1;
	}

	VOGETSRCFRAPI pGetAPI = (VOGETSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetSrcHLSAPI", 0);
    
	if (pGetAPI == NULL)
	{
		VOLOGI("get API voGetSrcHLSAPI failed");
		return -1;
	}
	pGetAPI(&m_SourceAPI);

	if(m_SourceAPI.Init == NULL)
		return -1;

	return 0;
}
