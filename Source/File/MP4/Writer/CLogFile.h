#pragma once

#include "voFile.h"

//#define _Output_Log_

#ifdef _Output_Log_
#define __D(fmt , value) CLogFile::WriterToFile(fmt , value);
#else
#define __D(fmt , value)
#endif //_Debug_Log_

class CLogFile
{
public:
	static CLogFile gLogFile;

public:
	CLogFile(void);
	virtual ~CLogFile(void);

	static void  WriterToFile(VO_PTCHAR pStr , int nValue = 0);
	static VO_S32  SetPosition(VO_S32 nOffset , VO_S32 nOrigin);
protected:
	static FILE *mFile;
};
