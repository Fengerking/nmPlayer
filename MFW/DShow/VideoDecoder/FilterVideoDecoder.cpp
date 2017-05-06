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

#include "CFilterVideoDecoder.h"


#include <initguid.h>
#include "FilterVideoDecoder_Guids.h"

#pragma warning (disable : 4996)

CUnknown * WINAPI CreateInstanceVideoDecoder(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CFilterVideoDecoder(TEXT("Video Decoder") , lpunk , CLSID_FilterVideoDecoder );
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
, { L"Output"           // strName
, FALSE               // bRendered
, TRUE                // bOutput
, FALSE               // bZero
, FALSE               // bMany
, &CLSID_NULL         // clsConnectsToFilter
, L"Input"            // strConnectsToPin
, 1                   // nTypes
, &sudPinTypes } };   // lpTypes


const AMOVIESETUP_FILTER sudFileMux[] =
{
	{&CLSID_FilterVideoDecoder,		// Filter CLSID
	L"vo Video Decoder 3",				// String name
	MERIT_DO_NOT_USE,			// Filter merit
	1,							// Number pins
	psudPins					// Pin details
	},
};

//  Object creation stuff
CFactoryTemplate g_Templates[] = {
	{L"VisualOn Filter One Source", &CLSID_FilterVideoDecoder, CreateInstanceVideoDecoder, NULL, &sudFileMux[0]},
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);



