
#include "voDownloadBufferMgr.h"
#include "voToolUtility.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voDownloadBufferMgr::voDownloadBufferMgr()
:m_bExit(VO_FALSE)
{

}

voDownloadBufferMgr::~voDownloadBufferMgr()
{
	UnInit();
}

VO_VOID voDownloadBufferMgr::Init()
{
	VOLOGI("+ Init");
	m_bExit = VO_FALSE;
	m_CacheManager.init();
	m_CacheManager.open();

	VOLOGI("- Init");
}
VO_VOID voDownloadBufferMgr::UnInit()
{
	VOLOGI("+ UnInit");
	m_bExit = VO_TRUE;
	Flush();	
	m_CacheManager.close();
	m_CacheManager.uninit();

	VOLOGI("- UnInit");
}

VO_VOID voDownloadBufferMgr::SetFollowingComponent(VO_PTR pComponent)
{
	m_CacheManager.SetFollowingComponent(pComponent);
}


VO_U32 voDownloadBufferMgr::PushData(VO_DATASOURCE_BUFFER_TYPE nBufferType, VO_SOURCE2_TRACK_TYPE nTrackType, VO_PTR pData)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	if(m_bExit)
		return ret;
	switch(nBufferType)
	{
	case VO_DATASOURCE_BUFFER_TYPE_SAMPLE:
		ret = PushSampleToChunkGroup(nTrackType, (_SAMPLE*)pData);
		break;
	case VO_DATASOURCE_BUFFER_TYPE_DATA:
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}
	VOLOGI("Push Data. BufferType:%d, TrackType:%d, ret:%08x",nBufferType, nTrackType, ret);
	return ret;
}

							
VO_U32 voDownloadBufferMgr::ChunkAgentInit(VO_VOID **ppAgent, VO_U32 uASID, VO_BOOL bSwitch)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_VOID *pHandle = NULL;
	ret = m_CacheManager.createChunkDataCache( uASID, &pHandle );

	_CHUNKS *pBAPairChunk = NULL;
	if( bSwitch )
		pBAPairChunk = FindBAChunk();

	*ppAgent = SaveChunkHandle( uASID, pHandle, pBAPairChunk);

	VOLOGI(" CHUNK BEGIN. CHUNKAGENT:%p ASID:%d, CurChunk(Handle:%p), BAPairChunk(Handle:%p) ", *ppAgent, uASID, pHandle, pBAPairChunk?pBAPairChunk->pHandle:NULL);
 	if(!(*ppAgent))
		ret = VO_RET_SOURCE2_FAIL;
	return ret;
}

VO_U32 voDownloadBufferMgr::ChunkAgentUninit(VO_VOID *pAgent, VO_U32 uASID, CHUNK_STATUS Switch)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	_CHUNKS *pChunk = SearchChunk((_CHUNKS*)pAgent);

	if( pChunk )
	{
		switch(Switch)
		{
			case SWITCH_TRUE:
			{
				if(pChunk->pBAPairChunk)
				{

					ret |= RemoveChunkHandle( pChunk);
					VOLOGI("Two continue BA. Drop it" );	
				}
				else
				{
					pChunk->bBAing = VO_TRUE;
					VOLOGI("Meet CHUNK_SWITCH & CHUNK_TAIL, it is a BA chunk. CurChunk ASID:%d, ChunkHandle: %p", pChunk->uASID, pChunk->pHandle );
				}
				break;
			}
			case SWITCH_FALSE:
			{
				VO_PTR pPairHandle = pChunk->pBAPairChunk ? pChunk->pBAPairChunk->pHandle : NULL;

				//for log
				VO_U32 uPairASID = 0;
				if(pPairHandle )
					uPairASID = pChunk->pBAPairChunk->uASID;
				VOLOGI("releaseChunks. Cur(Handle:%p, ASID:%d), Pair(Handle:%p, ASID:%d)", pChunk->pHandle, pChunk->uASID, pPairHandle, uPairASID);
				//for log

				if( pPairHandle )
				{
					ret |= m_CacheManager.mergeChunks(pPairHandle, pChunk->pHandle);
					ret |=m_CacheManager.useChunk(pPairHandle);
				}
				m_CacheManager.useChunk(pChunk->pHandle);

				ret |= RemoveChunkHandle( pChunk->pBAPairChunk);
				ret |= RemoveChunkHandle( pChunk);
				break;
			}
			case SWITCH_IGNORE:
			{
				ret |= RemoveChunkHandle( pChunk);
				VOLOGI("Ignore this useless chunk");
				break;
			}
			default:
				break;
		}
		
	}


	return ret;
}

VO_U32 voDownloadBufferMgr::DoSample(VO_SOURCE2_TRACK_TYPE nType, _SAMPLE *pSample)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	_CHUNKS *pChunk = SearchChunk((_CHUNKS*)pSample->pChunkAgent);
	if(pChunk)
	{
		
		SpecialFlagCheck((VO_SOURCE2_TRACK_TYPE)nType, pSample->uFlag, pSample->uTime, ret);
		SpecialFlagCheck_DS((VO_SOURCE2_TRACK_TYPE)nType, pSample->uDataSource_Flag, pSample->uTime, ret);
		ret = m_CacheManager.putSample(pChunk->pHandle, pSample, nType);
	}
	else
	{
		VOLOGI("ChunkAgent is NULL.ASID:%d", pSample->uASTrackID);
		ret = VO_RET_SOURCE2_FAIL;
	}
	return ret;
}
VO_U32 voDownloadBufferMgr::DoTrackTypeUnavailable(VO_SOURCE2_TRACK_TYPE nType, _SAMPLE *pSample)
{	
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VOLOGI("+DoTrackTypeUnavailable");
	_CHUNKS *pChunk = SearchChunk((_CHUNKS*)pSample->pChunkAgent);

	if( pChunk)
	{
		DoSample(nType, pSample);
		VOLOGI("%s Send VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE. HANDLE:%p, ASID:%d. ret:0x%08x", TrackType2String(nType), pChunk->pHandle, pSample->uASTrackID, ret);
	}
	else
	{
// 		VO_VOID *pHandle = NULL;
// 		m_CacheManager.createChunkDataCache(_VODS_INT32_MAX -1 , &pHandle);
// 		_CHUNKS *pChunk = SaveChunkHandle(_VODS_INT32_MAX -1, pHandle, NULL);
// 		pSample->pChunkAgent = pChunk;
// 		DoSample(nType, pSample);
// 		m_CacheManager.useChunk(pHandle);
// 		RemoveChunkHandle(pChunk);
		VO_U32 uTimes = pSample->pReserve1 ? (VO_U32)pSample->pReserve1 : 1;
		pSample->pReserve1 = 0;

		VO_VOID *pAgent = NULL;
		ChunkAgentInit(&pAgent, _VODS_INT32_MAX -1, VO_SOURCE2_TT_VIDEO == nType ? VO_TRUE : VO_FALSE);
		pSample->pChunkAgent = pAgent;
		for(VO_U32 i = 0; i< uTimes;i++)
			DoSample(nType, pSample);
		ChunkAgentUninit(pAgent, _VODS_INT32_MAX -1, SWITCH_FALSE);

//		VOLOGI("%s Send VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE. Create a virtual chunk to send it.", TrackType2String(nType), ret);


		VOLOGI("%s Send %d times VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE. Create a virtual chunk to send it.", TrackType2String(nType), uTimes, ret);
	}

	VOLOGI("-DoTrackTypeUnavailable");

	return ret;

}
VO_U32 voDownloadBufferMgr::DoEOS()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VOLOGI("+ EOS.")

	ret = ClearList();
	m_CacheManager.setEOS();

	VOLOGI("- EOS.")

	return ret;
}
VO_U32 voDownloadBufferMgr::ClearList()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	voCAutoLock lock( &m_LockList );

	while(!m_ChunksList.empty())
	{
		_CHUNKS* pChunk = m_ChunksList.front();
		ret = RemoveChunkHandle(pChunk);
	}

	return ret;
}
VO_U32 voDownloadBufferMgr::PushSampleToChunkGroup(VO_SOURCE2_TRACK_TYPE nType, _SAMPLE  *pSample)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	
	if( VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE & pSample->uDataSource_Flag)
		ret = DoTrackTypeUnavailable(nType, pSample);
	else if( VO_DATASOURCE_FLAG_EOS & pSample->uDataSource_Flag)
		ret = DoEOS();
	else
		ret = DoSample( nType, pSample);

	return ret;
}

_CHUNKS *voDownloadBufferMgr::FindBAChunk()
{
	voCAutoLock lock( &m_LockList );

	_CHUNKS *pChunk = NULL;
	if( m_ChunksList.empty())
		;
	else
	{
		std::list<_CHUNKS*>::iterator iter	= m_ChunksList.begin();
		std::list<_CHUNKS*>::iterator itere	= m_ChunksList.end();

		while( iter != itere)
		{
			if( (*iter)->bBAing  )
			{
				pChunk = (*iter);
				break;
			}
				iter ++;
		}
	}

	return pChunk;
}

_CHUNKS *voDownloadBufferMgr::SearchChunk(_CHUNKS *pChunk)
{
	voCAutoLock lock( &m_LockList );
	
	if( m_ChunksList.empty() || NULL == pChunk)
		return NULL;
	else
	{
		std::list<_CHUNKS*>::iterator iter	= m_ChunksList.begin();
		std::list<_CHUNKS*>::iterator itere	= m_ChunksList.end();

		while( iter != itere)
		{
			if( (*iter) == pChunk  )
			{
				return *iter;
			}
			iter ++;
		}
	}

	return NULL;
}

_CHUNKS *voDownloadBufferMgr::SaveChunkHandle(VO_U32 uASID, VO_PTR pHandle, _CHUNKS *pBAPairChunk)
{
	voCAutoLock lock( &m_LockList );

	_CHUNKS *pNewChunk = new _CHUNKS;
	memset(pNewChunk, 0x00, sizeof(_CHUNKS));
	pNewChunk->pHandle = pHandle;
	pNewChunk->pBAPairChunk = pBAPairChunk;
	pNewChunk->uASID = uASID;

	//for log
	VO_PTR pPairHandle = NULL;
	VO_U32 uPairASID = 0;
	if(pBAPairChunk)
	{
		pPairHandle = pBAPairChunk->pHandle;
		uPairASID = pBAPairChunk->uASID;
	}
	VOLOGI("Cur(Handle:%p, ASID:%d), Pair(Handle:%p, ASID:%d)", pHandle, uASID, pPairHandle, uPairASID);
	//for log

	m_ChunksList.push_back(pNewChunk);
	return pNewChunk;
}

VO_U32 voDownloadBufferMgr::RemoveChunkHandle(_CHUNKS* pChunk)
{

	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	if(pChunk && pChunk->pHandle)
	{
		VOLOGI("RemoveChunkHandle:%p, ASID:%d, PairChunk:%p", pChunk->pHandle, pChunk->uASID, pChunk->pBAPairChunk);
		ret = m_CacheManager.destroyChunkDataCache(pChunk->pHandle);
		voCAutoLock lock( &m_LockList );
		m_ChunksList.remove( pChunk );
		delete pChunk;
		pChunk = NULL;
	}

	return ret;

}


VO_VOID voDownloadBufferMgr::Flush()
{

	ClearList();

	m_CacheManager.open();
}

VO_VOID voDownloadBufferMgr::Stop()
{
	m_CacheManager.close();
}

VO_VOID voDownloadBufferMgr::Start()
{
	Flush();
}