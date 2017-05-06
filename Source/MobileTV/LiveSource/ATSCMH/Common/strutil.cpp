#include <string.h>
#include "macro.h"
#include "strutil.h"


char * StringDup(const char * _srcString)
{
	size_t len = strlen(_srcString);
	char * _s = new char[len + 1];
	if(_s == NULL)
		return NULL;
	memcpy(_s, _srcString, len);
	_s[len] = '\0';
	return _s;
}

char * StringDupSize(const char * _srcString)
{
	size_t len = strlen(_srcString);
	char * _s = new char[len + 1];
	if(_s == NULL)
		return NULL;

	_s[len] = '\0';
	return _s;
}

int Value(const char c)
{
	int value = 0;
	if(c >= '0' && c <= '9')
	{
		value = c - '0';
	}
	else if(c >= 'A' && c <= 'F') 
	{
		value = 10 + c - 'A';
	} 
	else if(c >= 'a' && c <= 'f') 
	{
		value = 10 + c - 'a';
	}
	else
	{
		value = 0;
	}

	return value;
}

bool HexStringToBinary(const char * _hexString, unsigned char ** __buffer, int * _bufLen)
{
	*__buffer = NULL;
	*_bufLen = 0;

	do
	{
		size_t hexStringLen = strlen(_hexString);
		if(hexStringLen == 0)
			break;

		*__buffer = new unsigned char[hexStringLen];
		if(*__buffer == NULL)
			break;

		unsigned char value1, value2;
		unsigned char * _to = (*__buffer);
		for(unsigned int i=0; i<hexStringLen; ++i)
		{
			value1 = Value(_hexString[i++]);
			if(i == hexStringLen)
			{
				value2 = 0;
				*_to++ = (value1 << 4) | value2;
				break;
			}

			value2 = Value(_hexString[i]);
			*_to++ = (value1 << 4) | value2;
		}
		*_bufLen = int(_to - *__buffer);

		return true;
	}while(0);

	SAFE_DELETE_ARRAY(*__buffer);
	return false;
}
