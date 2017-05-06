	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreQcomVdec.h

	Contains:	voCOMXCoreQcomVdec header file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18		East		Create file

*******************************************************************************/

#ifndef __voCOMXCoreQcomVdec_H__
#define __voCOMXCoreQcomVdec_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voCOMXBaseObject.h"

class voCOMXCoreQcomVdec : public voCOMXBaseObject
{
public:
	voCOMXCoreQcomVdec();
	virtual ~voCOMXCoreQcomVdec();

	virtual OMX_ERRORTYPE	Init(OMX_PTR hInst);
	virtual OMX_STRING		GetName(OMX_U32 nIndex);

	virtual OMX_ERRORTYPE	LoadComponent(OMX_COMPONENTTYPE* pHandle, OMX_STRING pName);
	virtual OMX_ERRORTYPE	FreeComponent(OMX_COMPONENTTYPE* pHandle);

	virtual OMX_ERRORTYPE	GetComponentsOfRole(OMX_STRING role, OMX_U32* pNumComps, OMX_U8** compNames);
	virtual OMX_ERRORTYPE	GetRolesOfComponent(OMX_STRING compName, OMX_U32* pNumRoles, OMX_U8** roles);

protected:
	OMX_STRING		m_szCompName;
	OMX_U32			m_nCompRoles;
	OMX_STRING*		m_szCompRoles;
};

#endif // __voCOMXCoreQcomVdec_H__
