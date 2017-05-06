#pragma once

#include "mp4cfg.h"
#include "mp4desc.h"
#include "CvoBaseObject.h"
#include "CvoBaseFileOpr.h"

//#define _LOCAL_FILE_

class CMp4WriterStream
	: public FileStream
	, public CvoBaseObject
	, public CvoBaseFileOpr
{
public:
	CMp4WriterStream(VO_FILE_OPERATOR* pFileOp);
	virtual ~CMp4WriterStream();

public:
	VO_BOOL			Open(VO_FILE_SOURCE *pFileSource);
	VO_VOID			Close();

	//virtual functions
public:
	virtual	long	Position();
	virtual	bool	SetPosition(long lPosition);
	virtual	bool	Move(long lDelta);
	virtual bool	Read(void* pData, uint32 uSize);
	virtual	bool	Write(const void* pData, uint32 uSize);
	virtual	VO_U64	Size();

protected:
	VO_PTR			m_hFile;
	long			m_lFilePos;

#ifdef _LOCAL_FILE_
	FILE	*gFile;
#endif
};
