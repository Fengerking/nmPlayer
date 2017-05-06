#include "CTsWriter.h"
#include "CBaseDumper.h"
#include "CTsAssembler.h"
#include "cmnMemory.h"
#include "CDumper.h"
#include "voLog.h"
#include "CCacheBuffer.h"
#include "voIndex.h"
#include "voCheck.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CTsWriter::CTsWriter(void)
:m_nBufDuration(0xFFFFFFFF)
,m_bInputPTS(VO_FALSE)
,m_hCheck(NULL)
,m_bDisbaleStep(VO_FALSE)
{
	m_pAudioCacheBuf = new CacheBufferList(MAX_CACHE_AUDIO_SAMPLE_COUNT);
	m_pVideoCacheBuf = new CacheBufferList(MAX_CACHE_VIDEO_SAMPLE_COUNT);
	memset(&m_TmpAudioSample,0,sizeof(m_TmpAudioSample));
	
}

CTsWriter::~CTsWriter(void)
{
	Close();
}

VO_U32  CTsWriter::Init(VO_TCHAR * pWorkingPath)
{
	VOLOGINIT(pWorkingPath);
	return voCheckLibInit(&m_hCheck,VO_INDEX_SNK_TS, VO_LCS_WORKPATH_FLAG, 0, pWorkingPath);
}

VO_U32 CTsWriter::Uninit()
{
	VOLOGUNINIT();
	VO_U32 ret = VO_ERR_SOURCE_OK;
	if (VO_NULL != m_hCheck)
	{
		ret = voCheckLibUninit(m_hCheck);
		m_hCheck = NULL;
	}
	return ret;
}
VO_U32	CTsWriter::Open(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam)
{
	
	if (VO_ERR_NONE != CBaseWriter::Open(pFileSource, pParam))
		return VO_ERR_FAILED;

	m_TmpAudioSample.Buffer = new VO_BYTE[((CTsAssembler *)m_pAssembler)->GetDefaultPESSize()];
	return VO_ERR_NONE;
}

CBaseAssembler* CTsWriter::doCreateAssembler()
{
	return new CTsAssembler;
}

VO_U32	CTsWriter::Close()
{
	SAFE_DELETE(m_pAudioCacheBuf);
	SAFE_DELETE(m_pVideoCacheBuf);
	if (m_TmpAudioSample.Buffer)
	{
		delete []m_TmpAudioSample.Buffer;
		m_TmpAudioSample.Buffer = NULL;
	}	
	CBaseWriter::Close();

	return VO_ERR_NONE;
}

VO_U32	CTsWriter::AddSample( VO_SINK_SAMPLE * pSample)
{
	VO_S64 llDelay = AVRescaleRound(700000LL, 90000LL, 1000000LL,5)*2;
	if (!m_bInputPTS)
	{
		pSample->Time *= 90;
	}
	pSample->Time += llDelay;

	VO_U32 Size = pSample->Size & 0x7FFFFFFF;
	VOLOGR("CTsWriter AddSample:CodecCnt == %d,nAV==%d,Time==%lld,Size==%d",m_pAssemblerInitInfo->codec_count,pSample->nAV,pSample->Time,Size);
	VO_U64 TmpTS = pSample->Time;
	TmpTS -= llDelay;
	if (pSample->nAV == 0)
	{
		CDumper::DumpAudioData((VO_PBYTE)&TmpTS,4);
		CDumper::DumpAudioData((VO_PBYTE)&pSample->Size,4);
		CDumper::RecordAudioLog(TmpTS,Size,(pSample->Size& 0x80000000));
		CDumper::DumpAudioData(pSample->Buffer,Size);
	}
	else
	{
		CDumper::DumpVideoData((VO_PBYTE)&TmpTS,4);
		CDumper::DumpVideoData((VO_PBYTE)&pSample->Size,4);
		CDumper::RecordVideoLog(TmpTS,Size,(pSample->Size& 0x80000000));
		CDumper::DumpVideoData((VO_PBYTE)pSample->Buffer,Size);
	}
	if(m_pAssemblerInitInfo->codec_count == 1 && m_nBufDuration != 0)
	{
		m_nBufDuration = 0;
	}
	pCacheBufferItem  pTmpVideoSample = NULL;
	pCacheBufferItem  pTmpAudioSample = NULL;
	if (pSample->nAV == 0)
	{
		m_pAudioCacheBuf->AddEntry(pSample);
	}
	else if (pSample->nAV == 1)
	{
		m_pVideoCacheBuf->AddEntry(pSample);
	}
	
	pTmpVideoSample = m_pVideoCacheBuf->PeekEntry(); 
	pTmpAudioSample = m_pAudioCacheBuf->PeekEntry(VO_TRUE);
	if (pTmpVideoSample && pTmpAudioSample)
	{
		VOLOGR("pVideoEntry->pBuf.DTS:%lld===pAudioEntry->pBuf.DTS=%lld",pTmpVideoSample->pBuf.DTS,pTmpAudioSample->pBuf.DTS);
		if (!m_bIsFirstVideo || pTmpVideoSample->pBuf.DTS <= pTmpAudioSample->pBuf.DTS )
		{
			if (m_TmpAudioSample.Size && pTmpVideoSample->pBuf.DTS - m_TmpAudioSample.DTS > llDelay/2)
			{
				AssembleAudioSample(&m_TmpAudioSample,VO_TRUE);
				m_TmpAudioSample.Size = 0;
				m_TmpAudioSample.Duration = 0;
			}
			if (!m_bIsFirstVideo)
			{
				m_bIsFirstVideo = VO_TRUE;
			}
			pTmpVideoSample = m_pVideoCacheBuf->GetEntry(); 
			AssembleSample(&pTmpVideoSample->pBuf,pTmpVideoSample->bKey);

		}
		else
		{
			pTmpAudioSample = m_pAudioCacheBuf->GetEntry(VO_TRUE);
			AssembleAudioSample(&pTmpAudioSample->pBuf,llDelay);
		}
	}
	else
	{
		VO_U64 llAudioDuration = 0;
		VO_U64 llVideoDuration = 0;
		m_pAudioCacheBuf->GetDuration(&llAudioDuration);
		m_pVideoCacheBuf->GetDuration(&llVideoDuration);
		VOLOGI("llAudioDuration==%lld,llVideoDuration=%lld",llAudioDuration,llVideoDuration);
		if (!pTmpVideoSample && llAudioDuration > m_nBufDuration)
		{
			pTmpAudioSample = m_pAudioCacheBuf->GetEntry(VO_TRUE);
			if (pTmpAudioSample)
			{
				AssembleAudioSample(&pTmpAudioSample->pBuf,llDelay);
			}
		}
		else if (!pTmpAudioSample && llVideoDuration > m_nBufDuration)
		{
			pTmpVideoSample = m_pVideoCacheBuf->GetEntry();
			if (pTmpVideoSample)
			{
				AssembleSample(&pTmpVideoSample->pBuf,pTmpVideoSample->bKey);
			}
		}
	}
	return VO_ERR_NONE;
}

VO_U32	CTsWriter::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (VO_PID_SINK_FLUSH == uID)
	{
		VO_S64 llDelay = AVRescaleRound(700000LL, 90000LL, 1000000LL,5)*2;
		VOLOGR("CTsWriter SetParam:CodecCnt == %d",m_pAssemblerInitInfo->codec_count);
		if ( m_pAssemblerInitInfo->codec_count > 1)
		{
			if (m_pAudioCacheBuf->m_pTmpCacheItem)
			{
				m_pAudioCacheBuf->DeleteEntry(m_pAudioCacheBuf->m_pTmpCacheItem);
				m_pAudioCacheBuf->m_pTmpCacheItem = NULL;
			}

			if (m_pVideoCacheBuf->m_pTmpCacheItem)
			{
				m_pVideoCacheBuf->DeleteEntry(m_pVideoCacheBuf->m_pTmpCacheItem);
				m_pVideoCacheBuf->m_pTmpCacheItem = NULL;
			}

			pCacheBufferItem pTmpVideoSample = NULL;
			pCacheBufferItem pTmpAudioSample = NULL;
			VO_BOOL bDelVieo = VO_FALSE;
			while(1)
			{
				pTmpVideoSample = m_pVideoCacheBuf->PeekEntry(VO_TRUE); 
				pTmpAudioSample = m_pAudioCacheBuf->PeekEntry(VO_TRUE);

				VOLOGR("CTsWriter SetParam2:pTmpVideoSample==%d,pTmpAudioSample==%d,",pTmpVideoSample,pTmpAudioSample);
				if (!pTmpVideoSample && !pTmpAudioSample)
				{
					break;
				}
				if (pTmpVideoSample && pTmpAudioSample)
				{
					VOLOGR("pVideoEntry->pBuf.DTS:%lld==pAudioEntry->pBuf.DTS=%lld",pTmpVideoSample->pBuf.DTS,pTmpAudioSample->pBuf.DTS);
					if (pTmpVideoSample->pBuf.DTS <= pTmpAudioSample->pBuf.DTS)
					{
						pTmpVideoSample = m_pVideoCacheBuf->GetEntry(VO_TRUE); 
						AssembleSample(&pTmpVideoSample->pBuf,pTmpVideoSample->bKey);
						bDelVieo = VO_TRUE;
					}
					else
					{
						pTmpAudioSample = m_pAudioCacheBuf->GetEntry(VO_TRUE);
						AssembleAudioSample(&pTmpAudioSample->pBuf,llDelay);
						bDelVieo = VO_FALSE;
					}
				}
				else
				{
					if (pTmpVideoSample)
					{
						pTmpVideoSample = m_pVideoCacheBuf->GetEntry(VO_TRUE); 
						AssembleSample(&pTmpVideoSample->pBuf,pTmpVideoSample->bKey);
						bDelVieo = VO_TRUE;
					}
					else
					{
						pTmpAudioSample = m_pAudioCacheBuf->GetEntry(VO_TRUE);
						AssembleAudioSample(&pTmpAudioSample->pBuf,llDelay);
						bDelVieo = VO_FALSE;
					}
				}
				if (!bDelVieo)
				{
					m_pAudioCacheBuf->DeleteEntry(m_pAudioCacheBuf->m_pTmpCacheItem);
					m_pAudioCacheBuf->m_pTmpCacheItem = NULL;
				}
				else
				{
					m_pVideoCacheBuf->DeleteEntry(m_pVideoCacheBuf->m_pTmpCacheItem);
					m_pVideoCacheBuf->m_pTmpCacheItem = NULL;
				}
			}
			if (m_TmpAudioSample.Size)
			{
				AssembleAudioSample(&m_TmpAudioSample,llDelay,VO_TRUE);
			}
			m_bIsFirstVideo = VO_FALSE;
			m_pAssembler->Flush();
			CDumper::CloseAllDumpFile();
			
		}
	}
	else if (VO_PID_SINK_AV_SYNC_BUFFER_DURATION == uID && pParam)
	{
		m_nBufDuration = *(VO_U32*)pParam;
	}
	else if (VO_PID_SINK_FORMAT_CHANGE == uID)
	{
		//VOLOGE("VO_PID_SINK_FORMAT_CHANGE begin");
		//((CTsAssembler*)m_pAssembler)->FormatChange();
	}
	else if (VO_PID_SINK_ASSEMBLE_PAT_PMT == uID)
	{
		VOLOGE("Assamble PAT again");
		((CTsAssembler*)m_pAssembler)->AssembleBasicTable(VO_TRUE);
	}
	else if (VO_PID_SINK_INPUT_PTS == uID)
	{
		m_bInputPTS = VO_TRUE;
	}
	else if (VO_PID_SINK_DISABLEAUDIOSTEPSIZE == uID)
	{
		m_bDisbaleStep = VO_TRUE;
	}
	if(VO_ERR_NONE == CBaseWriter::SetParam(uID, pParam))
		return VO_ERR_NONE;

	return VO_ERR_NONE;
}

VO_U32	CTsWriter::GetParam(VO_U32 uID, VO_PTR pParam)
{
	if(VO_ERR_NONE == CBaseWriter::GetParam(uID, pParam))
		return VO_ERR_NONE;

	return VO_ERR_NONE;
}

VO_U32 CTsWriter::AssembleSample(VO_SINK_SAMPLE * pSample,VO_BOOL bKeyFrame)
{
	if (pSample)
	{
		if (m_pAssembler)
		{
			m_pAssembler->AddSample(bKeyFrame , pSample);
		}
	}
	return VO_ERR_NONE;
}

VO_U32 CTsWriter::AssembleAudioSample(VO_SINK_SAMPLE * pSample,VO_S64 llDelay,VO_BOOL bForece)
{
	if (m_TmpAudioSample.Size && 
		((pSample->DTS - m_TmpAudioSample.DTS > llDelay/2)
		|| (m_TmpAudioSample.Size + pSample->Size > ((CTsAssembler *)m_pAssembler)->GetDefaultPESSize())
		|| bForece
		|| m_bDisbaleStep)
		)
	{
		AssembleSample(&m_TmpAudioSample,VO_TRUE);
		m_TmpAudioSample.Size = 0;
		m_TmpAudioSample.Duration = 0;
	}
	if (!m_TmpAudioSample.Size)
	{
		m_TmpAudioSample.DTS = pSample->DTS;
		m_TmpAudioSample.nAV = pSample->nAV;
		m_TmpAudioSample.Time = pSample->Time;
		if (pSample->Size > ((CTsAssembler *)m_pAssembler)->GetDefaultPESSize())
		{
			delete []m_TmpAudioSample.Buffer;
			m_TmpAudioSample.Buffer = new VO_BYTE[pSample->Size];
		}
	}
	memcpy(m_TmpAudioSample.Buffer + m_TmpAudioSample.Size,pSample->Buffer,pSample->Size & 0x7FFFFFFF);
	m_TmpAudioSample.Size += pSample->Size;
	m_TmpAudioSample.Duration += pSample->Duration;
	return VO_ERR_NONE;
}