	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWThumbnail.cpp

	Contains:	CVOWThumbnail class file

	Written by:	East Zhou

	Change History (most recent first):
	2011-03-29		East		Create file

*******************************************************************************/
#include "CVOWThumbnail.h"
#include "voDivXDRM.h"
#undef LOG_TAG
#define LOG_TAG "CVOWThumbnail"
#include "voLog.h"

CVOWThumbnail::CVOWThumbnail()
	: CVOWPlayer ()
	, m_nSpecWidth(0)
	, m_nSpecHeight(0)
	, m_nSpecPos(-1)
	, m_nSpecMode(VOA_GETFRAMEATTIME_FLAG_PREVIOUSKEYFRAME)
	, m_nSpecColor(VO_COLOR_RGB565_PACKED)
	, m_bForbidBlackFrame(false)
	, m_pVideoBuffer(NULL)
	, m_pVideoFormat(NULL)
	, m_pFileOP(NULL)
	, m_pDRMAPI(NULL)
{
	memset(&m_fmtVideo, 0, sizeof(m_fmtVideo));
}

CVOWThumbnail::~CVOWThumbnail()
{
}

int CVOWThumbnail::Init()
{
	int nRet = CVOWPlayer::Init();
	if(nRet != 0)
		return nRet;

	VO_U32 nValue = 1;
	VO_U32 nRC = m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_PlayFlagAV, &nValue);

	nValue = 0;
	nRC = m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_ThreadNum, &nValue);

	nValue = 1;
	nRC = m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_VOSDK, &nValue);

	VO_BOOL bThumnNail = VO_TRUE;
	nRC = m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_ForThumbnail, &bThumnNail);


	if(m_pDRMAPI)
		m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_DRM_API, m_pDRMAPI);
	if(m_pFileOP)
		m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_DRM_FileOP, m_pFileOP);


	return 0;
}

int CVOWThumbnail::prepareForCapture()
{
	if(m_hVOWPlay == NULL)
		return -1;

	if(m_nFile <= 0 && strlen (m_szFile) <= 0)
		return -1;

	VO_U32 nRC = 0;
	if(m_nFile > 0)
		nRC = m_fVOWAPI.Create(m_hVOWPlay, (VO_PTR)m_nFile, VO_VOMM_CREATE_SOURCE_ID, m_nOffset, m_nLength);
	else
	{
#ifdef _WIN32
		TCHAR tszFile[1024];
		::MultiByteToWideChar(CP_ACP, 0, m_szFile, -1, tszFile, sizeof(tszFile));
		nRC = m_fVOWAPI.Create(m_hVOWPlay, (VO_PTR)tszFile, VO_VOMM_CREATE_SOURCE_URL, m_nOffset, m_nLength);
#else	//_WIN32
		nRC = m_fVOWAPI.Create(m_hVOWPlay, (VO_PTR)m_szFile, VO_VOMM_CREATE_SOURCE_URL, m_nOffset, m_nLength);
#endif	//_WIN32
	}
	if(nRC != VO_ERR_NONE)
	{
		VOLOGE("Create the file graph failed!");
		return -1;
	}

	nRC = m_fVOWAPI.GetParam(m_hVOWPlay, VO_VOMMP_PMID_VideoFormat, &m_fmtVideo);
	if(m_fmtVideo.Width <= 0 || m_fmtVideo.Height <= 0)
		return -1;

	m_nDuration = 0;
	nRC = m_fVOWAPI.GetDuration(m_hVOWPlay, (VO_U32*)&m_nDuration);

	return 0;
}

void CVOWThumbnail::setResolution(int nWidth, int nHeight)
{
	m_nSpecWidth = nWidth;
	m_nSpecHeight = nHeight;
}

void CVOWThumbnail::setPos(int nPos, int nMode)
{
	m_nSpecPos = nPos;
	m_nSpecMode = nMode;
}

int CVOWThumbnail::getOrigResolution(int* pnWidth, int* pnHeight)
{
	if(pnWidth)
		*pnWidth = m_fmtVideo.Width;
	if(pnHeight)
		*pnHeight = m_fmtVideo.Height;

	return 0;
}

int CVOWThumbnail::SetFileBasedDrm(void* pFileOpt, void* pDrmAPI)
{
	m_pDRMAPI = (IVOFileBasedDRM*)pDrmAPI;
	m_pFileOP = (VO_FILE_OPERATOR*)pFileOpt;

	return 0;
}

int CVOWThumbnail::setPosB(int nPos, int nMode)
{
	if(nPos > 0)
	{
		if(nMode != VOA_GETFRAMEATTIME_FLAG_CURRENTFRAME)
		{
			VO_VOMM_NEARKEYFRAME nearKeyframe;
			nearKeyframe.Time = nPos;
			nearKeyframe.PreviousKeyframeTime = 0x7fffffffffffffffll;
			nearKeyframe.NextKeyframeTime = 0x7fffffffffffffffll;
			VO_U32 nRC = m_fVOWAPI.GetParam(m_hVOWPlay, VO_VOMMP_PMID_NearKeyFrame, &nearKeyframe);
			if(nRC == VO_ERR_NONE)
			{
				if(VOA_GETFRAMEATTIME_FLAG_PREVIOUSKEYFRAME == nMode)
				{
					if(0x7fffffffffffffffll != nearKeyframe.PreviousKeyframeTime)
						nPos = nearKeyframe.PreviousKeyframeTime;
				}
				else if(VOA_GETFRAMEATTIME_FLAG_NEXTKEYFRAME == nMode)
				{
					if(0x7fffffffffffffffll != nearKeyframe.NextKeyframeTime)
						nPos = nearKeyframe.NextKeyframeTime;
				}
				else if(VOA_GETFRAMEATTIME_FLAG_NEARESTKEYFRAME == nMode)
				{
					if(0x7fffffffffffffffll != nearKeyframe.PreviousKeyframeTime)
					{
						if(0x7fffffffffffffffll != nearKeyframe.NextKeyframeTime)
						{
							if(nearKeyframe.NextKeyframeTime - nPos < nPos - nearKeyframe.PreviousKeyframeTime)
								nPos = nearKeyframe.NextKeyframeTime;
							else
								nPos = nearKeyframe.PreviousKeyframeTime;
						}
						else
							nPos = nearKeyframe.PreviousKeyframeTime;
					}
					else
					{
						if(0x7fffffffffffffffll != nearKeyframe.NextKeyframeTime)
							nPos = nearKeyframe.NextKeyframeTime;
					}
				}
			}
		}

		m_fVOWAPI.SetCurPos(m_hVOWPlay, nPos);
	}

	return 0;
}

int CVOWThumbnail::captureB()
{
	m_pVideoBuffer = NULL;
	VO_U32 nValue = 1;
	int nTrySamples = 0;

	m_fVOWAPI.Run(m_hVOWPlay);

	while(nTrySamples < 50 && !m_pVideoBuffer)	// decode 50 frames to get thumbnail
	{
		VO_U32 nRC = m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_Playback, &nValue);
		if(nRC != VO_ERR_NONE)
		{
			VOLOGE("VO_VOMMP_PMID_Playback Error!");
			break;
		}

		nTrySamples++;
	}

	return 0;
}

int CVOWThumbnail::capture(VO_VIDEO_BUFFER ** ppVideoBuffer, VO_VIDEO_FORMAT ** ppVideoFormat, int* pnRotation)
{
	if(m_hVOWPlay == NULL)
		return -1;

	VO_RECT rcDisp;
	rcDisp.top = 0;
	rcDisp.left = 0;
	rcDisp.right = m_nSpecWidth;
	rcDisp.bottom = m_nSpecHeight;
	VO_U32 nRC = m_fVOWAPI.SetViewInfo(m_hVOWPlay, NULL, &rcDisp);

	if(m_nSpecPos != -1)
	{
		setPosB(m_nSpecPos, m_nSpecMode);
		captureB();
	}
	else
	{
		if(m_bForbidBlackFrame)
		{
			VO_BOOL bPlayClearestVideo = VO_TRUE;
			nRC = m_fVOWAPI.SetParam(m_hVOWPlay, VO_VOMMP_PMID_PlayClearestVideo, &bPlayClearestVideo);
		}
		captureB();
	}

	if(m_pVideoBuffer)
	{
		*ppVideoBuffer = m_pVideoBuffer;
		*ppVideoFormat = m_pVideoFormat;
	}

	if(pnRotation)
		nRC = m_fVOWAPI.GetParam(m_hVOWPlay, VO_VOMMP_PMID_VideoRotation, pnRotation);

	return 0;
}

int CVOWThumbnail::RenderVideo(VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize)
{
	if(m_nSpecMode == VOA_GETFRAMEATTIME_FLAG_CURRENTFRAME && pVideoBuffer->Time < m_nSpecPos)
		return VO_ERR_FINISH;

	if(pVideoBuffer->ColorType != m_nSpecColor)
		return VO_ERR_NOT_IMPLEMENT;

	m_pVideoBuffer = pVideoBuffer;
	m_pVideoFormat = pVideoSize;

	return VO_ERR_FINISH;
}

OMX_U32	CVOWThumbnail::vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	return VO_ERR_DRM_BASE;
}
