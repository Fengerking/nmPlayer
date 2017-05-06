#pragma once
#include "voYYDef_filcmn.h"
#include "CGFileChunk.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CGBuffer
	: public CvoBaseObject
	, public CvoBaseMemOpr
{
public:
	CGBuffer(VO_MEM_OPERATOR* pMemOp);
	virtual ~CGBuffer();

public:
	VO_BOOL		Init(VO_U32 dwBufferSize);
	VO_VOID		Release();

	//return buffer position, if VO_MAXU32, indicate fail!!
	VO_U32		Add(CGFileChunk* pFileChunk, VO_U32 dwLen);
	//only use it, not write data actually!!
	VO_U32		Use(VO_U32 dwLen);

	//get the internal buffer and write it directly
	//////////////////////////////////////////////////////////////////////////
	//return buffer position, will not fail!!
	VO_U32		GetForDirectWrite(VO_PBYTE* ppPtr, VO_U32 dwLen);
	VO_BOOL		DirectWrite(CGFileChunk* pFileChunk, VO_PBYTE pPtr, VO_U32 dwOffset, VO_U32 dwLen);
	//always return VO_TRUE
	VO_BOOL		DirectWrite2(VO_PBYTE pSource, VO_PBYTE pPtr, VO_U32 dwOffset, VO_U32 dwLen);
	//////////////////////////////////////////////////////////////////////////

	//return value:
	//1 - return buffer pointer to ppPtr
	//2 - return buffer content to pPtr
	VO_S32		Get(VO_PBYTE* ppPtr, VO_PBYTE pPtr, VO_U32 dwPos, VO_U32 dwLen);
	VO_S32		Peek(VO_PBYTE* ppPtr, VO_PBYTE pPtr, VO_U32 dwPos, VO_U32 dwLen);

	VO_BOOL		HasIdleBuffer(VO_U32 dwLen);

	VO_VOID		RemoveFrom(VO_U32 dwEnd);
	//added by yyang 20091214
	VO_VOID		RemoveTo(VO_U32 dwStart);
	VO_VOID		RemoveTo2(VO_U32 dwPos, VO_U32 dwLen);

	VO_U32		GetBufferSize(){ return m_dwSize; }

	VO_VOID		Flush();

protected:
	VO_PBYTE				m_pBuffer;
	VO_U32					m_dwSize;
	VO_U32					m_dwStart;
	VO_U32					m_dwEnd;
	VO_U32					m_dwIdle;
};

#ifdef _VONAMESPACE
}
#endif
