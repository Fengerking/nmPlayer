#include "sdp_utility.h"

const char * SDP_GetNextLine(const char * _sdpData)
{
	if(_sdpData == NULL || *_sdpData == '\0')
		return NULL;

	for(const char * _c=_sdpData; *_c != '\0'; ++_c)
	{
		if(*_c == '\n')
		{
			++_c; //skip '\n' to next newline
			return _c;
		}
	}
	return NULL;
}

size_t SDP_GetLineLength(const char * _sdpLine)
{
	if(_sdpLine == NULL || *_sdpLine == '\0')
		return 0;

	const char * _c = _sdpLine;
	while(*_c != '\0')
	{
		if(*_c == '\r' || *_c == '\n')
			break;

		++_c;
	}
	return _c - _sdpLine;
}

char * SDP_StringDup(const char * _string)
{
	if(_string == NULL || *_string == '\0')
		return NULL;

	size_t len = strlen(_string);
	char * _s = new char[len + 1];
	if(_s == NULL)
		return NULL;
	memcpy(_s, _string, len);
	_s[len] = '\0';
	return _s;
}

char * SDP_StringDup_N(const char * _string, size_t len)
{
	if(_string == NULL || *_string == '\0')
		return NULL;

	if(len > strlen(_string))
		len = strlen(_string);

	char * _s = new char[len + 1];
	if(_s == NULL)
		return NULL;
	memcpy(_s, _string, len);
	_s[len] = '\0';
	return _s;
}
