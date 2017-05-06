
#ifndef __CSourceIOBase_H__
#define __CSourceIOBase_H__

#include "voYYDef_SourceIO.h"
#include "voSource2_IO.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CSourceIOBase
{
public:
	CSourceIOBase(){m_nCurPos = 0;}
	virtual ~CSourceIOBase(){}
public:
	virtual VO_U32  Init(VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback)=0;
	virtual VO_U32	UnInit() = 0;
	virtual VO_U32	Open (VO_BOOL bIsAsyncOpen) = 0;
	virtual VO_U32 	Close () = 0;
	virtual VO_U32 	Read (VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pReadSize) = 0;
	virtual VO_U32 	Write (VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pWrittenSize) = 0;
	virtual VO_U32 	SetPos (VO_S64 ullPos , VO_SOURCE2_IO_POS RelativePos, VO_S64 *llActualPos) = 0;
	virtual VO_U32 	Flush () = 0;
	virtual VO_U32 	GetSize (VO_U64 * pSize) = 0;
	virtual VO_U32 	GetLastError() = 0;
	virtual VO_U32  GetParam(VO_U32 uParamID , VO_PTR pParam) = 0;
	virtual VO_U32	SetParam(VO_U32 uParamID , VO_PTR pParam) = 0;
	virtual VO_U32	GetSource(VO_PTR* pSource){return VO_SOURCE2_IO_OK;}
	virtual VO_U32	GetCurPos(VO_U64* llPos){return VO_SOURCE2_IO_OK;}
protected:
	VO_U64	m_nCurPos;
};
#ifdef _VONAMESPACE
}
#endif ///< _VONAMESPACE
#endif // __CSourceIOBase_H__
