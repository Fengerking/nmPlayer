#pragma once

// voPlugInIEPropPage.h : Declaration of the CvoPlugInIEPropPage property page class.


// CvoPlugInIEPropPage : See voPlugInIEPropPage.cpp for implementation.

class CvoPlugInIEPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CvoPlugInIEPropPage)
	DECLARE_OLECREATE_EX(CvoPlugInIEPropPage)

// Constructor
public:
	CvoPlugInIEPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VOPLUGINIE };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

