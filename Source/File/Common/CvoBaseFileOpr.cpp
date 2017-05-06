#include "voFile.h"
#include "CvoBaseFileOpr.h"
#include "cmnFile.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CvoBaseFileOpr::CvoBaseFileOpr(VO_FILE_OPERATOR* pFileOp)
	: m_pFileOp(pFileOp)
{
}

CvoBaseFileOpr::~CvoBaseFileOpr()
{
}

VO_PTR CvoBaseFileOpr::FileOpen(VO_FILE_SOURCE* pSource)
{
	VOLOGI("File handle from frame work m_pFileOp:%p*********************&&&&&&&&&&&&",m_pFileOp);
	return m_pFileOp ? m_pFileOp->Open(pSource) : cmnFileOpen(pSource);
}

VO_S32 CvoBaseFileOpr::FileRead(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	return m_pFileOp ? m_pFileOp->Read(pFile, pBuffer, uSize) : cmnFileRead(pFile, pBuffer, uSize);
}

VO_S32 CvoBaseFileOpr::FileWrite(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	return m_pFileOp ? m_pFileOp->Write(pFile, pBuffer, uSize) : cmnFileWrite(pFile, pBuffer, uSize);
}

VO_S32 CvoBaseFileOpr::FileFlush(VO_PTR pFile)
{
	return m_pFileOp ? m_pFileOp->Flush(pFile) : cmnFileFlush(pFile);
}

VO_S64 CvoBaseFileOpr::FileSeek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
	return m_pFileOp ? m_pFileOp->Seek(pFile, nPos, uFlag) : cmnFileSeek(pFile, nPos, uFlag);
}

VO_S64 CvoBaseFileOpr::FileSize(VO_PTR pFile)
{
	return m_pFileOp ? m_pFileOp->Size(pFile) : cmnFileSize(pFile);
}

VO_S64 CvoBaseFileOpr::FileSave(VO_PTR pFile)
{
	return m_pFileOp ? m_pFileOp->Save(pFile) : cmnFileSave(pFile);
}

VO_S32 CvoBaseFileOpr::FileClose(VO_PTR pFile)
{
	return m_pFileOp ? m_pFileOp->Close(pFile) : cmnFileClose(pFile);
}
