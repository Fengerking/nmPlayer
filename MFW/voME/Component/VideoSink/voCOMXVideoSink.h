	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoSink.h

	Contains:	voCOMXVideoSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXVideoSink_H__
#define __voCOMXVideoSink_H__

#include <voOMX_Index.h>

#include <voCOMXCompSink.h>
#include <voCOMXPortClock.h>

#include <CVideoRender.h>
#include "measure.h"

class voCOMXVideoSink : public voCOMXCompSink
{
public:
	voCOMXVideoSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXVideoSink(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE	FreeBuffer (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_U32 nPortIndex, 
										OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);


protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);
	virtual OMX_ERRORTYPE	CreateRender (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32 nPort);
	virtual OMX_ERRORTYPE	Disable (OMX_U32 nPort);

	virtual OMX_ERRORTYPE	InitPortType (void);

protected:
	virtual OMX_U32			BufferHandle (void);
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual OMX_ERRORTYPE	RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

	virtual OMX_BOOL		WaitForRenderTime(OMX_TICKS nTime, OMX_TICKS* pnCurrMediaTime);

	inline void				FinishBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	inline OMX_BOOL			SaveHoldBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	inline void				ProcHoldBuffer (void);

	bool                    isVO_420_PACK_2(OMX_COLOR_FORMATTYPE inColor);
	bool                    isVO_PLANAR420(OMX_COLOR_FORMATTYPE inColor);
protected:
	voCOMXPortClock *				m_pClockPort;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_VIDEO_PARAM_PORTFORMATTYPE 	m_videoFormat;


protected:
	OMX_VO_CHECKRENDERBUFFERTYPE	m_cbRenderBuffer;
	OMX_VO_VIDEOBUFFERTYPE			m_cbVideoBuffer;
	OMX_BUFFERHEADERTYPE *			m_pRenderBuffer;
	OMX_BUFFERHEADERTYPE *			m_pHoldBuffer;
	voCOMXThreadMutex				m_tmHoldBuffer;

	CVideoRender *					m_pVideoRender;
	VO_MEM_OPERATOR *				m_pMemOP;
	VO_IV_COLORTYPE					m_nColorType;

	OMX_PTR							m_hView;
	VO_RECT							m_rcDisp;

	VO_VIDEO_BUFFER					m_videoBuffer;
	VO_U32							m_nYUVWidth;
	VO_U32							m_nYUVHeight;

	VO_U32							m_nRendFrames;
	VO_U32							m_nTotalRendFrames;
	VO_U32							m_nTotalExecutingTime;	// not include (current - m_nExecutingStartTime)
	VO_U32							m_nExecutingStartTime;
	VO_U32							m_nRenderSpeed;
	VO_S32							m_nFrameDropped;
	VO_BOOL							m_bDropVideo;
	VO_S32							m_nWrongStepTime;

	CBaseConfig *					m_pCfgCodec;
	OMX_BOOL						m_bThread;

	OMX_S64							m_llLastFrameTime;
	OMX_BOOL						m_bHoldBuffer;
	OMX_BOOL						m_bDualCore;

	OMX_PTR							m_pDumpInputFile;

	OMX_S32*						m_pVRTimeStamp;
	OMX_S32*						m_pAudioPlayTime;
protected:
	static VO_S32	videosinkVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart);
		   VO_S32	videosinkVideoRender (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart);

private:
	FrameRator* m_pFRateMeter;
};

#endif //__voCOMXVideoSink_H__
