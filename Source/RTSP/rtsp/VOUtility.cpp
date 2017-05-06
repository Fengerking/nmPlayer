#include "VOUtility.h"
#include 		<stdarg.h>

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

static char tmpBuf[4096];
//void  VOAPI VOUtility::voLog(int id,const char* fileName,const char* logInfo,...)
void  VOAPI voLog(int id,const char* fileName,const char* logInfo,...)
{
	if(CUserOptions::UserOptions.m_bMakeLog==0)
		return;
#if 1
	
	va_list arg;
	va_start(arg, logInfo);
	vsprintf(tmpBuf, logInfo, arg);
	va_end(arg);
	CLog::Log.MakeLog((LOG_Level)id,(char*)fileName,tmpBuf);
	
#else
	FILE* file=log->GetFileByName((char*)fileName);
	if(file)
	{

		fprintf(file,"[%d]:", voGetCurrentTime());
		va_list arg;
		va_start(arg, logInfo);
		vfprintf(file,logInfo,arg);
		va_end(arg);
		fflush(file);
	}

#endif
}

#ifdef _VONAMESPACE
}
#endif