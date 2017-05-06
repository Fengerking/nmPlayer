#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
#   include <windows.h>
#elif defined(LINUX)
#   include <time.h>
#endif

#include <stdio.h>
#include "isomutil.h"

using namespace ISOM;

char* Util::TimeText(uint64 t)
{
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	SYSTEMTIME st;

	// FILETIME in Win32 is from jan 1, 1601
	// time between jan 1, 1601 and jan 1, 1904 in units of 100 nanoseconds 
#if 0
	memset(&st, 0, sizeof(st));
	st.wYear = 1904;
	st.wMonth = 1;
	st.wDay = 1;
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	uint64 offset = *(uint64*)&ft;
#else
	const uint64 offset = (uint64)0x0153b281e0fb4000;
#endif

	t *= 10000000;
	t += offset;
	FileTimeToSystemTime((FILETIME*)&t, &st);
	static char timetext[32];
	sprintf(timetext, "%d/%d/%d %d:%d:%d", 
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);
	return timetext;
#elif defined (LINUX)
	time_t st;
	struct tm *p;

	time(&st);
	p = gmtime(&st);
	static char timetext[32];
	sprintf(timetext, "%d/%d/%d %d:%d:%d", 
		1900+p->tm_year, p->tm_mon, p->tm_mday,	
		p->tm_hour, p->tm_min, p->tm_sec);

	return timetext;
#endif
}

uint64 Util::CurrentTime()
{
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	const uint64 offset = (uint64)0x0153b281e0fb4000;

	SYSTEMTIME st;
	GetSystemTime(&st);

	uint64 t;
	SystemTimeToFileTime(&st, (FILETIME*)&t);
	t -= offset;
	t /= 10000000;
	return t;
#elif defined (LINUX)
	return time((time_t*)NULL);
#endif
}

char* Util::LanguageText(uint16 lang)
{
	static char langtext[4];
	langtext[0] = (int8)((lang & (0x1f << 10)) >> 10) + 0x60;
	langtext[1] = (int8)((lang & (0x1f << 5)) >> 5) + 0x60;
	langtext[2] = (int8)((lang & 0x1f)) + 0x60;
	langtext[3] = 0;
	return langtext;
}

uint16 Util::LanguageCode(uint32 lang)
{
	uint16 code = 0;
	code |= (uint16)((lang & 0x000000ff) - 0x60);
	code |= (uint16)(((lang & 0x0000ff00) >> 8) - 0x60) << 5;
	code |= (uint16)(((lang & 0x00ff0000) >> 16) - 0x60) << 10;
	return code;
}

