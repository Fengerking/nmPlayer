	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreOne.cpp

	Contains:	voCOMXCoreOne class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <assert.h>
#include "voComponentEntry.h"
#include "voCOMXFileSource.h"
#include "voCOMXAudioDec.h"
#include "voCOMXAudioSink.h"
#include "voCOMXVideoDec.h"
#include "voCOMXVideoSink.h"
#include "voCOMXClockTime.h"
#include "voCOMXG1VideoCapture.h"
#include "voCOMXG1AudioCapture.h"
#include "voCOMXVideoEnc.h"
#include "voCOMXAudioEnc.h"
#include "voCOMXFileSink.h"
#include "voCOMXDataSource.h"
#include "voCOMXAudioEffect.h"
#include "voCOMXVideoEffect.h"
#include "voCOMXVideoResize.h"
#include "voCOMXCoreOne.h"

//#include "voCOMXAudioPassthr.h"

#define LOG_TAG "voCOMXCoreOne"
#include "voLog.h"

#undef	MODULE_NAME
#define MODULE_NAME "voOMXOne"  //your module name
#define BUILD_NO 1120  // your build number of your module
#include "voVersion.c"

#define FILE_SRC     "OMX.VisualOn.FileSource"
#define AUDIO_DEC    "OMX.VisualOn.Audio.Decoder.XXX"
#define AUDIO_SNK    "OMX.VisualOn.Audio.Sink"
#define VIDEO_DEC    "OMX.VisualOn.Video.Decoder.XXX"
#define VIDEO_SNK    "OMX.VisualOn.Video.Sink"
#define CLOCK_TIM    "OMX.VisualOn.Clock.Time"
#define GVIDEO_CPE   "OMX.VisualOn.G1Video.Capture"
#define GAUDIO_CPE   "OMX.VisualOn.G1Audio.Capture"
#define VIDEO_ENC    "OMX.VisualOn.Video.Encoder.XXX"
#define AUDIO_ENC    "OMX.VisualOn.Audio.Encoder.XXX"
#define FILE_SNK     "OMX.VisualOn.FileSink"
#define DATA_SRC     "OMX.VisualOn.DataSource"
#define AUDIO_EFT    "OMX.VisualOn.Audio.Effect"
#define VIDEO_EFT    "OMX.VisualOn.Video.Effect"
#define VIDEO_RSZ	 "OMX.VisualOn.Video.Resize"
#define AUDIO_PST	 "OMX.VisualOn.Audio.Passthrough"

voCOMXCoreOne::voCOMXCoreOne(void)
	: voCOMXBaseObject ()
	, m_pWorkingPath (NULL)
{
	//voShowModuleVersion(NULL);
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCoreOne::~voCOMXCoreOne(void)
{
	OMX_U32 i = 0;
	for (i = 0; i < m_uCompNum; i++)
		voOMXMemFree (m_szCompName[i]);

	for (i = 0; i < m_uCompNum; i++)
	{
		for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
		{
			voOMXMemFree (m_szCompRoles[i][j]);
		}

		voOMXMemFree (m_szCompRoles[i]);
	}

	cmnMemShowStatus ();
}

OMX_ERRORTYPE voCOMXCoreOne::Init (OMX_PTR hInst)
{
	m_uCompNum = VOOMX_COMPONENT_NUM;

	VO_TCHAR szCfgFile[256];
	if (m_pWorkingPath != NULL)
	{
		vostrcpy (szCfgFile, m_pWorkingPath);
		vostrcat (szCfgFile, _T("vomeOne.cfg"));
	}
	else
	{
		vostrcpy (szCfgFile, _T("vomeOne.cfg"));
	}

//	VOLOGI ("The OMX One config file is %s", szCfgFile);

	if(m_cfgOne.Open((OMX_STRING)szCfgFile))
	{
		OMX_U32 nCompCount = m_cfgOne.getCompCount();

		m_uCompNum = nCompCount;

		assert(nCompCount <= VOOMX_COMPONENT_NUM);

		OMX_U32 nRolesCount = 0;
		OMX_STRING pComName = NULL;
		OMX_STRING pRole = NULL;
		for (OMX_U32 j = 0; j < nCompCount; j++)
		{
			m_szCompName[j] = (OMX_STRING) voOMXMemAlloc (128);
			if (m_szCompName[j] == NULL)
				break;
			
			pComName = m_cfgOne.getCompNameByIndex(j);
			if(pComName)
				strcpy(m_szCompName[j], pComName);
			nRolesCount = m_cfgOne.getCompRolesCount(m_szCompName[j]);
			m_nCompRoles[j] =  nRolesCount;
			m_szCompRoles[j] = (OMX_STRING *) voOMXMemAlloc (m_nCompRoles[j] * sizeof(OMX_STRING *));
			if (m_szCompRoles[j] == NULL)
				break;

			for(OMX_U32 ii = 0; ii < nRolesCount; ii++)
			{
				m_szCompRoles[j][ii] = (OMX_STRING) voOMXMemAlloc (128);
				if (m_szCompRoles[j][ii] == NULL)
					break;

				pRole = m_cfgOne.getCompRoleByIndex(m_szCompName[j], ii);
				if(pRole)
					strcpy(m_szCompRoles[j][ii], pRole);
			}
		}
	}
	else
	{

		OMX_U32 i = 0;
		for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
			m_szCompName[i] = (OMX_STRING) voOMXMemAlloc (128);


		strcpy (m_szCompName[0], FILE_SRC);
		strcpy (m_szCompName[1], AUDIO_DEC);
		strcpy (m_szCompName[2], AUDIO_SNK);
		strcpy (m_szCompName[3], VIDEO_DEC);
		strcpy (m_szCompName[4], VIDEO_SNK);
		strcpy (m_szCompName[5], CLOCK_TIM);
		strcpy (m_szCompName[6], GVIDEO_CPE);
		strcpy (m_szCompName[7], GAUDIO_CPE);
		strcpy (m_szCompName[8], VIDEO_ENC);
		strcpy (m_szCompName[9], AUDIO_ENC);
		strcpy (m_szCompName[10], FILE_SNK);
		strcpy (m_szCompName[11], DATA_SRC);
		strcpy (m_szCompName[12], AUDIO_EFT);
		strcpy (m_szCompName[13], VIDEO_EFT);
		strcpy (m_szCompName[14], VIDEO_RSZ);
		strcpy (m_szCompName[15], AUDIO_PST);

		m_nCompRoles[0] = 8;		// File Source 
		m_nCompRoles[1] = 12;		// Audio Decoder
		m_nCompRoles[2] = 1;		// Audio Render	
		m_nCompRoles[3] = 10;		// Video Decoder
		m_nCompRoles[4] = 1;		// Video Render
		m_nCompRoles[5] = 1;		// Clock
		m_nCompRoles[6] = 1;		// Video Capture
		m_nCompRoles[7] = 1;		// Audio Capture
		m_nCompRoles[8] = 3;		// video encoder
		m_nCompRoles[9] = 5;		// audio encoder
		m_nCompRoles[10] = 3;		// file sink
		m_nCompRoles[11] = 1;		// Data Source
		m_nCompRoles[12] = 1;		// Audio Effect
		m_nCompRoles[13] = 1;		// Video Effect
		m_nCompRoles[14] = 1;		// Video Resize
		m_nCompRoles[15] = 1;		// Audio Passthrough

		for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
		{
			m_szCompRoles[i] = (OMX_STRING *) voOMXMemAlloc (m_nCompRoles[i] * 4);
			if (m_szCompRoles[i] != NULL)
			{
				for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
				{
					m_szCompRoles[i][j] = (OMX_STRING) voOMXMemAlloc (128);
				}
			}
		}

		// file source component roles
		strcpy (m_szCompRoles[0][0], "file_parser.mp4");
		strcpy (m_szCompRoles[0][1], "file_parser.avi");
		strcpy (m_szCompRoles[0][2], "file_parser.asf");
		strcpy (m_szCompRoles[0][3], "file_parser.mp3");
		strcpy (m_szCompRoles[0][4], "file_parser.aac");
		strcpy (m_szCompRoles[0][5], "file_parser.amr");
		strcpy (m_szCompRoles[0][6], "file_parser.wav");
		strcpy (m_szCompRoles[0][7], "file_parser.qcp");

		// audio decoder
		strcpy (m_szCompRoles[1][0], "audio_decoder.aac");
		strcpy (m_szCompRoles[1][1], "audio_decoder.mp3");
		strcpy (m_szCompRoles[1][2], "audio_decoder.wma");
		strcpy (m_szCompRoles[1][3], "audio_decoder.amr");
		strcpy (m_szCompRoles[1][4], "audio_decoder.qcelp13");
		strcpy (m_szCompRoles[1][5], "audio_decoder.evrc");
		strcpy (m_szCompRoles[1][6], "audio_decoder.adpcm");
		strcpy (m_szCompRoles[1][7], "audio_decoder.ac3");
		strcpy (m_szCompRoles[1][8], "audio_decoder.amrwbp");
		strcpy (m_szCompRoles[1][9], "audio_decoder.eac3");
		strcpy (m_szCompRoles[1][10], "audio_decoder.ra");
		strcpy (m_szCompRoles[1][11], "audio_decoder.ogg");

		// audio render
		strcpy (m_szCompRoles[2][0], "audio_sink.pcm");

		// video decoder
		strcpy (m_szCompRoles[3][0], "video_decoder.avc");
		strcpy (m_szCompRoles[3][1], "video_decoder.mpeg4");
		strcpy (m_szCompRoles[3][2], "video_decoder.wmv");
		strcpy (m_szCompRoles[3][3], "video_decoder.h263");
		strcpy (m_szCompRoles[3][4], "video_decoder.rv");
		strcpy (m_szCompRoles[3][5], "video_decoder.s263");
		strcpy (m_szCompRoles[3][6], "video_decoder.vp6");
		strcpy (m_szCompRoles[3][7], "video_decoder.div3");
		strcpy (m_szCompRoles[3][8], "video_decoder.mpeg2");
		strcpy (m_szCompRoles[3][9], "video_decoder.vp8");

		// video sink
		strcpy (m_szCompRoles[4][0], "video_sink.yuv-rgb");

		// clock time
		strcpy (m_szCompRoles[5][0], "others_clock.time");

		// video capture
		strcpy (m_szCompRoles[6][0], "video_source.yuv");

		// audio capture
		strcpy (m_szCompRoles[7][0], "audio_source.pcm");

		// video encoder
		strcpy (m_szCompRoles[8][0], "video_encoder.mpeg4");
		strcpy (m_szCompRoles[8][1], "video_encoder.h263");
		strcpy (m_szCompRoles[8][2], "video_encoder.avc");

		// audio encoder
		strcpy (m_szCompRoles[9][0], "audio_encoder.amrnb");
		strcpy (m_szCompRoles[9][1], "audio_encoder.aac");
		strcpy (m_szCompRoles[9][2], "audio_encoder.mp3");
		strcpy (m_szCompRoles[9][3], "audio_encoder.qcelp13");
		strcpy (m_szCompRoles[9][4], "audio_encoder.evrc");

		// file sink
		strcpy (m_szCompRoles[10][0], "file_sink.mp4");
		strcpy (m_szCompRoles[10][1], "file_sink.wav");
		strcpy (m_szCompRoles[10][2], "file_sink.dump");

		// VisualOn Data Source
		strcpy (m_szCompRoles[11][0], "MeidaData.Source");

		// Audio effect
		strcpy (m_szCompRoles[12][0], "audio_decoder.effect");

		// Video effect
		strcpy (m_szCompRoles[13][0], "video_decoder.effect");

		// Video Resize
		strcpy (m_szCompRoles[14][0], "video_resize.sw");

		// Audio Passthrough
		strcpy (m_szCompRoles[15][0], "audio_passthrough");
	}

	return OMX_ErrorNone;
}

OMX_STRING voCOMXCoreOne::GetName (OMX_U32 nIndex)
{
	if (nIndex < 0 || nIndex >= m_uCompNum)
		return NULL;

	return m_szCompName[nIndex];
}

OMX_ERRORTYPE voCOMXCoreOne::LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName)
{
	voCOMXBaseComponent * pComponent = NULL;

	if (!strcmp (pName, FILE_SRC))
		pComponent = new voCOMXFileSource (pHandle);
	else if (!strcmp (pName, AUDIO_DEC))
		pComponent = new voCOMXAudioDec (pHandle);
	else if (!strcmp (pName, AUDIO_SNK))
		pComponent = new voCOMXAudioSink (pHandle);
	else if (!strcmp (pName, VIDEO_DEC))
		pComponent = new voCOMXVideoDec (pHandle);
	else if (!strcmp (pName, VIDEO_SNK))
		pComponent = new voCOMXVideoSink (pHandle);
	else if (!strcmp (pName, CLOCK_TIM))
		pComponent = new voCOMXClockTime (pHandle);
	else if (!strcmp (pName, GVIDEO_CPE))
		pComponent = new voCOMXG1VideoCapture (pHandle);
	else if (!strcmp (pName, GAUDIO_CPE))
		pComponent = new voCOMXG1AudioCapture (pHandle);
	else if (!strcmp (pName, VIDEO_ENC))
		pComponent = new voCOMXVideoEnc (pHandle);
	else if (!strcmp (pName, AUDIO_ENC))
		pComponent = new voCOMXAudioEnc (pHandle);
	else if (!strcmp (pName, FILE_SNK))
		pComponent = new voCOMXFileSink (pHandle);
	else if (!strcmp (pName, DATA_SRC))
		pComponent = new voCOMXDataSource (pHandle);
	else if (!strcmp (pName, AUDIO_EFT))
		pComponent = new voCOMXAudioEffect (pHandle);
	else if (!strcmp (pName, VIDEO_EFT))
		pComponent = new voCOMXVideoEffect (pHandle);
	else if (!strcmp (pName, VIDEO_RSZ))
		pComponent = new voCOMXVideoResize (pHandle);
// 	else if (!strcmp (pName, AUDIO_PST))
// 		pComponent = new voCOMXAudioPassthr (pHandle);
	else
		return OMX_ErrorInvalidComponentName;

	if (pComponent == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pComponent;

	APIComponentFillInterface (pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreOne::FreeComponent (OMX_COMPONENTTYPE * pHandle)
{
	pHandle->ComponentDeInit (pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreOne::GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames)
{
	if (role == NULL || pNumComps == NULL)
		return OMX_ErrorBadParameter;

	if (compNames == NULL)
	{
		*pNumComps = 1;
		return OMX_ErrorNone;
	}

	OMX_U32		i, j;
	OMX_BOOL	bFound = OMX_FALSE;
	for (i = 0; i < m_uCompNum; i++)
	{
		for (j = 0; j < m_nCompRoles[i]; j++)
		{
			if (!strcmp (role, m_szCompRoles[i][j]))
			{
				strcpy ((char *)compNames[0], m_szCompName[i]);
				bFound = OMX_TRUE;
				break;
			}
		}
	}

	if (bFound)
		return OMX_ErrorNone;

	return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE voCOMXCoreOne::GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{
	if (compName == NULL)
		return OMX_ErrorInvalidComponentName;
	if (pNumRoles == NULL)
		return OMX_ErrorBadParameter;

	OMX_U32 i = 0;
	OMX_S32 nIndex = -1;
	for (i = 0; i < m_uCompNum; i++)
	{
		if (!strcmp (compName, m_szCompName[i]))
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex < 0)
		return OMX_ErrorInvalidComponentName;

	if (roles == NULL)
	{
		*pNumRoles = m_nCompRoles[nIndex];
		return OMX_ErrorNone;
	}

	OMX_U32 nCount = *pNumRoles;
	if (nCount >= m_nCompRoles[nIndex])
		nCount = m_nCompRoles[nIndex];

	for (i = 0; i < nCount; i++)
		strcpy ((char *)roles[i], m_szCompRoles[nIndex][i]);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreOne::SetWorkingPath (OMX_STRING pWorkingPath)
{
	m_pWorkingPath = (VO_TCHAR *)pWorkingPath;

	return OMX_ErrorNone;
}
