	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAndroidUtilities.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-05-23		JBF			Create file

*******************************************************************************/

#ifndef __CAndroidUtilities_H__
#define __CAndroidUtilities_H__

#include "voYYDef_Common.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef int (* VO_PROPERTY_GET) (const char * pKey, char * pValue, const char * pDefault);
typedef int (* VO_PROPERTY_SET) (const char * pKey, const char * pValue);

class CAndroidUtilities
{
public:
	CAndroidUtilities(void);
	virtual ~CAndroidUtilities(void);

	int GetProperty (const char * pKey, char * pValue, const char * pDefault);
	int SetProperty (const char * pKey, const char * pValue);

protected:
	VO_PTR				m_hLib;
	VO_PROPERTY_GET		m_fPropGet;
	VO_PROPERTY_SET		m_fPropSet;

};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CAndroidUtilities_H__
