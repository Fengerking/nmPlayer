#include "voFile.h"
#include "CvoBaseMemOpr.h"
#include "cmnMemory.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _IOS
static VO_U32 g_dwFRModuleID = 0;
#else
extern VO_U32 g_dwFRModuleID;
#endif

CvoBaseMemOpr::CvoBaseMemOpr(VO_MEM_OPERATOR* pMemOp)
	: m_pMemOp(pMemOp)
{
}

CvoBaseMemOpr::~CvoBaseMemOpr()
{
}

VO_PTR CvoBaseMemOpr::MemAlloc(VO_U32 uSize)
{
	VO_MEM_INFO vmi = {0};
	vmi.Size = uSize;
	m_pMemOp ? m_pMemOp->Alloc(g_dwFRModuleID, &vmi) : 
		cmnMemAlloc(g_dwFRModuleID, &vmi);

	return vmi.VBuffer;
}

VO_VOID CvoBaseMemOpr::MemFree(VO_PTR pBuff)
{
	m_pMemOp ? m_pMemOp->Free(g_dwFRModuleID, pBuff) : 
		cmnMemFree(g_dwFRModuleID, pBuff);
}

VO_VOID CvoBaseMemOpr::MemSet(VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize)
{
	m_pMemOp ? m_pMemOp->Set(g_dwFRModuleID, pBuff, uValue, uSize) : 
		cmnMemSet(g_dwFRModuleID, pBuff, uValue, uSize);
}

VO_VOID CvoBaseMemOpr::MemCopy(VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	m_pMemOp ? m_pMemOp->Copy(g_dwFRModuleID, pDest, pSource, uSize) : 
		cmnMemCopy(g_dwFRModuleID, pDest, pSource, uSize);
}

VO_U32 CvoBaseMemOpr::MemCheck(VO_PTR pBuffer, VO_U32 uSize)
{
	return m_pMemOp ? m_pMemOp->Check(g_dwFRModuleID, pBuffer, uSize) : 
		cmnMemCheck(g_dwFRModuleID, pBuffer, uSize);
}

VO_S32 CvoBaseMemOpr::MemCompare(VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize)
{
	return m_pMemOp ? m_pMemOp->Compare(g_dwFRModuleID, pBuffer1, pBuffer2, uSize) : 
		cmnMemCompare(g_dwFRModuleID, pBuffer1, pBuffer2, uSize);
}
