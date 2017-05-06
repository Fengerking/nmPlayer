	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreOne.h

	Contains:	voCOMXCoreOne header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCoreOne_H__
#define __voCOMXCoreOne_H__

#include "OMX_Component.h"
#include "voCOMXBaseObject.h"
#include "voCOMXCfgOne.h"
#include "voString.h"

#define VOOMX_COMPONENT_NUM		16  

class voCOMXCoreOne : public voCOMXBaseObject
{
public:
	voCOMXCoreOne(void);
	virtual ~voCOMXCoreOne(void);

	virtual OMX_ERRORTYPE	Init (OMX_PTR hInst);
	virtual OMX_STRING		GetName (OMX_U32 nIndex);

	virtual OMX_ERRORTYPE	LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName);
	virtual OMX_ERRORTYPE	FreeComponent (OMX_COMPONENTTYPE * pHandle);

	virtual OMX_ERRORTYPE	GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames);
	virtual OMX_ERRORTYPE	GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);

	virtual OMX_ERRORTYPE	SetWorkingPath (OMX_STRING pWorkingPath);

protected:
	OMX_STRING		m_szCompName[VOOMX_COMPONENT_NUM];
	OMX_U32			m_nCompRoles[VOOMX_COMPONENT_NUM];
	OMX_STRING *	m_szCompRoles[VOOMX_COMPONENT_NUM];

	voCOMXCfgOne    m_cfgOne;
	OMX_U32         m_uCompNum;

	VO_TCHAR *		m_pWorkingPath;
};

#endif // __voCOMXCoreOne_H__
