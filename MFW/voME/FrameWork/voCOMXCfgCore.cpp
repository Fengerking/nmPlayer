	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCfgCore.cpp

	Contains:	voCOMXCfgCore class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voCOMXCfgCore.h"
#include "voLog.h"

#define VOCOMP_FILE_SRC     (OMX_STRING)"OMX.VisualOn.FileSource"
#define VOCOMP_AUDIO_SNK    (OMX_STRING)"OMX.VisualOn.Audio.Sink"
#define VOCOMP_VIDEO_SNK    (OMX_STRING)"OMX.VisualOn.Video.Sink"
#define VOCOMP_CLOCK_TIM    (OMX_STRING)"OMX.VisualOn.Clock.Time"
#define VOCOMP_GVIDEO_CPE   (OMX_STRING)"OMX.VisualOn.G1Video.Capture"
#define VOCOMP_GAUDIO_CPE   (OMX_STRING)"OMX.VisualOn.G1Audio.Capture"
#define VOCOMP_FILE_SNK     (OMX_STRING)"OMX.VisualOn.FileSink"
#define VOCOMP_DATA_SRC     (OMX_STRING)"OMX.VisualOn.DataSource"
#define VOCOMP_AUDIO_EFT    (OMX_STRING)"OMX.VisualOn.Audio.Effect"
#define VOCOMP_VIDEO_EFT    (OMX_STRING)"OMX.VisualOn.Video.Effect"
#define VOCOMP_VIDEO_RSZ	(OMX_STRING)"OMX.VisualOn.Video.Resize"

voCOMXCfgCore::voCOMXCfgCore(void)
	: voCOMXBaseConfig ()
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCfgCore::~voCOMXCfgCore(void)
{

}

OMX_BOOL voCOMXCfgCore::Open (OMX_STRING pFile)
{
	OMX_BOOL bRC = voCOMXBaseConfig::Open (pFile);

	if (!bRC)
	{
		AddSection ((OMX_STRING)"Core");
#ifdef _WIN32
		AddItem ("Core", "VisualOn", "voOMXOne.dll");
#else
		AddItem ((OMX_STRING)"Core", (OMX_STRING)"VisualOn", (OMX_STRING)"voOMXOne.so");
#endif // _WIN32
	}

	return OMX_TRUE;
}

COMXCfgSect * voCOMXCfgCore::GetCoreSect (void)
{


	COMXCfgSect * pSect = FindSect ((OMX_STRING)"Core");

	return pSect;
}

OMX_STRING voCOMXCfgCore::GetSourceCompName (OMX_STRING pExt)
{


	COMXCfgItem * pItem = NULL;

	if (pExt != NULL)
		pItem = FindItem ((OMX_STRING)"FileSource", pExt);
	else
		pItem = FindItem ((OMX_STRING)"FileSource", (OMX_STRING)"ALL");

	if (pItem != NULL)
		return pItem->m_pValue;

	if (pItem == NULL)
		pItem = FindItem ((OMX_STRING)"FileSource", (OMX_STRING)"ALL");

	if (pItem != NULL)
		return pItem->m_pValue;

	return VOCOMP_FILE_SRC;
}

OMX_STRING voCOMXCfgCore::GetVideoSinkCompName (OMX_U32 nPriority)
{


	OMX_S8 szPriority[32];
	sprintf ((OMX_STRING)szPriority, ("%d"), (int)nPriority);

	COMXCfgItem * pItem = FindItem ((OMX_STRING)"VideoRender", (OMX_STRING)szPriority);

	if (pItem != NULL)
		return pItem->m_pValue;

	return VOCOMP_VIDEO_SNK;
}

OMX_STRING voCOMXCfgCore::GetAudioSinkCompName (OMX_U32 nPriority)
{


	OMX_S8 szPriority[32];
	sprintf ((OMX_STRING)szPriority, ("%d"), (int)nPriority);

	COMXCfgItem * pItem = FindItem ((OMX_STRING)"AudioRender", (OMX_STRING)szPriority);

	if (pItem != NULL)
		return pItem->m_pValue;

	return VOCOMP_AUDIO_SNK;
}

OMX_STRING voCOMXCfgCore::GetClockTimeCompName (OMX_U32 nPriority)
{


	OMX_S8 szPriority[32];
	sprintf ((OMX_STRING)szPriority, ("%d"), (int)nPriority);

	COMXCfgItem * pItem = FindItem ((OMX_STRING)"ClockTime", (OMX_STRING)szPriority);

	if (pItem != NULL)
		return pItem->m_pValue;

	return VOCOMP_CLOCK_TIM;
}

