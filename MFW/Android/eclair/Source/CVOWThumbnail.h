	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWThumbnail.h

	Contains:	CVOWThumbnail header file

	Written by:	East Zhou

	Change History (most recent first):
	2011-03-29		East		Create file

*******************************************************************************/
#ifndef __CVOWThumbnail_H__
#define __CVOWThumbnail_H__

#include "CVOWPlayer.h"

class CVOWThumbnail : public CVOWPlayer
{
public:
	CVOWThumbnail();
	virtual ~CVOWThumbnail();

public:
	virtual int			Init();

	virtual	int			prepareForCapture();	// must call after SetSource
	virtual void		setResolution(int nWidth, int nHeight);
	virtual void		setPos(int nPos, int nMode);
	virtual	void		setColor(VO_IV_COLORTYPE nColor) {m_nSpecColor = nColor;}
	virtual void		setForbidBlackFrame(bool bForbidBlackFrame) {m_bForbidBlackFrame = bForbidBlackFrame;}
	virtual int			capture(VO_VIDEO_BUFFER ** ppVideoBuffer, VO_VIDEO_FORMAT ** ppVideoFormat, int* pnRotation);
	virtual	int			getOrigResolution(int* pnWidth, int* pnHeight);
	virtual int			SetFileBasedDrm(void* pFileOpt, void* pDrmAPI);

protected:
	virtual int			RenderVideo(VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize);

	virtual int			setPosB(int nPos, int nMode);
	virtual int			captureB();

	OMX_U32				vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);

protected:
	int					m_nSpecWidth;
	int					m_nSpecHeight;
	int					m_nSpecPos;
	int					m_nSpecMode;
	VO_IV_COLORTYPE		m_nSpecColor;
	bool				m_bForbidBlackFrame;

	VO_VIDEO_FORMAT		m_fmtVideo;

	VO_VIDEO_BUFFER *	m_pVideoBuffer;
	VO_VIDEO_FORMAT *	m_pVideoFormat;
	VO_FILE_OPERATOR*	m_pFileOP;
	IVOFileBasedDRM*	m_pDRMAPI;
};

#endif // __CVOWThumbnail_H__
