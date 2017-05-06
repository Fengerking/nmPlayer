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
File:		CAviReader.cpp

Contains:	provides the interfaces to get the right data from an avi file 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#include "CAviReader.h"
#include "CAviAudioTrack.h"
#include "CAviVideoTrack.h"
#include "CAviMpeg2VideoTrack.h"
#include "fCC.h"
#include "voDRM.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif


static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	CAviReader* pReader = (CAviReader*)pUser;
	PAviDataParserSample pSample = (PAviDataParserSample)pParam;
	return pReader->OnFrame(pSample->pFileChunk, pSample->wStreamTwocc, pSample->dwLen);
}

CAviReader::CAviReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseStreamFileReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_HeaderParser(&m_chunk, pMemOp)
	, m_DataParser(&m_chunk, pMemOp)
	, m_bDivXDRM(VO_FALSE)
{
	m_pFileDataParser = &m_DataParser;
}

CAviReader::~CAviReader()
{
	Close();
}

VO_U32 CAviReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	Close();

 	VO_U32 rc = CBaseStreamFileReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	VO_BOOL bRet = m_HeaderParser.ReadFromFile();
	if(!bRet)
		return VO_ERR_SOURCE_OPENFAIL;

	rc = InitTracks(nSourceOpenFlags);
	m_HeaderParser.ReleaseTmpInfo();
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	VO_BOOL bIsDivX = VO_FALSE;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && ((CAviTrack*)m_ppTracks[i])->IsCodecDivX())
		{
			bIsDivX = VO_TRUE;

			break;
		}
	}

	if(bIsDivX)
	{
#ifndef _VO_FORCE_PASS_DIVX
		if(VO_ERR_DRM_OK != DRMIsSupported(VO_DRMTYPE_DIVX))
			return VO_ERR_SOURCE_OPENFAIL;
#endif	// _VO_FORCE_PASS_DIVX

		VO_PBYTE pStrdContent = VO_NULL;
		VO_U32 dwStrdLength = 0;
		if(m_HeaderParser.GetStrdContent(&pStrdContent, &dwStrdLength))
		{
			VO_DRM_INFO infoDRM = {0};
			infoDRM.pDRMHeader	= pStrdContent;
			infoDRM.nDRMHeader	= dwStrdLength;

			if(DRMInfo(VO_DRMTYPE_DIVX, &infoDRM) != VO_ERR_DRM_OK)
				return VO_ERR_SOURCE_CONTENTENCRYPT;

			m_bDivXDRM = VO_TRUE;
		}
	}

	if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	{
		SelectDefaultTracks();
		PrepareTracks();

		if(m_HeaderParser.HasFileIndex())
		{
			//remove no index track!!
			for(VO_U32 i = 0; i < m_nTracks; i++)
			{
				if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && (!((CAviTrack*)m_ppTracks[i])->m_pIndex || !((CAviTrack*)m_ppTracks[i])->m_pIndex->GetCount()))
					m_ppTracks[i]->SetInUsed(VO_FALSE);
			}
		}
		else
		{
			AviDataParserInitParam initParam(m_HeaderParser.GetStreamCount());
			m_DataParser.Init(&initParam);
			if(!m_DataParser.SetStartFilePos(m_HeaderParser.GetMediaDataFilePos() - 4))
				return VO_ERR_SOURCE_END;

			m_DataParser.SetOnDataCallback(this, VO_NULL, GOnData);
			FileGenerateIndex();

			for(VO_U32 i = 0; i < m_nTracks; i++)
			{
				if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed() && ((CBaseStreamFileTrack*)m_ppTracks[i])->IsIndexNull())	//still no frame, then we should delete it!!
					m_ppTracks[i]->SetInUsed(VO_FALSE);
			}
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviReader::Close()
{
	UnprepareTracks();
	UninitTracks();

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		SAFE_DELETE(m_ppTracks[i]);
	}

	return CBaseStreamFileReader::Close();
}

VO_U32 CAviReader::GetFileHeadDataInfo(headerdata_info* pHeadDataInfo)
{
	if(m_HeaderParser.HasIndxIndex())
	{
		VO_U32 nIdx = 0;
		pHeadDataInfo->arraysize = 0;
		for(VO_U32 i = 0; i < m_nTracks; i++)
		{
			CAviTrack* pTrack = (CAviTrack*)((m_ppTracks[i] && m_ppTracks[i]->IsInUsed()) ? m_ppTracks[i] : VO_NULL);
			if(pTrack)
			{
				PAviMediaSampleIndexBlock pIndexBlocks = VO_NULL;
				VO_U32 dwIndexBlocks = (((CAviIndxIndex*)pTrack->m_pIndex)->GetIndexBlocks(&pIndexBlocks));
				pHeadDataInfo->arraysize += dwIndexBlocks;
				if(pHeadDataInfo->ptr_array)
				{
					for(VO_U32 j = 0; j < dwIndexBlocks; j++)
					{
						pHeadDataInfo->ptr_array[nIdx].physical_pos = pIndexBlocks[j].ullIndexFilePos;
						pHeadDataInfo->ptr_array[nIdx].size = pIndexBlocks[j].dwEntries * 8;

						nIdx++;
					}
				}
			}
		}
	}
	else if(m_HeaderParser.HasIdx1Index())
	{
		pHeadDataInfo->arraysize = 1;
		if(pHeadDataInfo->ptr_array)
		{
			pHeadDataInfo->ptr_array[0].physical_pos = m_HeaderParser.GetIdx1FilePos();
			pHeadDataInfo->ptr_array[0].size = m_HeaderParser.GetIdx1Size();
		}
	}
	else	//no index
		pHeadDataInfo->arraysize = 0;

	pHeadDataInfo->reserved = VO_FILE_CHUNK_SIZE;	//PD should save more data because chunk operation.
	return VO_ERR_SOURCE_OK;
}

CAviTrack* CAviReader::GetTrack(VO_U16 wStreamTwocc)
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && ((CAviTrack*)m_ppTracks[i])->m_wStreamTwocc == wStreamTwocc)
			return (CAviTrack*)m_ppTracks[i];
	}
	return VO_NULL;
}

VO_U32 CAviReader::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_U32 rc = TracksCreate(m_HeaderParser.GetStreamCount());
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	PAviTrackInfo pti = m_HeaderParser.GetTrackInfoPtr();
	VO_U32 i = 0;
	while(pti)
	{
		CAviTrack* pNewTrack = NewTrack(pti);
		if(pNewTrack)
		{
			pNewTrack->SetBufferTime(10);
			VO_U32 dwMaxSampleSize = 0;
			pNewTrack->GetMaxSampleSize(&dwMaxSampleSize);
			pNewTrack->SetGlobalBufferExtSize(dwMaxSampleSize);

			m_ppTracks[i] = pNewTrack;

			i++;
		}

		pti = pti->next;
	}

	m_nTracks = i;

	return CBaseStreamFileReader::InitTracks(nSourceOpenFlags);
}

VO_U32 CAviReader::PrepareIndx()
{
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		CAviTrack* pTrack = (CAviTrack*)((m_ppTracks[i] && m_ppTracks[i]->IsInUsed()) ? m_ppTracks[i] : VO_NULL);
		if(pTrack)
		{
			PAviProposedIndexInfo pInfo = m_HeaderParser.GetProposedIndexInfo(pTrack->m_wStreamTwocc);
			if(pInfo)
			{
				CAviIndxIndex* pNewIndex = new CAviIndxIndex(&m_chunk, pInfo->dwEntriesInUse, pInfo->pullFilePos, m_pMemOp);
				if(!pNewIndex)
					return VO_ERR_OUTOF_MEMORY;

				if(!pNewIndex->Init())
				{
					delete pNewIndex;
					m_HeaderParser.RemoveIndex(FLAG_INDX);
					return VO_ERR_SOURCE_ERRORDATA;
				}

				pTrack->m_dwMaxSampleSize = pNewIndex->GetMaxSampleSize();
				if(VOTT_AUDIO == pTrack->GetType())
				{
					CAviAudioTrack* pAudioTrack = (CAviAudioTrack*)pTrack;
					VO_U32 dwDuration = static_cast<VO_U32>(pAudioTrack->m_dAudioVbrTimePerSample ? pAudioTrack->m_dAudioVbrTimePerSample * pNewIndex->GetCount() : 
						1000 * pNewIndex->GetTotalSize() / pAudioTrack->m_dwAvgBytesPerSec);
					//if(dwDuration > pTrack->m_dwDuration)
					pTrack->m_dwDuration = dwDuration;
				}

				if(pTrack->m_pIndex)
					delete pTrack->m_pIndex;
				pTrack->m_pIndex = pNewIndex;
			}
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviReader::PrepareIdx1()
{
	//new idx1 object and get the first object
	CAviIdx1Index* pIndex = VO_NULL;
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		CAviTrack* pTrack = (CAviTrack*)((m_ppTracks[i] && m_ppTracks[i]->IsInUsed()) ? m_ppTracks[i] : VO_NULL);
		if(pTrack)
		{
			CAviIdx1Index* pNewIndex = new CAviIdx1Index(&m_chunk, m_HeaderParser.GetIdx1FilePos(), m_HeaderParser.GetIdx1Size(), (pTrack->GetType() == VOTT_VIDEO) ? VO_TRUE : VO_FALSE, pTrack->m_wStreamTwocc, m_pMemOp);
			if(!pNewIndex)
				return VO_ERR_OUTOF_MEMORY;

			if(VOTT_AUDIO == pTrack->GetType())
				pNewIndex->SetAudioInfo(((CAviAudioTrack*)pTrack)->m_dwAvgBytesPerSec, ((VO_WAVEFORMATEX*)pTrack->m_pPropBuffer)->nBlockAlign);

			if(pTrack->m_pIndex)
				delete pTrack->m_pIndex;
			pTrack->m_pIndex = pNewIndex;
			if(!pIndex)
				pIndex = pNewIndex;
		}
	}

	//use the first idx1 object to do scan
	VO_U32 dwFirstChunkOffset = VO_MAXU32;
	if(pIndex)
		pIndex->DoScan(this, VO_NULL, GOnIdx1IndexEntry, dwFirstChunkOffset);

	VO_U32 dwStartPos = static_cast<VO_U32>( (dwFirstChunkOffset + 8 > m_HeaderParser.GetMediaDataFilePos()) ? 8 : m_HeaderParser.GetMediaDataFilePos() );

	//set information get by scan and do initialize
	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		CAviTrack* pTrack = (CAviTrack*)((m_ppTracks[i] && m_ppTracks[i]->IsInUsed()) ? m_ppTracks[i] : VO_NULL);
		if(pTrack)
		{
			if(VOTT_AUDIO == pTrack->GetType())
			{
				CAviAudioTrack* pAudioTrack = (CAviAudioTrack*)pTrack;
				VO_U32 dwDuration = 0;
				if(pAudioTrack->m_dwAvgBytesPerSec)
				{
					dwDuration = static_cast<VO_U32>(pAudioTrack->m_dAudioVbrTimePerSample ? pAudioTrack->m_dAudioVbrTimePerSample * pTrack->m_pIndex->GetCount() : 
						1000 * pAudioTrack->m_ullTotalSize / pAudioTrack->m_dwAvgBytesPerSec);
				}
				else
				{
					dwDuration = pTrack->m_pIndex->GetCount()*pAudioTrack->GetScale()/pAudioTrack->GetRate();
				}
				
				pTrack->m_dwDuration = dwDuration;
				
			}
			pIndex = (CAviIdx1Index*)pTrack->m_pIndex;
			if(pIndex)
			{
				pIndex->SetStartPos(dwStartPos);
				pIndex->Init();
			}
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviReader::PrepareTracks()
{
	//read index from file!!
	if(m_HeaderParser.HasIndxIndex())
	{
		VO_U32 nRC = PrepareIndx();
		if(VO_ERR_SOURCE_OK != nRC && m_HeaderParser.HasIdx1Index())
			PrepareIdx1();
	}
	else if(m_HeaderParser.HasIdx1Index())
		PrepareIdx1();
	else
	{
		//make tracks' duration to same
		//else seek to end will fail!!
		VO_U32 dwMaxDuration = 0;
		for(VO_U32 i = 0; i < m_nTracks; i++)
		{
			if(m_ppTracks[i] && m_ppTracks[i]->GetDuration() > dwMaxDuration)
				dwMaxDuration = m_ppTracks[i]->GetDuration();
		}

		for(VO_U32 i = 0; i < m_nTracks; i++)
		{
			if(m_ppTracks[i])
				m_ppTracks[i]->SetDuration(dwMaxDuration);

			if( VOTT_AUDIO == m_ppTracks[i]->GetType() )
			{
				CAviAudioTrack* pAudioTrack = (CAviAudioTrack*)m_ppTracks[i];

				VO_U32 uFourcc = 0;
				if (VO_ERR_SOURCE_OK == pAudioTrack->GetCodecCC(&uFourcc) && AudioFlag_AAC == uFourcc)
				{
					if ( pAudioTrack->m_pIndex && pAudioTrack->m_pIndex->IsChunkSizeEquilong() )
						pAudioTrack->m_dAudioVbrTimePerSample = 0;
				}
			}
		}
	}

	return CBaseStreamFileReader::PrepareTracks();
}

CAviTrack* CAviReader::NewTrack(PAviTrackInfo pTrackInfo)
{
	if(!pTrackInfo)
		return VO_NULL;

	if(pTrackInfo->StreamHeader.dwRate == 0)	//Now only support A/V track
		return VO_NULL;

	VO_U32 dwDuration = VO_U64(1000) * pTrackInfo->StreamHeader.dwLength * pTrackInfo->StreamHeader.dwScale / pTrackInfo->StreamHeader.dwRate;
	CAviTrack* pNewTrack = VO_NULL;
	if(FOURCC_auds == pTrackInfo->StreamHeader.fccStreamType)
		pNewTrack = new CAviAudioTrack(pTrackInfo->btStreamNum, dwDuration > 1 ? dwDuration : 0, this, m_pMemOp);
	else if(FOURCC_vids == pTrackInfo->StreamHeader.fccStreamType && dwDuration >= 0)
	{
		VO_U32 nCC = ((VO_BITMAPINFOHEADER*)(pTrackInfo->pPropBuffer))->biCompression;
		if (FOURCC_mpg2 == nCC || FOURCC_MPG2 == nCC || FOURCC_mpg1 == nCC || FOURCC_MPG1 == nCC || FOURCC_MPEG == nCC)
		{
			pNewTrack = new CAviMpeg2VideoTrack(pTrackInfo->btStreamNum, dwDuration, this, m_pMemOp);
		} 
		else
		{
			pNewTrack = new CAviVideoTrack(pTrackInfo->btStreamNum, dwDuration, this, m_pMemOp);
		}
	}

	if(pNewTrack)
	{
		pNewTrack->m_dwPropBufferSize = pTrackInfo->dwPropBufferSize;

		if(VOTT_AUDIO == pNewTrack->GetType() && pTrackInfo->dwPropBufferSize < VO_WAVEFORMATEX_STRUCTLEN)
			pNewTrack->m_dwPropBufferSize = VO_WAVEFORMATEX_STRUCTLEN;

		pNewTrack->m_pPropBuffer = NEW_BUFFER(pNewTrack->m_dwPropBufferSize);
		MemCopy(pNewTrack->m_pPropBuffer, pTrackInfo->pPropBuffer, pTrackInfo->dwPropBufferSize);

		if(VOTT_VIDEO == pNewTrack->GetType())
			((CAviVideoTrack*)pNewTrack)->m_ullAvgTimePerFrame = VO_S64(10000000) * pTrackInfo->StreamHeader.dwScale / pTrackInfo->StreamHeader.dwRate;
		else if(VOTT_AUDIO == pNewTrack->GetType())
		{
			VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)pNewTrack->m_pPropBuffer;
			if(pTrackInfo->dwPropBufferSize < VO_WAVEFORMATEX_STRUCTLEN)
				pwfe->cbSize = 0;
			((CAviAudioTrack*)pNewTrack)->m_dwAvgBytesPerSec = pwfe->nAvgBytesPerSec;
			((CAviAudioTrack*)pNewTrack)->m_wFileBitsPerSample = pwfe->wBitsPerSample;
			((CAviAudioTrack*)pNewTrack)->m_nFileBlockAlign = pwfe->nBlockAlign;
			((CAviAudioTrack*)pNewTrack)->m_nChannels = pwfe->nChannels;
			((CAviAudioTrack*)pNewTrack)->m_nScale = pTrackInfo->StreamHeader.dwScale;
			((CAviAudioTrack*)pNewTrack)->m_nRate = pTrackInfo->StreamHeader.dwRate;

			//CBR音轨用以下这个记述可实现同步
			//WAVEFORMATEX::nBlockAlign   1Block大小（Byte）
			//AVIStreamHeader::dwRate / AVIStreamHeader::dwScale  1秒内包含的Block个数
			//nBlockAlign=1，dwScale=1，dwRate=nAvgBytesPerSec   nAvgBytesPerSec是常量

			//VirtualDub的变通法
			//AVIStreamHeader::dwLength = 变换包的总数
			//WAVEFORMATEX::nBlockAlign = 变换包最大容量

			VO_S32 delta = pTrackInfo->StreamHeader.dwRate * pwfe->nBlockAlign / pTrackInfo->StreamHeader.dwScale - pwfe->nAvgBytesPerSec;
			if(delta > 1 || delta < -1) //VBR, we will compute time by index; else CBR time computed by bytes
			{
				((CAviAudioTrack*)pNewTrack)->m_dAudioVbrTimePerSample = double(pTrackInfo->StreamHeader.dwScale) * 1000 / pTrackInfo->StreamHeader.dwRate;
				if ( ((CAviAudioTrack*)pNewTrack)->m_dAudioVbrTimePerSample < 1)
					((CAviAudioTrack*)pNewTrack)->m_dAudioVbrTimePerSample = 0;
			}

			VO_U32 uFourcc = 0;
			if(pNewTrack->GetCodecCC(&uFourcc) == VO_ERR_SOURCE_OK)
			{
				if (AudioFlag_AC3 == uFourcc)
					((CAviAudioTrack*)pNewTrack)->m_dAudioVbrTimePerSample = 0;
				else if (AudioFlag_AAC == uFourcc)
					((CAviAudioTrack*)pNewTrack)->m_dAudioVbrTimePerSample = double(pTrackInfo->StreamHeader.dwScale) * 1000 / pTrackInfo->StreamHeader.dwRate;
			}
		}
	}

	return pNewTrack;
}

VO_VOID CAviReader::GOnIdx1IndexEntry(VO_PTR pParent, VO_PTR pParam, PAviOriginalIndexEntry pEntry, VO_U32 dwCurrIndexNum)
{
	CAviReader* pReader = (CAviReader*)pParent;
	pReader->OnIdx1IndexEntry(pEntry, dwCurrIndexNum);
}

VO_VOID CAviReader::OnIdx1IndexEntry(PAviOriginalIndexEntry pEntry, VO_U32 dwCurrIndexNum)
{
	CAviTrack* pTrack = GetTrack(pEntry->dwChunkId & 0xFFFF);
	if(pTrack)
		pTrack->OnIdx1(dwCurrIndexNum, pEntry);
}

VO_U8 CAviReader::OnFrame(CGFileChunk* pFileChunk, VO_U16 wStreamTwocc, VO_U32 dwLen)
{
	CAviTrack* pTrack = GetTrack(wStreamTwocc);
	if(pTrack)
	{
		if(!pTrack->AddFrame(pFileChunk, dwLen))
			return CBRT_FALSE;

		return CBRT_CONTINUABLENOTNEEDSKIP;
	}
	else
		return CBRT_CONTINUABLEANDNEEDSKIP;
}

VO_VOID CAviReader::OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser)
{
	CBaseStreamFileReader::OnNewDataParser(pRqstTrack, ppDataParser);

	CAviDataParser* pNewDataParser = new CAviDataParser(&m_chunkUseForDataParser, m_pMemOp);
	if(!pNewDataParser)
		return;

	AviDataParserInitParam initParam(m_HeaderParser.GetStreamCount());
	pNewDataParser->Init(&initParam);
	pNewDataParser->SetOnDataCallback(this, VO_NULL, GOnData);
	pNewDataParser->SetBlockStream(GetOtherTrackInUsed(pRqstTrack)->GetStreamNum());

	m_DataParser.SetBlockStream(pRqstTrack->GetStreamNum());

	for(VO_U32 i = 0; i < m_nTracks; i++)
	{
		if(m_ppTracks[i] && m_ppTracks[i]->IsInUsed())
			((CBaseStreamFileTrack*)m_ppTracks[i])->SetParseForSelf(VO_TRUE);
	}

	*ppDataParser = pNewDataParser;
}


VO_U32 CAviReader::GetSeekable(VO_BOOL* pIsSeekable)
{
	*pIsSeekable = m_HeaderParser.HasFileIndex() ? VO_TRUE : VO_FALSE;

	return VO_ERR_SOURCE_OK;
}
