	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIComp.h

	Contains:	CTIComp header file

	Written by:	David1
	Time: 2011/11/29

*******************************************************************************/
#ifndef __CTIComp_H__
#define __CTIComp_H__

#include <OMX_Component.h>
#include <media/IOMX.h>
#include <media/IMediaPlayerService.h>
#include <media/IMediaDeathNotifier.h>
#include <binder/IServiceManager.h>
#include "CBaseComp.h"

using namespace android;
struct TICompObserver;

class CTIComp : public CBaseComp, public RefBase
{
public:
  CTIComp(VO_CODEC_INIT_USERDATA * pUserData);
  virtual ~CTIComp(void);

	OMX_ERRORTYPE		Start(void);
	OMX_ERRORTYPE		Pause(void);
	OMX_ERRORTYPE		Stop(void);
	OMX_ERRORTYPE		Flush (void);

	OMX_ERRORTYPE		SetCompName (OMX_STRING pCompName);
	OMX_ERRORTYPE		ResetPort (int nIndex);
	void				ResetCrop(OMX_PTR ptr = NULL);

	OMX_ERRORTYPE		EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE		FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);

	VO_U32				SetVideoRenderSurface(VO_PTR pSuf);
	VO_U32				SetVideoInputData(VO_CODECBUFFER * pInput);
	VO_U32				GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);
	VO_U32				GetSourceParam(VO_U32 paramID, VO_PTR pData);

	OMX_ERRORTYPE				SendCommand (OMX_COMMANDTYPE nCmd, OMX_U32 nParam, OMX_PTR pData);
	OMX_ERRORTYPE				GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam, size_t size = 0);
	OMX_ERRORTYPE				SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam, size_t size = 0);
	OMX_ERRORTYPE				GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig, size_t size = 0);
	OMX_ERRORTYPE				SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig, size_t size = 0);
	OMX_ERRORTYPE				GetPortDefinition(OMX_PTR pParam, OMX_U32 index);

protected:
	friend class CTIPort;

	OMX_ERRORTYPE				Load2Idle(void);
	OMX_ERRORTYPE				Load2Executing(void);
	OMX_ERRORTYPE				CreatePorts (void);
	OMX_ERRORTYPE				SetComponentRole();
	OMX_ERRORTYPE				GetState(OMX_STATETYPE *pst = NULL);
	OMX_ERRORTYPE				connect();

protected: 
	VOMP_LISTENERINFO*				m_pListener;
	void*							m_pnode;	
	sp<IOMX>						m_pomx;
	sp<ANativeWindow>				m_pnativewindow;
	sp<IMediaPlayerService>			m_pmps;
	voCOMXThreadMutex				m_lock;
	void*							m_pdrmengine;
	OMX_U32							m_nflags;
	OMX_BOOL						m_bInited;

private:

	friend class TICompObserver;

	void on_message(const omx_message &msg);
};

#endif //__CTIComp_H__

