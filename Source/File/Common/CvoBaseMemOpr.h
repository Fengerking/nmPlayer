#ifndef __CvoBaseMemOpr_H__
#define __CvoBaseMemOpr_H__

#include "voYYDef_filcmn.h"
#include "voMem.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CvoBaseMemOpr
{
public:
	CvoBaseMemOpr(VO_MEM_OPERATOR* pMemOp);
	virtual ~CvoBaseMemOpr();

protected:
	VO_PTR	MemAlloc(VO_U32 uSize);
	VO_VOID	MemFree(VO_PTR pBuff);
	VO_VOID	MemSet(VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize);
	VO_VOID	MemCopy(VO_PTR pDest, VO_PTR pSource, VO_U32 uSize);
	VO_U32	MemCheck(VO_PTR pBuffer, VO_U32 uSize);
	VO_S32	MemCompare(VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize);

protected:
	VO_MEM_OPERATOR*	m_pMemOp;
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CvoBaseMemOpr_H__
