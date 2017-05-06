#include "voSource2WrapperManager.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSource2WrapperManager::voSource2WrapperManager(void)
: m_ullActualFileSize(0)
, m_pSourceWrapper(NULL)
{
}

voSource2WrapperManager::~voSource2WrapperManager(void)
{
	if (m_pSourceWrapper)
	{
		delete m_pSourceWrapper;
	}
}

VO_U32 voSource2WrapperManager::Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
{
	VOLOGINIT(pParam->strWorkPath);
	if (pParam)
	{
		VOLOGI("pParam %p", pParam);
	}

	VOLOGI("nFlag %d", nFlag);
	if ( NULL != pParam &&
		(pParam->uFlag & VO_SOURCE2_FLAG_INIT_ACTUALFILESIZE) )
	{
		VOLOGI("Push Play");
		m_pSourceWrapper = new voSource2WrapperPushPlay();
	}
	else
	{
		m_pSourceWrapper = new voSource2ParserWrapper();
	}

	return m_pSourceWrapper->Init(pSource, nFlag, pParam);
}


VO_U32 voSource2WrapperManager::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if (VO_PID_SOURCE2_ACTUALFILESIZE == nParamID)
	{
		VO_U64 *p = static_cast<VO_U64*>(pParam);
		m_ullActualFileSize = *p;
	}

	return m_pSourceWrapper->SetParam(nParamID, pParam);
}
