#pragma once

#include "isomwr.h"
#include "CvoBaseFileOpr.h"
#include "CvoBaseObject.h"

class CReserveSpace2 
	: public ReserveSpace
	, public CvoBaseObject
	, public CvoBaseFileOpr
{
public:
	CReserveSpace2(VO_FILE_OPERATOR* pFileOp , int nSize);
	~CReserveSpace2(void);

	virtual bool SetFile(VO_PTCHAR filename);
	virtual bool SetSize(int size);
	virtual void Release();

protected:
	VO_PTR		mvopFile;
	VO_TCHAR	mvoFileName[260];
};
