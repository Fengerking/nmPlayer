#include "CacheManager.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CacheManager::CacheManager()
: m_bufferOutput(NULL)
, m_bStop(VO_TRUE)
{
}


CacheManager::~CacheManager(void)
{
}

VO_U32 CacheManager::init()
{
	VOLOGI("init");
	m_MediaCache.flush();

	open();

	return 0;
}

VO_U32 CacheManager::uninit()
{
	VOLOGI("+uninit");
	close();
	VOLOGI("-uninit");

	return 0;
}

VO_U32 CacheManager::open()
{
	m_bStop = VO_FALSE;
	return 0;
}

VO_U32 CacheManager::close()
{
	VOLOGI("+close");
	m_bStop = VO_TRUE;

	//voCAutoLock lock(&m_lock);
	VOLOGI("-close");

	return 0;
}

VO_U32 CacheManager::putSample(void* h, const _SAMPLE* pSample, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	//voCAutoLock lock(&m_lock);

	ChunkDataCache* pCache = (ChunkDataCache*)h;

	VO_U32 uRet = VO_RET_SOURCE2_OK;
	do 
	{
		uRet = pCache->putSample(pSample, eTrackType);
		if (VO_RET_SOURCE2_OK != uRet)
		{
			useChunk(pCache);
		}
	} while (VO_RET_SOURCE2_NEEDRETRY == uRet && VO_FALSE == m_bStop);

	VOLOGI("hCache %p, Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", h, eTrackType, pSample->uSize, pSample->uFlag, pSample->uTime);


	//VO_U32 uRet = VO_RET_SOURCE2_OK;
	//do 
	//{
	//	uRet = pCache->putSample(pSample, eTrackType);
	//	if (VO_RET_SOURCE2_OK != uRet)
	//	{
	//		shared_ptr<_SAMPLE> p;
	//		VO_U32 uRet2 = pCache->getSample(&p, eTrackType);
	//		if (VO_RET_SOURCE2_OK == uRet2)
	//		{
	//			do 
	//			{
	//				uRet2 = m_bufferOutput->AddBuffer(eTrackType, (VO_PTR)&p);
	//			} while (uRet2 && VO_FALSE == m_bStop);
	//		}
	//	}
	//} while (VO_RET_SOURCE2_NEEDRETRY == uRet && VO_FALSE == m_bStop);

	return uRet;
}


VO_U32 fBA(ChunkDataCache *pFrom, ChunkDataCache *pTo)
{
	const int iType[3] = {1, 2 , 9};

	for (int i = 0; i < 3; i++)
	{
		std::list< shared_ptr<_SAMPLE> > *plist1 = NULL;
		std::list< shared_ptr<_SAMPLE> > *plist2 = NULL;

		pFrom->getSamples(&plist1, (VO_SOURCE2_TRACK_TYPE)iType[i]);//VO_SOURCE2_TT_AUDIO VO_SOURCE2_TT_VIDEO VO_SOURCE2_TT_SUBTITLE	
		pTo->getSamples(&plist2, (VO_SOURCE2_TRACK_TYPE)iType[i]);

		if ( plist2->empty() )
		{
			VOLOGI("Track %d, plist2->empty()", iType[i]);
			continue;
		}
		else
		{
			VOLOGI("Track %d, plist2 s %lld, e %lld", iType[i], plist2->front()->uTime, plist2->back()->uTime);
		}

		if ( plist1->empty() )
		{
			VOLOGI("Track %d, plist1->empty()", iType[i]);
			continue;
		}
		else
		{
			VOLOGI("Track %d, plist1 s %lld, e %lld", iType[i], plist1->front()->uTime, plist1->back()->uTime);
		}

		long long ullStartPoint	= plist1->front()->uTime;
		long long ullEndPoint	= plist1->back()->uTime;

		std::list< shared_ptr<_SAMPLE> >::iterator iter		= plist2->begin();
		std::list< shared_ptr<_SAMPLE> >::iterator itere	= plist2->end();

		while (iter != itere)
		{
			if (VO_SOURCE2_TT_VIDEO == iType[i])
			{
				if (VO_SOURCE2_FLAG_SAMPLE_KEYFRAME & (*iter)->uFlag)
				{
					if (ullStartPoint <= (*iter)->uTime)
					{
						VOLOGI("Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", iType[i], (*iter)->uSize, (*iter)->uFlag, (*iter)->uTime);

						std::list< shared_ptr<_SAMPLE> >::iterator iter0	= plist1->begin();
						std::list< shared_ptr<_SAMPLE> >::iterator itere0	= plist1->end();

						while (iter0 != itere0)
						{
							if ( (*iter0)->uTime >= (*iter)->uTime )
							{
								std::list< shared_ptr<_SAMPLE> >::iterator _iter = iter0;
								_iter++;

								while (_iter != itere0)
								{
									if ( (*_iter)->uTime < (*iter)->uTime )
									{
										iter0 = _iter;
									}
									else
									{
										break;
									}

									_iter++;
								}

								plist1->erase(iter0, itere0);
								break;
							}

							iter0++;
						}

						pTo->setpos(iter, (VO_SOURCE2_TRACK_TYPE)iType[i]);

						itere0	= plist1->end();
						plist1->splice(itere0, *plist2);

						break;
					}
				}
			}

			if (VO_SOURCE2_TT_AUDIO == iType[i])
			{
				if ( (*iter)->uTime >= ullStartPoint)
				{
					VOLOGI("Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", iType[i], (*iter)->uSize, (*iter)->uFlag, (*iter)->uTime);

					std::list< shared_ptr<_SAMPLE> >::iterator iter0		= plist1->begin();
					std::list< shared_ptr<_SAMPLE> >::iterator itere0	= plist1->end();

					while (iter0 != itere0)
					{
						if ( (*iter0)->uTime >= (*iter)->uTime )
						{
							plist1->erase(iter0, itere0);
							break;
						}

						iter0++;
					}

					pTo->setpos(iter, (VO_SOURCE2_TRACK_TYPE)iType[i]);

					itere0	= plist1->end();
					plist1->splice(itere0, *plist2);

					break;
				}
			}

			iter++;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		std::list< shared_ptr<_SAMPLE> > *plist2 = NULL;
		pTo->getSamples(&plist2, (VO_SOURCE2_TRACK_TYPE)iType[i]);
		if ( !plist2->empty() )
		{
			std::list< shared_ptr<_SAMPLE> > *plist1 = NULL;
			pFrom->getSamples(&plist1, (VO_SOURCE2_TRACK_TYPE)iType[i]);//VO_SOURCE2_TT_AUDIO VO_SOURCE2_TT_VIDEO VO_SOURCE2_TT_SUBTITLE

			if ( !plist1->empty() )
			{
				if (!(plist1->front()->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) &&
					!(plist2->front()->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) &&
					plist2->front()->uTime < plist1->front()->uTime)
				{
					VOLOGW("Track %d donot have BA point", iType[i]);

					plist1->clear();
				}
			}

			std::list< shared_ptr<_SAMPLE> >::iterator itere0	= plist1->end();
			plist1->splice(itere0, *plist2);
		}
	}

	return 0;
}

VO_U32 CacheManager::mergeChunks(void* h1, void* h2 /*= NULL*/)
{
	VOLOGI("h1 %p, h2 %p", h1, h2);
	//voCAutoLock lock(&m_lock);

	if (h2 != NULL)
	{
		fBA( static_cast<ChunkDataCache*>(h1), static_cast<ChunkDataCache*>(h2) );
	}

	return 0;
}

VO_U32 CacheManager::useChunk(void* h1)
{
	VOLOGI("h1 %p", h1);
	ChunkDataCache* p1 = (ChunkDataCache*)h1;

	VO_BOOL bNeedRetry = VO_TRUE;
	while (bNeedRetry)
	{
		//voCAutoLock lock(&m_lock);

		if (VO_TRUE == m_bStop)
		{
			return -2;
		}

		bNeedRetry = VO_FALSE;

		std::list< shared_ptr<_SAMPLE> > *plist = NULL;
		p1->getSamples(&plist, VO_SOURCE2_TT_VIDEO);
		VO_U32 uRet = m_bufferOutput->AddBuffers(VO_SOURCE2_TT_VIDEO, plist);
		if (VO_RET_SOURCE2_NEEDRETRY == uRet)
			bNeedRetry = VO_TRUE;

		plist = NULL;
		p1->getSamples(&plist, VO_SOURCE2_TT_AUDIO);//VO_SOURCE2_TT_AUDIO VO_SOURCE2_TT_VIDEO VO_SOURCE2_TT_SUBTITLE
		uRet = m_bufferOutput->AddBuffers(VO_SOURCE2_TT_AUDIO, plist);
		if (VO_RET_SOURCE2_NEEDRETRY == uRet)
			bNeedRetry = VO_TRUE;

		plist = NULL;
		p1->getSamples(&plist, VO_SOURCE2_TT_SUBTITLE);
		uRet = m_bufferOutput->AddBuffers(VO_SOURCE2_TT_SUBTITLE, plist);
		if (VO_RET_SOURCE2_NEEDRETRY == uRet)
			bNeedRetry = VO_TRUE;

		if (bNeedRetry)
			voOS_SleepExitable(200, &m_bStop);
	} 

	return 0;
}


VO_VOID CacheManager::setEOS()
{
	VOLOGI("AddEOS");

	shared_ptr<_SAMPLE> pEnd(new _SAMPLE);

	pEnd->uSize = 0;
	pEnd->uFlag = VO_SOURCE2_FLAG_SAMPLE_EOS;

	VO_U32 ret = VO_RET_SOURCE2_NEEDRETRY;
	while (ret && VO_FALSE == m_bStop)
		ret = m_bufferOutput->AddBuffer(VO_SOURCE2_TT_VIDEO, (VO_PTR)&pEnd);
	ret = VO_RET_SOURCE2_NEEDRETRY;
	while (ret && VO_FALSE == m_bStop)
		ret = m_bufferOutput->AddBuffer(VO_SOURCE2_TT_AUDIO, (VO_PTR)&pEnd);
	ret = VO_RET_SOURCE2_NEEDRETRY;
	while (ret && VO_FALSE == m_bStop)
		ret = m_bufferOutput->AddBuffer(VO_SOURCE2_TT_SUBTITLE, (VO_PTR)&pEnd);
}