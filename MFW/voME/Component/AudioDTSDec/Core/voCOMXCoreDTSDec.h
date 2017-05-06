	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreDoblyEffect.h

	Contains:	voCOMXCoreDoblyEffect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCoreDoblyEffect_H__
#define __voCOMXCoreDoblyEffect_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voCOMXBaseObject.h"

class voCOMXCoreDTSDec : public voCOMXBaseObject
{
public:
	voCOMXCoreDTSDec(void);
	virtual ~voCOMXCoreDTSDec(void);

	virtual OMX_ERRORTYPE	Init (OMX_PTR hInst);
	virtual OMX_STRING		GetName (OMX_U32 nIndex);

	virtual OMX_ERRORTYPE	LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName);
	virtual OMX_ERRORTYPE	FreeComponent (OMX_COMPONENTTYPE * pHandle);

	virtual OMX_ERRORTYPE	GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames);
	virtual OMX_ERRORTYPE	GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);

protected:
	OMX_STRING		m_szCompName;
	OMX_U32			m_nCompRoles;
	OMX_STRING *	m_szCompRoles;

};

#endif // __voCOMXCoreOne_H__
