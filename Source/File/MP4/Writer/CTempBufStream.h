#pragma once
#include "isomwr.h"

class CTempBufStream :
	public TempFilesStream
{
public:
	CTempBufStream(VO_PTCHAR tempdir, int blocksize);
	virtual ~CTempBufStream(void);

	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);
	virtual bool SetPosition(long pos);
protected:
	virtual void ReleaseNode(FilePathNode* node);
	virtual FilePathNode* AddNode();

};
