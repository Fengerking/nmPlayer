	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CMpeg4VideoDecoder.h

	Contains:	CMpeg4VideoDecoder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CMpeg4VideoDecoder_H__
#define __CMpeg4VideoDecoder_H__

#include <voVideo.h>
#include "CVideoDecoder.h"

#include ".\Include\mpeg4dec.h"

typedef VOMPEG4DECRETURNCODE (VOCODECAPI * VOMPEG4DECINIT) (HVOCODEC *phCodec);
typedef VOMPEG4DECRETURNCODE (VOCODECAPI * VOMPEG4DECPROCESS) (HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECVIDEOBUFFER *pOutData, VOCODECVIDEOFORMAT *pOutFormat);
typedef VOMPEG4DECRETURNCODE (VOCODECAPI * VOMPEG4DECUNINIT) (HVOCODEC hCodec);
typedef VOMPEG4DECRETURNCODE (VOCODECAPI * VOMPEG4DECSETPARAMETER) (HVOCODEC hCodec, LONG nID, LONG lValue);
typedef VOMPEG4DECRETURNCODE (VOCODECAPI * VOMPEG4DECGETPARAMETER) (HVOCODEC hCodec, LONG nID, LONG *plValue);

class CMpeg4VideoDecoder : public CVideoDecoder
{
public:
	CMpeg4VideoDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CMpeg4VideoDecoder (void);

	virtual VO_U32		Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_VIDEO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	HVOCODEC				m_hDec;

	VOMPEG4DECINIT			m_fInit;
	VOMPEG4DECPROCESS		m_fProcess;
	VOMPEG4DECUNINIT		m_fUninit;

	VOMPEG4DECSETPARAMETER	m_fSetParam;
	VOMPEG4DECGETPARAMETER	m_fGetParam;

	LPBYTE					m_pInputData;
	VOCODECVIDEOBUFFER		m_yuvBuffer;
	VOCODECVIDEOFORMAT		m_videoFormat;
};

#endif // __CMpeg4VideoDecoder_H__
