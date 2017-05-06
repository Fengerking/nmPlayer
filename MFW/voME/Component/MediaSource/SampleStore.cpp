	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

#include <stdio.h>
#include <string.h>

#include <voOMXOSFun.h>
#include <voOMXMemory.h>

#include "SampleStore.h"

namespace mtp{

CMemBlock::CMemBlock()
: m_pMemBlock(NULL)
, m_nBlockSize(0)
, m_readPointer(NULL)
, m_writePointer(NULL)
, m_bMemSufficient(false)
{
}

CMemBlock::~CMemBlock()
{
	Free();
}

bool CMemBlock::Alloc(int size)
{
	m_pMemBlock = (VO_BYTE *) voOMXMemAlloc(size);
	if(m_pMemBlock == NULL)
		return false;

	m_nBlockSize = size;
	m_readPointer = m_pMemBlock;
	m_writePointer = m_pMemBlock;
	m_bMemSufficient = true;
	return true;
}

void CMemBlock::Free()
{
	if(m_pMemBlock != NULL)
	{
		voOMXMemFree(m_pMemBlock);
		m_pMemBlock = NULL;
	}

	m_nBlockSize = 0;
	m_readPointer = NULL;
	m_writePointer = NULL;
	m_bMemSufficient = false;
}

int CMemBlock::Read(void * data, int size)
{
	if(AvailDataSize() < size)
		return 0;

	voOMXMemCopy(data, m_readPointer, size);
	m_readPointer += size;
	return size;
}

int CMemBlock::Write(void * data, int size)
{
	if(!m_bMemSufficient || AvailMemSize() < size)
	{
		m_bMemSufficient = true;
		return 0;
	}

	voOMXMemCopy(m_writePointer, data, size);
	m_writePointer += size;
	return size;
}

int CMemBlock::Peek(void * data, int size)
{
	if(AvailDataSize() < size)
		return 0;

	voOMXMemCopy(data, m_readPointer, size);
	return size;
}

int CMemBlock::Skip(int size)
{
	if(AvailDataSize() < size)
		return 0;

	m_readPointer += size;
	return size;
}

void CMemBlock::Clear()
{
	m_readPointer = m_pMemBlock;
	m_writePointer = m_pMemBlock;
	m_bMemSufficient = true;
}

int CMemBlock::AvailMemSize() 
{
	int nAvailMemSize = m_pMemBlock + m_nBlockSize - m_writePointer; 
	return nAvailMemSize; 
}

int CMemBlock::AvailDataSize() 
{ 
	int nAvailDataSize = m_writePointer - m_readPointer; 
	return nAvailDataSize; 
}


CSampleStore::CSampleStore(VO_SOURCE_TRACKTYPE sampleType)
: m_sampleType(sampleType)
, m_nSampleTotal(0)
, m_pKeySample(NULL)
{
	
}

CSampleStore::~CSampleStore()
{
	_List_MemBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		delete (*iter);
	}
	m_listMemBlock.clear();
}

VO_U32 CSampleStore::SinkSample(VO_SOURCE_SAMPLE * pSample)
{
	voCOMXAutoLock autoLock(&m_Mutex);

	do{
		CMemBlock * pMemBlock = NULL;

		_List_MemBlock_T::iterator iter;
		for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
		{
			if((*iter)->IsMemSufficient())
			{
				pMemBlock = *iter;
				break;
			}
		}

		if(pMemBlock == NULL)
		{
			pMemBlock = new CMemBlock();
			if(pMemBlock == NULL)
				break;

			if(!pMemBlock->Alloc(MEM_BLOCK_SIZE))
				break;

			m_listMemBlock.push_back(pMemBlock);
		}

		VO_U32 nSampleSize = (pSample->Size & 0X7FFFFFFF);
		if(pMemBlock->AvailMemSize() < (sizeof(VO_SOURCE_SAMPLE) + nSampleSize))
		{
			pMemBlock->SetMemDeficient();
			continue;
		}

		pMemBlock->Write(pSample, sizeof(VO_SOURCE_SAMPLE));
		pMemBlock->Write(pSample->Buffer, nSampleSize);

		++m_nSampleTotal;

		return VO_ERR_SOURCE_OK;

	}while(1);

	return VO_ERR_SOURCE_NEEDRETRY;
}

VO_U32 CSampleStore::GetSample(VO_SOURCE_SAMPLE * pSample)
{
	if(m_sampleType == VO_SOURCE_TT_AUDIO)
	{
		return GetAudioSample(pSample);
	}
	else if(m_sampleType == VO_SOURCE_TT_VIDEO)
	{
		return GetVideoSample(pSample);
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CSampleStore::FlushSample()
{
	voCOMXAutoLock autoLock(&m_Mutex);

	_List_MemBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		(*iter)->Clear();
	}

	m_nSampleTotal = 0;
	m_pKeySample = NULL;
	
	return VO_ERR_SOURCE_OK;
}

VO_U32 CSampleStore::GetAudioSample(VO_SOURCE_SAMPLE * pSample)
{
	voCOMXAutoLock autoLock(&m_Mutex);

	do{
		if(m_nSampleTotal == 0)
			break;

		CMemBlock * pMemBlock = m_listMemBlock.front();
		if(pMemBlock->AvailDataSize() == 0)
		{
			if(!pMemBlock->IsMemSufficient())
			{
				m_listMemBlock.pop_front();
				pMemBlock->Clear();
				m_listMemBlock.push_back(pMemBlock);
				continue;
			}
		}

		pMemBlock->Read(pSample, sizeof(VO_SOURCE_SAMPLE));
		pSample->Buffer = pMemBlock->ReadPointer();
		VO_U32 nSampleSize = (pSample->Size & 0X7FFFFFFF);
		pMemBlock->Skip(nSampleSize);

		--m_nSampleTotal;

		return VO_ERR_SOURCE_OK;

	}while(1);

	return VO_ERR_SOURCE_NEEDRETRY;
}

VO_U32 CSampleStore::GetVideoSample(VO_SOURCE_SAMPLE * pSample)
{
	voCOMXAutoLock autoLock(&m_Mutex);

	do{
		if(m_nSampleTotal == 0)
			break;

		if(m_pKeySample == NULL)
		{
			CheckKeySample(&m_pKeySample);
		}
		
		if(m_pKeySample != NULL)
		{
			if(m_pKeySample->Time <= pSample->Time)
			{
				GetKeySample(pSample);
				m_pKeySample = NULL;
				return VO_ERR_SOURCE_OK;
			}
		}

		CMemBlock * pMemBlock = m_listMemBlock.front();
		if(pMemBlock->AvailDataSize() == 0)
		{
			if(!pMemBlock->IsMemSufficient())
			{
				m_listMemBlock.pop_front();
				pMemBlock->Clear();
				m_listMemBlock.push_back(pMemBlock);
				continue;
			}
		}

		pMemBlock->Read(pSample, sizeof(VO_SOURCE_SAMPLE));
		pSample->Buffer = pMemBlock->ReadPointer();
		VO_U32 nSampleSize = (pSample->Size & 0X7FFFFFFF);
		pMemBlock->Skip(nSampleSize);

		--m_nSampleTotal;

		return VO_ERR_SOURCE_OK;

	}while(1);

	return VO_ERR_SOURCE_NEEDRETRY;
}

VO_U32 CSampleStore::CheckKeySample(VO_SOURCE_SAMPLE ** ppSample)
{
	*ppSample = NULL;

	_List_MemBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		CMemBlock * pMemBlock = *iter;
		VO_BYTE * readPointer = pMemBlock->ReadPointer();

		do
		{
			if(pMemBlock->AvailDataSize() < sizeof(VO_SOURCE_SAMPLE))
				break;

			VO_SOURCE_SAMPLE sample;
			pMemBlock->Peek(&sample, sizeof(VO_SOURCE_SAMPLE));
			if(sample.Size & 0x80000000) //key sample
			{
				*ppSample = (VO_SOURCE_SAMPLE *)pMemBlock->ReadPointer();
				break;
			}
		
			pMemBlock->Skip(sizeof(VO_SOURCE_SAMPLE));
			VO_U32 nSampleSize = (sample.Size & 0X7FFFFFFF);
			pMemBlock->Skip(nSampleSize);

		}while(1);

		pMemBlock->SetReadPointer(readPointer);

		if(*ppSample != NULL)
			return VO_ERR_SOURCE_OK;
	}

	return VO_ERR_SOURCE_NEEDRETRY;
}

VO_U32 CSampleStore::GetKeySample(VO_SOURCE_SAMPLE * pSample)
{
	do{
		CMemBlock * pMemBlock = m_listMemBlock.front();
		if(pMemBlock->AvailDataSize() == 0)
		{
			if(!pMemBlock->IsMemSufficient())
			{
				m_listMemBlock.pop_front();
				pMemBlock->Clear();
				m_listMemBlock.push_back(pMemBlock);
				continue;
			}
		}

		pMemBlock->Read(pSample, sizeof(VO_SOURCE_SAMPLE));
		pSample->Buffer = pMemBlock->ReadPointer();
		VO_U32 nSampleSize = (pSample->Size & 0X7FFFFFFF);
		pMemBlock->Skip(nSampleSize);

		--m_nSampleTotal;

		if(pSample->Size & 0x80000000) //key sample
			break;

	}while(1);

	return VO_ERR_SOURCE_OK;
}

} //mtp
