//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;

//
// classes used to support dll entrypoints for COM objects.
//
#include <windows.h>
#include <wxdebug.h>
#include <streams.h>

//
// DLL debugging structure for DebugZones
//
DBGPARAM dpCurSettings =
{
	TEXT("quartz"),
	{
	    ZONE_NAMES
	},
//	0x0000
//	0xffff	 	// Show everything
	DBG_BIT(0)  // just errors turned on by default
};

