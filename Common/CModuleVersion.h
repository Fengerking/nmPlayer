	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CModuleVersion.h

	Contains:	CModuleVersion header file

	Written by:	

	Change History (most recent first):
	2013-06-07		Jim			Create file

*******************************************************************************/

#ifndef __CModuleVersion_H__
#define __CModuleVersion_H__

#include "voYYDef_Common.h"
#include "CDllLoad.h"
#include "voOnStreamType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CModuleVersion : public CDllLoad
{
public:
	CModuleVersion ();
	virtual ~CModuleVersion (void);
    
public:
    int                 GetModuleVersion(VOOSMP_MODULE_VERSION* pVersion);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CModuleVersion_H__
