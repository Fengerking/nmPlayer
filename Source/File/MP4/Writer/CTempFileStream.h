#pragma once

#include "isomwr.h"
#include "CvoBaseFileOpr.h"
#include "CvoBaseObject.h"


class CTempFileStream
	: public TempFilesStream
	, public CvoBaseObject
	, public CvoBaseFileOpr
{
public:
	CTempFileStream(VO_FILE_OPERATOR* pFileOp , VO_PTCHAR tempdir, int blocksize);
	~CTempFileStream(void);

	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);
};
