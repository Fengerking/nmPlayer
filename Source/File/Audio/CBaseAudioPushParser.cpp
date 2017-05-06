#include "CBaseAudioPushParser.h"
#include "voCheck.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseAudioPushParser::CBaseAudioPushParser()
: m_hCheck(NULL)
{
	memset(&m_CallBack,0,sizeof(m_CallBack));
}

CBaseAudioPushParser::~CBaseAudioPushParser()
{
	
}

VO_U32 CBaseAudioPushParser::Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_TCHAR * pWorkingPath = (pParam != NULL) ? pParam->strWorkPath : NULL;
	VOLOGINIT(pWorkingPath);
	ret = voCheckLibInit(&m_hCheck, VO_INDEX_SRC_AUDIO, VO_LCS_WORKPATH_FLAG, 0, pWorkingPath);
	if (ret != VO_RET_SOURCE2_OK)
	{
		return ret;
	}
	if (pParam != NULL)
	{
		VO_SOURCE2_SAMPLECALLBACK *pCallBack = (VO_SOURCE2_SAMPLECALLBACK*)pParam->pInitParam;
		if (pCallBack != NULL)
		{
			m_CallBack.pUserData = pCallBack->pUserData;
			m_CallBack.SendData = pCallBack->SendData;
		}
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CBaseAudioPushParser::Uninit()
{
	VO_U32 ret = VO_ERR_SOURCE_OK;
	if (NULL != m_hCheck)
	{
		ret = voCheckLibUninit(m_hCheck);
		m_hCheck = NULL;
	}
	VOLOGUNINIT();
	return ret;
}