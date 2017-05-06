#include "ConfigFile.h"

#define MAX_PATH 260

CConfigFile::CConfigFile(void)
: m_hFile(NULL)
{
}

CConfigFile::~CConfigFile(void)
{
	Close();
}

VO_BOOL CConfigFile::Open(VO_PTCHAR filename)
{
	if (m_hFile)
		Close();

	VO_TCHAR	szFilePath[MAX_PATH];
	voOS_GetAppFolder(szFilePath, MAX_PATH);
	vostrcat(szFilePath, filename);

	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)szFilePath;
	fileSource.nMode = VO_FILE_READ_WRITE;
	m_hFile = cmnFileOpen(&fileSource);
	if (m_hFile == NULL)
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

void CConfigFile::Close()
{
	if (m_hFile)
	{
		cmnFileClose(m_hFile);
		m_hFile = NULL;
	}
}

bool CConfigFile::ReadData(void* buffer, int size)
{
	VO_S32 dwRead = cmnFileRead(m_hFile, buffer, size);
	return (dwRead == size);
}

bool CConfigFile::WriteData(void* buffer, int size)
{
	VO_S32 dwWrote = cmnFileWrite(m_hFile, buffer, size);
	return (dwWrote == size);
}

