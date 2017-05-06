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

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Thu Jun 24 19:41:57 2004
 */
/* Compiler settings for .\dvdcom.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __dvdcom_h__
#define __dvdcom_h__

/* Forward Declarations */ 

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_dvdcom_0000 */
/* [local] */ 

typedef unsigned char UINT8;

typedef unsigned short UINT16;

typedef unsigned int UINT24;

typedef unsigned int UINT32;

typedef unsigned __int64 UINT64;

typedef signed char INT8;

typedef short INT16;

typedef int INT32;

typedef __int64 INT64;

typedef /* [public] */ struct __MIDL___MIDL_itf_dvdcom_0000_0001
    {
    UINT8 key[ 5 ];
    }	CSS_KEY;

typedef /* [public] */ struct __MIDL___MIDL_itf_dvdcom_0000_0002
    {
    UINT8 challenge[ 10 ];
    }	CSS_CHALLENGE;

typedef /* [public] */ struct __MIDL___MIDL_itf_dvdcom_0000_0003
    {
    UINT32 sdks[ 512 ];
    }	CSS_SDKS;

#define DVDLBSZ	2048
#define	DVD_SELECT_UPPER 0
#define	DVD_SELECT_LOWER 1
#define	DVD_SELECT_LEFT  2
#define	DVD_SELECT_RIGHT 3


extern RPC_IF_HANDLE __MIDL_itf_dvdcom_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dvdcom_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


