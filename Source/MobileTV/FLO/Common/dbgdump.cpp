#include "dbgdump.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "voLog.h"
#include "mylog.h"

#ifdef WIN32
#define PATH_SEP "\\"
#else //WIN32
#define PATH_SEP "/"
#endif //WIN32

MYLOG_DEF


// CDumper
//==============================================================================

#ifndef WIN32

unsigned int GetTickCount()
{
    struct timeval timenow;
    gettimeofday(&timenow, NULL);

    unsigned int tick = timenow.tv_sec * 1000;
    tick += timenow.tv_usec / 1000;
    return tick;
}

#endif

unsigned int _AppStartTick = GetTickCount();

// CDumper
//==============================================================================

char CDumper::m_szFolder[256];
int CDumper::m_nMask = DEFAULT_LOG_MASK;

bool CDumper::SetFolder(const char* path)
{
	strcpy(m_szFolder, path);
#ifdef WIN32
	int s = (bool)CreateDirectoryA(m_szFolder, NULL);
#else
    int s = mkdir(m_szFolder, 0755);
#endif
    VOLOGI("mkdir %s... %d", m_szFolder, s);
	
	MYLOG_INIT();

	return true;
}

void CDumper::SetMask(int mask) 
{ 
    m_nMask = mask; 
    VOLOGI("Dump level: %d (0x%x)", mask, mask);
}


// CDumper
//==============================================================================

CDumper::CDumper()
{
	m_fDump = NULL;
	m_nEnable = 0;
}

CDumper::~CDumper()
{
	Close();
}

void CDumper::Close()
{
	if (m_fDump)
	{
        VOLOGI("Close file %p", m_fDump);
		fclose(m_fDump);
		m_fDump = NULL;
	}
}

bool CDumper::Open(const char* filename, int enable_flag)
{
    Close();
	m_nEnable = enable_flag;
	char path[256];
	sprintf(path, "%s" PATH_SEP "%s", m_szFolder, filename);
	if (!(m_nEnable && (m_nEnable & GetMask())))
		return false;
	m_fDump = fopen(path, "wb");
    VOLOGI("fopen %s (0x%x/0x%x)... %p", path, enable_flag, GetMask(), m_fDump);
	return m_fDump ? true : false;
}

// CTextDumper
//==============================================================================

CTextDumper::CTextDumper()
{
    m_cBuffer = 0;
}

void CTextDumper::Dump(const char* fmt, ...)
{
    if (m_fDump == NULL)
        return;
	if (!(m_nEnable && (m_nEnable & GetMask())))
		return;
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    //vfprintf(m_fDump, fmt, args);
    vsprintf(buf, fmt, args);
    va_end(args);
    int size = strlen(buf);
    fwrite(buf, size, 1, m_fDump);
    m_cBuffer += size;
    //if (m_cBuffer > 512)
    {
        fflush(m_fDump);
        m_cBuffer = 0;
    }
}

// CDataDumper
//==============================================================================

CDataDumper::CDataDumper()
{
    m_cDumped = 0;
}

void CDataDumper::Dump(const void* data, int size)
{
    if (m_fDump == NULL)
        return;
	if (!(m_nEnable && (m_nEnable & GetMask())))
		return;
    fwrite(data, size, 1, m_fDump);
    m_cDumped += size;
}


// CSimplePacketDumper
//==============================================================================

CSimplePacketDumper::CSimplePacketDumper()
{
    m_cPacket = 0;
}

void CSimplePacketDumper::Dump(const void* data, int size)
{
    if (m_fDump == NULL)
        return;
	if (!(m_nEnable && (m_nEnable & GetMask())))
		return;
    fwrite(&size, sizeof(size), 1, m_fDump);
    fwrite(data, size, 1, m_fDump);
    m_cDumped += sizeof(size) + size;
    m_cPacket++;
}

