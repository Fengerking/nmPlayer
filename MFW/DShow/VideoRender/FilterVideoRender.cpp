	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		FilterVideoDecoder.cpp

	Contains:	FilterVideoDecoder class file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>

#include "CFilterVideoRender.h"


#include <initguid.h>
#include "FilterVideoRender_Guids.h"

#pragma warning (disable : 4996)

CUnknown * WINAPI CreateInstanceVideoRender(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CFilterVideoRender(TEXT("vo Video Render") , lpunk , CLSID_FilterVideoRender , phr );
    if(punk == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
    return punk;
}



const AMOVIESETUP_MEDIATYPE
sudPinTypes =   { &MEDIATYPE_Video                // clsMajorType
, &MEDIASUBTYPE_NULL }  ;       // clsMinorType

const AMOVIESETUP_PIN psudPins[] = { 
{ L"Input"            // strName
, FALSE               // bRendered
, FALSE               // bOutput
, FALSE               // bZero
, FALSE               // bMany
, &CLSID_NULL         // clsConnectsToFilter
, L"Output"           // strConnectsToPin
, 1                   // nTypes
, &sudPinTypes }      // lpTypes
 };   // lpTypes


const AMOVIESETUP_FILTER sudFileMux[] =
{
	{&CLSID_FilterVideoRender,		// Filter CLSID
	L"vo Video Render 3",				// String name
	MERIT_DO_NOT_USE,			// Filter merit
	1,							// Number pins
	psudPins					// Pin details
	},
};

//  Object creation stuff
CFactoryTemplate g_Templates[] = {
	{L"VisualOn Filter Video Render", &CLSID_FilterVideoRender, CreateInstanceVideoRender, NULL, &sudFileMux[0]},
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);



