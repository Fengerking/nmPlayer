	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterSourceSource.cpp

	Contains:	CFilterSourceSource class file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>

#include "CFilterSource.h"

#include <initguid.h>
#include "FilterSource_Guids.h"

#pragma warning (disable : 4996)

CUnknown * WINAPI CreateSource (LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CFilterSource(lpunk, phr);
    if(punk == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
    return punk;
}


// Setup data
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
	&MEDIATYPE_Stream,       // Major type
	&MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudPin =
{
	L"Output",              // Pin string name
	FALSE,                  // Is it rendered
	TRUE,                   // Is it an output
	FALSE,                  // Can we have none
	FALSE,                  // Can we have many
	&CLSID_NULL,            // Connects to filter
	NULL,                   // Connects to pin
	1,                      // Number of types
	&sudPinTypes };       // Pin details

	const AMOVIESETUP_FILTER sudFileReader =
	{
		&CLSID_FilterSource,    // Filter CLSID
		 L"voFile Source",       // String name
		MERIT_DO_NOT_USE,       // Filter merit
		1,                      // Number pins
		&sudPin               // Pin details
	};


	// COM global table of objects in this dll

	CFactoryTemplate g_Templates[] = {
		{ L"voFile Source"
		, &CLSID_FilterSource
		,CreateSource
		, NULL
		, &sudFileReader }
	};
	int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


