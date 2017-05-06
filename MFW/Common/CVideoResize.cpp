	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioResample.cpp

	Contains:	CAudioResample class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "CVideoResize.h"

#define LOG_TAG "CVideoResize"
#include "voLog.h"

typedef VO_S32 (VO_API * VOVIDEORESIZEGEEAPI) (VO_CLRCONV_DECAPI * pFunc, VO_U32 uFlag);

CVideoResize::CVideoResize(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode(hInst, pMemOP)
	, m_hResize (NULL)
{
	memset(&m_ccData, 0, sizeof(m_ccData));
}

CVideoResize::~CVideoResize()
{
	Uninit();
}

VO_U32 CVideoResize::Init()
{
	Uninit();

	if(LoadLib(m_hInst) == 0)
	{
		VOLOGE("LoadLib was failed!");
		return VO_ERR_NOT_IMPLEMENT;
	}

	VO_U32 nRC = m_funResize.CCInit(&m_hResize, NULL);
	if (nRC != VO_ERR_NONE || m_hResize == NULL)
	{
		VOLOGE("m_funResize.Init was failed! Result 0X%08X", (unsigned int)nRC);
		return VO_ERR_NOT_IMPLEMENT;
	}

	memset(&m_ccData, 0, sizeof(m_ccData));

	return nRC;
}

VO_U32 CVideoResize::Uninit()
{
	if(m_hResize != NULL)
	{
		m_funResize.CCUninit(m_hResize);
		m_hResize = NULL;
	}

	FreeLib();

	return 1;
}

VO_U32 CVideoResize::Process(VO_VIDEO_FORMAT * pInputFormat, VO_VIDEO_BUFFER * pInput, VO_VIDEO_FORMAT * pOutputFormat, VO_VIDEO_BUFFER * pOutput)
{
	if(m_hResize == NULL)
		return VO_ERR_WRONG_STATUS;

	if(m_ccData.nInType != pInput->ColorType || m_ccData.nInWidth != pInputFormat->Width || m_ccData.nInHeight != pInputFormat->Height || 
		m_ccData.nOutType != pOutput->ColorType || m_ccData.nOutWidth != pOutputFormat->Width || m_ccData.nOutHeight != pOutputFormat->Height)
	{
		m_ccData.nInType = pInput->ColorType;
		m_ccData.nInWidth = pInputFormat->Width;
		m_ccData.nInHeight = pInputFormat->Height;
		m_ccData.nInStride = pInput->Stride[0];
		m_ccData.nInUVStride = pInput->Stride[1];
		m_ccData.nOutType = pOutput->ColorType;
		m_ccData.nOutWidth = pOutputFormat->Width;
		m_ccData.nOutHeight = pOutputFormat->Height;
		m_ccData.nOutStride = pOutput->Stride[0];
		m_ccData.nOutUVStride = pOutput->Stride[1];
		m_ccData.nIsResize = 1;
		VO_U32 nRC = m_funResize.CCSetParam(m_hResize, VO_PID_CC_INIINSTANCE, &m_ccData);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE("VO_PID_CC_INIINSTANCE was failed! Result 0X%08X", (unsigned int)nRC);
			return nRC;
		}
	}

	m_ccData.pInBuf[0] = pInput->Buffer[0];
	m_ccData.pInBuf[1] = pInput->Buffer[1];
	m_ccData.pInBuf[2] = pInput->Buffer[2];
	m_ccData.pOutBuf[0] = pOutput->Buffer[0];
	m_ccData.pOutBuf[1] = pOutput->Buffer[1];
	m_ccData.pOutBuf[2] = pOutput->Buffer[2];

	return m_funResize.CCProcess (m_hResize, &m_ccData);
}

VO_U32 CVideoResize::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if(m_hResize == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock(&m_Mutex);
	return 	m_funResize.CCSetParam(m_hResize, uParamID, pData);
}

VO_U32 CVideoResize::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if(m_hResize == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock(&m_Mutex);
	return 	m_funResize.CCGetParam(m_hResize, uParamID, pData);
}

VO_U32 CVideoResize::LoadLib(VO_HANDLE hInst)
{
	vostrcpy (m_szDllFile, _T("voColorConversion"));
	vostrcpy (m_szAPIName, _T("voGetClrConvAPI"));

#if defined _WIN32
	vostrcat (m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat (m_szDllFile, _T(".so"));
#elif defined __SYMBIAN32__
	vostrcat (m_szDllFile, _T(".dll"));
#elif defined _MAC_OS	
	vostrcat (m_szDllFile, _T(".dylib"));
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOVIDEORESIZEGEEAPI pAPI = (VOVIDEORESIZEGEEAPI)m_pAPIEntry;
	pAPI (&m_funResize, 0);

	return 1;
}
