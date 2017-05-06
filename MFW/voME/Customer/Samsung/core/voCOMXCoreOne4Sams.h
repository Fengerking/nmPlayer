	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreOne4Sams.h

	Contains:	voCOMXCoreOne4Sams header file

	Written by:	Xia GuangTai

*******************************************************************************/

#ifndef __voCOMXCoreOne4Sams_H__
#define __voCOMXCoreOne4Sams_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voCOMXBaseObject.h"

#define VOOMX_SAMS_COMPONENT_NUM		2

class voCOMXCoreOne4Sams : public voCOMXBaseObject
{
public:
	voCOMXCoreOne4Sams(void);
	virtual ~voCOMXCoreOne4Sams(void);

	virtual OMX_ERRORTYPE	Init (OMX_PTR hInst);
	virtual OMX_STRING		GetName (OMX_U32 nIndex);

	virtual OMX_ERRORTYPE	LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName);
	virtual OMX_ERRORTYPE	FreeComponent (OMX_COMPONENTTYPE * pHandle);

	virtual OMX_ERRORTYPE	GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames);
	virtual OMX_ERRORTYPE	GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);

protected:
	OMX_STRING		m_szCompName[VOOMX_SAMS_COMPONENT_NUM];
	OMX_U32			m_nCompRoles[VOOMX_SAMS_COMPONENT_NUM];
	OMX_STRING *	m_szCompRoles[VOOMX_SAMS_COMPONENT_NUM];

	OMX_U32         m_uCompNum;
};

#endif // __voCOMXCoreOne4Sams_H__
