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
#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#endif

#include "CAudioResample.h"

#define LOG_TAG "CAudioResample"
#include "voLog.h"

//#pragma warning (disable : 4996)

CAudioResample::CAudioResample(void)
	: m_hResample (NULL)
{
	LoadDll ();
}

CAudioResample::~CAudioResample ()
{
	Release ();
}

OMX_U32 CAudioResample::SetFormat (OMX_S32 nInRate, OMX_S32 nInChannels, OMX_S32 nOutRate, OMX_U32 nOutChannels)
{
	if (m_hResample == NULL)
		return -1;

	m_rsAPI.SetParam (m_hResample, VO_PID_RESAMPLE_INRATE, &nInRate);
	m_rsAPI.SetParam (m_hResample, VO_PID_AUDIO_CHANNELS, &nInChannels);

	m_rsAPI.SetParam (m_hResample, VO_PID_RESAMPLE_OUTRATE, &nOutRate);\

	int nSelCh = VO_CHANNEL_FRONT_LEFT;
	if (nOutChannels == 2)
		nSelCh = VO_CHANNEL_FRONT_LEFT | VO_CHANNEL_FRONT_RIGHT;
	m_rsAPI.SetParam (m_hResample, VO_PID_RESAMPLE_SELECTCHS, &nSelCh);

	return 0;
}

OMX_U32 CAudioResample::ResampleAudio (VO_CODECBUFFER * pInBuf, VO_CODECBUFFER * pOutBuf)
{
	if (m_hResample == NULL)
		return -1;

	unsigned int nRC = m_rsAPI.SetInputData (m_hResample, pInBuf);

	OMX_U32 nResampleSize = 0;
	int nOutSize = pOutBuf->Length;
	do
	{
		nRC = m_rsAPI.GetOutputData (m_hResample, pOutBuf, &m_outInfo);

		if (nRC == 0)
		{
			nResampleSize = nResampleSize + pOutBuf->Length;

			pOutBuf->Buffer += pOutBuf->Length;
			pOutBuf->Length = nOutSize - nResampleSize;
		}

	}while (nRC != VO_ERR_INPUT_BUFFER_SMALL);

	pOutBuf->Length = nResampleSize;

	return 0;
}

typedef VO_S32 (VO_API * VOGETRESAMPLEAPI) (VO_AUDIO_CODECAPI * pFunc);

OMX_U32 CAudioResample::LoadDll (void)
{
	vostrcpy(m_szDllFile, _T("voResample"));
	vostrcpy(m_szAPIName, _T("voGetResampleAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGI ("LoadLib fail");
		return -1;
	}

	VOGETRESAMPLEAPI pAPI = (VOGETRESAMPLEAPI) m_pAPIEntry;
	if (pAPI == NULL)
		return -1;

	pAPI (&m_rsAPI);

	m_rsAPI.Init (&m_hResample, VO_AUDIO_CodingPCM, NULL);

	return 1;
}

OMX_U32 CAudioResample::Release (void)
{
	if (m_hResample != NULL)
	{
		m_rsAPI.Uninit (m_hResample);
		m_hResample = NULL;
	}

	return 1;
}
