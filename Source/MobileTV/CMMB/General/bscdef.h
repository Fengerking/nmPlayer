#ifndef  _DSC_DEF_H_
#define  _DSC_DEF_H_

#ifdef WIN32
//#include <Windows.h>
#include "vocrstypes.h"

#else
typedef unsigned char		BYTE;
typedef unsigned char*		LPBYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;
typedef	unsigned long		LONG32;				
typedef unsigned __int64	LONG64;
typedef unsigned __int64	LONGLONG;

typedef void				VOID;


#define	NULL 0

#endif




#endif