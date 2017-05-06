#include "CTempFileStream.h"

CTempFileStream::CTempFileStream(VO_FILE_OPERATOR* pFileOp , VO_PTCHAR tempdir, int blocksize)
: TempFilesStream(tempdir , blocksize)
, CvoBaseFileOpr(pFileOp)
{
}

CTempFileStream::~CTempFileStream(void)
{
}

bool CTempFileStream::Read(void* data, uint32 size)
{
	if (size != (uint32)_blockSize)
		return false;
	if (_currentNode == NULL)
		return false;

	VO_FILE_SOURCE fileSource;
	memset(&fileSource , 0 , sizeof(VO_FILE_SOURCE));
	fileSource .pSource = _currentNode->path; 
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = FileOpen(&fileSource);
	if (hFile == NULL)
		return false;

	VO_S32 nRC = FileRead(hFile , data , size);
	FileClose(hFile);

	if (nRC == (VO_S32)size )
	{
		FilePathNode* tmp = _currentNode;
		_currentNode = _currentNode->next;
		_current++;
		ReleaseNode(tmp);

		return true;
	}

	return false;
}

bool CTempFileStream::Write(const void* data, uint32 size)
{
	if (size != (uint32)_blockSize)
		return false;
	FilePathNode* node = AddNode();
	if (node == NULL)
		return false;

	VO_FILE_SOURCE fileSource;
	memset(&fileSource , 0 , sizeof(VO_FILE_SOURCE));
	fileSource .pSource = node->path; 
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nMode = VO_FILE_WRITE_ONLY;

	VO_PTR hFile = FileOpen(&fileSource);
	if (hFile == NULL)
		return false;

	VO_S32 nRC = FileWrite(hFile , (VO_PTR)data , size);
	FileClose(hFile);

	if (nRC == (VO_S32)size)
	{
		if (_currentNode)
			_currentNode->next = node;
		else
			_headNode = node;
		_currentNode = node;
		_current++;
		_total++;

		return true;
	}
	else
		ReleaseNode(node);

	return false;
}
