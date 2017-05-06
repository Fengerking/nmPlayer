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
	0x36357401, 0xa8ad, 0x44b0, 0x87, 0xda, 0x4e, 0x93, 0xc1, 0x12, 0x18, 0xf2)



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
