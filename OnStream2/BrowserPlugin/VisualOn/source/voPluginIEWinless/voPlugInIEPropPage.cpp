// voPlugInIEPropPage.cpp : Implementation of the CvoPlugInIEPropPage property page class.

#include "stdafx.h"
#include "voPlugInIE.h"
#include "voPlugInIEPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CvoPlugInIEPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CvoPlugInIEPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CvoPlugInIEPropPage, "VOPLUGINIE.voPlugInIEPropPage.1",
	0xd984a681, 0xe403, 0x4e2e, 0x86, 0x8e, 0xd, 0x66, 0x26, 0xa0, 0x20, 0xc5)



// CvoPlugInIEPropPage::CvoPlugInIEPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CvoPlugInIEPropPage

BOOL CvoPlugInIEPropPage::CvoPlugInIEPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VOPLUGINIE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CvoPlugInIEPropPage::CvoPlugInIEPropPage - Constructor

CvoPlugInIEPropPage::CvoPlugInIEPropPage() :
	COlePropertyPage(IDD, IDS_VOPLUGINIE_PPG_CAPTION)
{
}



// CvoPlugInIEPropPage::DoDataExchange - Moves data between page and properties

void CvoPlugInIEPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CvoPlugInIEPropPage message handlers
