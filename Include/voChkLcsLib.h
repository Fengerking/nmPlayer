/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

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
