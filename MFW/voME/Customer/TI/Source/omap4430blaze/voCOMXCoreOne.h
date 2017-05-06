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

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voCOMXBaseObject.h"

#define VOOMX_COMPONENT_NUM		1

#define VOINFO(format, ...) do { \
	const char* pfile = strrchr(__FILE__, '/'); \
	pfile = (pfile == NULL ? __FILE__ : pfile + 1); \
  LOGI("%s::%s()->%d: " format, pfile, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);

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

protected:
	OMX_STRING		m_szCompName[VOOMX_COMPONENT_NUM];
	OMX_U32			m_nCompRoles[VOOMX_COMPONENT_NUM];
	OMX_STRING *	m_szCompRoles[VOOMX_COMPONENT_NUM];

};

#endif // __voCOMXCoreOne_H__
