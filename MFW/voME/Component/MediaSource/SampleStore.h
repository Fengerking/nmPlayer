	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

#ifndef __SAMPLESTORE_H__
#define __SAMPLESTORE_H__

#include "voSource.h"
#include "list_T.h"

#include "voCOMXThreadMutex.h"

namespace mtp{

#define MEM_BLOCK_SIZE (1024*512)

class CMemBlock
{
public:
	CMemBlock();
	~CMemBlock();

public:
	bool Alloc(int size);
	void Free();

	int Read(void * data, int size);
	int Write(void * data, int size);
	int Peek(void * data, int size);
	int Skip(int size);

	void Clear();

public:
	int AvailMemSize();
	int AvailDataSize();

	VO_BYTE * ReadPointer() { return m_readPointer; }
	VO_BYTE * WritePointer() { return m_writePointer; }

	void SetReadPointer(VO_BYTE * readPointer) { m_readPointer = readPointer; }
	void SetWritePointer(VO_BYTE * writePointer) { m_writePointer = writePointer; }

	bool IsMemSufficient() { return m_bMemSufficient; }
	void SetMemDeficient() { m_bMemSufficient = false; }

public:
	VO_BYTE		* m_pMemBlock;
	int           m_nBlockSize;
	VO_BYTE		* m_readPointer;
	VO_BYTE		* m_writePointer;
	bool          m_bMemSufficient;
};


class CSampleStore
{
	typedef list_T<CMemBlock *, allocator<CMemBlock *, 10> > _List_MemBlock_T;

public:
	CSampleStore(VO_SOURCE_TRACKTYPE sampleType);
	~CSampleStore();

public:
	VO_U32 SinkSample(VO_SOURCE_SAMPLE * pSample);
	VO_U32 GetSample(VO_SOURCE_SAMPLE * pSample);
	VO_U32 FlushSample();

protected:
	VO_U32 GetAudioSample(VO_SOURCE_SAMPLE * pSample);
	VO_U32 GetVideoSample(VO_SOURCE_SAMPLE * pSample);

	VO_U32 CheckKeySample(VO_SOURCE_SAMPLE ** ppSample);
	VO_U32 GetKeySample(VO_SOURCE_SAMPLE * pSample);

protected:
	VO_SOURCE_TRACKTYPE    m_sampleType;

protected:
	voCOMXThreadMutex   m_Mutex;
	_List_MemBlock_T	m_listMemBlock;

protected:
	VO_U32    m_nSampleTotal;

	VO_SOURCE_SAMPLE	* m_pKeySample;
};

} //mtp

#endif //__SAMPLESTORE_H__