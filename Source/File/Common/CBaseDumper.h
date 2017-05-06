#pragma once
#include "voYYDef_filcmn.h"
#include "voSink.h"
#include "CvoBaseFileOpr.h"
#include "CvoBaseMemOpr.h"
#include "CMemPool.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CBaseDumper : public CvoBaseFileOpr, public CvoBaseMemOpr
{
public:
	CBaseDumper(VO_FILE_OPERATOR* pFileOp);
	virtual ~CBaseDumper(void);

public:
	virtual VO_U32 Open(VO_FILE_SOURCE* pFileSource);
	virtual VO_U32 Close();
	virtual VO_U32 Write(VO_BYTE* pData, VO_U32 nLen);
	virtual VO_U32 Flush();

private:
	VO_PTR	AllocChunkMemory();

private:
	VO_PTR		m_hFile;
	
	CMemPool	m_MemPool;
	VO_U32		m_nChunkSize;
	VO_U32		m_nCurrPos;
	VO_BYTE*	m_pChunk;
};

#ifdef _VONAMESPACE
}
#endif
