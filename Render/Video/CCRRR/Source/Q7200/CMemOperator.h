#pragma once
#include "windows.h"
#include "sdkbase.h"

typedef struct _MemInfo
{
	BYTE *pVirtual;
	BYTE *pPhysical;
	int  nSize;
	CRITICAL_SECTION csLock;
}MemInfo;

class CMemOperator
{
public:
	CMemOperator(void);
	virtual ~CMemOperator(void);

	static CMemOperator *gpMemOperator;

	static void *	StaticAlloc(int size, uint32* pPhysAddr);
	static void		StaticFree(void *pBuf);
	static void		StaticLock (void* buffer);
	static void		StaticUnlock (void* buffer);

	void *	Alloc(int size, BYTE* pPhysAddr);
	void	Free(void *pBuf);
	void	Lock (void* buffer);
	void	Unlock (void* buffer);

	BYTE *	AllocMem(int size , BYTE **ppPhyAddr);
	void	FreeMem(BYTE *pVir);

	BYTE *	Find(BYTE *pBuf);
	BYTE *	GetPhyAddress(BYTE *pBuf);

	BYTE *	GetUVBufVir(){ return mpUVBufVir; }
	BYTE *	GetUVBufPhy(){ return mpUVBufPhy; }

	void	ReflushCacheBuf();
protected:
	int		FindPos(BYTE *pBuf);
	int		FindEmptyPos();
protected:
	int  mnArrayNum;
	int  mnBufferNum;
	int  mnCurIndex;

	MemInfo *mpMemArray;

	BYTE *mpUVBufVir;
	BYTE *mpUVBufPhy;
	int	  mnUVBufSize;

};
