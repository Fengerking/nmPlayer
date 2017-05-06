#include "CTempBufStream.h"

#define LOG_TAG "CTempBufStream"
#include "voLog.h"

CTempBufStream::CTempBufStream(VO_PTCHAR tempdir, int blocksize)
: TempFilesStream(tempdir , blocksize)
{
}

CTempBufStream::~CTempBufStream(void)
{
}

bool CTempBufStream::Read(void* data, uint32 size)
{
	
	if (size != (uint32)_blockSize)
		return false;
	if (_currentNode == NULL)
		return false;

	memcpy(data , _currentNode->pBuf , size);

	FilePathNode* tmp = _currentNode;
	_currentNode = _currentNode->next;
	_current++;
	ReleaseNode(tmp);

	return true;
}

bool CTempBufStream::Write(const void* data, uint32 size)
{
	if (size != (uint32)_blockSize)
		return false;
	FilePathNode* node = AddNode();
	if (node == NULL)
		return false;

	node->pBuf = new VO_BYTE[size];
	if(node->pBuf == NULL)
		return false;

	memcpy(node->pBuf , data , size);

	if (_currentNode)
		_currentNode->next = node;
	else
		_headNode = node;
	_currentNode = node;
	_current++;
	_total++;
	
	return true;
}

void CTempBufStream::ReleaseNode(FilePathNode* node)
{
	if(node->pBuf != NULL)
	{
		delete [] node->pBuf;
		node->pBuf = NULL;
	}

	delete node;
	node = NULL;
}

FilePathNode* CTempBufStream::AddNode()
{
	FilePathNode* node = new FilePathNode();
	node->next = NULL;
	node->pBuf = NULL;
	
	return node;
}

bool CTempBufStream::SetPosition(long pos)
{
	if (pos)
		return false;
	_current = 0;
	_currentNode = _headNode;
	return true;
}
