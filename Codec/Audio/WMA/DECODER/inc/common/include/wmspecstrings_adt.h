//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#pragma once
/*************************************************************************
* DEFINITIONS OF NEW TYPES 
*************************************************************************/
#if !defined(__midl)
#define __compname_props \
        __type_has_adt_prop(compname,nullterminated) \
        __type_has_adt_prop(compname,valid_schars) \
        __type_has_adt_prop(compname,correct_len) \
        __nullterminated
#if defined(UNICODE) || defined(_UNICODE)
#define  __TCHAR unsigned short
#else
#define  __TCHAR char
#endif
typedef __compname_props char* ValidCompNameA;
typedef __compname_props unsigned short* ValidCompNameW;
typedef __compname_props const unsigned short* ConstValidCompNameW;

#ifdef _IOS
//typedef __compname_props  TCHAR* SAL_ValidCompNameT;
//typedef __compname_props const  TCHAR* SAL_ConstValidCompNameT;
#elif defined (_MAC_OS)
//typedef __compname_props  TCHAR* SAL_ValidCompNameT;
//typedef __compname_props const  TCHAR* SAL_ConstValidCompNameT;
#else
typedef __compname_props  __$TCHAR* SAL_ValidCompNameT;
typedef __compname_props const  __$TCHAR* SAL_ConstValidCompNameT;
#endif
#undef __compname_props
#undef __TCHAR
#endif

/*************************************************************************
* DEFINITIONS OF INLINE FUNCTIONS FOR CASTING TO THE NEW TYPES : USER
*************************************************************************/

// tag: 2010.11.12
/*
#if (_MSC_VER >= 1000) && !defined(__midl) && defined(_PREFAST_)
#ifdef  __cplusplus
extern "C" {
#endif
void __inline __nothrow __SAL_ValidCompNameA(__out_has_type_adt_props(ValidCompNameA) const void *expr) { expr;}
void __inline __nothrow __SAL_ValidCompNameW(__out_has_type_adt_props(ValidCompNameW) const void *expr) { expr;}
#ifdef  __cplusplus
}
#endif
#define __assume_ValidCompNameA(expr) __SAL_ValidCompNameA(expr)
#define __assume_ValidCompNameW(expr) __SAL_ValidCompNameW(expr)
#else
 */
#define __assume_ValidCompNameA(expr) 
#define __assume_ValidCompNameW(expr)
//#endif

