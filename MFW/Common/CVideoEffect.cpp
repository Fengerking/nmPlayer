	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoEffect.cpp

	Contains:	CVideoEffect class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voString.h"
#include "voOSFunc.h"

#include "CVideoEffect.h"
//#include "voCaptionParser.h"

#define LOG_TAG "CVideoEffect"
#include "voLog.h"

//typedef VO_S32 (VO_API * ptr_voGetCaptionParserAPI)(VO_CAPTION_PARSER_API * pParser);

CVideoEffect::CVideoEffect (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hEffect (NULL)
	, m_funEffect (NULL)
	, m_pInputBuffer (NULL)
	, m_pUserBuffer (NULL)
	, m_nUserLength (0)
	, m_nEffectType (VO_VE_Null)
	, m_bEffectOn (VO_FALSE)
{
	memset(&m_Format, 0, sizeof(VO_VIDEO_FORMAT));
}

CVideoEffect::~CVideoEffect ()
{
	Uninit ();
}

VO_U32 CVideoEffect::Init (VO_VIDEO_FORMAT * pFormat)
{
	VO_U32 nRC = VO_ERR_NONE;

	Uninit ();

	if(pFormat)
	{
		memcpy(&m_Format, pFormat, sizeof(VO_VIDEO_FORMAT));
	}

	//memset(&m_OutUseData, 0, sizeof( VO_USERBUFFER ));

	//if (LoadLib (m_hInst) == 0)
	//{
	//	return VO_ERR_NOT_IMPLEMENT;
	//}
	
	//if(m_nEffectType == VO_VE_CloseCaptionOn)
	//{
	//	VO_CAPTION_PARSER_INIT_INFO info;
	//	memset( &info , 0 , sizeof( VO_CAPTION_PARSER_INIT_INFO ) );
	//	info.nType = VO_CAPTION_TYPE_EIA708;
	//	((VO_CAPTION_PARSER_API *)m_funEffect)->Open( &m_hEffect , &info );
	//}

	return nRC;
}

VO_U32 CVideoEffect::Uninit (void)
{
	//if(m_nEffectType == 1)
	//{
	//	if(m_funEffect && m_hEffect)
	//	{
	//		((VO_CAPTION_PARSER_API *)m_funEffect)->Close(m_hEffect);
	//	
	//		delete ((VO_CAPTION_PARSER_API *)m_funEffect);

	//		m_funEffect = NULL;

	//		m_hEffect = NULL;
	//	}
	//}

	if(m_pUserBuffer)
	{
		delete m_pUserBuffer;
		m_pUserBuffer = NULL;
		m_nUserLength = 0;
	}


	FreeLib ();	

	return 0;
}

VO_U32 CVideoEffect::SetInputData (VO_VIDEO_BUFFER * pInput)
{
	m_pInputBuffer = pInput;
	return 0;
}

VO_U32 CVideoEffect::GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pVideoInfo, VO_PTR pUseData)
{
	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;

	if(!m_bEffectOn)
	{
		return VO_ERR_NONE;
	}

	return nRC;
}

VO_U32 CVideoEffect::Flush (void)
{
	int Ret = 0;

	if(m_bEffectOn)
	{
		if(m_funEffect != NULL || m_hEffect != NULL)
		{
		}
	}

	return 0;
}

VO_U32 CVideoEffect::Enable (VO_BOOL bEnable)
{
	m_bEffectOn = bEnable;
	
	if(m_bEffectOn)
	{
		if(m_funEffect == NULL || m_hEffect == NULL)
		{
			Init(&m_Format);
		}
	}

	return 0;
}

VO_U32 CVideoEffect::SetVideoEffect (VO_VIDEO_EFFECTTYPE ve_type)
{
	m_nEffectType = ve_type;

	if(m_bEffectOn)
	{
		if(m_funEffect == NULL || m_hEffect == NULL)
		{
			Init(&m_Format);
		}
	}

	return 0;
}


VO_U32 CVideoEffect::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	int Ret = 0;

	if(m_funEffect && m_hEffect)
	{
		//if (m_nEffectType == VO_VE_CloseCaptionOn)
		//	Ret = ((VO_CAPTION_PARSER_API *)m_funEffect)->SetParam( m_hEffect , uParamID , pData);
	}

	return 	Ret;
}

VO_U32 CVideoEffect::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	int Ret = 0;

	if(m_funEffect && m_hEffect)
	{
		//if (m_nEffectType == VO_VE_CloseCaptionOn)
		//	Ret =((VO_CAPTION_PARSER_API *)m_funEffect)->GetParam( m_hEffect , uParamID , pData);
	}

	return Ret;	
}


VO_U32 CVideoEffect::LoadLib (VO_HANDLE hInst)
{
#ifdef _IOS

	return 0;
#else
	//if (m_nEffectType == VO_VE_CloseCaptionOn)
	//{
	//	vostrcpy (m_szDllFile, _T("voCaptionParser"));
	//	vostrcpy (m_szAPIName, _T("voGetCaptionParserAPI"));
	//}

	//if (CBaseNode::LoadLib (m_hInst) == 0)
	//{
	//	VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
	//	return 0;
	//}
	//
	//if (m_nEffectType == VO_VE_CloseCaptionOn)
	//{
	//	ptr_voGetCaptionParserAPI pAPI = (ptr_voGetCaptionParserAPI) m_pAPIEntry;
	//	m_funEffect = new VO_CAPTION_PARSER_API;
	//	pAPI ((VO_CAPTION_PARSER_API *)m_funEffect);
	//}

	return 	1;
#endif
}
