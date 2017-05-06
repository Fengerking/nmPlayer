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
#include "CMpeg4VideoDecoder.h"


CMpeg4VideoDecoder::CMpeg4VideoDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CVideoDecoder (hInst, nCoding, pMemOP)
	, m_hDec (NULL)
{
}

CMpeg4VideoDecoder::~CMpeg4VideoDecoder ()
{
	Uninit ();
}

VO_U32 CMpeg4VideoDecoder::Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_VIDEO_FORMAT * pFormat)
{
	Uninit ();

	if (LoadLib (m_hInst) == 0)
		return VO_ERR_VOMMP_VideoDecoder;

	m_OutputInfo.Format.Width = pFormat->Width;
	m_OutputInfo.Format.Height = pFormat->Height;
	m_OutputInfo.InputUsed = 0;

	VOMPEG4DECRETURNCODE rc = m_fInit (&m_hDec);

	return VO_ERR_NONE;
}

VO_U32 CMpeg4VideoDecoder::Uninit (void)
{
	if (m_hDec != NULL)
		m_fUninit (m_hDec);
	m_hDec = NULL;

	return 0;
}

VO_U32 CMpeg4VideoDecoder::SetInputData (VO_CODECBUFFER * pInput)
{
	m_pInputData = pInput->Buffer;
	m_nInputSize = pInput->Length;

	return 0;
}

VO_U32 CMpeg4VideoDecoder::GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat)
{
	VO_U32 nRC = 0;

	if (m_nInputSize <= 0)
		return VO_ERR_INPUT_BUFFER_SMALL;
	
	VOMPEG4DECRETURNCODE rc;
	VOCODECDATABUFFER inBuffer;

	inBuffer.buffer = m_pInputData;
	inBuffer.length = m_nInputSize;

	rc = m_fProcess (m_hDec, &inBuffer, &m_yuvBuffer, &m_videoFormat);
	if (rc != VORC_OK)
	{
		Sleep (5);
		m_nInputSize = 0;
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	m_pInputData += inBuffer.length;
	m_nInputSize -= inBuffer.length;
	if (m_nInputSize <= 8)
		m_nInputSize = 0;

	pOutput->Buffer[0] = m_yuvBuffer.data_buf[0];
	pOutput->Buffer[1] = m_yuvBuffer.data_buf[1];
	pOutput->Buffer[2] = m_yuvBuffer.data_buf[2];

	pOutput->Stride[0] = m_yuvBuffer.stride[0];
	pOutput->Stride[1] = m_yuvBuffer.stride[1];
	pOutput->Stride[2] = m_yuvBuffer.stride[2];

	pVideoFormat->Width = m_videoFormat.width;
	pVideoFormat->Height = m_videoFormat.height;

	return VO_ERR_NONE;
}

VO_U32 CMpeg4VideoDecoder::Flush (void)
{
	return 0;
}

VO_U32 CMpeg4VideoDecoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	return 0XFF;
}

VO_U32 CMpeg4VideoDecoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	return 0XFF;
}

VO_U32 CMpeg4VideoDecoder::LoadLib (VO_HANDLE hInst)
{
	m_hDll = LoadLibrary (_T("voMpeg4Dec.dll"));

	m_fInit = (VOMPEG4DECINIT) GetProcAddress (m_hDll, _T("voMPEG4DecInit"));
	m_fProcess = (VOMPEG4DECPROCESS) GetProcAddress (m_hDll, _T("voMPEG4DecProcess"));
	m_fUninit = (VOMPEG4DECUNINIT) GetProcAddress (m_hDll, _T("voMPEG4DecUninit"));
	m_fSetParam = (VOMPEG4DECSETPARAMETER) GetProcAddress (m_hDll, _T("voMPEG4DecSetParameter"));
	m_fGetParam = (VOMPEG4DECGETPARAMETER) GetProcAddress (m_hDll, _T("voMPEG4DecGetParameter"));

	return 1;
}
