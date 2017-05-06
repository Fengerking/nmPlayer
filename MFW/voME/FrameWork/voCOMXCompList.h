	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompList.h

	Contains:	voCOMXCompList header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXCompList_H__
#define __voCOMXCompList_H__

#include <OMX_Core.h>
#include <OMX_Component.h>
#include <vomeAPI.h>

#include "voCOMXBaseObject.h"

#include "voCOMXCfgCore.h"
#include "voCOMXCoreLoader.h"

struct SCompItem
{
	OMX_STRING			Name;
	voCOMXCoreLoader *	Loader;
	SCompItem *			Next;
};

struct SCompHandle
{
	OMX_COMPONENTTYPE *	Handle;
	voCOMXCoreLoader *	Loader;
	SCompHandle *		Next;
};

// wrapper for whatever critical section we have
class voCOMXCompList : public voCOMXBaseObject
{
public:
    voCOMXCompList(void);
    virtual ~voCOMXCompList(void);

	virtual void					SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData);

	virtual OMX_ERRORTYPE			Init (voCOMXCfgCore * pConfig, OMX_STRING pWorkPath = NULL);

	virtual OMX_COMPONENTTYPE *		CreateComponent (OMX_STRING pName, OMX_PTR pAppData,
													 OMX_CALLBACKTYPE* pCallBacks);
	virtual void					FreeComponent (OMX_COMPONENTTYPE * pComp);

	virtual OMX_ERRORTYPE			GetComponentName (OMX_STRING pName, OMX_U32 nIndex);
	virtual OMX_ERRORTYPE			GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);

	virtual void					SetLibOperator (OMX_VO_LIB_OPERATOR * pLibOP) {m_pLibOP = pLibOP;}


protected:
	virtual OMX_ERRORTYPE			LoadCore (void);

	virtual	void					ReleaseHandles (void);
	virtual	void					ReleaseItems (void);
	virtual	void					ReleaseLoader (void);

	virtual voCOMXCoreLoader *		FindLoader (OMX_STRING pName);

protected:
	voCOMXCfgCore *					m_pConfig;
	OMX_STRING						m_pWorkPath;

	OMX_U32							m_nLoaderNum;
	voCOMXCoreLoader **				m_ppCompLoader;

	OMX_U32							m_nItemCount;
	SCompItem *						m_pFirstItem;

	OMX_U32							m_nHandleCount;
	SCompHandle *					m_pFirstHandle;

	VOMECallBack					m_pCallBack;
	OMX_PTR							m_pUserData;

	OMX_VO_LIB_OPERATOR *			m_pLibOP;
};

#endif //__voCOMXCompList_H__
