#pragma once
#include "voCSemaphore.h"
#include "voCMutex.h"
#include "voFile.h"


struct FreedRange{
	FreedRange(void) : iBegin(0), iLen(0), pNextFreedRange(NULL) {}

	~FreedRange(void) {
		if (pNextFreedRange)
			delete pNextFreedRange;
	};

	VO_S32 iBegin;
	VO_S32 iLen;

	FreedRange * pNextFreedRange;
};

class CStreamCache
{
public:
	CStreamCache(void);
	~CStreamCache(void);

	VO_VOID open();

	VO_S32 write(const VO_BYTE *, VO_U32);
	VO_S64 seek(VO_S64 , VO_FILE_POS);
	VO_S32 read(VO_BYTE *, VO_U32);
	VO_S32 peek(VO_BYTE *, VO_U32);

	VO_VOID close();

	VO_S64 FileSize();
	VO_VOID setFileSize(VO_S64 llSize);
	VO_S32 setFileEnd();
protected:
	VO_S32 dowrite(const VO_BYTE *, VO_U32);
	VO_S32 writeBuf(const VO_BYTE *, VO_U32);

	VO_S32 doread(VO_BYTE *, VO_U32);
	VO_S32 readBuf(VO_BYTE *, VO_U32);
	VO_VOID LogFreedRange(VO_U32);

	VO_S32 dopeek(VO_BYTE *, VO_U32);
	VO_S32 peekBuf(VO_BYTE *, VO_U32);
private:
	VO_BOOL m_bRunning;		//False: close

	VO_S64 m_llFileSize;	//Finished: read no block; ING: block

	VO_S64 m_llPosBegin;	//llReaded	llUsed
	VO_S64 m_llPosEnd;		//llWritten	llUseable

	VO_S64 m_llPosCur;

	VO_BYTE *m_pBuf;

	FreedRange *m_pFreedRanges;

	voCSemaphore	m_event;
	voCMutex		m_lock;
};

