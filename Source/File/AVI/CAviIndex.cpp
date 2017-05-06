/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		CAviIndex.cpp

Contains:	The wrapper for avi index 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#include "CAviIndex.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define AVI_INDEX_INDX_MAX_COUNT		0x2000		//8K
#define AVI_INDEX_IDX1_MAX_COUNT		0x1000		//4K

#define read_avi_object(obj)	ptr_read_pointer(&obj, sizeof(AviObject))

//Base Class
CAviBaseIndex::CAviBaseIndex(VOAVIINDEXTYPE Type, CGFileChunk* pFileChunk, VO_U32 dwReadCountOnce, VO_MEM_OPERATOR* pMemOp)
	: CvoFileBaseParser(pFileChunk, pMemOp)
	, m_Type(Type)
	, m_dwCount(0)
	, m_dwReadCountOnce(dwReadCountOnce)
	, m_pEntries(VO_NULL)
	, m_dwEntryStart(0)
	, m_dwEntryCount(0)
{
}

CAviBaseIndex::~CAviBaseIndex()
{
	SAFE_MEM_FREE(m_pEntries);
}

VO_BOOL CAviBaseIndex::GetEntryByIndex(VO_U32 dwIndex, VO_U64* pullPosInFile, VO_U32* pdwSize, VO_U64* pullDdPosInFile /* = VO_NULL */, VO_U32* pdwDdSize /* = VO_NULL */)
{
	if(dwIndex >= m_dwCount)
		return VO_FALSE;

	if(m_dwEntryStart > dwIndex || m_dwEntryStart + m_dwEntryCount <= dwIndex)
	{
		m_dwEntryStart = dwIndex / m_dwReadCountOnce * m_dwReadCountOnce;
		m_dwEntryCount = ReadIndexFromFileB(m_dwEntryStart, m_dwReadCountOnce);
		if(!m_dwEntryCount)
			return VO_FALSE;
	}

	PAviStandardIndexEntry pEntry = m_pEntries + (dwIndex - m_dwEntryStart);

	if(pullPosInFile)
		*pullPosInFile = pEntry->dwOffset + GetStartPos(dwIndex);
	if(pdwSize)
		*pdwSize = pEntry->dwSize;

	return VO_TRUE;
}

VO_BOOL CAviBaseIndex::GetEntryByTotalSize(VO_U32& dwTotalSize, VO_U32* pdwIndex)
{
	VO_U32 dwAudioTotalSize = 0, dwSize = 0;
	for(VO_U32 i = 0; i < m_dwCount; i++)
	{
		if(!GetEntryByIndex(i, VO_NULL, &dwSize))
			return VO_FALSE;

		dwAudioTotalSize += GET_SIZE(dwSize);
		if(dwAudioTotalSize >= dwTotalSize)
		{
			dwTotalSize = dwAudioTotalSize - GET_SIZE(dwSize);

			if(pdwIndex)
				*pdwIndex = i;

			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

VO_U32 CAviBaseIndex::GetEntryPrevKeyFrame(VO_U32 dwIndex)
{
	VO_U32 dwSize = 0;
	for(VO_U32 i = dwIndex; i >= 0; i--)
	{
		if(!GetEntryByIndex(i, VO_NULL, &dwSize))
			return VO_MAXU32;

		if(IS_KEY_FRAME(dwSize))
			return i;
	}

	return VO_MAXU32;
}

VO_U32 CAviBaseIndex::GetEntryNextKeyFrame(VO_U32 dwIndex)
{
	VO_U32 dwSize = 0;
	for(VO_U32 i = dwIndex + 1; i < m_dwCount; i++)
	{
		if(!GetEntryByIndex(i, VO_NULL, &dwSize))
			return VO_MAXU32;

		if(IS_KEY_FRAME(dwSize))
			return i;
	}

	return VO_MAXU32;
}

VO_BOOL CAviBaseIndex::IsChunkSizeEquilong()
{
	for (VO_U32 i = 1; i < m_dwEntryCount && i < m_dwCount; i++)
	{
		if ( (m_pEntries[i - 1].dwSize > m_pEntries[i].dwSize ?  m_pEntries[i - 1].dwSize - m_pEntries[i].dwSize : m_pEntries[i].dwSize - m_pEntries[i - 1].dwSize) > 1 )
			return VO_FALSE;
	}

	return VO_TRUE;
}

//AVI_INDEX_INDX
CAviIndxIndex::CAviIndxIndex(CGFileChunk* pFileChunk, VO_U32 dwIndexPoses, VO_U64* pullFilePos, VO_MEM_OPERATOR* pMemOp)
	: CAviBaseIndex(AVI_INDEX_INDX, pFileChunk, AVI_INDEX_INDX_MAX_COUNT, pMemOp)
	, m_dwIndexPoses(dwIndexPoses)
	, m_pEntryBlocks(VO_NULL)
	, m_dwMaxSampleSize(0)
	, m_ullTotalSize(0)
{
	m_pEntryBlocks = NEW_OBJS(AviMediaSampleIndexBlock, m_dwIndexPoses);
	for(VO_U32 i = 0; i < m_dwIndexPoses; i++)
	{
		m_pEntryBlocks[i].dwPrevEntries = 0;
		m_pEntryBlocks[i].dwEntries = 0;
		m_pEntryBlocks[i].ullIndexFilePos = pullFilePos[i];
		m_pEntryBlocks[i].ullStartPosInFile = 0;
	}
}

CAviIndxIndex::~CAviIndxIndex()
{
	SAFE_MEM_FREE(m_pEntryBlocks);
}

VO_BOOL CAviIndxIndex::Init()
{
	m_dwMaxSampleSize = 0;
	m_ullTotalSize = 0;
	m_dwCount = 0;

	for(VO_U32 i = 0; i < m_dwIndexPoses; i++)
	{
		if(!m_pFileChunk->FLocate(m_pEntryBlocks[i].ullIndexFilePos))
			return VO_FALSE;

		AviObject ao;
		read_avi_object(ao);

		if(TWOCC_ix != (ao.fcc & 0xFFFF))
			return VO_FALSE;

		//AVI Standard Index Chunk
		//skip wLongsPerEntry, bIndexSubType, bIndexType
		ptr_skip(4);
		ptr_read_dword(m_pEntryBlocks[i].dwEntries);
		//skip dwChunkId
		ptr_skip(4);
		ptr_read_qword(m_pEntryBlocks[i].ullStartPosInFile);
		//skip dwReserved
		ptr_skip(4);

		m_pEntryBlocks[i].ullIndexFilePos = m_pFileChunk->FGetFilePos();
		if(i > 0)
			m_pEntryBlocks[i].dwPrevEntries = m_pEntryBlocks[i - 1].dwPrevEntries + m_pEntryBlocks[i - 1].dwEntries;
		else
			m_pEntryBlocks[i].dwPrevEntries = 0;

		m_dwCount += m_pEntryBlocks[i].dwEntries;
	}

	if(m_dwCount < m_dwReadCountOnce)
		m_dwReadCountOnce = m_dwCount;

	m_pEntries = NEW_OBJS(AviStandardIndexEntry, m_dwReadCountOnce);
	if(!m_pEntries)
		return VO_FALSE;

	//scan!!
	PAviStandardIndexEntry pEntry = m_pEntries;
	for(VO_U32 i = 0; i < m_dwIndexPoses; i++)
	{
		if(!m_pFileChunk->FLocate(m_pEntryBlocks[i].ullIndexFilePos))
			return VO_FALSE;

		VO_U32 dwCount = m_pEntryBlocks[i].dwEntries;
		while(dwCount > 0)
		{
			VO_U32 dwToRead = VO_MIN(m_dwReadCountOnce, dwCount);
			ptr_read_pointer(pEntry, dwToRead * sizeof(AviStandardIndexEntry));

			VO_U32 dwEntrySize = 0;
			for(VO_U32 j = 0; j < dwToRead; j++)
			{
				dwEntrySize = GET_SIZE(pEntry[j].dwSize);
				if(dwEntrySize > m_dwMaxSampleSize)
					m_dwMaxSampleSize = dwEntrySize;

				m_ullTotalSize += dwEntrySize;
			}

			dwCount -= dwToRead;
		}
	}
	//End of scan

	m_dwEntryStart = 0;
	m_dwEntryCount = ReadIndexFromFileB(m_dwEntryStart, m_dwReadCountOnce);

	return VO_TRUE;
}

VO_U32 CAviIndxIndex::ReadIndexFromFileB(VO_U32 dwStart, VO_U32 dwCount)
{
	if(dwStart >= m_dwCount)
		return 0;

	VO_U32 dwCountRead = dwCount;
	if(dwStart + dwCount > m_dwCount)
		dwCountRead = m_dwCount - dwStart;

	PAviStandardIndexEntry pEntry = m_pEntries;
	VO_U32 dwPosInBlock = 0, dwReadTotal = 0, dwReadOnce = 0;
	for(VO_U32 i = 0; i < m_dwIndexPoses; i++)
	{
		if(0 == dwReadTotal)	//not start read
		{
			if(m_pEntryBlocks[i].dwPrevEntries <= dwStart && m_pEntryBlocks[i].dwPrevEntries + m_pEntryBlocks[i].dwEntries > dwStart)
			{
				dwPosInBlock = dwStart - m_pEntryBlocks[i].dwPrevEntries;
				//found
				if(!m_pFileChunk->FLocate(m_pEntryBlocks[i].ullIndexFilePos + dwPosInBlock * sizeof(AviStandardIndexEntry), VO_TRUE))
					return 0;

				//read
				dwReadOnce = VO_MIN(m_pEntryBlocks[i].dwEntries - dwPosInBlock, dwCount);
				ptr_read_pointer(pEntry, dwReadOnce * sizeof(AviStandardIndexEntry));

				dwReadTotal += dwReadOnce;
			}
		}
		else
		{
			//read directly
			if(!m_pFileChunk->FLocate(m_pEntryBlocks[i].ullIndexFilePos, VO_TRUE))
				return 0;

			dwReadOnce = VO_MIN(m_pEntryBlocks[i].dwEntries, dwCount - dwReadTotal);
			ptr_read_pointer(pEntry, dwReadOnce * sizeof(AviStandardIndexEntry));

			dwReadTotal += dwReadOnce;
		}

		if(dwReadTotal == dwCountRead)	//finish
			break;

		pEntry = m_pEntries + dwReadTotal;
	}

	return dwCountRead;
}

VO_U64 CAviIndxIndex::GetStartPos(VO_U32 dwIndex)
{
	for(VO_U32 i = 0; i < m_dwIndexPoses; i++)
	{
		if(m_pEntryBlocks[i].dwPrevEntries <= dwIndex && m_pEntryBlocks[i].dwPrevEntries + m_pEntryBlocks[i].dwEntries > dwIndex)
			return m_pEntryBlocks[i].ullStartPosInFile;
	}

	return 0;
}

VO_U32 CAviIndxIndex::GetIndexBlocks(PAviMediaSampleIndexBlock* ppEntryBlocks)
{
	if(ppEntryBlocks)
		*ppEntryBlocks = m_pEntryBlocks;

	return m_dwIndexPoses;
}

//AVI_INDEX_IDX1
CAviIdx1Index::CAviIdx1Index(CGFileChunk* pFileChunk, VO_U64 qwFilePos, VO_U32 dwSize, VO_BOOL bVideo, VO_U16 wStreamTwocc, VO_MEM_OPERATOR* pMemOp)
	: CAviBaseIndex(AVI_INDEX_IDX1, pFileChunk, AVI_INDEX_IDX1_MAX_COUNT, pMemOp)
	, m_qwIndexFilePos(qwFilePos)
	, m_qwStartPosInFile(0)
	, m_dwTotalIndex(dwSize >> 4)
	, m_dwIndexIndex(0)
	, m_bVideo(bVideo)
	, m_wStreamTwocc(wStreamTwocc)
	, m_pActEntries(VO_NULL)
{
	m_pFileEntries = NEW_OBJS(AviOriginalIndexEntry, VO_MIN(m_dwTotalIndex, AVI_INDEX_IDX1_ONCE));
	m_pdwIndexIndex = NEW_OBJS(VO_U32, ((m_dwTotalIndex + AVI_INDEX_IDX1_MAX_COUNT - 1) / AVI_INDEX_IDX1_MAX_COUNT));
}

CAviIdx1Index::~CAviIdx1Index()
{
	SAFE_MEM_FREE(m_pActEntries);
	SAFE_MEM_FREE(m_pFileEntries);
	SAFE_MEM_FREE(m_pdwIndexIndex);
}

VO_BOOL CAviIdx1Index::DoScan(VO_PTR pParent, VO_PTR pParam, ONINDEXENTRYCALLBACK fCallback, VO_U32& dwFirstChunkOffset)
{
	if(!m_pFileChunk->FLocate(m_qwIndexFilePos))
		return VO_FALSE;

	VO_U32 dwIndexNum = m_dwTotalIndex;

	dwFirstChunkOffset = VO_MAXU32;
	while(dwIndexNum > 0)
	{
		VO_U32 uiTmpNum = VO_MIN(AVI_INDEX_IDX1_ONCE, dwIndexNum);
		VO_U32 dwReaded = 0;
		if(!m_pFileChunk->FRead3(m_pFileEntries, uiTmpNum * sizeof(AviOriginalIndexEntry), &dwReaded))
			return VO_FALSE;

		if(dwReaded < uiTmpNum * sizeof(AviOriginalIndexEntry))
			uiTmpNum = dwReaded / sizeof(AviOriginalIndexEntry);

		PAviOriginalIndexEntry pTmp = m_pFileEntries;
		if(VO_MAXU32 == dwFirstChunkOffset)
			dwFirstChunkOffset = pTmp->dwOffset;

		for(VO_U32 w = 0; w < uiTmpNum; w++)
		{
			fCallback(pParent, pParam, pTmp, dwIndexNum - w);

			pTmp++;
		}

		dwIndexNum -= uiTmpNum;
	}

	return VO_TRUE;
}

VO_VOID CAviIdx1Index::AddCount(VO_U32 dwCurrIndexNum)
{
	if(!(m_dwCount % AVI_INDEX_IDX1_MAX_COUNT))
	{
		m_pdwIndexIndex[m_dwIndexIndex] = m_dwTotalIndex - dwCurrIndexNum;
		m_dwIndexIndex++;
	}

	m_dwCount++;
}

VO_BOOL CAviIdx1Index::Init()
{
	if(m_dwCount < m_dwReadCountOnce)
		m_dwReadCountOnce = m_dwCount;

	m_pEntries = NEW_OBJS(AviStandardIndexEntry, m_dwReadCountOnce);
	if(!m_pEntries)
		return VO_FALSE;

	m_dwEntryStart = 0;
	m_dwEntryCount = ReadIndexFromFileB(m_dwEntryStart, m_dwReadCountOnce);

	if (!m_bVideo && m_dwCount < 5 && m_pEntries->dwSize > m_dwAvgBytesPerSec)
	{
		PAviStandardIndexEntry pEntriesSrc = m_pEntries;
		VO_S32 iCountSrc = m_dwCount;

		m_dwCount = 0;
		for (int iTmp = 0; iTmp <  iCountSrc; iTmp++)
		{
			VO_U32 dwSizeO		= pEntriesSrc->dwSize;

			//0.25 second!!
			VO_U32 dwEntrySize = m_dwAvgBytesPerSec / 4;

			//must block align!!
			//otherwise some audio will not correct, such as ADPCM!!
			dwEntrySize = (dwEntrySize + m_nBlockAlign - 1) / m_nBlockAlign * m_nBlockAlign;

			m_dwCount += dwSizeO / dwEntrySize;
			if(dwSizeO % dwEntrySize)
				m_dwCount++;

			pEntriesSrc++;
		}

		PAviStandardIndexEntry pEntriesDes = NEW_OBJS(AviStandardIndexEntry, m_dwCount);
		if(!pEntriesDes)
			return VO_FALSE;

		pEntriesSrc = m_pEntries;
		VO_S32 iCount = 0;
		for (int iTmp = 0; iTmp <  iCountSrc; iTmp++)
		{
			VO_U32 dwPosInFileO = pEntriesSrc->dwOffset;
			VO_U32 dwSizeO = pEntriesSrc->dwSize;

			//0.25 second!!
			VO_U32 dwEntrySize = m_dwAvgBytesPerSec / 4;

			//must block align!!
			//otherwise some audio will not correct, such as ADPCM!!
			dwEntrySize = (dwEntrySize + m_nBlockAlign - 1) / m_nBlockAlign * m_nBlockAlign;

			VO_S32 iCount0 = dwSizeO / dwEntrySize;
			if(dwSizeO % dwEntrySize)
				iCount0++;

			for(VO_S32 i = iCount; i < iCount + iCount0 - 1; i++)
			{
				pEntriesDes[i].dwOffset = dwPosInFileO + (i - iCount) * dwEntrySize;
				pEntriesDes[i].dwSize = dwEntrySize;

				//VOLOGI("i %d:\toffset %d,\tsize %d", i, pEntriesDes[i].dwOffset, pEntriesDes[i].dwSize);
			}

			iCount += iCount0;
			pEntriesDes[iCount - 1].dwOffset = dwPosInFileO + (iCount0 - 1) * dwEntrySize;
			pEntriesDes[iCount - 1].dwSize = dwSizeO - (iCount0 - 1) * dwEntrySize;

			//VOLOGI("i %d:\toffset %d,\tsize %d", iCount - 1, pEntriesDes[iCount - 1].dwOffset, pEntriesDes[iCount - 1].dwSize);

			pEntriesSrc++;
		}

		SAFE_MEM_FREE(m_pEntries);
		m_pEntries = pEntriesDes;

		m_dwEntryStart = 0;
		m_dwEntryCount = m_dwCount;
	}

	return VO_TRUE;
}

VO_U32 CAviIdx1Index::ReadIndexFromFileB(VO_U32 dwStart, VO_U32 dwCount)
{
	if(dwStart >= m_dwCount)
		return 0;

	VO_U32 dwCountRead = dwCount;
	if(dwStart + dwCount > m_dwCount)
		dwCountRead = m_dwCount - dwStart;

	VO_U32 dwTotalReadFileIndex = m_pdwIndexIndex[dwStart / AVI_INDEX_IDX1_MAX_COUNT];
	if(!m_pFileChunk->FLocate(m_qwIndexFilePos + dwTotalReadFileIndex * sizeof(AviOriginalIndexEntry), VO_TRUE))
		return 0;

	PAviOriginalIndexEntry pTmp = VO_NULL;
	VO_U32 dwTotalRead = 0;
	VO_BOOL bHasDdChunk = VO_FALSE;
	while(VO_TRUE)
	{
		VO_U32 dwIndexNum = VO_MIN(AVI_INDEX_IDX1_ONCE, m_dwTotalIndex - dwTotalReadFileIndex);
		VO_U32 dwReaded = 0;
		if(!m_pFileChunk->FRead3(m_pFileEntries, dwIndexNum * sizeof(AviOriginalIndexEntry), &dwReaded))
			return 0;

		if(dwReaded < dwIndexNum * sizeof(AviOriginalIndexEntry))
			dwIndexNum = dwReaded / sizeof(AviOriginalIndexEntry);

		pTmp = m_pFileEntries;
		for(VO_U32 w = 0; w < dwIndexNum; w++)
		{
			if(m_wStreamTwocc == (pTmp->dwChunkId & 0xFFFF))
			{
				if(m_bVideo)
				{
					if(TWOCC_dd == (pTmp->dwChunkId >> 16))	//'dd' chunk
					{
						if(!m_pActEntries)
						{
							m_pActEntries = NEW_OBJS(AviStandardIndexEntry, m_dwReadCountOnce);
							if(!m_pActEntries)
								return 0;
						}

						m_pEntries[dwTotalRead].dwSize = pTmp->dwSize;
						m_pEntries[dwTotalRead].dwSize |= 0x40000000;
						m_pEntries[dwTotalRead].dwOffset = pTmp->dwOffset;

						bHasDdChunk = VO_TRUE;
					}
					else	//raw data
					{
						if(bHasDdChunk)
						{
							m_pActEntries[dwTotalRead].dwSize = pTmp->dwSize;
							if(!(pTmp->dwFlags & 0x10))	//NOT key frame
								m_pActEntries[dwTotalRead].dwSize |= 0x80000000;

							m_pActEntries[dwTotalRead].dwOffset = pTmp->dwOffset;
						}
						else
						{
							m_pEntries[dwTotalRead].dwSize = pTmp->dwSize;
							if ((pTmp->dwSize & 0x40000000))
								return 0;

							if(!(pTmp->dwFlags & 0x10))	//NOT key frame
								m_pEntries[dwTotalRead].dwSize |= 0x80000000;

							m_pEntries[dwTotalRead].dwOffset = pTmp->dwOffset;
						}

						dwTotalRead++;

						bHasDdChunk = VO_FALSE;
					}
				}
				else
				{
					m_pEntries[dwTotalRead].dwSize = pTmp->dwSize;
					m_pEntries[dwTotalRead].dwOffset = pTmp->dwOffset;
					dwTotalRead++;
				}

				if(dwTotalRead == dwCountRead)
					return dwCountRead;
			}

			pTmp++;
		}

		dwTotalReadFileIndex += dwIndexNum;
	}
}

VO_U64 CAviIdx1Index::GetStartPos(VO_U32 dwIndex)
{
	return m_qwStartPosInFile;
}

VO_BOOL CAviIdx1Index::GetEntryByIndex(VO_U32 dwIndex, VO_U64* pullPosInFile, VO_U32* pdwSize, VO_U64* pullDdPosInFile, VO_U32* pdwDdSize)
{
	VO_U64 ullPosInFile = 0;
	VO_U32 dwSize = 0;
	if(!CAviBaseIndex::GetEntryByIndex(dwIndex, &ullPosInFile, &dwSize))
		return VO_FALSE;

	if(IS_DD_FRAME(dwSize))
	{
		if (!m_pActEntries)
			return VO_FALSE;

		if(pullDdPosInFile)
			*pullDdPosInFile = ullPosInFile;
		if(pdwDdSize)
			*pdwDdSize = GET_SIZE(dwSize);

		PAviStandardIndexEntry pEntry = m_pActEntries + (dwIndex - m_dwEntryStart);

		if(pullPosInFile)
			*pullPosInFile = pEntry->dwOffset + GetStartPos(dwIndex);
		if(pdwSize)
			*pdwSize = pEntry->dwSize | 0x40000000;
	}
	else
	{
		if(pullPosInFile)
			*pullPosInFile = ullPosInFile;
		if(pdwSize)
			*pdwSize = dwSize;
	}

	return VO_TRUE;
}