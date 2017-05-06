	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAndroidUtilities.cpp

	Contains:	vo base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-05-23		JBF			Create file

*******************************************************************************/
#include "CAndroidUtilities.h"

#ifdef _LINUX_ANDROID
#include <dlfcn.h>
#ifndef __VO_NDK__
#include <cutils/properties.h>
#else
#include <sys/system_properties.h>
#endif  //__VO_NDK__
#endif // _LINUX_ANDROID

#define LOG_TAG "CAndroidUtilities"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAndroidUtilities::CAndroidUtilities(void)
	: m_hLib (NULL)
	, m_fPropGet (NULL)
	, m_fPropSet (NULL)
{
#ifdef _LINUX_ANDROID
#ifdef __VO_NDK__
	m_hLib = dlopen ("libcutils.so", RTLD_NOW);
	if (m_hLib != NULL)
	{
		m_fPropGet = (VO_PROPERTY_GET) dlsym (m_hLib, "property_get");
		m_fPropSet = (VO_PROPERTY_SET) dlsym (m_hLib, "property_set");
	}
	
	VOLOGI ("The handle of lib is %08X", (int)m_hLib);
#endif // __VO_NDK__
#endif // _LINUX_ANDROID
}

CAndroidUtilities::~CAndroidUtilities(void)
{
#ifdef _LINUX_ANDROID
	if (m_hLib != NULL)
		dlclose (m_hLib);
#endif // _LINUX_ANDROID
}

int CAndroidUtilities::GetProperty (const char * pKey, char * pValue, const char * pDefault)
{
	int nRC = -1;
#ifdef _LINUX_ANDROID
#ifdef __VO_NDK__
//	if (m_fPropGet != NULL)
//		nRC = m_fPropGet (pKey, pValue, pDefault);
	nRC = __system_property_get (pKey, pValue);
#else
	nRC = property_get (pKey, pValue, pDefault);
#endif // __VO_NDK__
#endif // _LINUX_ANDROID

	return nRC;
}

int CAndroidUtilities::SetProperty (const char * pKey, const char * pValue)
{
	int nRC = -1;
#ifdef _LINUX_ANDROID
#ifdef __VO_NDK__
	if (m_fPropSet != NULL)
		nRC = m_fPropSet (pKey, pValue);
#else
	nRC = property_set (pKey, pValue);
#endif // __VO_NDK__
#endif // _LINUX_ANDROID

	return nRC;
}
