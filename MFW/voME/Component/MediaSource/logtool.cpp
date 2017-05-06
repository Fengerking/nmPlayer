
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
#   include <Windows.h>
#elif defined(LINUX)
#   include "vocrstypes.h"
#   include "vocrsapis.h"
#   include <stdarg.h>
#endif

#include "logtool.h"

CLogTool CLogTool::LogTool;

CLogTool::CLogTool(void)
: m_bEnableLog(false)
{

}

CLogTool::~CLogTool(void)
{
	list_T<CLogFile *>::iterator iter;
	for(iter=m_listLogFile.begin(); iter!=m_listLogFile.end(); ++iter)
	{
		delete *iter;
	}
	m_listLogFile.clear();
}

void CLogTool::EnableLog(bool bEnable)
{
	m_bEnableLog = bEnable;
	if(bEnable)
	{
		::GetModuleFileName(NULL, m_outputDir, MAX_PATH);

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
		TCHAR * _s = _tcsrchr(m_outputDir, _T('\\'));
		_tcscpy(_s, _T("\\LogOutput\\"));
#elif defined(LINUX)
		TCHAR* _s =  _tcsrchr(m_outputDir, _T('/'));
		_tcscpy(_s, _T("/LogOutput/"));
#endif
		CreateDirectory(m_outputDir, NULL);	
	}
}

CLogFile * CLogTool::CreateLogFile(const TCHAR * _fileName)
{
	CLogFile * pLogFile = NULL;
	list_T<CLogFile *>::iterator iter;
	for(iter=m_listLogFile.begin(); iter!=m_listLogFile.end(); ++iter)
	{
		if(_tcscmp((*iter)->m_fileName, _fileName) == 0)
		{
			pLogFile = (*iter);
			break;
		}
	}
	if(pLogFile == NULL)
	{
		pLogFile = new CLogFile();
		if(pLogFile == NULL)
			return NULL;

		if(!pLogFile->Create(_fileName, m_outputDir))
		{
			delete pLogFile;
			return NULL;
		}

		m_listLogFile.push_back(pLogFile);
	}

	return pLogFile;
}

FILE * CLogTool::GetHandle(const TCHAR * _fileName)
{
	CLogFile * pLogFile = NULL;
	list_T<CLogFile *>::iterator iter;
	for(iter=m_listLogFile.begin(); iter!=m_listLogFile.end(); ++iter)
	{
		if(_tcscmp((*iter)->m_fileName, _fileName) == 0)
		{
			pLogFile = (*iter);
			return pLogFile->m_pLogFile;
		}
	}
	return NULL;
}

void CLogTool::LogOutput(TCHAR * _fileName, const char * _format, ...)
{
	if(!m_bEnableLog)
		return;

	CLogFile * pLogFile = CreateLogFile(_fileName);
	if(pLogFile == NULL)
		return;

	char _text[512];
	va_list va;
    va_start(va, _format);
    vsprintf(_text, _format, va);
    va_end(va);
	pLogFile->Output(_text);
}

void CLogTool::LogOutput(TCHAR * _fileName, const void * _data, int len)
{
	if(!m_bEnableLog)
		return;

	CLogFile * pLogFile = CreateLogFile(_fileName);
	if(pLogFile == NULL)
		return;

	pLogFile->Output(_data, len);
}


// class CLogFile 
CLogFile::CLogFile()
: m_pLogFile(NULL)
{
}

CLogFile::~CLogFile()
{
	if(m_pLogFile != NULL)
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

bool CLogFile::Create(const TCHAR * _fileName, const TCHAR * _outputDir)
{
	_tcscpy(m_fileName, _outputDir);
	_tcscat(m_fileName, _fileName);
	m_pLogFile = _tfopen(m_fileName, _T("wb"));
	_tcscpy(m_fileName, _fileName);
	return (m_pLogFile != NULL);
}

void CLogFile::Output(const char * _text)
{
	if(m_pLogFile == NULL)
		return;

	fwrite(_text, 1, strlen(_text), m_pLogFile);
	fflush(m_pLogFile);
}

void CLogFile::Output(const void * _data, int len)
{
	if(m_pLogFile == NULL)
		return;

	fwrite(_data, 1, len, m_pLogFile);
	fflush(m_pLogFile);
}
