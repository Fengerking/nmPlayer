	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoH264Decoder.h

	Contains:	CVideoH264Decoder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CVideoH264Decoder_H__
#define __CVideoH264Decoder_H__

#include <voVideo.h>
#include "CVideoDecoder.h"

class CVideoH264Decoder : public CVideoDecoder
{
public:
	CVideoH264Decoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP);
	virtual ~CVideoH264Decoder (void);

	virtual void		SetFourCC (VO_U32 nFourCC);

	virtual VO_U32		Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_VIDEO_FORMAT * pFormat);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat);
	virtual VO_U32		Flush (void);

protected:
	VO_BOOL				m_bAVC;
	VO_U32				m_nNALLengthSize;
};

#endif // __CVideoH264Decoder_H__
