#ifndef __CMp4BaseFileStream_H__
#define __CMp4BaseFileStream_H__

#include "mpxio.h"
#include "CvoBaseObject.h"
#include "CvoBaseFileOpr.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMp4BaseFileStream
	: public FileStream
	, public CvoBaseObject
	, public CvoBaseFileOpr
{
public:
	CMp4BaseFileStream(VO_FILE_OPERATOR* pFileOp);
	virtual ~CMp4BaseFileStream();

public:
	virtual VO_BOOL	Open(VO_FILE_SOURCE* pFileSource) = 0;
	virtual VO_BOOL	Open(VO_VOID* pBuffer, int64 nSize) = 0;
	virtual VO_VOID	Close() = 0;

	//virtual functions
public:
	virtual	ABSPOS	Position();
	virtual	bool	SetPosition(ABSPOS lPosition) = 0;
	virtual	bool	Move(RELPOS lDelta) = 0;
	virtual bool	Read(void* pData, uint32 uSize) = 0;
	virtual	bool	Write(const void* pData, uint32 uSize) = 0;
	//virtual	long	 Size() = 0;
	virtual	VO_U64	 Size() = 0;

	virtual VO_PTR	GetFileHandle() {return m_hFile;}

protected:
	VO_PTR			m_hFile;
	ABSPOS			m_lFilePos;
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4BaseFileStream_H__
