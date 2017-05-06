#include "COSDASHSource.h"
#include "voASControllerAPI.h"

#define  LOG_TAG    "COSDASHSource"
#include "voLog.h"


COSDASHSource::COSDASHSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: COSBaseSource(pLibop)	
{	
	LoadDll();
}

COSDASHSource::~COSDASHSource ()
{
	unLoadDll();
}

int	COSDASHSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	return nRC;
}

int	COSDASHSource::Uninit()
{
	int nRC = COSBaseSource::Uninit();
	return nRC;
}


int COSDASHSource::GetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSDASHSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSDASHSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
	if(nRC) return nRC;

	nRC = callBackEvent ();
	return nRC;
}

int COSDASHSource::LoadDll()
{
	if(m_pLibop == NULL)
	{
		VOLOGI("Load library Operator is NULL");
		return -1;
	}
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voLiveSrcDASH", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voLiveSrcDASH failed");
		return -1;
	}

	VOGETSRCFRAPI pGetAPI = (VOGETSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetASCDASHAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API voGetASCDASHAPI failed");
		return -1;
	}

	pGetAPI(&m_SourceAPI);

	if(m_SourceAPI.Init == NULL)
	{
		VOLOGI("m_SourceAPI.Init is null");
		return -1;
	}

	return 0;
}
