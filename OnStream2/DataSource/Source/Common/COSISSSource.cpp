#include "COSISSSource.h"
#include "voASControllerAPI.h"

#define  LOG_TAG    "COSISSSource"
#include "voLog.h"

COSISSSource::COSISSSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: COSBaseSource(pLibop)	
{	
	LoadDll();
}

COSISSSource::~COSISSSource ()
{
	unLoadDll();
}

int	COSISSSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	return nRC;
}

int	COSISSSource::Uninit()
{
	int nRC = COSBaseSource::Uninit();
	return nRC;
}


int COSISSSource::GetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSISSSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSISSSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
	if(nRC) return nRC;

	nRC = callBackEvent ();
	return nRC;
}

int COSISSSource::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voLiveSrcISS", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voLiveSrcISS failed");
		return -1;
	}

	VOGETSRCFRAPI pGetAPI = (VOGETSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetASCISSAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API voGetASCISSAPI failed");
		return -1;
	}
	pGetAPI(&m_SourceAPI);

	if(m_SourceAPI.Init == NULL)
		return -1;

	return 0;
}

