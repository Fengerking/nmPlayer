#ifndef _DEBUG_DUMPER_H_
#define _DEBUG_DUMPER_H_

#include <stdio.h>

extern unsigned int _AppStartTick;

#ifdef WIN32
#include <windows.h>
#else //WIN32
extern unsigned int GetTickCount();
#endif //WIN32

inline unsigned int GetAppTick()
{
    return GetTickCount() - _AppStartTick;
}

class CDumper
{
public:
	CDumper();
	~CDumper();

	static bool SetFolder(const char* path);
	static void SetMask(int mask);
	static int GetMask() { return m_nMask; }

	bool Open(const char* filename, int enable_flag);
	void Close();

protected:
	static char m_szFolder[256];
	static int m_nMask;
	FILE* m_fDump;
	int m_nEnable;
};

class CDataDumper : public CDumper
{
public:
    CDataDumper();
	void Dump(const void* data, int size);

protected:
    int m_cDumped;
};

class CTextDumper : public CDumper
{
public:
    CTextDumper();
    void Dump(const char* fmt, ...);

private:
    int m_cBuffer;
};


class CSimplePacketDumper : public CDataDumper
{
public:
    CSimplePacketDumper();
	void Dump(const void* data, int size);

protected:
    int m_cPacket;
};



#endif //_DEBUG_DUMPER_H_

