#pragma once

#include "CBaseStreamFileReader.h"
#include "voCSemaphore.h"


class CMTVReader : public CBaseStreamFileReader
{
public:
	CMTVReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CMTVReader(void);

public:
	virtual VO_VOID		FileGenerateIndex();
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();
	virtual VO_U32		GetFileSize(VO_S64* pllFileSize);
	virtual VO_U32		GetInfo(VO_SOURCE_INFO* pSourceInfo);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);

	virtual VO_U32		OnSetPos(VO_S64* pPos);

	virtual VO_VOID		OnData(VO_PTR pData){};
	virtual VO_VOID		OnBlock(VO_PTR pBlock){};

	static VO_U8		WhenDataParsed(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam);
	static VO_BOOL		WhenBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam);

protected:
	VO_BOOL IsCanGenerateIndex();


private:
	//voCSemaphore	m_StepEvent;
protected:
	//VO_BOOL				m_bSeeking;
};
