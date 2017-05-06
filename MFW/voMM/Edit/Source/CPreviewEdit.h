/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CPreviewEdit.h

Contains:	CPreviewEdit header file

Written by:	 Yu Wei

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#ifndef __CPreviewEdit_H__
#define __CPreviewEdit_H__

#include "CBaseEdit.h"



class CPreviewEdit : public CBaseEdit
{

protected:
	VO_VIDEO_BUFFER			m_videoRenderBuffer;

	VO_S64					m_nAudioRenderStart;
	VO_U32					m_nAudioRenderFill;
	VO_U32					m_nAudioRenderCount;


	VO_S64					m_nVideoRenderTime;
	VO_U32					m_nVideoRenderCount;
	VO_U32					m_nVideoRenderSpeed;
	VO_U32					m_nVideoRenderDropTime;
	VO_BOOL					m_bVideoRenderDrop;

	VO_U32					m_nAudioBytesPerSec;
	VO_U32					m_nAudioRenderSize;
	VO_U32					m_nAudioRenderStep;



	 
protected:
	virtual VO_U32		Release (void);
	virtual VO_U32		AllocAudioRenderBuffer (void);
	virtual VO_U32      UpdateWithConfig (void);
	
public:
	CPreviewEdit (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CPreviewEdit (void);

	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);


	virtual VO_U32 		Run (void);
	virtual VO_U32 		Stop (void);

	virtual VO_U32		SetCurPos (VO_S32 nPos);

	virtual VO_U32		PlaybackVideoFrame (void);
	virtual VO_U32		PlaybackAudioFrame ();

	virtual VO_U32		PlaybackAudioLoop (void);
	virtual VO_U32		PlaybackVideoLoop (void);
};

#endif // __CPreviewEdit_H__
