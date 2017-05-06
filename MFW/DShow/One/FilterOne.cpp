	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterOneSource.cpp

	Contains:	CFilterOneSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-08-14		JBF			Create file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>

#include "CFilterOneSource.h"
#include "CFilterOneRender.h"

#include <initguid.h>
#include "FilterOne_Guids.h"

#pragma warning (disable : 4996)

CUnknown * WINAPI CreateInstanceSource (LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CFilterOneSource(lpunk, phr);
    if(punk == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
    return punk;
}

CUnknown * WINAPI CreateInstanceRender (LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CFilterOneRender(lpunk, phr);
    if(punk == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
    return punk;
}

// Setup data
const AMOVIESETUP_MEDIATYPE sudPinTypes[] =
{
	{&MEDIATYPE_Stream, &MEDIASUBTYPE_VOONE},
};

const AMOVIESETUP_PIN sudPins[] =
{
	{L"Stream",                   // Pin string name
	FALSE,                      // Is it rendered
	TRUE,                      // Is it an output
	FALSE,                      // Allowed none
	FALSE,                      // Likewise many
	&CLSID_NULL,                // Connects to filter
	L"Output",                  // Connects to pin
	1,                          // Number of types
	&sudPinTypes[0]                // Pin information
	},

	{L"Stream",                   // Pin string name
	TRUE,                      // Is it rendered
	FALSE,                      // Is it an output
	FALSE,                      // Allowed none
	FALSE,                      // Likewise many
	&CLSID_NULL,                // Connects to filter
	L"Input",                  // Connects to pin
	1,                          // Number of types
	&sudPinTypes[0]                // Pin information
	}
};

const AMOVIESETUP_FILTER sudFileMux[] =
{
	{&CLSID_FilterOneSource,		// Filter CLSID
	L"voOne Source",				// String name
	MERIT_DO_NOT_USE,			// Filter merit
	1,							// Number pins
	&sudPins[0]					// Pin details
	},

	{&CLSID_FilterOneRender,	// Filter CLSID
	L"voOne Render",				// String name
	MERIT_DO_NOT_USE,			// Filter merit
	1,							// Number pins
	&sudPins[1]					// Pin details
	}
};

//  Object creation stuff
CFactoryTemplate g_Templates[] = {
	{L"VisualOn Filter One Source", &CLSID_FilterOneSource, CreateInstanceSource, NULL, &sudFileMux[0]},
	{L"VisualOn Filter One Render", &CLSID_FilterOneRender, CreateInstanceRender, NULL, &sudFileMux[1]}
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);



