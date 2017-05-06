	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2010				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEMetadataMediaFormat.cpp

	Contains:	CVOMEMetadataMediaFormat class file

	Written by: Neo Yang	

	Change History (most recent first):
	2010-07-21		Neo Yang			Create file

*******************************************************************************/
#define LOG_TAG "CVOMEMetadataMediaFormat"

#include <stdio.h>

#ifdef _LINUX
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX


#include "CVOMEMetadataMediaFormat.h"

#include "voOMX_FilePipe.h"
#include "voCOMXBaseConfig.h"
#include "cmnFile.h"
#include "voLog.h"

VO_Codec_Map  g_VideoDecMap[] = 
{
	{ VO_VIDEO_CodingMPEG2, (char*)"voMPEG2Dec"      },
	{ VO_VIDEO_CodingH263,  (char*)"voMPEG4Dec"      },
	{ VO_VIDEO_CodingS263,  (char*)"voMPEG4Dec"      },
	{ VO_VIDEO_CodingMPEG4, (char*)"voMPEG4Dec"      },
	{ VO_VIDEO_CodingH264,  (char*)"voH264Dec"       },
	{ VO_VIDEO_CodingWMV,   (char*)"voWMVDec"        },
	{ VO_VIDEO_CodingVC1,   (char*)"voVC1Dec"        },
	{ VO_VIDEO_CodingRV,    (char*)"voRealVideoDec"  },
	{ VO_VIDEO_CodingMJPEG, (char*)"voMJPEGDec"      },
	{ VO_VIDEO_CodingDIVX,  (char*)"voDIVX3Dec"      },
	{ VO_VIDEO_CodingVP6,   (char*)"voVP6Dec"        },
	{ -1,                   NULL              }
};

VO_Codec_Map  g_AudioDecMap[] = 
{
	{ VO_AUDIO_CodingPCM,     (char*)"voADPCMDec"   },
	{ VO_AUDIO_CodingADPCM,   (char*)"voADPCMDec"   },
	{ VO_AUDIO_CodingAMRNB,   (char*)"voAMRNBDec"   },
	{ VO_AUDIO_CodingAMRWB,   (char*)"voAMRWBDec"   },
	{ VO_AUDIO_CodingAMRWBP,  (char*)"voAMRWBPDec"  },
	{ VO_AUDIO_CodingQCELP13, (char*)"voQCELPDec"   },
	{ VO_AUDIO_CodingEVRC,    (char*)"voEVRCDec"    },
	{ VO_AUDIO_CodingAAC,     (char*)"voAACDec"     },
	{ VO_AUDIO_CodingAC3,     (char*)"voAC3Dec"     },
	{ VO_AUDIO_CodingEAC3,    (char*)"voEAC3Dec"    },
	{ VO_AUDIO_CodingFLAC,    NULL           },
	{ VO_AUDIO_CodingMP1,     (char*)"voMP3Dec"     },
	{ VO_AUDIO_CodingMP3,     (char*)"voMP3Dec"     },
	{ VO_AUDIO_CodingOGG,     NULL           },
	{ VO_AUDIO_CodingWMA,     (char*)"voWMADec"     },
	{ VO_AUDIO_CodingRA,      (char*)"voRADec"      },
	{ VO_AUDIO_CodingMIDI,    NULL           },
	{ -1,                     NULL           }
};

CVOMEMetadataMediaFormat::CVOMEMetadataMediaFormat(VO_SOURCE_READAPI funFileRead, VO_PTR hSourceFile)
:m_funFileRead (funFileRead)
,m_hSourceFile (hSourceFile)
{

}

CVOMEMetadataMediaFormat::~CVOMEMetadataMediaFormat()
{

}

VO_U32 CVOMEMetadataMediaFormat::CheckIsSupportedAudio()
{
	VO_S32  nAudioCodec = GetMediaCodec(METADATA_MF_GET_AUDIO);	
	if (nAudioCodec < 0)
		return METADATA_MF_NOTEXIST;
	else
		return CheckSupportedCodec(nAudioCodec, g_AudioDecMap);
}

VO_U32 CVOMEMetadataMediaFormat::CheckIsSupportedVideo()
{
	VO_S32  nVideoCodec = GetMediaCodec(METADATA_MF_GET_VIDEO);	
	if (nVideoCodec < 0)
		return METADATA_MF_NOTEXIST;
	else
		return CheckSupportedCodec(nVideoCodec, g_VideoDecMap);
}

VO_S32 CVOMEMetadataMediaFormat::GetMediaCodec(VO_U32 nFlag) 
{
	VO_U32 i = 0, bCodecFound = 0;
	VO_SOURCE_INFO	filInfo;
	VO_SOURCE_TRACKINFO	trkInfo;

	m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

	for (i = 0; i < filInfo.Tracks; i++)
	{
		m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
		if (nFlag == METADATA_MF_GET_AUDIO &&
				 (trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO)) // Getting the Video codec
		{
			bCodecFound = 1;
			break;
		} 
		else if (nFlag == METADATA_MF_GET_VIDEO && 
				(trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))  // Getting the Audio codec
		{
			bCodecFound = 1;
			break;
		}
	}

	if (bCodecFound) 
		return trkInfo.Codec;
	// codec not exist
	return -1;
}

VO_U32 CVOMEMetadataMediaFormat::CheckSupportedCodec(VO_U32 nCodecID, VO_Codec_Map *pCodecMap)
{
	VO_U32 i        = 0;
	VO_U32 nResult  = METADATA_MF_NOTSUPPORTED;

	for (i = 0; pCodecMap[i].m_nCodecType > 0; i++)
	{
		if (pCodecMap[i].m_nCodecType ==(int)nCodecID) 
		{
			if (VO_TRUE == CheckIsShareLibExits(pCodecMap[i].m_pCodecName))
				nResult = METADATA_MF_SUPPORTED;
			else 
				nResult = METADATA_MF_NOTSUPPORTED;

			VOLOGI ("@@@@@@@@ Match the codec:%s", pCodecMap[i].m_pCodecName);	
			break;
		}
	}

	return nResult;
}

VO_BOOL CVOMEMetadataMediaFormat::CheckIsShareLibExits(VO_PCHAR pCodecName)
{
	VO_BOOL bResult =  VO_FALSE;

#ifdef _LINUX
	if (!pCodecName)
		return VO_FALSE;

	vostrcpy(m_szDllFile, pCodecName);
	vostrcat(m_szDllFile, _T(".so"));

	VO_TCHAR libname[256];
	vostrcpy(libname, m_szDllFile);

	vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/");
	vostrcat(m_szDllFile, libname);
	m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	if (m_hDll == NULL)
	{
		vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/lib");
		vostrcat(m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	}

	if (m_hDll == NULL)
	{
		vostrcpy (m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		if (m_hDll == NULL)
		{
			vostrcpy(m_szDllFile, "lib");
			vostrcat(m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		}
	}

	if (m_hDll) 
	{
		bResult = VO_TRUE;  dlclose(m_hDll);
	}
#endif // _LINUX	

	return bResult;
}	

