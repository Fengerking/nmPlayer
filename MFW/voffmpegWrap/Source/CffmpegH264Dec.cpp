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
#include "voMMPlay.h"
#include "voOSFunc.h"

#include "CffmpegH264Dec.h"
#include <stdlib.h>

#include "voH264.h"
#include "voRealVideo.h"
#include "voIOMXDec.h"
#include "voAMediaCodec.h"

#include "vompType.h"

#ifdef _IOS
#include "voWMV.h"
#include "voRealVideo.h"
#include "voMPEG2.h"
#include "voVC1.h"
#endif

#define LOG_TAG "CVideoDecoder"
#include "voLog.h"


typedef VO_S32 (VO_API * VOGETVIDEODECAPI) (VO_VIDEO_DECAPI * pDecHandle);

CVideoDecoder::CVideoDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hDec (NULL)
	, m_nCoding (nCoding)
	, m_nFourCC (0)
	, m_bSetHeadData (VO_FALSE)
	, m_bFirstSample (VO_TRUE)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nFrameRate (0)
	, m_llPrevTime (-1)
	, m_nDelayTime (0)
	, m_pInputBuffer (NULL)
	, m_nInputUsed (0)
	, m_nVideoCount(0)
	, m_nMemShared(0)
	, m_nDecFrames (0)
	, m_nVideoArtifact (0)
	, m_nPassCodec (0)
	, m_pYUVPassData (NULL)
	, m_nPassCount (0)
	, m_pVMOP (NULL)
	, m_hCheck (NULL)
	, mnSourceType(1)
	, m_nDisableDeblock(0)
	, m_psrc(NULL)
	, m_nSetVideoNameType(0)
	, m_nSetVideoAPIType(0)
	, mIsDrmDataAppended(false)
{
	memset(&mHeadData , 0 , sizeof(mHeadData));

	memset(&m_sSetVideoName, 0, sizeof(VO_TCHAR)*256);	
	memset(&m_sSetVideoAPI, 0, sizeof(VO_TCHAR)*256);

	m_memVideo.Alloc = voVideoMemAlloc;
	m_memVideo.Free = voVideoMemFree;
	m_memVideo.Set = voVideoMemSet;
	m_memVideo.Copy = voVideoMemCopy;
	m_memVideo.Check = voVideoMemCheck;
	m_memVideo.Compare = voVideoMemCompare;
	m_memVideo.Move = voVideoMemMove;
}

CVideoDecoder::~CVideoDecoder ()
{
	Uninit ();

	if (m_pYUVPassData != NULL)
		delete []m_pYUVPassData;

	if(mHeadData.Buffer != NULL)
	{
		delete [] mHeadData.Buffer;
		mHeadData.Buffer = NULL;
	}
}

void CVideoDecoder::SetListenerInfo(VO_PTR pListenerInfo)
{
	if (pListenerInfo != NULL)
		memcpy(&m_listenerinfo, pListenerInfo, sizeof(VOMP_LISTENERINFO));
}

VO_U32 CVideoDecoder::Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_VIDEO_FORMAT * pFormat)
{
	if (m_nCoding > VO_VIDEO_CodingH265 || m_nCoding < VO_VIDEO_CodingMPEG2)
	{
		if (m_pError != NULL)
			vostrcpy (m_pError, _T("The video format is unknown!"));
		VOLOGE ("The coding %d was unknown!", (int)m_nCoding);
		return VO_ERR_VOMMP_VideoDecoder;
	}

	if(nHeadSize > 0)
	{
		if(mHeadData.Buffer != NULL)
		{
			delete [] mHeadData.Buffer;
			mHeadData.Buffer = NULL;
		}
		mHeadData.Buffer = new VO_BYTE[nHeadSize];
		memcpy(mHeadData.Buffer , pHeadData , nHeadSize);
		mHeadData.Length = nHeadSize;
	}

	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;
	m_bSetHeadData = VO_FALSE;

	while (nRC != VO_ERR_NONE)
	{
		Uninit ();

		if (LoadLib (m_hInst) == 0)
		{
			VOLOGE ("LoadLib was failed!");
			return VO_ERR_VOMMP_VideoDecoder;
		}

		m_OutputInfo.Format.Width = pFormat->Width;
		m_OutputInfo.Format.Height = pFormat->Height;
		m_OutputInfo.InputUsed = 0;

		m_nVideoWidth = pFormat->Width;
		m_nVideoHeight = pFormat->Height;

		VO_CODEC_INIT_USERDATA	initInfo;
		//initInfo.memData = &m_memVideo;
		memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
		initInfo.memflag = VO_IMF_USERMEMOPERATOR;
		initInfo.memData = m_pMemOP;
		// david @ 2011/11/30
#ifdef _MAC_OS
		initInfo.reserved1 = (unsigned long)m_psrc;
		initInfo.reserved2 = (unsigned long)&m_listenerinfo;
#else
		initInfo.reserved1 = (VO_U32)m_psrc;
		initInfo.reserved2 = (VO_U32)&m_listenerinfo;
#endif

		if (m_pLibOP != NULL)
		{
			initInfo.memflag |= 0X10;
			initInfo.libOperator = m_pLibOP;
		}

		// here we can force block license check for short play, East 20111129
		if(m_bForThumbnail)
			initInfo.memflag |= 0x100;
			
		if (m_nPassCodec == 0)
		{
			m_funDec.SetParam (NULL, VO_PID_COMMON_LOGFUNC, m_pcbVOLOG);
			nRC = m_funDec.Init (&m_hDec, (VO_VIDEO_CODINGTYPE)m_nCoding, &initInfo);

			if (nRC != VO_ERR_NONE || m_hDec == NULL)
			{
				VOLOGE ("m_funDec.Init was failed! Result 0X%08X", (unsigned int)nRC);
				return VO_ERR_NOT_IMPLEMENT;
			}
			UpdateParam ();
			nRC = SetHeadData (pHeadData, nHeadSize);
		}
		else
		{
			nRC = VO_ERR_NONE;
		}

		if (nRC == VO_ERR_NONE)
			break;

		if (m_bVOUsed)
			break;

		m_bVOUsed = VO_TRUE;
	}

	if (m_nPassCodec > 0 && m_pYUVPassData == NULL)
	{
		m_pYUVPassData = new VO_BYTE[m_nVideoWidth * m_nVideoHeight * 2];
		memset (m_pYUVPassData, 0, m_nVideoWidth * m_nVideoHeight);
		memset (m_pYUVPassData + m_nVideoWidth * m_nVideoHeight, 127, m_nVideoWidth * m_nVideoHeight);
	}

	return nRC;
}

void CVideoDecoder::ResetDecoder(VO_PBYTE pHeadData /* = NULL */, VO_U32 nHeadSize /* = 0 */)
{
	if (m_nPassCodec > 0)
		return;

	if (m_hDec != NULL)
	{
		m_funDec.Uninit (m_hDec);
		m_hDec = NULL;
	}

	if((NULL != pHeadData) && (nHeadSize > 0))
	{
		// head data modified
		if(mHeadData.Buffer != NULL)
		{
			delete [] mHeadData.Buffer;
			mHeadData.Buffer = NULL;
		}
		mHeadData.Buffer = new VO_BYTE[nHeadSize];
		memcpy(mHeadData.Buffer, pHeadData, nHeadSize);
		mHeadData.Length = nHeadSize;
	}

	VO_CODEC_INIT_USERDATA	initInfo;
	//initInfo.memData = &m_memVideo;
	memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = m_pMemOP;

	if (m_pLibOP != NULL)
	{
		initInfo.memflag |= 0X10;
		initInfo.libOperator = m_pLibOP;
	}

	// here we can force block license check for short play, East 20111129
	if(m_bForThumbnail)
		initInfo.memflag |= 0x100;

	m_funDec.Init (&m_hDec, (VO_VIDEO_CODINGTYPE)m_nCoding, &initInfo);

	UpdateParam ();

	SetHeadData (mHeadData.Buffer , mHeadData.Length);
}

VO_U32 CVideoDecoder::UpdateParam (void)
{
	if (m_nPassCodec > 0)
		return 0;

	VOLOGI("Set CPU Number %d into video decoder!", (int)m_nCPUNum);
	m_funDec.SetParam (m_hDec, VO_PID_COMMON_CPUNUM, &m_nCPUNum);

	m_funDec.SetParam(m_hDec, VO_PID_VIDEO_THUMBNAIL_MODE, &m_bForThumbnail);

	VO_U32 nRC = 0;
	if (m_pVMOP != NULL)
		SetParam (VO_PID_VIDEO_VIDEOMEMOP, m_pVMOP);

	if(m_nVideoCount > 1)
	{
		int nVideoCount = m_nVideoCount;
		nRC = SetParam (VO_PID_COMMON_FRAME_BUF_EX, &nVideoCount);
		if(nRC == VO_ERR_NONE)
		{
			m_nMemShared = 1;
		}
	}

	if (m_nCoding == VO_VIDEO_CodingH264)
	{
		VO_H264FILEFORMAT nStreamFormat = VO_H264_ANNEXB;
		if(memcmp((char*)&m_nFourCC, (char*)"AVC1", 4) == 0)
			nStreamFormat = VO_H264_AVC;
		else if(memcmp((char*)&m_nFourCC, (char*)"AVC0", 4) == 0)
			nStreamFormat = VO_H264_14496_15;
		nRC = SetParam (VO_ID_H264_STREAMFORMAT, &nStreamFormat);

		// default we should output real frame as input, East 20110516
		int nJump2I = 0;
		nRC = SetParam (VO_ID_H264_ENABLE_JUMP2I, &nJump2I);

		// disable error log for formal release, East 20111008
		int nOptLog = OPT_DISABLE_ERRORLOG;
		nRC = SetParam (VO_ID_H264_OPT_FLAG, &nOptLog);

		// if live(RTSP, live streaming...), make h264 decoder more stable, East 20110607
		if(mnSourceType == 0)
		{
			int nStreamingMode = 1;
			nRC = SetParam (VO_ID_H264_STREAMINGMODE, &nStreamingMode);
		}
	}

	if (m_pConfig != NULL && !m_bVOUsed)
	{
		VO_PCHAR pCoreFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"Core");
		VO_PCHAR pCompName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Comp");

		if (pCoreFile != NULL)
			SetParam (VO_PID_COMMON_CoreFile, pCoreFile);
		if (pCoreFile != NULL)
			SetParam (VO_PID_COMMON_CompName, pCompName);
	}

	m_funDec.SetParam (m_hDec, VO_PID_VIDEO_FORMAT, &m_OutputInfo);


	// added by gtxia 
	m_funDec.SetParam(m_hDec, VO_ACODEC_DRM, (void*)&mIsDrmDataAppended);
	return 0;
}

VO_U32 CVideoDecoder::SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (m_nPassCodec > 0)
		return 0;

	if (pHeadData == NULL || nHeadSize == 0)
		return 0;

	if (m_nCoding == VO_VIDEO_CodingH263 || m_nCoding == VO_VIDEO_CodingS263 ||
		m_nCoding == VO_VIDEO_CodingMJPEG || m_nCoding == VO_VIDEO_CodingDIVX)
		return 0;

	VO_CODECBUFFER buffHead;
	buffHead.Buffer = pHeadData;
	buffHead.Length = nHeadSize;

	VO_U32 nRC = 0;
	if (nHeadSize > 0)// && m_nCoding != VO_VIDEO_CodingH264)
    {
		nRC = m_funDec.SetParam (m_hDec, VO_PID_COMMON_HEADDATA, &buffHead);
    }

	if (nRC != VO_ERR_NONE)
	{
		VOLOGW ("m_funDec.SetParam (VO_PID_COMMON_HEADDATA) failed. 0X%08X", (unsigned int)nRC);
	}

	if (nRC == VO_ERR_NONE)
		m_funDec.GetParam (m_hDec, VO_PID_VIDEO_FORMAT, &m_OutputInfo);

	m_bSetHeadData = VO_TRUE;

	return nRC;
}

VO_U32 CVideoDecoder::Uninit (void)
{
	if (m_hDec != NULL)
	{
		voCAutoLock lock (&m_Mutex);
		m_funDec.Uninit (m_hDec);
		m_hDec = NULL;
	}

	FreeLib ();

	return 0;
}

VO_U32 CVideoDecoder::Start(void)
{
	if (m_bVOUsed)
		return 0;

	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_START, &nValue);
}

VO_U32 CVideoDecoder::Pause(void)
{
	if (m_bVOUsed)
		return 0;

	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_PAUSE, &nValue);
}

VO_U32 CVideoDecoder::Stop(void)
{
	if (m_bVOUsed)
		return 0;

	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_STOP, &nValue);
}

VO_U32 CVideoDecoder::SetInputData (VO_CODECBUFFER * pInput)
{
	if ((m_nCoding == VO_VIDEO_CodingH265) && (pInput->Time == 0 && !m_bFirstSample))
		ResetDecoder ();

	m_pInputBuffer = pInput;
	if (m_nPassCodec > 0)
		return 0;

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	if (m_nFrameRate == 0)
	{
		if (m_llPrevTime == -1)
			m_llPrevTime = pInput->Time;
		if (pInput->Time > m_llPrevTime)
			m_nFrameRate = VO_U32(1000.0 / (pInput->Time - m_llPrevTime) + 0.5);
		m_llPrevTime = pInput->Time;
	}

	if (m_bFirstSample)
	{
		m_bFirstSample = VO_FALSE;

		if (HandleFirstSample (pInput) == VO_ERR_NONE)
			return VO_ERR_INPUT_BUFFER_SMALL;
	}


	m_OutputInfo.InputUsed = 0;
	m_nInputUsed = 0;

	//unsigned char * pData = (unsigned char *)pInput->Buffer;
	//VOLOGI ("Buffer Size % 8d, DATA: 0X%02X%02X%02X%02X%02X%02X%02X%02X  %02X%02X%02X%02X%02X%02X%02X%02X", pInput->Length, pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7], pData[8], pData[9], pData[10], pData[11], pData[12], pData[13], pData[14], pData[15]);

//	VOLOGI ("Try to set the input data into decoder.");

	VO_U32 nRC = m_funDec.SetInputData (m_hDec, pInput);
	if(VO_ERR_IOMXDEC_NeedRetry == nRC)
		return nRC;

//	if (nRC == VO_ERR_NONE)
//		VOLOGI ("After set the input data into decoder.Return %08X, Size % 6d", 
//					nRC, pInput->Length);

	return VO_ERR_NONE;
}

VO_U32 CVideoDecoder::GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_BOOL * pbContinuous)
{
	if (m_nPassCodec > 0)
	{
		pOutput->Buffer[0] = m_pYUVPassData;
		pOutput->Buffer[1] = m_pYUVPassData + m_nVideoWidth * m_nVideoHeight;
		pOutput->Buffer[2] = m_pYUVPassData + m_nVideoWidth * m_nVideoHeight * 5 / 4;
		pOutput->Stride[0] = m_nVideoWidth;
		pOutput->Stride[1] = m_nVideoWidth / 2;
		pOutput->Stride[2] = m_nVideoWidth / 2;

		pOutput->ColorType = VO_COLOR_YUV_PLANAR420;
		pOutput->Time = m_pInputBuffer->Time;

		pVideoFormat->Width = m_nVideoWidth;
		pVideoFormat->Height = m_nVideoHeight;
		*pbContinuous = VO_FALSE;

		memset (m_pYUVPassData, 0, m_nVideoWidth * m_nVideoHeight);
		memset (m_pYUVPassData + m_nPassCount * m_nVideoWidth, 188, 5 * m_nVideoWidth);

		m_nPassCount += 3;
		if (m_nPassCount + 6 >= m_nVideoHeight)
			m_nPassCount = 0;
		return 0;
	}

	if (m_hDec == 0 || m_pInputBuffer == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = 0;

//	if (m_pInputBuffer->Length <= m_OutputInfo.InputUsed)
//		return VO_ERR_INPUT_BUFFER_SMALL;

/*
	// test raw H264 data
	VO_CODECBUFFER	inBuff;
	bool			bFound = false;
	int				dwNal = 0X01000000;
	VO_PBYTE		pBuffer = m_pInputBuffer->Buffer + 1;
	while (pBuffer - m_pInputBuffer->Buffer < m_pInputBuffer->Length - 4)
	{
		if (!memcmp (pBuffer, &dwNal, 4))
		{
			bFound = true;
			break;
		}
		pBuffer++;
	}

	if (bFound)
	{
		inBuff.Buffer = m_pInputBuffer->Buffer + 4;
		inBuff.Length = pBuffer - m_pInputBuffer->Buffer - 4;
		inBuff.Time = m_pInputBuffer->Time;
		m_pInputBuffer->Length = m_pInputBuffer->Length - inBuff.Length - 4;
		m_pInputBuffer->Buffer = pBuffer;
	}
	else
	{
		inBuff.Buffer = m_pInputBuffer->Buffer + 4;
		inBuff.Length = m_pInputBuffer->Length - 4;
		inBuff.Time = m_pInputBuffer->Time;
		m_pInputBuffer->Length = 0;
		m_pInputBuffer->Buffer = NULL;
	}
	nRC = m_funDec.SetInputData (m_hDec, &inBuff);
*/

	if (m_nCoding == VO_VIDEO_CodingH264 || m_nCoding == VO_VIDEO_CodingRV)
	{
		// int nNonRef = 0;
		EDeblcokFlag eDisableDeblockEdge = (EDeblcokFlag)0;

		m_nDisableDeblock = 0;
		if (m_bForThumbnail)
			m_nDisableDeblock = 1;
		else
		{
#ifndef _HTC	//HTC want to get better quality instead of performance, East, 2010/05/27
			if (m_nDelayTime > 100 && m_nDecFrames > 1)
			{
				m_nDisableDeblock = 1;
				//nNonRef = OPT_FULLPIX_NONREF;
			}
			else if(m_nDelayTime > 80 && m_nDecFrames > 1)
			{
				eDisableDeblockEdge = EDF_ONLYMBEDGE;
			}
#endif
		}

		if (m_nCoding == VO_VIDEO_CodingH264)
		{
			if (m_OutputInfo.Format.Width > 320 && m_OutputInfo.Format.Height > 240)
			{
				m_funDec.SetParam (m_hDec, VO_ID_H264_DISABLEDEBLOCK, &m_nDisableDeblock);
				m_funDec.SetParam (m_hDec, VO_ID_H264_DEBLOCKFLAG, &eDisableDeblockEdge);
				//m_funDec.SetParam (m_hDec,VO_ID_H264_OPT_FLAG, &nNonRef);
			}
		}
		else if (m_nCoding == VO_VIDEO_CodingRV)
		{
			m_funDec.SetParam (m_hDec, VO_PID_DEC_RealVideo_DISABLEDEBLOCKING, &m_nDisableDeblock);
		}
	}
	
#if 0
	static int time = 0;
	static int count = 0;
	
	int use = voOS_GetSysTime();
//	VOLOGI ("voRealDecStart");

	nRC = m_funDec.GetOutputData (m_hDec, pOutput, &m_OutputInfo);
	time += voOS_GetSysTime() - use;
	
	count++;
	if (count > 200)
	{
		printf("Video dec time = %d, total time = %d, frame count = %d\n", time/count, time, count);
		time = 0;
		count = 0;
	}
	
#else
//	VOLOGI ("Try to get output data into decoder.");

	nRC = m_funDec.GetOutputData (m_hDec, pOutput, &m_OutputInfo);

//	if (nRC == VO_ERR_NONE && (m_OutputInfo.Format.Width > 0 && m_OutputInfo.Format.Height > 0))
//		VOLOGI ("After get output data into decoder.Return %08X", nRC);

#endif

	pVideoFormat->Type =  m_OutputInfo.Format.Type;
/*
//	m_pInputBuffer->Length = 0;
//	VOLOGI ("Get Output Data Buffer 0X%08X! Result 0X%08X, Flag 0X%08X", pOutput->Buffer[0], nRC, m_OutputInfo.Flag);
	if (m_nVideoArtifact > 0 && m_nCoding == VO_VIDEO_CodingH264)
	{
		if (nRC != VO_ERR_NONE && nRC != VO_ERR_INPUT_BUFFER_SMALL && m_OutputInfo.Format.Type != VO_VIDEO_FRAME_B)
		{
			VO_U32	nFlush = 1;
			nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_FLUSH, &nFlush);
		}
	}
*/
	if (nRC == VO_ERR_NONE && (m_OutputInfo.Format.Width > 0 && m_OutputInfo.Format.Height > 0))
	{
		pVideoFormat->Width = m_OutputInfo.Format.Width;
		pVideoFormat->Height = m_OutputInfo.Format.Height;

		if(pbContinuous)
			*pbContinuous = (m_OutputInfo.Flag & VO_VIDEO_OUTPUT_MORE) ? VO_TRUE : VO_FALSE;

		//for real video, one frame processed once, so we regard all buffer used!!
		//East, 2009/09/24
		if(VO_VIDEO_CodingRV == m_nCoding)
			m_OutputInfo.InputUsed = m_pInputBuffer->Length;

		if (pOutput->Buffer[0] != NULL)
		{
			m_nDecFrames++;
			//if (m_nFrameRate > 0)
			//	pOutput->Time = (m_nDecFrames * 1000) / m_nFrameRate;

			if (m_nCoding == VO_VIDEO_CodingRV)
			{
				if (m_nVideoWidth == (VO_U32)(m_OutputInfo.Format.Width) * 2)
				{
					if (m_nVideoHeight == (VO_U32)(m_OutputInfo.Format.Height))
					{
						pVideoFormat->Height = m_OutputInfo.Format.Height / 2;
						pOutput->Stride[0] = pOutput->Stride[0] * 2;
						pOutput->Stride[1] = pOutput->Stride[1] * 2;
						pOutput->Stride[2] = pOutput->Stride[2] * 2;
					}
				}
			}
		}
	}

//	if (m_bVOUsed)
//		pOutput->Time = 0;

//	if (m_nInputUsed >= m_OutputInfo.InputUsed)
//		return VO_ERR_INPUT_BUFFER_SMALL;
	m_nInputUsed = m_OutputInfo.InputUsed;

	if(VO_ERR_IOMXDEC_NeedRetry == nRC)
		return nRC;

	nRC = nRC & 0X8000FFFF;

	return nRC;
}

VO_U32 CVideoDecoder::HandleFirstSample (VO_CODECBUFFER * pInput)
{
	if (m_nPassCodec > 0)
		return 0;

	if (m_nCoding== VO_VIDEO_CodingMPEG4)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingH263)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingS263)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingH264)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingWMV)
	{
		if (pInput->Length < sizeof (VO_BITMAPINFOHEADER) || pInput->Buffer == NULL)
			return VO_ERR_NOT_IMPLEMENT;

		VO_BITMAPINFOHEADER * pBmpHeader = (VO_BITMAPINFOHEADER *)pInput->Buffer;
//		if (pBmpHeader->biWidth == m_OutputInfo.Format.Width &&
//			pBmpHeader->biHeight == m_OutputInfo.Format.Height)
		if (pBmpHeader->biSize == pInput->Length && (pBmpHeader->biWidth <= 1920 && pBmpHeader->biHeight <= 1080))
		{
			SetHeadData (pInput->Buffer, pInput->Length);
			return VO_ERR_NONE;
		}
	}
	else if (m_nCoding== VO_VIDEO_CodingRV && !m_bSetHeadData)
	{
		if(pInput->Length < sizeof(VORV_FORMAT_INFO) || pInput->Buffer == NULL)
			return VO_ERR_NOT_IMPLEMENT;

		VORV_FORMAT_INFO* pFormat = (VORV_FORMAT_INFO*)pInput->Buffer;
		if(sizeof(VORV_FORMAT_INFO) + pFormat->ulOpaqueDataSize == pInput->Length && 
			(pFormat->usBitCount <= 32 && pFormat->usWidth <= 1920 && pFormat->usHeight <= 1080))
		{
			SetHeadData(pInput->Buffer, pInput->Length);
			return VO_ERR_NONE;
		}

		return VO_ERR_NOT_IMPLEMENT;
	}		
	else if (m_nCoding== VO_VIDEO_CodingMPEG2)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingMJPEG)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingDIVX)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingVP6)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_VIDEO_CodingVP8)
		return VO_ERR_NOT_IMPLEMENT;
	else
		return VO_ERR_NOT_IMPLEMENT;

	return VO_ERR_NOT_IMPLEMENT;
}


VO_VIDEO_FRAMETYPE CVideoDecoder::GetFrameType (VO_CODECBUFFER * pBuffer)
{
//	if (!m_bVOUsed)
//		return VO_VIDEO_FRAME_NULL;

	if (m_nPassCodec > 0)
		return VO_VIDEO_FRAME_NULL;

//	if (m_nCoding == VO_VIDEO_CodingWMV)
//		return VO_VIDEO_FRAME_NULL;

	VO_VIDEO_FRAMETYPE	nFrameType = VO_VIDEO_FRAME_NULL;
	VO_CODECBUFFER		inputBuffer;
	nFrameType = nFrameType;
	if (pBuffer != NULL)
	{
		memcpy (&inputBuffer, pBuffer, sizeof (VO_CODECBUFFER));
	}
	else if (m_pInputBuffer != NULL)
	{
		inputBuffer.Buffer = m_pInputBuffer->Buffer + m_nInputUsed;
		inputBuffer.Length = m_pInputBuffer->Length - m_nInputUsed;
	}
	else
		return VO_VIDEO_FRAME_NULL;

	if (inputBuffer.Length < 6)
		return VO_VIDEO_FRAME_NULL;

	if (m_nCoding == VO_VIDEO_CodingH264)
	{
		unsigned char * pBuffer = inputBuffer.Buffer;
		if (pBuffer[2]==0 && pBuffer[3]==1)
			pBuffer+=4;
		else
			pBuffer+=3;

		return GetH264FrameType (pBuffer , inputBuffer.Length - 4);
	}

	inputBuffer.Time = VO_VIDEO_FRAME_NULL;
	VO_U32 nRC = GetParam (VO_PID_VIDEO_FRAMETYPE, &inputBuffer);
	if (nRC != VO_ERR_NONE)
		return VO_VIDEO_FRAME_NULL;

	return VO_VIDEO_FRAMETYPE (inputBuffer.Time);
}

VO_VIDEO_FRAMETYPE CVideoDecoder::GetFrameType (VO_PBYTE pBuffer, VO_U32 nBuffSize)
{
	if (m_nPassCodec > 0)
		return VO_VIDEO_FRAME_NULL;

	//VO_VIDEO_FRAMETYPE	nFrameType = VO_VIDEO_FRAME_NULL;
	VO_CODECBUFFER		inputBuffer;

	inputBuffer.Buffer = pBuffer;
	inputBuffer.Length = nBuffSize;
	if (inputBuffer.Length < 6)
		return VO_VIDEO_FRAME_NULL;

	if (m_nCoding == VO_VIDEO_CodingH264)
	{
		unsigned char * pBuffer = inputBuffer.Buffer;
		if (pBuffer[2]==0 && pBuffer[3]==1)
			pBuffer+=4;
		else
			pBuffer+=3;
		return GetH264FrameType (pBuffer , inputBuffer.Length - 4);
	}

	inputBuffer.Time = VO_VIDEO_FRAME_NULL;
	VO_U32 nRC = GetParam (VO_PID_VIDEO_FRAMETYPE, &inputBuffer);
	if (nRC != VO_ERR_NONE)
		return VO_VIDEO_FRAME_NULL;

	return VO_VIDEO_FRAMETYPE (inputBuffer.Time);
}

VO_U32 CVideoDecoder::Flush (void)
{
	if (m_nPassCodec > 0)
	{
		m_nPassCount = 0;
		return 0;
	}

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32	nFlush = 1;	
	VO_U32 nRC = m_funDec.SetParam (m_hDec, VO_PID_COMMON_FLUSH, &nFlush);
	if (m_nCoding == VO_VIDEO_CodingH264)
	{
		nFlush = 1;
		nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_FLUSH, &nFlush);

// 		if(mnSourceType == 0)
// 			ResetDecoder();
	}

	m_nInputUsed = 0;
	m_OutputInfo.InputUsed = 0;
	m_nDecFrames = 0;

	return nRC;
}

VO_U32 CVideoDecoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (uParamID == VO_PID_COMMON_LOGFUNC)
		m_pcbVOLOG = (VO_LOG_PRINT_CB *)pData;
			
	if (m_nPassCodec > 0)
		return 0;

	if (uParamID == VO_PID_VIDEO_VIDEOMEMOP)
		m_pVMOP = (VO_MEM_VIDEO_OPERATOR *) pData;

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return 	m_funDec.SetParam (m_hDec, uParamID, pData);
}

VO_U32 CVideoDecoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_nPassCodec > 0)
		return 0;

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return 	m_funDec.GetParam (m_hDec, uParamID, pData);
}

VO_U32 CVideoDecoder::GetFormat (VO_VIDEO_FORMAT * pFormat)
{
	pFormat->Width = m_OutputInfo.Format.Width;
	pFormat->Height = m_OutputInfo.Format.Height;

	return VO_ERR_NONE;
}

VO_U32 CVideoDecoder::LoadLib (VO_HANDLE hInst)
{
#define _LIB
#ifdef _VO_LIB
#ifdef _VO_LIB_RV
	if (m_nCoding== VO_VIDEO_CodingRV)
		voGetRVDecAPI(&m_funDec, 0);
#endif //_VO_LIB_RV
#else
#ifdef _LIB
	if(m_nCoding == VO_VIDEO_CodingH264)
{
VOLOGE("Eoollo @@@@@@@@@@@@@@@@@@@");
		voGetH264DecAPI(&m_funDec, 0);
}
	else
		return 0;
#else	//_LIB
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_nCoding== VO_VIDEO_CodingMPEG4)
	{
		strcpy (m_szCfgItem, "Video_Dec_MPEG4");
		vostrcpy (m_szDllFile, _T("voMPEG4Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingH263)
	{
		strcpy (m_szCfgItem, "Video_Dec_H263");
		vostrcpy (m_szDllFile, _T("voMPEG4Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingS263)
	{
		strcpy (m_szCfgItem, "Video_Dec_S263");
		vostrcpy (m_szDllFile, _T("voMPEG4Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingH264)
	{
		strcpy (m_szCfgItem, "Video_Dec_H264");
		vostrcpy (m_szDllFile, _T("voH264Dec"));
		vostrcpy (m_szAPIName, _T("voGetH264DecAPI"));

		if (m_pConfig != NULL)
			m_nVideoArtifact = m_pConfig->GetItemValue (m_szCfgItem, (char*)"Artifact", 0);
	}
	else if (m_nCoding== VO_VIDEO_CodingH265)
	{
		strcpy (m_szCfgItem, "Video_Dec_H265");
		vostrcpy (m_szDllFile, _T("voH265Dec"));
		vostrcpy (m_szAPIName, _T("voGetH265DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingWMV)
	{
		strcpy (m_szCfgItem, "Video_Dec_WMV");
		vostrcpy (m_szDllFile, _T("voWMVDec"));
		vostrcpy (m_szAPIName, _T("voGetWMV9DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingVC1)
	{
		strcpy (m_szCfgItem, "Video_Dec_VC1");
		vostrcpy (m_szDllFile, _T("voVC1Dec"));
		vostrcpy (m_szAPIName, _T("voGetVC1DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingRV)
	{
		strcpy (m_szCfgItem, "Video_Dec_REAL");
		vostrcpy (m_szDllFile, _T("voRealVideoDec"));
		vostrcpy (m_szAPIName, _T("voGetRVDecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingMPEG2)
	{
		strcpy (m_szCfgItem, "Video_Dec_MPEG2");
		vostrcpy (m_szDllFile, _T("voMPEG2Dec"));
		vostrcpy (m_szAPIName, _T("voGetMPEG2DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingMJPEG)
	{
		strcpy (m_szCfgItem, "Video_Dec_MJPEG");
		vostrcpy (m_szDllFile, _T("voMJPEGDec"));
		vostrcpy (m_szAPIName, _T("voGetMJPEGDecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingDIVX)
	{
		strcpy (m_szCfgItem, "Video_Dec_DIVX");
		vostrcpy (m_szDllFile, _T("voDIVX3Dec"));
		vostrcpy (m_szAPIName, _T("voGetDIVX3DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingVP6)
	{
		strcpy (m_szCfgItem, "Video_Dec_VP6");
		vostrcpy (m_szDllFile, _T("voVP6Dec"));
		vostrcpy (m_szAPIName, _T("voGetVP6DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingVP7)
	{
		strcpy (m_szCfgItem, "Video_Dec_VP7");
		vostrcpy (m_szDllFile, _T("voVP7Dec"));
		vostrcpy (m_szAPIName, _T("voGetVP7DecAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingVP8)
	{
		strcpy (m_szCfgItem, "Video_Dec_VP8");
		vostrcpy (m_szDllFile, _T("voVP8Dec"));
		vostrcpy (m_szAPIName, _T("voGetVP8DecAPI"));
	}

	if (m_pConfig != NULL)
	{
		m_nPassCodec = m_pConfig->GetItemValue (m_szCfgItem, (char*)"PassCodec", 0);

		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");

		if (pDllFile != NULL)
			VOLOGI ("The video dec module is %s from config file.", pDllFile);

		if (pDllFile != NULL &&!strcmp (pDllFile, "voOMXWrap"))
		{
			m_bVOUsed = VO_FALSE;
			if(m_bOMXComp)
			{
				pDllFile = NULL;
				pApiName = NULL;
			}			
		}		
	}

	if(m_nCoding == m_nSetVideoNameType)
	{
		pDllFile = (VO_PCHAR)m_sSetVideoName;
		VOLOGI ("The video dec module is %s from setting.", pDllFile);
	}

	if(m_nCoding == m_nSetVideoAPIType)
	{
		pApiName = (VO_PCHAR)m_sSetVideoAPI;
		VOLOGI ("The video api is %s from setting.", pApiName);
	}

	if (m_nPassCodec > 0)
		return 1;

	if (pDllFile == NULL)
	{
		m_bVOUsed = VO_TRUE;
	}

#if defined _WIN32
	if (pDllFile != NULL && !m_bVOUsed)
	{
		TCHAR voDllFile[256];
		_tcscpy (voDllFile, m_szDllFile);

		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));

		if (!_tcscmp (voDllFile, m_szDllFile))
			m_bVOUsed = VO_TRUE;
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL && !m_bVOUsed)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL && !m_bVOUsed)
	{
		if (!vostrcmp (m_szDllFile, pDllFile))
			m_bVOUsed = VO_TRUE;

		vostrcpy (m_szDllFile, pDllFile);
	}
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);
#elif defined __SYMBIAN32__
	if (pDllFile != NULL && !m_bVOUsed)
	{
		if (!vostrcmp (m_szDllFile, pDllFile))
			m_bVOUsed = VO_TRUE;

		vostrcpy (m_szDllFile, pDllFile);
	}
	vostrcat (m_szDllFile, _T(".dll"));

	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);

#elif defined _MAC_OS	
	if (pDllFile != NULL && !m_bVOUsed)
	{
		if (!vostrcmp (m_szDllFile, pDllFile))
			m_bVOUsed = VO_TRUE;
		
		vostrcpy (m_szDllFile, pDllFile);
	}
	vostrcat (m_szDllFile, _T(".dylib"));
	
	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);
#endif
	

	//	VOLOGI ("File %s, API %s .", m_szDllFile, m_szAPIName);

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETVIDEODECAPI pAPI = (VOGETVIDEODECAPI) m_pAPIEntry;
	pAPI (&m_funDec);
#endif	//_LIB
#endif // _VO_LIB

	VOLOGI ("m_bVOUsed %d", m_bVOUsed);

	return 1;
}

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

bool	CVideoDecoder::IsRefFrame(VO_CODECBUFFER * pBuffer)
{
	if(m_nCoding != VO_VIDEO_CodingH264)
		return false;


	if (m_nPassCodec > 0)
		return false;

	VO_VIDEO_FRAMETYPE	nFrameType = VO_VIDEO_FRAME_NULL;
	VO_CODECBUFFER		inputBuffer;
	nFrameType = nFrameType;
	if (pBuffer != NULL)
	{
		memcpy (&inputBuffer, pBuffer, sizeof (VO_CODECBUFFER));
	}
	else if (m_pInputBuffer != NULL)
	{
		inputBuffer.Buffer = m_pInputBuffer->Buffer + m_nInputUsed;
		inputBuffer.Length = m_pInputBuffer->Length - m_nInputUsed;
	}
	else
		return false;

	if (inputBuffer.Length < 6)
		return false;

	int size = inputBuffer.Length;
	char * buffer = (char *)inputBuffer.Buffer;
	if (buffer[2]==0 && buffer[3]==1)
	{
		buffer+=4;
		size -= 4;
	}
	else
	{
		buffer+=3;
		size -= 3;
	}

	int naluType = buffer[0]&0x0f;
	int isRef	 = 1;
	while(naluType!=1&&naluType!=5)//find next NALU
	{
		//buffer = GetNextFrame(buffer,size)
		char* p = buffer;  
		char* endPos = buffer+size;
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				size  -= p-buffer;
				buffer = p+3;
				naluType = buffer[0]&0x0f;
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				size  -= p-buffer;
				buffer = p+4;
				naluType = buffer[0]&0x0f;
				break;
			}
		}
		if(p>=endPos)
			return true; 
	}
	
	if(naluType == 5)
		return true;

	//if(naluType==1)
	{
		isRef = (buffer[0]>>5) & 3;

		//VOLOGI("....isRef %d", (int)isRef);
	}
	return (isRef != 0);
}

VO_VIDEO_FRAMETYPE CVideoDecoder::GetH264FrameType(unsigned char * buffer , int size)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len;//value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;

	while(naluType!=1&&naluType!=5)//find next NALU
	{
		//buffer = GetNextFrame(buffer,size)
		unsigned char* p = buffer;  
		unsigned char* endPos = buffer+size;
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				size  -= p-buffer;
				buffer = p+3;
				naluType = buffer[0]&0x0f;
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				size  -= p-buffer;
				buffer = p+4;
				naluType = buffer[0]&0x0f;
				break;
			}
		}
		if(p>=endPos)
			return VO_VIDEO_FRAME_NULL; 

	}
	if(naluType==5)
		return VO_VIDEO_FRAME_I;//I_FRAME

	buffer++;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return VO_VIDEO_FRAME_NULL;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if (inf>=5)
	{
		inf-=5;
	}
	if(inf<0||inf>2)
		return VO_VIDEO_FRAME_NULL;
	else if (inf == 0)
		return VO_VIDEO_FRAME_P;
	else if (inf == 1)
		return VO_VIDEO_FRAME_B;
	else
		return VO_VIDEO_FRAME_I;

}

VO_BOOL CVideoDecoder::IsIOMXWorking()
{
	VO_BOOL bIsWorking = VO_FALSE;
	VO_U32 nRC = GetParam(VO_PID_IOMXDEC_IsWorking, &bIsWorking);
	if(VO_ERR_NONE != nRC)
		return VO_FALSE;

	return bIsWorking;
}

VO_U32 CVideoDecoder::SetVideoLibFile (VO_U32 nVideoType, VO_PTCHAR pFileName)
{
	m_nSetVideoNameType = nVideoType;
	
	if(pFileName)
		vostrcpy(m_sSetVideoName, pFileName);

	return 0;
}

VO_U32 CVideoDecoder::SetVideoAPIName (VO_U32 nVideoType, VO_PTCHAR pAPIName)
{
	m_nSetVideoAPIType = nVideoType;
	
	if(pAPIName)
		vostrcpy(m_sSetVideoAPI, pAPIName);

	return 0;
}

VO_U32 CVideoDecoder::voVideoMemAlloc (VO_S32 uID,  VO_MEM_INFO * pMemInfo)
{
	if (!pMemInfo)
		return VO_ERR_INVALID_ARG;

	pMemInfo->VBuffer = malloc (pMemInfo->Size);

	return 0;
}

VO_U32 CVideoDecoder::voVideoMemFree (VO_S32 uID, VO_PTR pMem)
{
	free (pMem);

	return 0;
}

VO_U32 CVideoDecoder::voVideoMemSet (VO_S32 uID, VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize)
{
	memset (pBuff, uValue, uSize);

	return 0;
}

VO_U32 CVideoDecoder::voVideoMemCopy (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	memcpy (pDest, pSource, uSize);

	return 0;
}

VO_U32 CVideoDecoder::voVideoMemCheck (VO_S32 uID, VO_PTR pBuffer, VO_U32 uSize)
{
	return 0;
}

VO_S32 CVideoDecoder::voVideoMemCompare (VO_S32 uID, VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize)
{
	return memcmp(pBuffer1, pBuffer2, uSize);
}

VO_U32 CVideoDecoder::voVideoMemMove (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	memmove (pDest, pSource, uSize);

	return 0;
}
bool CVideoDecoder::setSampleDrmInfo(const bool isDrmAppended)
{
	mIsDrmDataAppended = isDrmAppended;
	return true;
}
