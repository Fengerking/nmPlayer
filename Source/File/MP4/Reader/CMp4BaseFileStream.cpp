#include "CMp4BaseFileStream.h"
#include "fMacros.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMp4BaseFileStream::CMp4BaseFileStream(VO_FILE_OPERATOR* pFileOp)
	: CvoBaseFileOpr(pFileOp)
	, m_hFile(VO_NULL)
	, m_lFilePos(0)
{
}

CMp4BaseFileStream::~CMp4BaseFileStream()
{
}


ABSPOS CMp4BaseFileStream::Position()
{
	return m_lFilePos;
}
