#pragma once
#include "voType.h"
#include "voString.h"
#include "voOSFunc.h"
#include "cmnFile.h"

class CConfigFile
{
public:
	CConfigFile(void);
	~CConfigFile(void);

	VO_BOOL Open(VO_PTCHAR filename);
	void Close();

	bool ReadData(void* buffer, int size);
	bool WriteData(void* buffer, int size);

private:
	VO_PTR m_hFile;
};
