	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoBaseObject.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-31		JBF			Create file

*******************************************************************************/

#ifndef __CvoBaseObject_H__
#define __CvoBaseObject_H__

#include "voYYDef_Common.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CvoBaseObject
{
public:
	CvoBaseObject(void);
	virtual ~CvoBaseObject(void);

protected:
	VO_U8		m_szName[512];

public:
	static	int	g_voSDKObjectNum;

};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CvoBaseObject_H__
