#include "CLogFile.h"

CLogFile CLogFile::gLogFile;
FILE *	 CLogFile::mFile = NULL;

CLogFile::CLogFile(void)
{
#ifdef _Output_Log_
#ifdef WIN32
	mFile = fopen("d:\\Mp4Writer.log" , "w");
#else //WIN32
	mFile = fopen("/sdcard/Mp4Writer.log" , "wb");
#endif //WIN32
#endif
}

CLogFile::~CLogFile(void)
{
	if(mFile != NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}
}

void  CLogFile::WriterToFile(VO_PTCHAR pStr , int nValue)
{
	if(mFile)
	{
		fprintf(mFile , "%s  %d \r\n" ,pStr , nValue);
		fflush(mFile);
	}
}

VO_S32  CLogFile::SetPosition(VO_S32 nOffset , VO_S32 nOrigin)
{
	if(mFile)
	{
		return fseek(mFile , nOffset , nOrigin);
	}

	return 0;
}