#pragma once

#include "voYYDef_filcmn.h"
#include "CvoFileParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CvoStreamFileDataParser : public CvoFileDataParser
{
public:
	CvoStreamFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CvoStreamFileDataParser(void);

public:
	VO_VOID	SetFileSize(VO_U64 nSize){m_nFileSize = nSize;};

	//
	virtual VO_BOOL	GetFileInfo(VO_SOURCE_INFO* pSourceInfo)=0;
	virtual	VO_S64	GetFirstTimeStamp()=0;
	virtual	VO_S64	Reset()=0;

protected:
	virtual VO_U64	CheckTimestamp(VO_U64 time)=0;

protected:
	VO_U64	m_nFileSize;
};

#ifdef _VONAMESPACE
}
#endif
