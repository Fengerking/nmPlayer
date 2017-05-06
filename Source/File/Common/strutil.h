#ifndef __STRUTIL_H__
#define __STRUTIL_H__


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif
#endif


char * StringDup(const char * _srcString);
char * StringDupSize(const char * _srcString);

int Value(const char c);
bool HexStringToBinary(const char * _hexString, unsigned char ** __buffer, int * _bufLen);


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif
#endif

#endif //__STRUTIL_H__