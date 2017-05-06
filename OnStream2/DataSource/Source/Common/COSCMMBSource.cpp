#include "COSCMMBSource.h"

#define  LOG_TAG    "COSCMMBSource"
#include "voLog.h"

COSCMMBSource::COSCMMBSource(VO_SOURCE2_LIB_FUNC *pLibop)
: COSBaseSource(pLibop)	
{
  strcpy (m_szFileDll, "voCmmbSrc");
  strcpy (m_szFileAPI, "voGetCMMBSrcApi");
  LoadDll();
}

COSCMMBSource::~COSCMMBSource(void)
{
}

int COSCMMBSource::GetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSCMMBSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSCMMBSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
	if(nRC) return nRC;

	nRC = callBackEvent ();
	return nRC;
}

int COSCMMBSource::LoadDll()
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

  VOGETLOCALSRCFRAPI pGetAPI = (VOGETLOCALSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, m_szFileAPI, 0);

  if (pGetAPI == NULL)
  {
    VOLOGI("get API %s failed", m_szFileAPI);
    return -1;
  }

  pGetAPI(&m_SourceAPI, 0);

  if(m_SourceAPI.Init == NULL)
    return -1;

  return 0;
}
