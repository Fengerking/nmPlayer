#include "CReserveSpace2.h"

CReserveSpace2::CReserveSpace2(VO_FILE_OPERATOR* pFileOp , int nSize)
: ReserveSpace(nSize)
, CvoBaseFileOpr(pFileOp)
{
	mvopFile = NULL;
}

CReserveSpace2::~CReserveSpace2(void)
{
}

bool CReserveSpace2::SetFile(VO_PTCHAR filename)
{
#ifndef _WIN32
	return true;
#endif  //_WIN32
	Release();

	VO_FILE_SOURCE fileSource;
	memset(&fileSource , 0 , sizeof(VO_FILE_SOURCE));
	fileSource .pSource = filename; 
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nMode = VO_FILE_WRITE_ONLY;

	mvopFile = FileOpen(&fileSource);
	if(mvopFile == NULL)
	{
		return false;
	}

	memset(mvoFileName , 0 , 260 * sizeof(VO_TCHAR));
	vostrcpy(mvoFileName , filename);

	return SetSize(_size);
}

bool CReserveSpace2::SetSize(int size)
{
#ifndef _WIN32
	return true;
#endif  //_WIN32

	if(mvopFile == NULL)
	{
		return false;
	}

	FileSeek(mvopFile , size , VO_FILE_BEGIN);
	return true;
}

void CReserveSpace2::Release()
{
#ifndef _WIN32
	return;
#endif  //_WIN32
	if(mvopFile == NULL)
		return ;

	FileClose(mvopFile);
	mvopFile = NULL;

#ifdef _WIN32
	DeleteFile(mvoFileName);
#elif defined _LINUX || defined LINUX
	remove(mvoFileName);
#endif

}