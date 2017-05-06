#if !defined  __VOINFO_H__
#define __VOINFO_H__

#include <utils/Log.h>

#if defined  __VODBG__

#define VOINFO(format, ...) do { \
	const char* pfile = strrchr(__FILE__, '/'); \
	pfile = (pfile == NULL ? __FILE__ : pfile + 1); \
  LOGI("David %s::%s()->%d: " format, pfile, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);

#else

#define VOINFO(format, ...) 

#endif

#endif //__VOINFO_H__
