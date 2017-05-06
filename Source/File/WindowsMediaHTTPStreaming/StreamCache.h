#ifndef __CStreamCache_H__
#define __CStreamCache_H__

#include "voCSemaphore.h"
#include "voCMutex.h"
#include "voFile.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CStreamCache
{
public:
	CStreamCache(void);
	~CStreamCache(void);

	VO_VOID start();
	VO_VOID pause();
	VO_VOID close();

	VO_S32 write(const VO_BYTE *, VO_U32);
	VO_S64 seek(VO_S64 , VO_FILE_POS);
	VO_S32 read(VO_BYTE *, VO_U32);
	VO_S32 peek(VO_BYTE *, VO_U32);

	VO_S64 FileSize();
	VO_VOID setFileSize(VO_S64 llSize);
	VO_S32 setFileEnd();

	VO_S32 freeBufSize() { return iSizeBuf  - iSizeCaching - static_cast<VO_S32>(m_llPosEnd - m_llPosBegin); }
	VO_S64 WrittenSize() { return m_llPosEnd; }

protected:
	VO_S32 dowrite(const VO_BYTE *, VO_U32);

	VO_S32 dopeek(VO_BYTE *, VO_U32);
	VO_S32 doread(VO_BYTE *, VO_U32);

	//VO_VOID LogFreedRange(VO_U32);

private:
	enum STATE_Cache {
		Cache_Stop,		//close, need return
		Cache_Running,	//nomal
		Cache_End		
	};

	STATE_Cache m_StateCache;

	const VO_S32 iSizeBuf;
	const VO_S32 iSizeCaching;
	
	VO_BYTE *m_pBuf;

	volatile VO_S64 m_llFileSize;	//Finished: read no block; ING: block

	volatile VO_S64 m_llPosBegin;	//for reading thread; all data before it been read
	volatile VO_S64 m_llPosEnd;		//for writing thread

	VO_S64 m_llPosCur;

	voCSemaphore m_EventRead;
	voCSemaphore m_EventWrite;

	voCMutex		m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __CStreamCache_H__