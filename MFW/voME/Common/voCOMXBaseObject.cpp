	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseObject.cpp

	Contains:	vo base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-31		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "voOMXMemory.h"

#define LOG_TAG "voCOMXBaseObject"

#include "voCOMXBaseObject.h"
#include "voLog.h"



OMX_S32 voCOMXBaseObject::g_ObjectNum = 0;

voCOMXBaseObject::voCOMXBaseObject(void):m_pObjName(NULL)
{
	m_pObjName = (OMX_STRING) voOMXMemAlloc (256);
	if (m_pObjName != NULL)
		strcpy (m_pObjName, __FILE__);
	g_ObjectNum++;
}

voCOMXBaseObject::~voCOMXBaseObject(void)
{
	voOMXMemFree (m_pObjName);
	g_ObjectNum--;
}
