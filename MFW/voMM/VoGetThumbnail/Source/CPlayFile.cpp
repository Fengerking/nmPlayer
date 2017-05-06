	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "cmnFile.h"
#include "CPlayFile.h"
#include "CFileFormatCheck.h"

#include "COutAudioRender.h"
#include "voOSFunc.h"
#include <windows.h>

//#include "CRTSPSource.h"
//#include "CHTTPPDSource.h"
#include "CGDIVideoRender.h"

#include "voLog.h"

#pragma warning (disable : 4996)


CPlayFile::CPlayFile(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: CPlayGraph (hInst, pMemOP, pFileOP)
{
	VOLOGF ();
}

CPlayFile::~CPlayFile ()
{
	VOLOGF ();
	//cmnMemShowStatus();
}

VO_U32 CPlayFile::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength, int nWidth, int nHeight, VO_PTR hBitmap)
{
	VOLOGW ();
							DWORD dwTime = GetTickCount();

	Release ();
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after release --- %d", dwTime);
							dwTime = GetTickCount();

	HBITMAP* hBitmapHandle = (HBITMAP*)hBitmap;
	*hBitmapHandle = NULL;

	voCAutoLock lock(&m_csPlay);

	VO_FILE_FORMAT voFF = VO_FILE_FFUNKNOWN;

	//if ((nType & 0X0F) == 1)
	//{
	//	VO_TCHAR szURL[1024];
	//	vostrcpy (szURL, (VO_PTCHAR)pSource);
	//	//vostrupr (szURL);

	//	if (!vostrncmp (szURL, _T("RTSP://"), 6))
	//		voFF = VO_FILE_FFSTREAMING_RTSP;
	//	else if (!vostrncmp (szURL, _T("rtsp://"), 6))
	//		voFF = VO_FILE_FFSTREAMING_RTSP;
	//	else if (!vostrncmp (szURL, _T("MMS://"), 5))
	//		voFF = VO_FILE_FFSTREAMING_RTSP;
	//	else if (!vostrncmp (szURL, _T("mms://"), 5))
	//		voFF = VO_FILE_FFSTREAMING_RTSP;
	//	else if (!vostrncmp (szURL, _T("HTTP://"), 6))
	//		voFF = VO_FILE_FFSTREAMING_HTTPPD;
	//	else if (!vostrncmp (szURL, _T("http://"), 6))
	//		voFF = VO_FILE_FFSTREAMING_HTTPPD;
	//}

	VO_U32 uFileFlag = VO_FILE_TYPE_NAME;
	if ((nType & 0X0F) == VO_VOMM_CREATE_SOURCE_URL)
		uFileFlag = VO_FILE_TYPE_NAME;
	else if ((nType & 0X0F) == VO_VOMM_CREATE_SOURCE_HANDLE)
		uFileFlag = VO_FILE_TYPE_HANDLE;
	else
		uFileFlag = VO_FILE_TYPE_ID;

	if ( voFF == VO_FILE_FFUNKNOWN)
	{
		CFileFormatCheck filCheck(m_pFileOP, m_pMemOP);

		VO_FILE_SOURCE filSource;
		memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
		filSource.nMode = VO_FILE_READ_ONLY;
		filSource.nFlag = uFileFlag;
		filSource.pSource = pSource;
		filSource.nOffset = nOffset;
		filSource.nLength = nLength;

		voFF = filCheck.GetFileFormat (&filSource, 0xffffffff, 0xffffffff );
	}
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after  GetFileFormat--- %d", dwTime);
							dwTime = GetTickCount();

	m_pSource = new CFileSource (m_hInst, voFF, m_pMemOP, m_pFileOP,NULL);
	if (m_pSource == NULL)
	{
		vostrcpy (m_szError, _T("The file source box could not be created!"));
		VOLOGE ("%s", m_szError);
		return VO_ERR_OUTOF_MEMORY;
	}
	m_pSource->SetConfig (m_pCodecCfg);
	m_pSource->SetErrorText (m_szError);
	m_pSource->SetVOSDK (VO_TRUE);//m_bVOSDK
	m_pSource->SetForThumbnail (VO_TRUE);//VO_TRUE);//m_bForThumbnail

							dwTime = GetTickCount() - dwTime;
							VOLOGE ("befor  load source--- %d", dwTime);
							dwTime = GetTickCount();
	VO_U32 nRC = m_pSource->LoadSource (pSource, uFileFlag, nOffset, nLength);
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after  load source--- %d", dwTime);
							dwTime = GetTickCount();
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("It couuld not load the source!");
		return VO_ERR_VOMMP_FileFormat;
	}

	VO_SOURCE_INFO	filInfo;
	m_pSource->GetSourceInfo (&filInfo);
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after  GetSourceInfo--- %d", dwTime);
							dwTime = GetTickCount();
	if (filInfo.Tracks == 0)
	{
		vostrcpy (m_szError, _T("There is not an available track in file!"));
		VOLOGE ("%s", m_szError);
		return VO_ERR_VOMMP_FileFormat;
	}

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;

	for (i = 0; i < filInfo.Tracks; i++)
	{
		m_pSource->GetTrackInfo (i, &trkInfo);
		if (m_nDuration < trkInfo.Duration)
			m_nDuration = trkInfo.Duration;

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1 && (m_nPlayFlag & 0X02) == 2)
		{
			continue;
		}
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO) && m_nVideoTrack == -1);// && (m_nPlayFlag & 0X01) == 1)
		{
							//dwTime = GetTickCount();
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("befor   decoder init--- %d", dwTime);
							dwTime = GetTickCount();
			m_nVideoTrack = i;
			m_pSource->GetVideoFormat (m_nVideoTrack, &m_fmtVideo);

			m_pVideoDec = new CVideoDecoder (m_hInst, trkInfo.Codec, m_pMemOP);
			if (m_pVideoDec == NULL)
			{
				vostrcpy (m_szError, _T("The video decoder could not be created!"));
				VOLOGE ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}

			m_pVideoDec->SetVOSDK (VO_TRUE);//m_bVOSDK);
			m_pVideoDec->SetConfig (m_pCodecCfg);
			m_pVideoDec->SetErrorText (m_szError);

			VO_U32	nFourCC = 0;
			nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
			m_pVideoDec->SetFourCC (nFourCC);
			//m_pVideoDec->SetVOSDK (VO_FALSE);

			// the head data for WMV is different
			if (trkInfo.Codec == VO_VIDEO_CodingWMV)
			{
				nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_BITMAPINFOHEADER, &trkInfo.HeadData);
				if (nRC != VO_ERR_NONE)
					return VO_ERR_VOMMP_AudioDecoder;

				VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)trkInfo.HeadData;
				trkInfo.HeadSize = pBmpInfo->biSize;
			}

#ifdef LINUX
			// printf the head info.
			sprintf (m_szError, "VOLOG Video Track Head Info: Pointer: %x,	Size: %d \r\n", trkInfo.HeadData, trkInfo.HeadSize);
			if (m_pCallBack != NULL)
				m_pCallBack (m_pUserData, VO_VOMMP_CBID_DebugText, (VO_PTR) m_szError, 0);
			printf ("VOLOG Video Track Head Info: Pointer: %x,	Size: %d \r\n", trkInfo.HeadData, trkInfo.HeadSize);
#endif // LINUX

			if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
			{
				nRC = m_pVideoDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &m_fmtVideo);
			}
			else
			{
				VO_SOURCE_SAMPLE	sample;
				sample.Buffer = NULL;
				sample.Size = 0;

				nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);

				if (nRC != VO_ERR_NONE)
				{
					sample.Buffer = NULL;
					sample.Size = 0;
				}
				else
				{
					sample.Size = sample.Size & 0X7FFFFFFF;
				}

//				nRC = m_pVideoDec->Init (sample.Buffer, sample.Size, &m_fmtVideo);
				//if use first sample data to initialize decoder, do not care the return value!!
				//East 2009/12/27
				m_pVideoDec->Init (sample.Buffer, sample.Size, &m_fmtVideo);
				nRC = VO_ERR_NONE;
			}

			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("Init video decoder failed!");
				return VO_ERR_VOMMP_VideoDecoder;
			}

			nRC = m_pVideoDec->GetParam (VO_PID_VIDEO_ASPECTRATIO, &m_nRatio);

							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after decoder init --- %d", dwTime);
							dwTime = GetTickCount();
			//to get thumbnail
			CGDIVideoRender* pRender = new CGDIVideoRender (m_hInst, NULL, m_pMemOP);;

			VO_S64 nNewPos = 0;
			nRC = m_pSource->SetTrackPos (m_nVideoTrack, &nNewPos);

			int k = 0;
			while(k<500)
			{
				k+=(k/8>0?k/8:1);
				VO_SOURCE_SAMPLE	sample;
				sample.Buffer = NULL;
				sample.Size = 0;
				sample.Time = 0;

				//DWORD dwTime = GetTickCount();
				VO_U32 nRC = m_pSource->GetTrackData (m_nVideoTrack, &sample);
				if (nRC == VO_ERR_SOURCE_END)
				{
					break;
				}
				else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
				{
					Sleep(2);
				}
				else if (nRC == VO_ERR_SOURCE_OK)
				{
					if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO) )
					{
						//if(m_bOnlyShowIFrame)
						{
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("before decoder decode data --- %d", dwTime);
							dwTime = GetTickCount();

							//VO_CODECBUFFER			m_videoSourceBuffer;
							m_videoSourceBuffer.Buffer = sample.Buffer;
							m_videoSourceBuffer.Length = sample.Size & 0X7FFFFFFF;
							m_videoSourceBuffer.Time  = sample.Time;
							VO_U32 nFlag = 1;
							m_pVideoDec->SetParam(VO_PID_VIDEO_OUTPUTMODE, &nFlag);
							nRC = m_pVideoDec->SetInputData (&m_videoSourceBuffer);
							if (nRC != VO_ERR_NONE)
								break;
							int jj = 0;//fmtVideo.Type;
							jj = m_pVideoDec->GetFrameType(&m_videoSourceBuffer);
							//VO_VIDEO_FORMAT		fmtVideo;
							//VO_VIDEO_BUFFER			m_videoRenderBuffer;
							memset (&m_videoRenderBuffer, 0, sizeof (VO_VIDEO_BUFFER));
							nRC = m_pVideoDec->GetOutputData (&m_videoRenderBuffer, &m_fmtVideo);
							if (nRC == (VO_ERR_INPUT_BUFFER_SMALL))
							{
								if(k<5)
									continue;
								else
									break;
							}

							if (m_videoRenderBuffer.Buffer[0] == NULL)
							{
								continue;
							}

							dwTime = GetTickCount() - dwTime;
							VOLOGE ("before Check Black Frame --- %d", dwTime);
							dwTime = GetTickCount();
							if(CheckBlackFrame (&m_videoRenderBuffer,  &m_fmtVideo))
							{
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after Check Black Frame --- %d", dwTime);
							dwTime = GetTickCount();
								continue;
							}
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after decoder decode data --- %d", dwTime);
							dwTime = GetTickCount();

							//dwTime = GetTickCount() - dwTime;
							//VOLOGE ("decoder time --- %d", dwTime);
							//dwTime = GetTickCount();
							//render now 
							VO_RECT r;
							r.left = 0;
							r.top = 0;
							r.bottom = nHeight;
							r.right = nWidth;
							pRender->SetDispRect(NULL,&r);
							//this->SetViewInfo(NULL, &r);

							pRender->SetVideoInfo (m_fmtVideo.Width, m_fmtVideo.Height,  VO_COLOR_YVU_PLANAR420);
							//pRender->UpdateSize();
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("before render time --- %d", dwTime);
							dwTime = GetTickCount();
							pRender->Render(&m_videoRenderBuffer,0,VO_TRUE);
							pRender->GetBitmap(	(HBITMAP*)hBitmap );
							dwTime = GetTickCount() - dwTime;
							VOLOGE ("after render time --- %d", dwTime);
							dwTime = GetTickCount();
							//	goto LINE_RELEASE_1;
							//}
//							continue;
						}
					}
				}
				if(pRender)
					delete pRender;
				break;
			}
			break;

		}
	}

	VOLOGW ();

	return VO_ERR_NONE;
}
bool CPlayFile::CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize)
{
	if (pVideoBuffer == NULL || pVideoSize == NULL)
		return false;

	if (pVideoBuffer->ColorType != VO_COLOR_YUV_PLANAR420)
		return false;

	//if(1)
	//{
		int nLightPixs = 0;
		int nTotalPixs = 0;

		int	nX = 0;
		int nY = 0;
		int nW = pVideoSize->Width;
		int nH = pVideoSize->Height;

		if (nW > 176)
		{
			nW = 176;
			nX =  (pVideoSize->Width - nW) / 2;
		}
		if (nH > 144)
		{
			nH = 144;
			nY = (pVideoSize->Height - nH) / 2;
		}

		unsigned char * pVideo = NULL;
		for (int i = nY; i < nY + nH; i++)
		{
			pVideo = pVideoBuffer->Buffer[0] + i * pVideoBuffer->Stride[0];
			for (int j = nX; j < nX + nW; j++)
			{
				if (pVideo[j] > 50)
					nLightPixs++;

				nTotalPixs++;
			}
		}

		bool bBlackFrame;
		if (nLightPixs > nTotalPixs / 5)
			bBlackFrame = false;
		else
			bBlackFrame = true;

		VOLOGW ("Result L: %d, A: %d, R: %d", nLightPixs, nTotalPixs, m_bBlackFrame);
		return bBlackFrame;
}
