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
#include "voH264.h"

#include "CVideoH264Decoder.h"

#pragma warning (disable : 4996)

CVideoH264Decoder::CVideoH264Decoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CVideoDecoder (hInst, nCoding, pMemOP)
	, m_bAVC (VO_TRUE)
	, m_nNALLengthSize (4)
{
}

CVideoH264Decoder::~CVideoH264Decoder ()
{
	Uninit ();
}

void CVideoH264Decoder::SetFourCC (VO_U32 nFourCC)
{
	m_nFourCC = nFourCC;

	if (m_nFourCC == '1CVA')
		m_bAVC = VO_TRUE;
	else
		m_bAVC = VO_FALSE;
}

VO_U32 CVideoH264Decoder::Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_VIDEO_FORMAT * pFormat)
{
	Uninit ();

	if (LoadLib (m_hInst) == 0)
		return VO_ERR_VOMMP_VideoDecoder;

	m_OutputInfo.Format.Width = pFormat->Width;
	m_OutputInfo.Format.Height = pFormat->Height;
	m_OutputInfo.InputUsed = 0;

	VO_CODEC_INIT_USERDATA	initInfo;
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = m_pMemOP;

	VO_U32 nRC = m_funDec.Init (&m_hDec, (VO_VIDEO_CODINGTYPE)m_nCoding, &initInfo);

	if (nHeadSize > 0)
	{
		VO_CODECBUFFER buffHead;
		buffHead.Buffer = pHeadData;
		buffHead.Length = nHeadSize;

		VO_U32	nStreamType = VO_H264_ANNEXB;

		if (m_bAVC)
		{
			nStreamType = VO_H264_14496_15;
			nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_STREAMFORMAT, &nStreamType);

			VO_PBYTE	pData = pHeadData;
			VO_U32		nDataLen = nHeadSize;

			while (nDataLen > 0)
			{
				VO_S32 len = *pData++;
				len <<= 8;
				len += *pData++;

				//forbid main profile
				if(len >= 2 && pData[1] == 0x4d)
					return VO_ERR_VOMMP_VideoDecoder;

				buffHead.Buffer = pData;
				buffHead.Length = len;
				nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_SEQUENCE_PARAMS, &buffHead);

				nDataLen -= len + 2;
				pData += len;

				if (nDataLen <= 0)
					break;

				len = *pData++;
				len <<= 8;
				len += *pData++;

				buffHead.Buffer = pData;
				buffHead.Length = len;
				nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_PICTURE_PARAMS, &buffHead);

				nDataLen -= len + 2;
				pData += len;
			}
		}
		else
		{
			nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_STREAMFORMAT, &nStreamType);
	
			nRC = m_funDec.SetParam (m_hDec, VO_PID_COMMON_HEADDATA, &buffHead);
		}

		if (nRC == VO_ERR_NONE)
			nRC = m_funDec.GetParam (m_hDec, VO_PID_VIDEO_FORMAT, &m_OutputInfo);
	}

	return VO_ERR_NONE;
}

VO_U32 CVideoH264Decoder::SetInputData (VO_CODECBUFFER * pInput)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

//	m_nInputSize = pInput->Length;
//	m_nInputUsed = 0;

//	if (m_nInputSize == 0)
	{
		m_nInputSize = pInput->Length ;
		m_pInputData = pInput->Buffer;
	}

	VO_U32 nRC = 0;

	if (m_nInputSize > 4)
	{
		VO_S32	nFrameSize = 0;

		for (int i = 0; i < m_nNALLengthSize; i++)
		{
			nFrameSize <<= 8;
			nFrameSize += *m_pInputData++;
			if (nFrameSize == m_nInputSize - i - 1)
			{
				m_nNALLengthSize = i + 1;
				break;
			}
		}
		m_nInputSize -= m_nNALLengthSize;
		if (nFrameSize > m_nInputSize || nFrameSize <= 0)
		{
			m_nInputSize = 0;
			return VO_ERR_INPUT_BUFFER_SMALL;
		}

		pInput->Buffer = m_pInputData;
		pInput->Length = m_nInputSize;

		nRC = m_funDec.SetInputData (m_hDec, pInput);
	}
	else
	{
		nRC = m_funDec.SetInputData (m_hDec, pInput);
	}

	return 0;
}

VO_U32 CVideoH264Decoder::GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = 0;

	if (m_nInputSize == 0 || m_nInputSize <= m_nInputUsed + 10)
	{
		m_nInputUsed = 0;
		nRC = VO_ERR_INPUT_BUFFER_SMALL;
		return nRC;
	}

	nRC = m_funDec.GetOutputData (m_hDec, pOutput, &m_OutputInfo);

	if (nRC == VO_ERR_NONE)
	{
		pVideoFormat->Width = m_OutputInfo.Format.Width;
		pVideoFormat->Height = m_OutputInfo.Format.Height;
	}

	if (m_OutputInfo.InputUsed > 0 && m_nInputUsed == m_OutputInfo.InputUsed)
	{
		m_nInputUsed = 0;
		nRC = VO_ERR_INPUT_BUFFER_SMALL;
		return nRC;
	}

	m_nInputUsed += m_OutputInfo.InputUsed;

	nRC = nRC & 0X8000FFFF;


	m_nInputSize = 0;

	return nRC;
}

VO_U32 CVideoH264Decoder::Flush (void)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32	nFlush = 1;

	VO_U32 nRC = m_funDec.SetParam (m_hDec, VO_ID_H264_FLUSH, &nFlush);

	m_nInputSize = 0;
	m_nInputUsed = 0;
	m_OutputInfo.InputUsed = 0;

	return nRC;
}

