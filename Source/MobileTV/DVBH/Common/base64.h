#ifndef __BASE64_H__
#define __BASE64_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
	Returns a newly allocated array - of size "resultSize" - that
	the caller is responsible for delete[].
*/
unsigned char * Base64Decode(char * in, unsigned & resultSize, bool trimTrailingZeros = true);

/*
	Returns a NULL-terminated string that
	the caller is responsible for delete[].
*/
char * Base64Encode(char const * origSigned, unsigned origLength);


#ifdef __cplusplus
}
#endif

#endif //__BASE64_H__