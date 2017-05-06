	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCfgOne.h

	Contains:	voCOMXCfgOne header file

    Aim :       Make the One have different component number and roles through this config file
	Written by:	XiaGuangTai

	Change History (most recent first):
	2009-12-14		gtxia			Create file

*******************************************************************************/

#ifndef _VOCOMX_CFG_ONE_H
#define _VOCOMX_CFG_ONE_H

#include "voCOMXBaseConfig.h"

class voCOMXCfgOne : public voCOMXBaseConfig
{
public:
	voCOMXCfgOne();
	virtual ~voCOMXCfgOne();

public:
	int             getCompCount();
	OMX_STRING      getCompNameByIndex(const int index);
	int             getCompRolesCount(OMX_STRING& inComName);
	OMX_STRING      getCompRoleByIndex(OMX_STRING& inComName, const int index);
	
};
#endif // _VOCOMX_CFG_ONE_H
