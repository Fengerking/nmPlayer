#include "voBufferIO.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VO_PTR VO_API BufferIOOpen (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	int i = 0;
	i = i;

	BufferIO * pBufferOP = new BufferIO ();
	if (pBufferOP == NULL)
		return NULL;

	VO_PTR pFile = pBufferOP->Open (pSource);
	if (pFile == NULL)
	{
		delete pBufferOP;
		pBufferOP = NULL;
	}

	return pBufferOP;
}

VO_S32 VO_API BufferIORead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;
	
	int i = 0;
	i = i;
	BufferIO * pBufferOP = (BufferIO *)pHandle;

	return pBufferOP->Read (pBuffer, uSize);
}

VO_S32 VO_API BufferIOWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	int i = 0;
	i = i;

	BufferIO * pBufferOP = (BufferIO *)pHandle;

	return pBufferOP->Write (pBuffer, uSize);
}

VO_S32 VO_API BufferIOFlush (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	int i = 0;
	i = i;
	BufferIO * pBufferOP = (BufferIO *)pHandle;

	return pBufferOP->Flush ();
}

VO_S64 VO_API BufferIOSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag)
{
	if (pHandle == 0)
		return 0;

	int i = 0;
	i = i;
	BufferIO * pBufferOP = (BufferIO *)pHandle;

	return pBufferOP->Seek (nPos, uFlag);
}

VO_S64 VO_API BufferIOSize (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	int i = 0;
	i = i;
	BufferIO * pBufferOP = (BufferIO *)pHandle;

	return pBufferOP->Size ();
}

VO_S64 VO_API BufferIOSave (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	int i = 0;
	i = i;

	BufferIO * pBufferOP = (BufferIO *)pHandle;

	return pBufferOP->Save ();
}

VO_S32 VO_API BufferIOClose (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	int i = 0;
	i = i;

	BufferIO * pBufferOP = (BufferIO *)pHandle;

	pBufferOP->Close ();
	delete pBufferOP;

	return 0;
}


#if defined __cplusplus
extern "C" {
#endif

	VO_S32 VO_API voGetBufferFileOperatorAPI (VO_FILE_OPERATOR * pFileOperator, VO_U32 uFlag)
	{
		pFileOperator->Open = BufferIOOpen;
		pFileOperator->Read = BufferIORead;
		pFileOperator->Write = BufferIOWrite;
		pFileOperator->Flush = BufferIOFlush;
		pFileOperator->Seek = BufferIOSeek;
		pFileOperator->Size = BufferIOSize;
		pFileOperator->Save = BufferIOSave;
		pFileOperator->Close = BufferIOClose;
		return VO_ERR_NONE;
	}

#if defined __cplusplus
}
#endif

BufferIO::BufferIO()
:m_lFilePos(0)
{
	
}
BufferIO::~BufferIO()
{

}
VO_PTR	BufferIO::Open (VO_FILE_SOURCE * pSource)
{
	m_nSize = pSource->nLength;
	return m_pBuffer = (VO_CHAR*) pSource->pSource;
}
VO_S32 	BufferIO::Read (VO_PTR pBuffer, VO_U32 uSize)
{
	if(!m_pBuffer)
		return false;
	if(m_lFilePos + uSize > m_nSize)
	{
		uSize = m_nSize - m_lFilePos;
	}
	VO_CHAR* buffer = m_pBuffer + m_lFilePos;
	memcpy(pBuffer, buffer, uSize);
	m_lFilePos += uSize;

	return uSize;
}
VO_S32 	BufferIO::Write (VO_PTR pBuffer, VO_U32 uSize)
{
	return 0;
}
VO_S32 	BufferIO::Flush (void)
{
	return 0;
}
VO_S64 	BufferIO::Seek (VO_S64 nPos, VO_FILE_POS uFlag)
{
	if( nPos > m_nSize )
		return -1;
	m_lFilePos = nPos;
	return m_lFilePos;
}
VO_S64 	BufferIO::Size (void)
{
	return long((!m_pBuffer) ? -1 : m_nSize);
}
VO_S64 	BufferIO::Save (void)
{
	return 0;
}
VO_S32 	BufferIO::Close (void)
{
	m_pBuffer = 0;
	m_nSize = m_lFilePos = 0;
	return 0;
}

