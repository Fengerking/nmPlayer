	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileFormatCheck.cpp

	Contains:	CFileFormatCheck class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-11-22		JBF			Create file

*******************************************************************************/
#include "voString.h"
#include "CFileFormatCheck.h"

#define LOG_TAG "CFileFormatCheck"
#include "voLog.h"
#include "cmnMemory.h"
#include "cmnFile.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const VO_U32 s_dwMP3SamplingRates[4][3] = 
{
	{11025, 12000, 8000,  },
	{0,     0,     0,     },
	{22050, 24000, 16000, },
	{44100, 48000, 32000  }
};

const VO_U32 s_dwMP3Bitrates[2][3][15] = 
{
	{
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}
	},
	{
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}
	}
};

const VO_U16 vo_ac3_frame_size_tab[38][3] = { { 64,   69,   96   },  { 64,   70,   96   },{ 80,   87,   120  }, { 80,   88,   120  },
	{ 96,   104,  144  },   { 96,   105,  144  },   { 112,  121,  168  },  { 112,  122,  168  }, { 128,  139,  192  },  { 128,  140,  192  },
	{ 160,  174,  240  }, { 160,  175,  240  }, { 192,  208,  288  }, { 192,  209,  288  }, { 224,  243,  336  },  { 224,  244,  336  },
	{ 256,  278,  384  },{ 256,  279,  384  },{ 320,  348,  480  },{ 320,  349,  480  },{ 384,  417,  576  },{ 384,  418,  576  },
	{ 448,  487,  672  }, { 448,  488,  672  },{ 512,  557,  768  },{ 512,  558,  768  },{ 640,  696,  960  },{ 640,  697,  960  }, { 768,  835,  1152 },
	{ 768,  836,  1152 }, { 896,  975,  1344 },{ 896,  976,  1344 },{ 1024, 1114, 1536 },{ 1024, 1115, 1536 },{ 1152, 1253, 1728 },  { 1152, 1254, 1728 },
	{ 1280, 1393, 1920 },  { 1280, 1394, 1920 },
};

const VO_U32 s_dwMP3Coefficients[2][3] = 
{
	{48,144,144},
	{48,144,72}
};

#define VO_CHECK_HEADER_SIZE		0x8000	//32K
#define ID3V2_HEADER_LEN			10

#define CALCULATE_SIZE_SYNCHSAFE(size_array)	((size_array[0] << 21) | (size_array[1] << 14) | (size_array[2] << 7) | size_array[3])

#define CHECK_EXT_NAME(name, type)	if(!vostrcmp(pExt, _T(#name)))\
	return type;

#define DOUBLE_CHECK_IF_FAIL		2

CFileFormatCheck::CFileFormatCheck(VO_FILE_OPERATOR* pFileOP, VO_MEM_OPERATOR* pMemOP)
	: m_pFileOP(pFileOP)
	, m_pMemOP(pMemOP)
{
	if( !m_pMemOP )
	{
		cmnMemFillPointer(0);
		m_pMemOP = &g_memOP;
	}

	if( !m_pFileOP )
	{
		cmnFileFillPointer();
		m_pFileOP = &g_fileOP;
	}
}

CFileFormatCheck::~CFileFormatCheck()
{
}

VO_FILE_FORMAT CFileFormatCheck::GetFileFormat(VO_PTCHAR pFilePath, VO_U32 dwCheckFlag)
{
	VO_TCHAR szFile[1024*2];
	vostrcpy(szFile, pFilePath);
	VO_U32 nLen = vostrlen(szFile);
	VO_PTCHAR pExt = szFile + nLen - 1;
	while(*pExt != _T ('.'))
	{
		pExt--;
		if(pExt < szFile)	// some URL has no '.', so protect not crash, East 20111020
			return VO_FILE_FFUNKNOWN;
	}
	pExt++;

	VO_PTCHAR pChar = pExt;
	VO_U32 nChars = vostrlen(pExt);
	for(VO_U32 i = 0; i < nChars; i++)
	{
		if((*pChar) <= _T('z') && (*pChar) >= _T('a'))
			*pChar = *pChar - (_T('a') - _T('A'));
		pChar++;
	}

	if(dwCheckFlag & FLAG_CHECK_AVI)
	{
		CHECK_EXT_NAME(AVI, VO_FILE_FFMOVIE_AVI)
		CHECK_EXT_NAME(DIVX, VO_FILE_FFMOVIE_AVI)
	}
	if(dwCheckFlag & FLAG_CHECK_ASF)
	{
		CHECK_EXT_NAME(ASF, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(ASFD, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WMV, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WMVD, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WMA, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WMAD, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(ASX, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WAX, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WVX, VO_FILE_FFMOVIE_ASF)
		CHECK_EXT_NAME(WMX, VO_FILE_FFMOVIE_ASF)
	}
	if(dwCheckFlag & FLAG_CHECK_MP4)
	{
		CHECK_EXT_NAME(MP4, VO_FILE_FFMOVIE_MP4)
		CHECK_EXT_NAME(M4A, VO_FILE_FFMOVIE_MP4)
		CHECK_EXT_NAME(M4V, VO_FILE_FFMOVIE_MP4)
		CHECK_EXT_NAME(3GP, VO_FILE_FFMOVIE_MP4)
		CHECK_EXT_NAME(3G2, VO_FILE_FFMOVIE_MP4)
		CHECK_EXT_NAME(3GPP, VO_FILE_FFMOVIE_MP4)
	}
	if(dwCheckFlag & FLAG_CHECK_MOV)
	{
		CHECK_EXT_NAME(MOV, VO_FILE_FFMOVIE_MOV)
	}
	if(dwCheckFlag & FLAG_CHECK_REAL)
	{
		CHECK_EXT_NAME(RM, VO_FILE_FFMOVIE_REAL)
		CHECK_EXT_NAME(RA, VO_FILE_FFMOVIE_REAL)
		CHECK_EXT_NAME(RMVB, VO_FILE_FFMOVIE_REAL)
	}
	if(dwCheckFlag & FLAG_CHECK_FLV)
	{
		//FLV
		CHECK_EXT_NAME(FLV, VO_FILE_FFMOVIE_FLV)
	}
	if(dwCheckFlag & FLAG_CHECK_MIDI)
	{
		CHECK_EXT_NAME(MID, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MIDI, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MMF, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MIDD, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(XMF, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MXMF, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MA1, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MA2, VO_FILE_FFAUDIO_MIDI)
		CHECK_EXT_NAME(MA3, VO_FILE_FFAUDIO_MIDI)
	}
	if(dwCheckFlag & FLAG_CHECK_OGG)
	{
		CHECK_EXT_NAME(OGG, VO_FILE_FFAUDIO_OGG)
	}
	if(dwCheckFlag & FLAG_CHECK_MPG)
	{
		CHECK_EXT_NAME(MPG, VO_FILE_FFMOVIE_MPG)
		CHECK_EXT_NAME(MPEG, VO_FILE_FFMOVIE_MPG)
		CHECK_EXT_NAME(DAT, VO_FILE_FFMOVIE_MPG)
		CHECK_EXT_NAME(MPV, VO_FILE_FFMOVIE_MPG)
		CHECK_EXT_NAME(MPE, VO_FILE_FFMOVIE_MPG)
	}
	if(dwCheckFlag & FLAG_CHECK_SDP)
	{
		CHECK_EXT_NAME(SDP, VO_FILE_FFAPPLICATION_SDP)
		CHECK_EXT_NAME(SDP2, VO_FILE_FFAPPLICATION_SDP)
	}
	if(dwCheckFlag & FLAG_CHECK_AMR)
	{
		CHECK_EXT_NAME(AMR, VO_FILE_FFAUDIO_AMR)
	}
	if(dwCheckFlag & FLAG_CHECK_AWB)
	{
		CHECK_EXT_NAME(AWB, VO_FILE_FFAUDIO_AWB)
	}
	if(dwCheckFlag & FLAG_CHECK_QCP)
	{
		CHECK_EXT_NAME(QCP, VO_FILE_FFAUDIO_QCP)
	}
	if(dwCheckFlag & FLAG_CHECK_WAV)
	{
		CHECK_EXT_NAME(WAV, VO_FILE_FFAUDIO_WAV)
		CHECK_EXT_NAME(WAD, VO_FILE_FFAUDIO_WAV)
	}
	if(dwCheckFlag & FLAG_CHECK_FLAC)
	{
		CHECK_EXT_NAME(FLAC, VO_FILE_FFAUDIO_FLAC)
	}
	if(dwCheckFlag & FLAG_CHECK_AU)
	{
		CHECK_EXT_NAME(AU, VO_FILE_FFAUDIO_AU)
	}
	if(dwCheckFlag & FLAG_CHECK_MKV)
	{
		CHECK_EXT_NAME(MKV, VO_FILE_FFMOVIE_MKV)
	} 
	if(dwCheckFlag & FLAG_CHECK_MP3)
	{
		CHECK_EXT_NAME(MP1, VO_FILE_FFAUDIO_MP3)
		CHECK_EXT_NAME(MP2, VO_FILE_FFAUDIO_MP3)
		CHECK_EXT_NAME(MP3, VO_FILE_FFAUDIO_MP3)
		CHECK_EXT_NAME(MPD, VO_FILE_FFAUDIO_MP3)
		CHECK_EXT_NAME(MP3-DRM, VO_FILE_FFAUDIO_MP3)
	}
	if(dwCheckFlag & FLAG_CHECK_AAC)
	{
		CHECK_EXT_NAME(AAC, VO_FILE_FFAUDIO_AAC)
	}
	if  (dwCheckFlag & FLAG_CHECK_AC3)
	{
		CHECK_EXT_NAME(AC3, VO_FILE_FFAUDIO_AC3)
	}
	if  (dwCheckFlag & FLAG_CHECK_DTS)
	{
		CHECK_EXT_NAME(DTS, VO_FILE_FFAUDIO_DTS)
	}
	if(dwCheckFlag & FLAG_CHECK_TS)
	{
		CHECK_EXT_NAME(TS, VO_FILE_FFMOVIE_TS)
	}
	if(dwCheckFlag & FLAG_CHECK_DV)
	{
		CHECK_EXT_NAME(DV, VO_FILE_FFMOVIE_DV)
	}
	if(dwCheckFlag & FLAG_CHECK_H263)
	{
		CHECK_EXT_NAME(263, VO_FILE_FFVIDEO_H263)
		CHECK_EXT_NAME(H263, VO_FILE_FFVIDEO_H263)
	}
	if(dwCheckFlag & FLAG_CHECK_H264)
	{
		CHECK_EXT_NAME(264, VO_FILE_FFVIDEO_H264)
		CHECK_EXT_NAME(H264, VO_FILE_FFVIDEO_H264)
	}
	if(dwCheckFlag & FLAG_CHECK_MPEG4)
	{
		CHECK_EXT_NAME(MPG4, VO_FILE_FFVIDEO_MPEG4)
		CHECK_EXT_NAME(MPEG4, VO_FILE_FFVIDEO_MPEG4)
		CHECK_EXT_NAME(MPV4, VO_FILE_FFVIDEO_MPEG4)
	}
	if(dwCheckFlag & FLAG_CHECK_CMMB)
	{
		CHECK_EXT_NAME(MFS, VO_FILE_FFMOVIE_CMMB)
	}
	if(dwCheckFlag & FLAG_CHECK_H265)
	{
		CHECK_EXT_NAME(265, VO_FILE_FFVIDEO_H265)
		CHECK_EXT_NAME(H265, VO_FILE_FFVIDEO_H265)
	}
	if(dwCheckFlag & FLAG_CHECK_RAWDATA)
	{
		CHECK_EXT_NAME(RGB555, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(RGB565, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(RGB24, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(RGB32, VO_FILE_FFVIDEO_RAWDATA)

		CHECK_EXT_NAME(YUV, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(YVU, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(AYUV, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(UYVY, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(Y411, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(Y211, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(YUY2, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(YVYU, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(YUYV, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(IYUV, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(YV12, VO_FILE_FFVIDEO_RAWDATA)
		CHECK_EXT_NAME(YVU9, VO_FILE_FFVIDEO_RAWDATA)
	}
	if( dwCheckFlag & FLAG_CHECK_APE )
	{
		CHECK_EXT_NAME(APE,VO_FILE_FFAUDIO_APE)
		CHECK_EXT_NAME(APL,VO_FILE_FFAUDIO_APE)
		CHECK_EXT_NAME(MAC,VO_FILE_FFAUDIO_APE)
	}
	if( dwCheckFlag & FLAG_CHECK_ALAC )
	{
		CHECK_EXT_NAME(ALAC,VO_FILE_FFAUDIO_ALAC)
		CHECK_EXT_NAME(M4A,VO_FILE_FFAUDIO_ALAC)
	}

	return VO_FILE_FFUNKNOWN;
}

VO_FILE_FORMAT CFileFormatCheck::GetFileFormat(VO_FILE_SOURCE* pSource, VO_U32 dwCheckFlag, VO_U32 dwSgstFlag /* = FLAG_CHECK_NONE */)
{
	VO_FILE_FORMAT voFF = VO_FILE_FFUNKNOWN;
	if(pSource == NULL)
		return voFF;

	VO_S32 nHeadSize = VO_CHECK_HEADER_SIZE;
	VO_PBYTE pHeadData = NULL;

	voMemAlloc(pHeadData, m_pMemOP, 0, nHeadSize + 1);
	if(!pHeadData)
		return voFF;

	VO_PTR hFile = m_pFileOP->Open(pSource);

	if(hFile == NULL)
	{
		m_pMemOP->Free(0, pHeadData);

		return voFF;
	}

	nHeadSize = -2;

	while(-2 == nHeadSize)
		nHeadSize = m_pFileOP->Read(hFile, pHeadData, VO_CHECK_HEADER_SIZE);
	//add \0 here to avoid possible crash when using strstr
	pHeadData[VO_CHECK_HEADER_SIZE] = 0;

	if(nHeadSize <= 0)
	{
		m_pMemOP->Free(0, pHeadData);
		m_pFileOP->Close(hFile);

		return voFF;
	}

	VO_S64 llFileHeaderSize = 0;
	while(SkipID3V2Header(hFile, pHeadData, nHeadSize, llFileHeaderSize)) {}

	m_pFileOP->Close(hFile);

	//check by suggest file format!!
	dwSgstFlag &= dwCheckFlag;	//check only in check flags
	if((dwSgstFlag & FLAG_CHECK_AVI) && IsAVI(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_AVI;
	else if((dwSgstFlag & FLAG_CHECK_ASF) && IsASF(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_ASF;
	else if((dwSgstFlag & FLAG_CHECK_MP4) && IsMP4(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_MP4;
	else if((dwSgstFlag & FLAG_CHECK_MOV) && IsMOV(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_MOV;
	else if((dwSgstFlag & FLAG_CHECK_REAL) && IsREAL(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_REAL;
	else if((dwSgstFlag & FLAG_CHECK_FLV) && IsFLV(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_FLV;
	else if((dwSgstFlag & FLAG_CHECK_MIDI) && IsMID(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_MIDI;
	else if((dwSgstFlag & FLAG_CHECK_OGG) && IsOGG(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_OGG;
	else if((dwSgstFlag & FLAG_CHECK_MPG) && IsMPG(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_MPG;
	else if((dwSgstFlag & FLAG_CHECK_SDP) && IsSDP(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAPPLICATION_SDP;
	else if((dwSgstFlag & FLAG_CHECK_AMR) && IsAMR(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_AMR;
	else if((dwSgstFlag & FLAG_CHECK_AWB) && IsAWB(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_AWB;
	else if((dwSgstFlag & FLAG_CHECK_QCP) && IsQCP(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_QCP;
	else if((dwSgstFlag & FLAG_CHECK_WAV) && IsWAV(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_WAV;
	else if((dwSgstFlag & FLAG_CHECK_FLAC) && IsFLAC(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_FLAC;
	else if((dwSgstFlag & FLAG_CHECK_AU) && IsAU(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_AU;
	else if((dwSgstFlag & FLAG_CHECK_MKV) && IsMKV(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_MKV;
	else if((dwSgstFlag & FLAG_CHECK_MP3) && IsMP3(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_MP3;
	else if((dwSgstFlag & FLAG_CHECK_AAC) && IsAAC(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_AAC;
	else if((dwSgstFlag & FLAG_CHECK_AC3) && IsAC3(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_AC3;
	else if((dwSgstFlag & FLAG_CHECK_APE) && IsAPE(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_APE;
	else if((dwSgstFlag & FLAG_CHECK_DTS) && IsDTS(pHeadData, nHeadSize))
		voFF = VO_FILE_FFAUDIO_DTS;
	else if((dwSgstFlag & FLAG_CHECK_TS) && IsTS(pHeadData, nHeadSize))
		voFF = VO_FILE_FFMOVIE_TS;
	else if((dwSgstFlag & FLAG_CHECK_WEBVTT) && IsWEBVTT(pHeadData, nHeadSize))
		voFF = VO_FILE_FFSUBTITLE_WEBVTT;
	

	if(VO_FILE_FFUNKNOWN == voFF)
	{
		//not suggest file format
		dwCheckFlag &= (~dwSgstFlag);	//not to check checked file format!!
		if((dwCheckFlag & FLAG_CHECK_AVI) && IsAVI(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_AVI;
		else if((dwCheckFlag & FLAG_CHECK_ASF) && IsASF(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_ASF;
		else if((dwCheckFlag & FLAG_CHECK_MP4) && IsMP4(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_MP4;
		else if((dwCheckFlag & FLAG_CHECK_MOV) && IsMOV(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_MOV;
		else if((dwCheckFlag & FLAG_CHECK_REAL) && IsREAL(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_REAL;
		else if((dwCheckFlag & FLAG_CHECK_FLV) && IsFLV(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_FLV;
		else if((dwCheckFlag & FLAG_CHECK_MIDI) && IsMID(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_MIDI;
		else if((dwCheckFlag & FLAG_CHECK_OGG) && IsOGG(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_OGG;
		else if((dwCheckFlag & FLAG_CHECK_MPG) && IsMPG(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_MPG;
		else if((dwCheckFlag & FLAG_CHECK_SDP) && IsSDP(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAPPLICATION_SDP;
		else if((dwCheckFlag & FLAG_CHECK_AMR) && IsAMR(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_AMR;
		else if((dwCheckFlag & FLAG_CHECK_AWB) && IsAWB(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_AWB;
		else if((dwCheckFlag & FLAG_CHECK_QCP) && IsQCP(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_QCP;
		else if((dwCheckFlag & FLAG_CHECK_WAV) && IsWAV(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_WAV;
		else if((dwCheckFlag & FLAG_CHECK_FLAC) && IsFLAC(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_FLAC;
		else if((dwCheckFlag & FLAG_CHECK_AU) && IsAU(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_AU;
		else if((dwCheckFlag & FLAG_CHECK_MKV) && IsMKV(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_MKV;
		else if((dwCheckFlag & FLAG_CHECK_MP3) && IsMP3(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_MP3;
		else if((dwCheckFlag & FLAG_CHECK_AAC) && IsAAC(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_AAC;
		else if((dwCheckFlag & FLAG_CHECK_AC3) && IsAC3(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_AC3;
		else if((dwCheckFlag & FLAG_CHECK_APE) && IsAPE(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_APE;
		else if((dwCheckFlag & FLAG_CHECK_DTS) && IsDTS(pHeadData, nHeadSize))
			voFF = VO_FILE_FFAUDIO_DTS;
		else if((dwCheckFlag & FLAG_CHECK_TS) && IsTS(pHeadData, nHeadSize))
			voFF = VO_FILE_FFMOVIE_TS;
		else if((dwCheckFlag & FLAG_CHECK_WEBVTT) && IsWEBVTT(pHeadData, nHeadSize))
			voFF = VO_FILE_FFSUBTITLE_WEBVTT;
	}

	m_pMemOP->Free(0, pHeadData);

	return voFF;
}

// by Jason Gao
VO_BOOL CFileFormatCheck::IsMP4 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if(pHeadData == NULL || nHeadSize < 8)
		return VO_FALSE;

	VO_U32 dwFileTypeBoxSize = 0;
	VO_PBYTE p = pHeadData + 4;
	if(p[0] == 'f' && p[1] == 't' && p[2] == 'y' && p[3] == 'p')
	{
		if(nHeadSize < 12)
			return VO_FALSE;

		if ( (p[4] == '3') && ((p[5] == 'g') || (p[5] == 'G')) ) //3gp, 3g2
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'p') || (p[5] == 'P')) ) //mp4, mp42
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'o') || (p[5] == 'O')) ) //mp4, MOV
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'm') || (p[5] == 'M')) ) //MMP4
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 's') || (p[5] == 'S')) ) //MSNV
			return VO_TRUE;
		if ( ((p[4] == 'w') || (p[4] == 'W')) && ((p[5] == 'm') || (p[5] == 'M')) ) //MSNV
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'v') || (p[5] == 'V')) ) //mp4, MV4
			return VO_TRUE;
		if ( ((p[4] == 'a') || (p[4] == 'A')) && ((p[5] == 'v') || (p[5] == 'V')) ) //mp4, MV4
			return VO_TRUE;
		if ( ((p[4] == 'i') || (p[4] == 'I')) && ((p[5] == 's') || (p[5] == 'S')) ) //mp4, m4v
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && (p[5] == '4') ) //mp4, mp4
			return VO_TRUE;
		if ( (p[4] == 's') && ((p[5] == 'k') || (p[5] == 'm')) ) //3gp, 3g2
			return VO_TRUE;
		if ( (p[4] == 'k') && (p[5] == '3') && (p[6] == 'g') ) //k3g1...
			return VO_TRUE;

		// 3GPP2 EZmovie for KDDI 3G cellphones
		if((p[4] == 'k' || p[4] == 'K') && 
			(p[5] == 'd' || p[5] == 'D') && 
			(p[6] == 'd' || p[6] == 'D') && 
			(p[7] == 'i' || p[7] == 'I'))
			return VO_TRUE;

		dwFileTypeBoxSize = (pHeadData[0] << 24) | (pHeadData[1] << 16) | (pHeadData[2] << 8) | pHeadData[3];
	}
	else if((p[0] == 's' && p[1] == 'k' && p[2] == 'i' && p[3] == 'p') || 
		(p[0] == 'f' && p[1] == 'r' && p[2] == 'e' && p[3] == 'e'))	// some mp4 file use 'free' box instead of 'skip' box, East 20111008
		dwFileTypeBoxSize = (pHeadData[0] << 24) | (pHeadData[1] << 16) | (pHeadData[2] << 8) | pHeadData[3];

	//if major brand can not be identified, we add check to 'moov' or 'mdat'
	//East 2010/02/26
	if((VO_U32)nHeadSize < dwFileTypeBoxSize + 8)
		return VO_FALSE;

	p = pHeadData + dwFileTypeBoxSize + 4;
	if((p[0] == 'm' && p[1] == 'o' && p[2] == 'o' && p[3] == 'v') || 
		(p[0] == 'm' && p[1] == 'd' && p[2] == 'a' && p[3] == 't'))
		return VO_TRUE;

	return VO_FALSE;
}

// by Jason Gao
VO_BOOL CFileFormatCheck::IsMOV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 10)
		return VO_FALSE;

	VO_PBYTE p = pHeadData + 4;
	if (p[0] != 'f') return VO_FALSE;
	if (p[1] != 't') return VO_FALSE;
	if (p[2] != 'y') return VO_FALSE;
	if (p[3] != 'p') return VO_FALSE;
	if ( (p[4] == 'q') && (p[5] == 't') ) //qt
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsASF (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 16)
		return VO_FALSE;

	//{75b22630-668e-11cf-a6d9-00aa0062ce6c}
	VO_PBYTE p = pHeadData;
	if(p[0] != 0x30) return VO_FALSE;
	if(p[1] != 0x26) return VO_FALSE;
	if(p[2] != 0xb2) return VO_FALSE;
	if(p[3] != 0x75) return VO_FALSE;
	if(p[4] != 0x8e) return VO_FALSE;
	if(p[5] != 0x66) return VO_FALSE;
	if(p[6] != 0xcf) return VO_FALSE;
	if(p[7] != 0x11) return VO_FALSE;
	if(p[8] != 0xa6) return VO_FALSE;
	if(p[9] != 0xd9) return VO_FALSE;
	if(p[10] != 0x00) return VO_FALSE;
	if(p[11] != 0xaa) return VO_FALSE;
	if(p[12] != 0x00) return VO_FALSE;
	if(p[13] != 0x62) return VO_FALSE;
	if(p[14] != 0xce) return VO_FALSE;
	if(p[15] != 0x6c) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsAVI (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"AVI "
	if(p[0] != 0x41) return VO_FALSE;
	if(p[1] != 0x56) return VO_FALSE;
	if(p[2] != 0x49) return VO_FALSE;
	if(p[3] != 0x20) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsREAL (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//".RMF"
	if(p[0] != 0x2e) return VO_FALSE;
	if(p[1] != 0x52) return VO_FALSE;
	if(p[2] != 0x4d) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;

	return VO_TRUE;
}

VO_PBYTE CFileFormatCheck::MPGFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen;
	VO_U32 LeftData = 0xFFFFFFFF;
	const VO_U32 wAVCSyncWord = 0x000001BA;///<or 0x000001
	while(pHead < pTail)
	{
		LeftData <<= 8;
		LeftData |= *pHead++;
		if (LeftData == wAVCSyncWord)
		{
			return pHead;
		}
	}
	return NULL;
}

VO_BOOL CFileFormatCheck::MPGParseHeader(VO_PBYTE pHeader, VO_U32 dwLen)
{
	if (dwLen < 11)///<8 bytes's MPEG1 Packer header+ next 3 bytes sync word
	{
		return VO_FALSE;
	}
	VO_PBYTE pHead = pHeader;
	VO_BOOL beMPEG2 = ((pHead[0] >> 6)==0x01)?VO_TRUE:VO_FALSE;
	if(!beMPEG2 && (pHead[0] >> 4) != 0x2)///<not mpeg1
		return VO_FALSE;
	
	if (beMPEG2)
	{
		if (dwLen < 13)///<10 bytes's MPEG1 Packer header+ next 3 bytes's sync word
		{
			return VO_FALSE;
		}
		if(((pHead[0]&0x4) != 0x4) || ((pHead[2]&0x4) != 0x4) 
			||((pHead[4]&0x4) != 0x4) || ((pHead[5]&0x1) != 0x1) 
			|| ((pHead[8] & 0x3) != 0x3))
		{
			return VO_FALSE;
		}
		VO_U32 uStuffByte = pHead[9] & 0x3;
		pHead += 10 + uStuffByte;

	}
	else
	{
		if (((pHead[0] & 0x1) != 0x1) || ((pHead[2] & 0x1) != 0x1)
			|| ((pHead[4] & 0x1) != 0x1) || ((pHead[5] & 0x80) != 0x80)|| ((pHead[7] & 0x1) != 0x1)
			)
		{
			return VO_FALSE;
		}
		pHead += 8;
	}
	if (pHead[0] != 0x0 || pHead[1] != 0x0 || pHead[2] != 0x1)///<next synec work 0x000001
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsMPG (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{	
	if (!pHeadData || nHeadSize < 15)///<4 bytes's syncword + 8 bytes's MPEG1 Packer header+ next 3 bytes sync word
	{
		return VO_FALSE;
	}
	VO_PBYTE pMPGHead = MPGFindHeadInBuffer(pHeadData,nHeadSize);
	
	if(!pMPGHead || !MPGParseHeader(pMPGHead,nHeadSize - (pMPGHead - pHeadData)))
		return VO_FALSE;
	
	return VO_TRUE; 
}

VO_BOOL CFileFormatCheck::IsAAC (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	//check if ADIF format!!
	if(pHeadData[0] == 0x41 && pHeadData[1] == 0x44 && pHeadData[2] == 0x49 && pHeadData[3] == 0x46)
		return VO_TRUE;

	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pAACHead = NULL;
	VO_BYTE btAACHead[4];

	VO_U32 dwFrameSize = 0;
	VO_S32 nDoubleCheck = 0;
	while( (pAACHead = AACFindHeadInBuffer(pBuf, dwBufSize)) != 0) //MCW_
	{
		dwBufSize -= (pAACHead - pBuf);
		pBuf = pAACHead;
		if(AACParseHeader(pAACHead, &dwFrameSize))
		{
			m_pMemOP->Copy(0, btAACHead, pAACHead, sizeof(btAACHead));
			btAACHead[3] &= 0xF0;

			//check next header!!
			if(dwBufSize < dwFrameSize + 6)
				return VO_FALSE;

			pAACHead = pBuf + dwFrameSize;
			if(!m_pMemOP->Compare(0, btAACHead, pAACHead, 3) && btAACHead[3] == (pAACHead[3] & 0xF0))	//this header is what we want to find!!
			{
				nDoubleCheck--;
				if(nDoubleCheck < 0)
					return VO_TRUE;

				// skip whole frame data
				dwBufSize -= dwFrameSize;
				pBuf += dwFrameSize;
				continue;
			}
			else
				nDoubleCheck = DOUBLE_CHECK_IF_FAIL;
		}
		else
			nDoubleCheck = DOUBLE_CHECK_IF_FAIL;

		dwBufSize--;
		pBuf++;
	}

	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsAC3 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{

	VO_U16 FirstFrmPos = 0;
	VO_U16 syn_word = 0;
	VO_U8 frmsizecode = 0;
	VO_U8 streadid = 0;
	VO_U16 framesize = 0;
	VO_PBYTE p = pHeadData;
	while(VO_TRUE){
		syn_word = (((syn_word&0x0000)|(p[FirstFrmPos]))<<8)|(((syn_word&0xFF00)|p[FirstFrmPos+1]));
		if(0x0B77 == syn_word)
		{
			//ACW_UNUSED <strmid_frmsize>: VO_U8 strmid_frmsize = 0;
			frmsizecode = p[FirstFrmPos + 4]&0x3F;
			streadid = (p[FirstFrmPos + 4]&0xC0)>>6;
			framesize = vo_ac3_frame_size_tab[frmsizecode][streadid]*2;
			syn_word = (((syn_word&0x0000)|(p[FirstFrmPos + framesize]))<<8)|(((syn_word&0xFF00)|p[FirstFrmPos + framesize + 1]));
			if(0x0B77 == syn_word)// a ac3 format file
			{
				break;
			}

		}
		if((FirstFrmPos + 2) < nHeadSize)
			FirstFrmPos += 2;
		else
			break;
	}	
	if(FirstFrmPos + 2 >= nHeadSize)
		return VO_FALSE;
	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsAMR (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 6)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"#!AMR\n"
	if(p[0] != 0x23) return VO_FALSE;
	if(p[1] != 0x21) return VO_FALSE;
	if(p[2] != 0x41) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;
	if(p[4] != 0x52) return VO_FALSE;
	if(p[5] != 0x0a) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsAWB (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 9)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"#!AMR-WB\n"
	if(p[0] != 0x23) return VO_FALSE;
	if(p[1] != 0x21) return VO_FALSE;
	if(p[2] != 0x41) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;
	if(p[4] != 0x52) return VO_FALSE;
	if(p[5] != 0x2d) return VO_FALSE;
	if(p[6] != 0x57) return VO_FALSE;
	if(p[7] != 0x42) return VO_FALSE;
	if(p[8] != 0x0a) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsDTS(VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	// 0x7FFE8001
	if(p[0] != 0x7F) return VO_FALSE;
	if(p[1] != 0xFE) return VO_FALSE;
	if(p[2] != 0x80) return VO_FALSE;
	if(p[3] != 0x01) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsMP3 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pMP3Head = NULL;
	VO_BYTE btMP3Head[4];

	VO_U32 dwFrameSize = 0;
	VO_S32 nDoubleCheck = 0;
	while ((pMP3Head = MP3FindHeadInBuffer(pBuf, dwBufSize)) != 0)
	{
		dwBufSize -= (pMP3Head - pBuf);
		pBuf = pMP3Head;

		if(MP3ParseHeader(pMP3Head, &dwFrameSize))
		{
			m_pMemOP->Copy(0, btMP3Head, pMP3Head, sizeof(btMP3Head));
			btMP3Head[2] &= 0x0C;
			btMP3Head[3] &= 0x8C;

			//check next header!!
			if(dwBufSize < dwFrameSize + 4)
				return VO_FALSE;

			pMP3Head = pBuf + dwFrameSize;

			if(!m_pMemOP->Compare(0, btMP3Head, pMP3Head, 2) && btMP3Head[2] == (pMP3Head[2] & 0x0C) && btMP3Head[3] == (pMP3Head[3] & 0x8C))	//this header is what we want to find!!
			{
				nDoubleCheck--;
				if(nDoubleCheck < 0)
					return VO_TRUE;

				// skip whole frame data
				dwBufSize -= dwFrameSize;
				pBuf += dwFrameSize;
				continue;
			}
			else
				nDoubleCheck = DOUBLE_CHECK_IF_FAIL;
		}
		else
			nDoubleCheck = DOUBLE_CHECK_IF_FAIL;

		dwBufSize--;
		pBuf++;
	}

	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsQCP (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"QLCM"
	if(p[0] != 0x51) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x43) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsWAV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"WAVE"
	if(p[0] != 0x57) return VO_FALSE;
	if(p[1] != 0x41) return VO_FALSE;
	if(p[2] != 0x56) return VO_FALSE;
	if(p[3] != 0x45) return VO_FALSE;

	return VO_TRUE;
}

// by Jason Gao
VO_BOOL CFileFormatCheck::IsMID (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"MThd", "MMMD", "XMF_", "RIFF...MThd"
	if(p[0] == 'M' && p[1] == 'T' && p[2] == 'h' && p[3] == 'd') return VO_TRUE;	/* SMF */
	if(p[0] == 'M' && p[1] == 'M' && p[2] == 'M' && p[3] == 'D') return VO_TRUE;	/* MMF */
	if(p[0] == 'X' && p[1] == 'M' && p[2] == 'F' && p[3] == '_') return VO_TRUE;	/* XMF or MXMF*/
	if(p[0] == 'R' && p[1] == 'I' && p[2] == 'F' && p[3] == 'F')
	{
		if(nHeadSize < 24)
			return VO_FALSE;

		p += 20;
		if(p[0] == 'M' && p[1] == 'T' && p[2] == 'h' && p[3] == 'd') return VO_TRUE;	/* SMF */	
	}

	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsFLAC (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"fLaC"
	if(p[0] != 0x66) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x61) return VO_FALSE;
	if(p[3] != 0x43) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsAU (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if(!pHeadData || nHeadSize < 4)
		return VO_FALSE;

	//".snd"
	if(pHeadData[0] != 0x2e) return VO_FALSE;
	if(pHeadData[1] != 0x73) return VO_FALSE;
	if(pHeadData[2] != 0x6e) return VO_FALSE;
	if(pHeadData[3] != 0x64) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsOGG (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"OggS"
	if(p[0] != 0x4f) return VO_FALSE;
	if(p[1] != 0x67) return VO_FALSE;
	if(p[2] != 0x67) return VO_FALSE;
	if(p[3] != 0x53) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsFLV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 3)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"FLV"
	if(p[0] != 0x46) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x56) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL	CFileFormatCheck::IsMKV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//IS EBML
	if(p[0] != 0x1a) return VO_FALSE;
	if(p[1] != 0x45) return VO_FALSE;
	if(p[2] != 0xdf) return VO_FALSE;
	if(p[3] != 0xa3) return VO_FALSE;

	return VO_TRUE;
}

VO_PBYTE CFileFormatCheck::FindTSPacketHeader(VO_PBYTE pData, VO_S32 nDataSize, VO_S32 nPacketSize)
{
	VO_PBYTE p1 = pData;
	VO_PBYTE p2 = pData + nDataSize - nPacketSize;
	while((p1 < p2) && (p1 - pData <= nPacketSize))
	{
		if((*p1 == 0x47) && (*(p1 + nPacketSize) == 0x47))
			return p1;
		p1++;
	}
	return NULL;
}

VO_BOOL CFileFormatCheck::FindTSPacketHeader2(VO_PBYTE pData, VO_S32 cbData, VO_S32 packetSize)
{
	VO_PBYTE p = FindTSPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		VO_PBYTE p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
		{
			if (*p2 == 0x47)
				return VO_TRUE;
		}
	}
	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsTS (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL)
		return VO_FALSE;

	if (nHeadSize >= 189 && FindTSPacketHeader2(pHeadData, nHeadSize, 188))
		return VO_TRUE;

	if (nHeadSize >= 193 && FindTSPacketHeader2(pHeadData, nHeadSize, 192))
		return VO_TRUE;

	if (nHeadSize >= 205 && FindTSPacketHeader2(pHeadData, nHeadSize, 204))
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsSDP (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::SkipID3V2Header(VO_PTR pFile, VO_PBYTE pHeadData, VO_S32& nHeadSize, VO_S64& llFileHeaderSize)
{
	if(nHeadSize < ID3V2_HEADER_LEN)
		return VO_FALSE;

	static const VO_CHAR byteID3[3] = {0x49, 0x44, 0x33};
	if(m_pMemOP->Compare(0, pHeadData, (VO_PTR)byteID3, 3))	//no ID3v2 information!!
		return VO_FALSE;

	//calculate the size and skip it!!
	VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE((pHeadData + 6));
	llFileHeaderSize += (dwSize + ID3V2_HEADER_LEN);
	if(-1 == m_pFileOP->Seek(pFile, llFileHeaderSize, VO_FILE_BEGIN))
	{
		nHeadSize = 0;
		return VO_FALSE;
	}

	VO_S32 nRes = -2;
	while(-2 == nRes)
		nRes = m_pFileOP->Read(pFile, pHeadData, VO_CHECK_HEADER_SIZE);

	nHeadSize = (nRes > 0) ? nRes : 0;

	return VO_TRUE;
}

VO_PBYTE CFileFormatCheck::MP3FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 4)
		return NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 3;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xE0 != (pHead[1] & 0xE0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 111XXXXX found!!
		return pHead;
	}

	return NULL;
}

VO_BOOL CFileFormatCheck::MP3ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize)
{
	//check version
	VO_BYTE ver = (pHeader[1] >> 3) & 0x03;
	if(1 == ver)
		return VO_FALSE;

	//check layer
	VO_BYTE layer = 3 - ((pHeader[1] >> 1) & 0x03);
	if(3 == layer)
		return VO_FALSE;

	//check sampling rate
	VO_BYTE idx = (pHeader[2] >> 2) & 0x03;
	if(0x03 == idx)	//reserved
		return VO_FALSE;

	VO_U32 dwSamplesPerSec = s_dwMP3SamplingRates[ver][idx];

	//check bit rate
	idx = (pHeader[2] >> 4) & 0x0F;
	if(0xF == idx)	//free or bad
		return VO_FALSE;

	VO_BYTE bLSF = (3 != ver) ? 1 : 0;
	VO_U32 dwBitrate = s_dwMP3Bitrates[bLSF][layer][idx] * 1000;	//KBit to Bit
	if(!dwBitrate)
		return VO_FALSE;

	if(pdwFrameSize)
	{
		//padding bit
		VO_BYTE btPaddingBit = (pHeader[2] >> 1) & 0x01;

		//frame size;
		if(0 == layer)
			*pdwFrameSize = (12 * dwBitrate / dwSamplesPerSec + btPaddingBit) * 4;
		else
			*pdwFrameSize = s_dwMP3Coefficients[bLSF][layer] * dwBitrate / dwSamplesPerSec + btPaddingBit;
	}

	return VO_TRUE;
}

VO_PBYTE CFileFormatCheck::AACFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 6)
		return NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 5;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xF0 != (pHead[1] & 0xF0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 1111XXXX found!!
		return pHead;
	}

	return NULL;
}

VO_BOOL CFileFormatCheck::AACParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize)
{
	//check ID
	//if(!(pHeader[1] & 0x8))
	//	return VO_FALSE;

	//check Layer
	//if(pHeader[1] & 0x6)
	//	return VO_FALSE;

	//check Profile (profile=3 is reserved, but the file could be played still.)
	//if(0x3 == (pHeader[2] >> 6))
	//	return VO_FALSE;

	//check Sampling frequency index (0xC-0xF are reserved.)
	//if(((pHeader[2] >> 2) & 0xF) > 0xB)
	//	return VO_FALSE;

	// because length of frame header = 7
	VO_U32 dwFrameSize = ((pHeader[3] & 0x03) << 11) | (pHeader[4] << 3) | (pHeader[5] >> 5);
	//if(dwFrameSize < 6)
	if(dwFrameSize < 7)
		return VO_FALSE;

	if(pdwFrameSize)
		*pdwFrameSize = dwFrameSize;

	return VO_TRUE;
}

VO_BOOL CFileFormatCheck::IsAPE (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if( pHeadData[0] == 'M' &&
		pHeadData[1] == 'A' &&
		pHeadData[2] == 'C' &&
		pHeadData[3] == ' ' )
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL	CFileFormatCheck::IsWEBVTT(VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if(!pHeadData)
		return VO_FALSE;
	if(strstr((VO_CHAR*)pHeadData,"WEBVTT"))
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL CFileFormatCheck::IsTTML(VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if(!pHeadData)
		return VO_FALSE;
	if(strstr((VO_CHAR*)pHeadData,"<?xml"))
		return VO_TRUE;

	return VO_FALSE;
}

VO_FILE_FORMAT CFileFormatCheck::Flag2FileFormat(VO_U32 dwFlag)
{
	switch(dwFlag)
	{
	case FLAG_CHECK_AVI:
		return VO_FILE_FFMOVIE_AVI;

	case FLAG_CHECK_ASF:
		return VO_FILE_FFMOVIE_ASF;

	case FLAG_CHECK_MP4:
		return VO_FILE_FFMOVIE_MP4;

	case FLAG_CHECK_MOV:
		return VO_FILE_FFMOVIE_MOV;

	case FLAG_CHECK_REAL:
		return VO_FILE_FFMOVIE_REAL;

	case FLAG_CHECK_FLV:
		return VO_FILE_FFMOVIE_FLV;

	case FLAG_CHECK_MIDI:
		return VO_FILE_FFAUDIO_MIDI;

	case FLAG_CHECK_OGG:
		return VO_FILE_FFAUDIO_OGG;

	case FLAG_CHECK_MPG:
		return VO_FILE_FFMOVIE_MPG;

	case FLAG_CHECK_SDP:
		return VO_FILE_FFAPPLICATION_SDP;

	case FLAG_CHECK_AMR:
		return VO_FILE_FFAUDIO_AMR;

	case FLAG_CHECK_AWB:
		return VO_FILE_FFAUDIO_AWB;

	case FLAG_CHECK_QCP:
		return VO_FILE_FFAUDIO_QCP;

	case FLAG_CHECK_WAV:
		return VO_FILE_FFAUDIO_WAV;

	case FLAG_CHECK_FLAC:
		return VO_FILE_FFAUDIO_FLAC;

	case FLAG_CHECK_MP3:
		return VO_FILE_FFAUDIO_MP3;

	case FLAG_CHECK_AAC:
		return VO_FILE_FFAUDIO_AAC;

	case FLAG_CHECK_AU:
		return VO_FILE_FFAUDIO_AU;

	case FLAG_CHECK_APE:
		return VO_FILE_FFAUDIO_APE;
		
	case FLAG_CHECK_ALAC:
		return VO_FILE_FFAUDIO_ALAC;

	default:
		return VO_FILE_FFUNKNOWN;
	}
}

VO_U32 CFileFormatCheck::FileFormat2Flag(VO_FILE_FORMAT ff)
{
	switch(ff)
	{
	case VO_FILE_FFMOVIE_AVI:
		return FLAG_CHECK_AVI;

	case VO_FILE_FFMOVIE_ASF:
		return FLAG_CHECK_ASF;

	case VO_FILE_FFMOVIE_MP4:
		return FLAG_CHECK_MP4;

	case VO_FILE_FFMOVIE_MOV:
		return FLAG_CHECK_MOV;

	case VO_FILE_FFMOVIE_REAL:
		return FLAG_CHECK_REAL;

	case VO_FILE_FFMOVIE_FLV:
		return FLAG_CHECK_FLV;

	case VO_FILE_FFAUDIO_MIDI:
		return FLAG_CHECK_MIDI;

	case VO_FILE_FFAUDIO_OGG:
		return FLAG_CHECK_OGG;

	case VO_FILE_FFMOVIE_MPG:
		return FLAG_CHECK_MPG;

	case VO_FILE_FFAPPLICATION_SDP:
		return FLAG_CHECK_SDP;

	case VO_FILE_FFAUDIO_AMR:
		return FLAG_CHECK_AMR;

	case VO_FILE_FFAUDIO_AWB:
		return FLAG_CHECK_AWB;

	case VO_FILE_FFAUDIO_QCP:
		return FLAG_CHECK_QCP;

	case VO_FILE_FFAUDIO_WAV:
		return FLAG_CHECK_WAV;

	case VO_FILE_FFAUDIO_FLAC:
		return FLAG_CHECK_FLAC;

	case VO_FILE_FFAUDIO_MP3:
		return FLAG_CHECK_MP3;

	case VO_FILE_FFAUDIO_AAC:
		return FLAG_CHECK_AAC;

	case VO_FILE_FFAUDIO_AU:
		return FLAG_CHECK_AU;

	case VO_FILE_FFAUDIO_ALAC:
		return FLAG_CHECK_ALAC;

	case VO_FILE_FFAUDIO_APE:
		return FLAG_CHECK_APE;

	default:
		return FLAG_CHECK_NONE;
	}
}
