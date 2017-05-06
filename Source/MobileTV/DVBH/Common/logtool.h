#ifndef __LOGTOOL_H__
#define __LOGTOOL_H__

#include "commonheader.h"
#include "list_T.h"

class CLogFile
{
public:
	CLogFile();
	~CLogFile();
public:
	bool Create(const TCHAR * _fileName, const TCHAR * _outputDir);
	void Output(const char * _text);
	void Output(const void * _data, int len);
public:
	TCHAR  m_fileName[MAX_PATH];
	FILE * m_pLogFile;
};

class CLogTool
{
public:
	static CLogTool LogTool;

public:
	CLogTool(void);
	~CLogTool(void);

public:
	void EnableLog(bool bEnable);
public:
	CLogFile * CreateLogFile(const TCHAR * _fileName);
	FILE * GetHandle(const TCHAR * _fileName);
public:
	void LogOutput(TCHAR * _fileName, const char * _format, ...);
	void LogOutput(TCHAR * _fileName, const void * _data, int len);

protected:
	bool  m_bEnableLog;
protected:
	TCHAR m_outputDir[MAX_PATH];
protected:
	list_T<CLogFile *> m_listLogFile;
};


#endif //__LOGTOOL_H__