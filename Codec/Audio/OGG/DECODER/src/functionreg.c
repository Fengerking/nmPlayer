//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    functionreg.c

Abstract:

    function registry c file.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#include "functionreg.h"
#include "floor.h"

VorbisFuncFloor     *Floor_P[] = 
{
    &floor0_exportbundle,
    &floor1_exportbundle,
};

VorbisFuncResidue   *Residue_P[]=	//_residue_P
{
	&residue0_exportbundle,
	&residue1_exportbundle,
	&residue2_exportbundle,
};

VorbisFuncMapping   *Mapping_P[] = //_mapping_P
{
	&mapping0_exportbundle,
};

