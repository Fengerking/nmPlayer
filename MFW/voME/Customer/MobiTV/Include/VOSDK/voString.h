	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voString.h

	Contains:	module and ID define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voString_H__
#define __voString_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined _WIN32 || defined LINUX
#include <string.h>
#include <stdio.h>
#endif

#ifdef _WIN32
#include <tchar.h>
#endif // _WIN32

#include "voType.h"

#ifdef _WIN32
#define VO_TCHAR		TCHAR
#define VO_PTCHAR		TCHAR*

#define vostrcat         _tcscat
#define vostrcat_s       _tcscat_s
#define vostrchr         _tcschr
#define vostrcpy         _tcscpy
#define vostrcpy_s       _tcscpy_s
#define vostrcspn        _tcscspn
#define vostrlen         _tcslen
#define vostrnlen        _tcsnlen
#define vostrncat        _tcsncat
#define vostrncat_s      _tcsncat_s
#define vostrncat_l      _tcsncat_l
#define vostrncat_s_l    _tcsncat_s_l
#define vostrncpy        _tcsncpy
#define vostrncpy_s      _tcsncpy_s
#define vostrncpy_l      _tcsncpy_l
#define vostrncpy_s_l    _tcsncpy_s_l
#define vostrpbrk        _tcspbrk
#define vostrrchr        _tcsrchr
#define vostrspn         _tcsspn
#define vostrstr         _tcsstr
#define vostrtok         _tcstok
#define vostrtok_s       _tcstok_s
#define vostrtok_l       _tcstok_l
#define vostrtok_s_l     _tcstok_s_l
#define vostrerror       _tcserror
#define vostrerror_s     _tcserror_s

#define vostrdup         _tcsdup
#define vostrnset        _tcsnset
#define vostrnset_s      _tcsnset_s
#define vostrnset_l      _tcsnset_l
#define vostrnset_s_l    _tcsnset_s_l
#define vostrrev         _tcsrev
#define vostrset         _tcsset
#define vostrset_s       _tcsset_s
#define vostrset_l       _tcsset_l
#define vostrset_s_l     _tcsset_s_l

#define vostrcmp         _tcscmp
#define vostricmp        _tcsicmp
#define vostricmp_l      _tcsicmp_l
#define vostrnccmp       _tcsnccmp
#define vostrncmp        _tcsncmp
#define vostrncicmp      _tcsncicmp
#define vostrncicmp_l    _tcsncicmp_l
#define vostrnicmp       _tcsnicmp
#define vostrnicmp_l     _tcsnicmp_l

#define vostrcoll        _tcscoll
#define vostrcoll_l      _tcscoll_l
#define vostricoll       _tcsicoll
#define vostricoll_l     _tcsicoll_l
#define vostrnccoll      _tcsnccoll
#define vostrnccoll_l    _tcsnccoll_l
#define vostrncoll       _tcsncoll
#define vostrncoll_l     _tcsncoll_l
#define vostrncicoll     _tcsncicoll
#define vostrncicoll_l   _tcsncicoll_l
#define vostrnicoll      _tcsnicoll
#define vostrnicoll_l    _tcsnicoll_l

#define vostrclen       _tcsclen
#define vostrcnlen      _tcscnlen
#define vostrnccat      _tcsnccat
#define vostrnccat_s    _tcsnccat_s
#define vostrnccat_l    _tcsnccat_l
#define vostrnccat_s_l  _tcsnccat_s_l
#define vostrnccpy      _tcsnccpy
#define vostrnccpy_s    _tcsnccpy_s
#define vostrnccpy_l    _tcsnccpy_l
#define vostrnccpy_s_l  _tcsnccpy_s_l
#define vostrncset      _tcsncset
#define vostrncset_s    _tcsncset_s
#define vostrncset_l    _tcsncset_l
#define vostrncset_s_l  _tcsncset_s_l

#define vostrdec		_tcsdec
#define vostrinc		_tcsinc
#define vostrnbcnt		_tcsnbcnt
#define vostrnccnt		_tcsnccnt
#define vostrnextc		_tcsnextc
#define vostrninc		_tcsninc
#define vostrspnp		_tcsspnp

#define vostrlwr		_tcslwr
#define vostrlwr_l		_tcslwr_l
#define vostrlwr_s		_tcslwr_s
#define vostrlwr_s_l	_tcslwr_s_l
#define vostrupr		_tcsupr
#define vostrupr_l		_tcsupr_l
#define vostrupr_s		_tcsupr_s
#define vostrupr_s_l	_tcsupr_s_l
#define vostrxfrm		_tcsxfrm
#define vostrxfrm_l		_tcsxfrm_l

#else

typedef char TCHAR, *PTCHAR;
#define VO_TCHAR		char
#define VO_PTCHAR		char*
#define _tcscpy strcpy
#define _stprintf sprintf

#define _T
#define _TEXT

#define vostrcat        strcat
#define vostrcat_s      strcat_s
#define vostrchr        strchr
#define vostrcpy        strcpy
#define vostrcpy_s      strcpy_s
#define vostrcspn       strcspn
#define vostrlen        strlen
#define vostrnlen       strnlen
#define vostrncat       strncat
#define vostrncat_s     strncat_s
#define vostrncat_l     strncat_l
#define vostrncat_s_l   strncat_s_l
#define vostrncpy       strncpy
#define vostrncpy_s     strncpy_s
#define vostrncpy_l     strncpy_l
#define vostrncpy_s_l   strncpy_s_l
#define vostrpbrk       strpbrk
#define vostrrchr       strrchr
#define vostrspn        strspn
#define vostrstr        strstr
#define vostrtok        strtok
#define vostrtok_s      strtok_s
#define vostrtok_l      strtok_l
#define vostrtok_s_l    strtok_s_l
#define vostrerror      strerror
#define vostrerror_s    strerror_s

#define vostrdup        strdup
#define vostrnset       strnset
#define vostrnset_s     strnset_s
#define vostrnset_l     strnset_l
#define vostrnset_s_l   strnset_s_l
#define vostrrev        strrev
#define vostrset        strset
#define vostrset_s      strset_s
#define vostrset_l      strset_l
#define vostrset_s_l    strset_s_l

#define vostrcmp        strcmp
#define vostricmp       strcmp
#define vostricmp_l     stricmp_l
#define vostrnccmp      strnccmp
#define vostrncmp       strncmp
#define vostrncicmp     strncicmp
#define vostrncicmp_l   strncicmp_l
#define vostrnicmp      strnicmp
#define vostrnicmp_l    strnicmp_l

#define vostrcoll       strcoll
#define vostrcoll_l     strcoll_l
#define vostricoll      stricoll
#define vostricoll_l    stricoll_l
#define vostrnccoll     strnccoll
#define vostrnccoll_l   strnccoll_l
#define vostrncoll      strncoll
#define vostrncoll_l    strncoll_l
#define vostrncicoll    strncicoll
#define vostrncicoll_l  strncicoll_l
#define vostrnicoll     strnicoll
#define vostrnicoll_l   strnicoll_l

#define vostrclen       _tcsclen
#define vostrcnlen      _tcscnlen
#define vostrnccat      _tcsnccat
#define vostrnccat_s    _tcsnccat_s
#define vostrnccat_l    _tcsnccat_l
#define vostrnccat_s_l  _tcsnccat_s_l
#define vostrnccpy      _tcsnccpy
#define vostrnccpy_s    _tcsnccpy_s
#define vostrnccpy_l    _tcsnccpy_l
#define vostrnccpy_s_l  _tcsnccpy_s_l
#define vostrncset      _tcsncset
#define vostrncset_s    _tcsncset_s
#define vostrncset_l    _tcsncset_l
#define vostrncset_s_l  _tcsncset_s_l

#define vostrdec		strdec
#define vostrinc		strinc
#define vostrnbcnt		strnbcnt
#define vostrnccnt		strnccnt
#define vostrnextc		strnextc
#define vostrninc		strninc
#define vostrspnp		strspnp

#define vostrlwr		strlwr
#define vostrlwr_l		strlwr_l
#define vostrlwr_s		strlwr_s
#define vostrlwr_s_l	strlwr_s_l
#define vostrupr		strupr
#define vostrupr_l		strupr_l
#define vostrupr_s		strupr_s
#define vostrupr_s_l	strupr_s_l
#define vostrxfrm		strxfrm
#define vostrxfrm_l		strxfrm_l

#endif // _WIN32

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voString_H__
