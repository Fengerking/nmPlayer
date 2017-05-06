	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voChkLcsLib.h

	Contains:	voChkLcsLib header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-07_02		JBF			Create file

*******************************************************************************/

#ifndef __voChkLcsLib_H__
#define __voChkLcsLib_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// return value:
// 0	is ok.
// <0	is Failed.  -1 Can't find the voVidDec.dll file
//					-2 Can't get function from voVidDec.dll file
//					-3 Can't create the buffer
//					-4 Can't init in voVidDec.Dll.
//					-5 The voVidDec.Dll is not available
//					-6 Check license file failed.
// >0	is limit time. (seconds)
int	__cdecl		CheckLicenseFile (void * hInst);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voChkLcsLib_H__