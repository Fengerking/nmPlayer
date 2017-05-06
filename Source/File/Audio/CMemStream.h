#ifndef _CMemStream_H
#define _CMemStream_H

#include "voType.h"
#include "voLog.h"
#include "voSource2.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CMemStream
{
public:
	CMemStream();
	virtual ~CMemStream();
public:
	VO_U32		Init(VO_PBYTE pData ,VO_U64 uSize);
	VO_U32		Read(VO_PBYTE pOutBuf,VO_U64 uReadSize,VO_U64 * pActulReadSize);
	VO_U32		Skip(VO_S64 ullDelta);
	VO_U32		GetPosition(VO_U64 &ullCurPos);
	VO_U32		GetMemSize(VO_U64 &ullSize);
	VO_PBYTE	GetMem(){return m_pBuf;}
private:
	VO_PBYTE	m_pBuf;
	VO_U64		m_uBufLen;
	VO_PBYTE	m_pBufPointer;
};

const VO_U32 MIN_SIZE_MEMORY_BUFFER = 0X10000;///<set the min buffer size is 64k
const VO_U32 MAX_SIZE_MEMORY_BUFFER = 0X200000;///<set the max buffer size is 2M

class CMemFileIO
{
public:
	CMemFileIO();
	virtual ~CMemFileIO();
	VO_VOID		Reset(){m_uBufSize = 0;}
public:
	VO_U32		Open(VO_PBYTE pData ,VO_U64 ullSize);
	VO_U32		Read(VO_PBYTE pBuffer, VO_U64 uSize,VO_U64 * pActulReadSize = NULL);
	VO_U32		Write(VO_PBYTE pBuffer, VO_U64 uSize);
	VO_U32		Flush ();
	VO_U32		Seek (VO_S64 llDelta,VO_SOURCE2_IO_POS RelativePos = VO_SOURCE2_IO_POS_CURRENT);
	VO_U32		Size (VO_U64 &ullSize);
	VO_U32		Save ();
	VO_U32		Close ();
	VO_U32		Position(VO_U64& ullPos);
	VO_PBYTE	GetDirectMemory(){return m_Mem.GetMem();}
private:
	VO_PBYTE	m_pBufIO;
	VO_U64		m_uAllocBufSize;///<actully alloced buffer size 
	VO_U64		m_uBufSize;///<current m_pBufIO length
	
	CMemStream	m_Mem;
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CMemStream_H


