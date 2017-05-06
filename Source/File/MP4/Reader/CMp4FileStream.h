#ifndef __CMp4FileStream_H__
#define __CMp4FileStream_H__

#include "mpxio.h"
#include "CvoBaseObject.h"
#include "CvoBaseFileOpr.h"

#ifdef _SUPPORT_PUSHMODE
#include "CMp4BaseFileStream.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef _SUPPORT_PUSHMODE
class CMp4PullFileStream:public CMp4BaseFileStream
#else
class CMp4FileStream
	: public FileStream
	, public CvoBaseObject
	, public CvoBaseFileOpr
#endif
{
public:
#ifdef _SUPPORT_PUSHMODE
	CMp4BaseFileStream(VO_FILE_OPERATOR* pFileOp);
	virtual ~CMp4BaseFileStream();
#else
	CMp4FileStream(VO_FILE_OPERATOR* pFileOp);
	virtual ~CMp4FileStream();
#endif
public:
	VO_BOOL			Open(VO_FILE_SOURCE* pFileSource);
	VO_VOID			Close();

	//virtual functions
public:
	virtual	ABSPOS	Position();
	virtual	bool	SetPosition(ABSPOS lPosition);
	virtual	bool	Move(RELPOS lDelta);
	virtual bool	Read(void* pData, uint32 uSize);
	virtual	bool	Write(const void* pData, uint32 uSize);
	//virtual	long	Size();
	virtual	VO_U64	Size();
	virtual VO_PTR	GetFileHandle() {return m_hFile;}

protected:
	VO_PTR			m_hFile;
	ABSPOS			m_lFilePos;
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4FileStream_H__
