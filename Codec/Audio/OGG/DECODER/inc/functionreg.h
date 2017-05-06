//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    functionreg.h

Abstract:

    function registry header file.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#ifndef __FUNCTION_REG_H_
#define __FUNCTION_REG_H_

#include "floor.h"
#include "residue.h"
#include "mapping.h"

#define VI_TRANSFORMB 1
#define VI_WINDOWB 1
#define VI_TIMEB 1
#define VI_FLOORB 2
#define VI_RESB 3
#define VI_MAPB 1

extern VorbisFuncFloor     *Floor_P[];		//_floor_P
extern VorbisFuncResidue   *Residue_P[];	//_residue_P
extern VorbisFuncMapping   *Mapping_P[];	//_mapping_P

#endif	//__FUNCTION_REG_H_
