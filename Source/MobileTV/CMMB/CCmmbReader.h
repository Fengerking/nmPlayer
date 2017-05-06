#pragma once
#include "voParser.h"

#include "CMTVReader.h"
#include "CCmmbFileDataParser.h"

class CCmmbReader : public CMTVReader
{
public:
	CCmmbReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CCmmbReader(void);

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetInfo(VO_SOURCE_INFO* pSourceInfo);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);

	virtual VO_VOID		OnData(VO_PTR pData);
	virtual VO_VOID		OnBlock(VO_PTR pBlock);


protected:
	CCmmbFileDataParser	m_FileDataParser;
};
