#ifndef __CvoBaseFileOpr_H__
#define __CvoBaseFileOpr_H__

#include "voYYDef_filcmn.h"
#include "voFile.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CvoBaseFileOpr
{
public:
	CvoBaseFileOpr(VO_FILE_OPERATOR* pFileOp);
	virtual ~CvoBaseFileOpr();

protected:
	VO_PTR	FileOpen(VO_FILE_SOURCE* pSource);
	VO_S32	FileRead(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);
	VO_S32	FileWrite(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);
	VO_S32	FileFlush(VO_PTR pFile);
	VO_S64	FileSeek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);
	VO_S64	FileSize(VO_PTR pFile);
	VO_S64	FileSave(VO_PTR pFile);
	VO_S32	FileClose(VO_PTR pFile);

protected:
	VO_FILE_OPERATOR*	m_pFileOp;
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CvoBaseFileOpr_H__
