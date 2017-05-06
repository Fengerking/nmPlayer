#ifndef __STRUTIL_H__
#define __STRUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif


char * StringDup(const char * _srcString);
char * StringDupSize(const char * _srcString);

int Value(const char c);
bool HexStringToBinary(const char * _hexString, unsigned char ** __buffer, int * _bufLen);


#ifdef __cplusplus
}
#endif

#endif //__STRUTIL_H__