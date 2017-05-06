	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseCompWrap.h

	Contains:	CBaseCompWrap header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-06-25		JBF			Create file

*******************************************************************************/
#ifndef __CBaseCompWrap_H__
#define __CBaseCompWrap_H__

#include "voVideo.h"
#include "voAudio.h"
#include "CBaseComp.h"

// wrapper for whatever critical section we have
class CBaseCompWrap
{
public:
    CBaseCompWrap(VO_COMPWRAP_CODECTYPE nCodecType, VO_U32 nCoding,VO_CODEC_INIT_USERDATA * pUserData);
    virtual ~CBaseCompWrap(void);

	virtual VO_U32 vdSetInputData(VO_CODECBUFFER * pInput);
	virtual VO_U32 vdGetOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);

	virtual VO_U32 adSetInputData(VO_CODECBUFFER * pInput);
	virtual VO_U32 adGetOutputData(VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);

	virtual VO_U32 vrSetVideoInfo(VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	virtual VO_U32 vrSetInputData(VO_VIDEO_BUFFER * pInput);

	virtual VO_U32 SetParam(VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32 GetParam(VO_S32 uParamID, VO_PTR pData);

protected:
	CBaseComp *		m_pCompWrap;
};

#endif //__CBaseCompWrap_H__
