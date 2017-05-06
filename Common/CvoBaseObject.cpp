	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoBaseObject.cpp

	Contains:	vo base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-31		JBF			Create file

*******************************************************************************/
//#include "stdafx.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

int CvoBaseObject::g_voSDKObjectNum = 0;

CvoBaseObject::CvoBaseObject(void)
{
	g_voSDKObjectNum++;
}

CvoBaseObject::~CvoBaseObject(void)
{
	g_voSDKObjectNum--;
}
