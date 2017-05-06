#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#  include <windows.h>
#elif defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
#  include <time.h>
#endif
 
#include <stdio.h>
#include "mpxutil.h"

//namespace MPx {
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

char* __stdcall Util::TimeText(uint64 t)
{

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

#  if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
	 const uint64 offset = (uint64)0x0153b281e0fb4000;
#  elif defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
	 // no using it in LINUX os
	//  const uint64 offset = (uint64)0x0153b281e0fb4000ll;
#  endif 
#endif

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
    SYSTEMTIME st;
	t *= 10000000;
	t += offset;
	FileTimeToSystemTime((FILETIME*)&t, &st);
	static char timetext[32];
	sprintf(timetext, "%d/%d/%d %d:%d:%d", 
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);
	return timetext;
#elif defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
	time_t st = time(NULL);
	struct tm *pm = localtime(&st);
	static char buf[32];
	
	strftime(buf, sizeof(buf), "%Y/%m%/d %T", pm);
	return buf;
#endif
}

uint64 __stdcall Util::CurrentTime()
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
	const uint64 offset = (uint64)0x0153b281e0fb4000;

	SYSTEMTIME st;
	GetSystemTime(&st);

	uint64 t;
	SystemTimeToFileTime(&st, (FILETIME*)&t);
	t -= offset;
	t /= 10000000;
	return t;
#elif defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
	const uint64 offset = (uint64)2082844800;
	return offset + time(NULL);
#endif
}

char* __stdcall Util::LanguageText(uint16 lang)
{
	static char langtext[4];
	langtext[0] = (int8)((lang & (0x1f << 10)) >> 10) + 0x60;
	langtext[1] = (int8)((lang & (0x1f << 5)) >> 5) + 0x60;
	langtext[2] = (int8)((lang & 0x1f)) + 0x60;
	langtext[3] = 0;
	return langtext;
}

uint16 __stdcall Util::LanguageCode(uint32 lang)
{
	uint16 code = 0;
	code |= (uint16)((lang & 0x000000ff) - 0x60);
	code |= (uint16)(((lang & 0x0000ff00) >> 8) - 0x60) << 5;
	code |= (uint16)(((lang & 0x00ff0000) >> 16) - 0x60) << 10;
	return code;
}


//} //namespace MPx
