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

#ifndef __voIrdetoDRM_H__
#define __voIrdetoDRM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM.h"

typedef struct
{
	VO_PCHAR			pURL;
	VO_PCHAR			pManifest;
} VO_IrdetoDRM_INFO;

enum IrdetoDRM_DecryptProcess
{
    IrdetoDRM_DecryptProcess_BEGIN,
    IrdetoDRM_DecryptProcess_PROCESSING,
    IrdetoDRM_DecryptProcess_END,
};

typedef struct  
{
    unsigned int                segmentID;
    IrdetoDRM_DecryptProcess    process;
}VO_IrdetoDRM_DecryptINFO;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voIrdetoDRM_H__
