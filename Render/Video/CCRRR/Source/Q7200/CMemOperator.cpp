#include "CMemOperator.h"
#include "pmem_ids.h"
#include "pmem.h"
#include "myceddk.h"

DWORD CACHE_MASK = 0x0c;
DWORD CACHE_FLAG = 0x0c;

#define CACHE_SYNC_DISCARD      0x001   /* write back & discard all cached data */
#define CACHE_SYNC_INSTRUCTIONS 0x002   /* discard all cached instructions */
#define CACHE_SYNC_WRITEBACK    0x004   /* write back but don't discard data cache*/
#define CACHE_SYNC_FLUSH_I_TLB  0x008   /* flush I-TLB */
#define CACHE_SYNC_FLUSH_D_TLB  0x010   /* flush D-TLB */
#define CACHE_SYNC_FLUSH_TLB    (CACHE_SYNC_FLUSH_I_TLB|CACHE_SYNC_FLUSH_D_TLB)    /* flush all TLB */
#define CACHE_SYNC_L2_WRITEBACK 0x020   /* write-back L2 Cache */
#define CACHE_SYNC_L2_DISCARD   0x040   /* discard L2 Cache */


CMemOperator * CMemOperator::gpMemOperator = NULL;

CMemOperator::CMemOperator(void)
: mnArrayNum(6)
, mnBufferNum(0)
, mnCurIndex(0)
, mpMemArray(NULL)
, mpUVBufVir(NULL)
, mpUVBufPhy(NULL)
, mnUVBufSize(0)
{
	gpMemOperator = this;

	mpMemArray = new MemInfo[mnArrayNum];
	ZeroMemory(mpMemArray , mnArrayNum * sizeof(MemInfo));
}

CMemOperator::~CMemOperator(void)
{
	if(mpMemArray != NULL)
	{
		for(int i = 0 ; i < mnBufferNum ; i++)
		{
			if(mpMemArray[i].pVirtual != NULL)
			{
#ifdef _DEBUG
				printf("FREE  Address : %x \r\n" , (int )(mpMemArray[i].pVirtual));
#endif //_DEBUG
				//DeleteCriticalSection((&mpMemArray[i].csLock));
				pmem_free(mpMemArray[i].pVirtual);
				mpMemArray[i].pVirtual = NULL;
				mpMemArray[i].pPhysical = NULL;
			}
		}

		delete [] mpMemArray;
		mpMemArray = NULL;
	}

	if(mpUVBufVir != NULL)
	{
#ifdef _DEBUG
		printf("FREE Address : %x \r\n" , mpUVBufVir);
#endif //_DEBUG
		pmem_free(mpUVBufVir);
		mpUVBufVir = NULL;
	}
}

void *		CMemOperator::StaticAlloc(int size, uint32* pPhysAddr)
{
	return gpMemOperator->Alloc(size , (BYTE *)pPhysAddr);
}

void		CMemOperator::StaticFree(void *pBuf)
{
	return gpMemOperator->Free(pBuf);
}

void		CMemOperator::StaticLock (void* buffer)
{
	return gpMemOperator->Lock(buffer);
}

void		CMemOperator::StaticUnlock (void* buffer)
{
	return gpMemOperator->Unlock(buffer);
}

void *CMemOperator::Alloc(int size, BYTE * pPhysAddr)
{
	int nIndex = FindEmptyPos();
	if(nIndex == -1)
	{
		int     nArrayCount = mnArrayNum + 4;
		MemInfo *pTemp = new MemInfo[nArrayCount];

		memcpy(pTemp , mpMemArray , mnArrayNum * sizeof(MemInfo));

		delete [] mpMemArray;
		mpMemArray = pTemp;
		mnArrayNum = nArrayCount;

		nIndex = FindEmptyPos();
	}

	mnBufferNum ++;

	mpMemArray[nIndex].pVirtual = (BYTE*) pmem_malloc(size, PMEM_GRAPHICS_TEXTURE_ID);
	mpMemArray[nIndex].pPhysical = (BYTE*) PMEM_GET_PHYS_ADDR(mpMemArray[nIndex].pVirtual);
	pPhysAddr = mpMemArray[nIndex].pPhysical;

	VirtualSetAttributes(mpMemArray[nIndex].pVirtual , size , CACHE_FLAG , CACHE_MASK , NULL);

	mpMemArray[nIndex].nSize = size;


	//InitializeCriticalSection(&(mpMemArray[nIndex].csLock));

	if(mpUVBufVir == NULL)
	{
		mnUVBufSize = size / 2;
		mpUVBufVir = (BYTE*) pmem_malloc(mnUVBufSize , PMEM_GRAPHICS_TEXTURE_ID);
		mpUVBufPhy = (BYTE*) PMEM_GET_PHYS_ADDR(mpUVBufVir);
		if(mpUVBufVir != NULL)
			ZeroMemory(mpUVBufVir , mnUVBufSize);

#ifdef _DEBUG
		printf("Alloc Address : %x \r\n" , mpUVBufVir);
#endif //_DEBUG

		VirtualSetAttributes(mpUVBufVir , mnUVBufSize , CACHE_FLAG , CACHE_MASK ,  NULL);
	}
#ifdef _DEBUG
	printf("Alloc size : %d Address : %x \r\n" , size , (int )(mpMemArray[nIndex].pVirtual));
#endif //_DEBUG
	return mpMemArray[nIndex].pVirtual;

}

void CMemOperator::Free(void *pBuf)
{
	for(int i = 0 ; i < mnArrayNum ; i++)
	{
		if(pBuf >= mpMemArray[i].pVirtual && pBuf < (mpMemArray[i].pVirtual + mpMemArray[i].nSize))
		{
#ifdef _DEBUG
			printf("Free buffer Address : %x \r\n" , (int )(mpMemArray[i].pVirtual));
#endif //_DEBUG
			pmem_free(mpMemArray[i].pVirtual);
			mpMemArray[i].pVirtual = NULL;
			mpMemArray[i].pPhysical = NULL;

			mnBufferNum--;

			if(mnBufferNum == 0)
			{
				if(mpUVBufVir != NULL)
				{
					pmem_free(mpUVBufVir);
					mpUVBufVir = NULL;
				}
			}
		}
	}
}

void CMemOperator::Lock (void* buffer)
{
	//int nIndex = FindPos((BYTE *)buffer);
	//if(nIndex != -1)
	//{
	//	EnterCriticalSection(&(mpMemArray[nIndex].csLock));
	//}
}

void CMemOperator::Unlock (void* buffer)
{
	//int nIndex = FindPos((BYTE *)buffer);
	//if(nIndex != -1)
	//{
	//	LeaveCriticalSection(&(mpMemArray[nIndex].csLock));
	//}
}

BYTE* CMemOperator::Find(BYTE *pBuf)
{
	int nIndex = FindPos(pBuf);
	if(nIndex != -1)
		return mpMemArray[nIndex].pVirtual;

	return NULL;
}

BYTE *	CMemOperator::GetPhyAddress(BYTE *pBuf)
{
	int nIndex = FindPos(pBuf);
	if(nIndex != -1)
	{
		int nOffset = pBuf - mpMemArray[nIndex].pVirtual;
		return mpMemArray[nIndex].pPhysical + nOffset;
	}

	return NULL;
}

int		CMemOperator::FindPos(BYTE *pBuf)
{
	for(int i = 0 ; i < mnBufferNum ; i++)
	{
		if(pBuf >= mpMemArray[i].pVirtual && pBuf < (mpMemArray[i].pVirtual + mpMemArray[i].nSize))
		{
#ifdef _DEBUG
//			printf("Find memory index : %d \r\n" , i);
#endif
			mnCurIndex = i;
			return i;
		}
	}

	return -1;
}

int		CMemOperator::FindEmptyPos()
{
	for (int i = 0 ; i < mnArrayNum ; i++)
	{
		if(mpMemArray[i].pVirtual == NULL)
			return i;
	}

	return -1;
}

BYTE *	CMemOperator::AllocMem(int size , BYTE **ppPhyAddr)
{
	BYTE *pVir = (BYTE*) pmem_malloc(size  , PMEM_GRAPHICS_TEXTURE_ID);
	*ppPhyAddr = (BYTE*) PMEM_GET_PHYS_ADDR(pVir);
	if(pVir != NULL)
		ZeroMemory(pVir , size );

#ifdef _DEBUG
	printf("AllocMem Address : %x \r\n" , pVir);
#endif //_DEBUG

	return pVir;
}

void	CMemOperator::FreeMem(BYTE *pVir)
{
#ifdef _DEBUG
	printf("FreeMem Address : %x \r\n" , pVir);
#endif //_DEBUG
	pmem_free(pVir);
}

void	CMemOperator::ReflushCacheBuf()
{
	CacheRangeFlush(mpMemArray[mnCurIndex].pVirtual , mpMemArray[mnCurIndex].nSize , CACHE_SYNC_WRITEBACK );
	CacheRangeFlush(mpUVBufVir , mnUVBufSize , CACHE_SYNC_WRITEBACK);
}
