#ifndef __CMp4PushFileStream_H__
#define __CMp4PushFileStream_H__

#include "mpxio.h"
#include "CvoBaseObject.h"
#include "CvoBaseFileOpr.h"
#include "CMp4BaseFileStream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMp4PushFileStream
	: public CMp4BaseFileStream
{
public:
	CMp4PushFileStream(VO_FILE_OPERATOR* pFileOp);
	virtual ~CMp4PushFileStream();

public:
	virtual VO_BOOL	Open(void* pBuffer, int64 nSize);
	virtual VO_BOOL	Open(VO_FILE_SOURCE* pFileSource);
	virtual VO_VOID			Close();

	//virtual functions
public:
	virtual	ABSPOS	Position();
	virtual	bool	SetPosition(ABSPOS lPosition);
	virtual	bool	Move(RELPOS lDelta);
	virtual bool	Read(void* pData, uint32 uSize);
	virtual	bool	Write(const void* pData, uint32 uSize);
	//virtual	long 	Size();
	virtual	VO_U64 	Size();

	virtual VO_PTR	GetFileHandle() {return m_hFile;}

protected:
	VO_PTR			m_hFile;
	ABSPOS			m_lFilePos;

	VO_CHAR*	m_pBuffer;
	int64   m_nSize;
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4PushFileStream_H__
