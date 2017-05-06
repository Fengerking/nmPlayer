
/*
 * david 2009-06-19
 * VisualOn
 */

#if !defined __VO_UTF8_TOOLS_H__
#define __VO_UTF8_TOOLS_H__

#include "tchar.h"

#define BYTE_1_REP          0x80   
#define BYTE_2_REP          0x800 
#define BYTE_3_REP          0x10000 
#define BYTE_4_REP          0x200000 

#define SIGMASK_1_1         0x80
#define SIGMASK_2_1         0xe0
#define SIGMASK_3_1         0xf0
#define SIGMASK_4_1         0xf8

//#if defined UNICODE || defined _UNICODE
//typedef TCHAR vowchar_t;
//#else
//typedef unsigned short vowchar_t;
//#endif

#if defined __cplusplus
extern "C" {
#endif

int vowcslen(const TCHAR* str); 
int VO_UCS2ToUTF8(const TCHAR *szSrc, const int nSrcLen, char *strDst, const int nDstLen);
int VO_UTF8ToUCS2(const char *szSrc, const int nSrcLen, TCHAR *strDst, const int nDstLen);

#if defined __cplusplus
}
#endif

#endif

