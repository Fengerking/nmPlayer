	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXComponent.h

	Contains:	Android IOMX component header file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-20	East		Create file

*******************************************************************************/
#ifndef __CvoIOMXComponent_H__
#define __CvoIOMXComponent_H__

#include <media/IOMX.h>
#include <media/IMediaPlayerService.h>

#if defined (_GB) || defined (_FRY)
#include <surfaceflinger/Surface.h>
#endif	// _GB, FRY

#include <cutils/properties.h>
#include "OMX_Component.h"
#include "voVideo.h"
#include "voCOMXThreadMutex.h"
#include "CvoIOMXInPort.h"
#include "CvoIOMXOutPort.h"
#include "voOMXThread.h"
#include "voCOMXTaskQueue.h"
#include "voCOMXThreadSemaphore.h"

typedef enum COMP_TRANS_STATUS
{
	COMP_TRANS_None				= 0x00, 
	COMP_TRANS_Idle2Loaded, 
	COMP_TRANS_WaitRes2Loaded, 
	COMP_TRANS_Loaded2WaitRes, 
	COMP_TRANS_WaitRes2Idle, 
	COMP_TRANS_Loaded2Idle, 
	COMP_TRANS_Pause2Idle, 
	COMP_TRANS_Execute2Idle, 
	COMP_TRANS_Idle2Execute, 
	COMP_TRANS_Pause2Execute, 
	COMP_TRANS_Idle2Pause, 
	COMP_TRANS_Execute2Pause, 
	COMP_TRANS_Any2Invalid, 
	COMP_TRANS_MAX				= 0x7FFFFFFF
} COMP_TRANS_STATUS;

typedef enum COMP_TYPE
{
	COMP_UNKNOWN	= 0x00, 
	COMP_QCOM, 
	COMP_TI_OMAP3, 
	COMP_TI_OMAP4, 
	COMP_NVIDIA, 
	COMP_SAMSUNG, 
	COMP_HUAWEI, 
	COMP_MARVELL, 
	COMP_STE, 
	COMP_SEU, 
	COMP_TELECHIPS, 
	COMP_RKCHIPS_A11,
	COMP_RKCHIPS_MK808,
	COMP_MAX		= 0x7FFFFFFF
} COMP_TYPE;

typedef struct EVENT_TYPE {
	OMX_EVENTTYPE	eEvent;
	OMX_U32			nData1;
	OMX_U32			nData2;
} EVENT_TYPE;

struct voIOMXObserver;
class CvoIOMXComponent : public RefBase
{
	friend class voIOMXObserver;
	friend class CvoIOMXPort;
	friend class CvoIOMXInPort;
	friend class CvoIOMXOutPort;

public:
	static OMX_U32				FindNALHeaderInBuffer(OMX_U8 * pBuffer, OMX_U32 nLength);
	static OMX_U32				EventHandleProc(OMX_PTR pParam);
	
	OMX_BOOL						m_bProbeMode;
public:
	CvoIOMXComponent(OMX_VIDEO_CODINGTYPE eCodec);
	virtual ~CvoIOMXComponent();

public:
	// 1, allocate component
	// 2, set state to Loaded
	virtual OMX_ERRORTYPE		Init();
	// 1, set state to Loaded from (Loaded / Executing)
	// 2, free component
	virtual void				Uninit();

	// if state is Loaded, set state to Executing
	virtual OMX_ERRORTYPE		SetInputData(VO_CODECBUFFER * pInput);
	virtual OMX_ERRORTYPE		GetOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);

	virtual OMX_ERRORTYPE		SetParam(VO_S32 uParamID, VO_PTR pData);
	virtual OMX_ERRORTYPE		GetParam(VO_S32 uParamID, VO_PTR pData);

protected:	// IOMX wrap functions
	OMX_ERRORTYPE		SendCommand(OMX_COMMANDTYPE nCmd, OMX_S32 nParam);
	OMX_ERRORTYPE		GetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pParams, size_t nSize);
	OMX_ERRORTYPE		SetParameter(OMX_INDEXTYPE nIndex, const OMX_PTR pParams, size_t nSize);
	OMX_ERRORTYPE		GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pParams, size_t nSize);
	OMX_ERRORTYPE		SetConfig(OMX_INDEXTYPE nIndex, const OMX_PTR pParams, size_t nSize);
	// implemented from ICS
	OMX_ERRORTYPE		GetState(OMX_STATETYPE * peState);

	// used by output port
	OMX_ERRORTYPE		AllocateBuffer(OMX_U32 nPortIndex, size_t nBufferSize, IOMX::buffer_id * phBuffer, void ** ppBuffer);
	// used by input port
	OMX_ERRORTYPE		AllocateBufferWithBackup(OMX_U32 nPortIndex, const sp<IMemory>& piMemory, IOMX::buffer_id * phBuffer);
	OMX_ERRORTYPE		FreeBuffer(OMX_U32 nPortIndex, IOMX::buffer_id hBuffer);
	OMX_ERRORTYPE		FillBuffer(IOMX::buffer_id hBuffer);
	OMX_ERRORTYPE		EmptyBuffer(IOMX::buffer_id hBuffer, OMX_U32 nOffset, OMX_U32 nLength, OMX_U32 nFlags, OMX_TICKS nTimeStamp);

	OMX_ERRORTYPE		GetExtensionIndex(const char * szName, OMX_INDEXTYPE * pnIndex);

#if defined (_GB) || defined (_FRY)
	// after checking stagefright_surface_output_msm7x30.cpp
	// libstagefrighthw.so will check color format and if HW decoder, then create, otherwise fail
	sp<IOMXRenderer>	CreateRenderer(const sp<Surface>& piSurface, const OMX_STRING szCompName, OMX_COLOR_FORMATTYPE eColor, 
		size_t nVideoWidth, size_t nVideoHeight, size_t nDispWidth, size_t nDispHeight, OMX_S32 nRotationDegrees);

#ifdef OMAP_ENHANCEMENT
	sp<IOMXRenderer>	CreateRenderer(const sp<Surface>& piSurface, const OMX_STRING szCompName, OMX_COLOR_FORMATTYPE eColor, 
		size_t nVideoWidth, size_t nVideoHeight, size_t nDispWidth, size_t nDispHeight, OMX_S32 nRotationDegrees, OMX_S32 nIsS3D, OMX_S32 nNumOfOpBuffers = -1);
	Vector< sp<IMemory> >	GetBuffers(const sp<IOMXRenderer>& piRenderer);
	OMX_ERRORTYPE		UseBuffer(OMX_U32 nPortIndex, const sp<IMemory>& piMemory, IOMX::buffer_id * phBuffer, size_t nSize);
#endif	// OMAP_ENHANCEMENT
#endif	// _GB, _FRY

#ifdef _ICS
#ifdef _KK
	OMX_ERRORTYPE		PrepareForAdaptivePlayback(OMX_U32 nPortIndex, OMX_BOOL bEnable, OMX_U32 maxFrameWidth, OMX_U32 maxFrameHeight);
#endif // _KK
	OMX_ERRORTYPE		EnableGraphicBuffers(OMX_U32 nPortIndex, OMX_BOOL bEnable);
	OMX_ERRORTYPE		GetGraphicBufferUsage(OMX_U32 nPortIndex, OMX_U32 * pnUsage);
	OMX_ERRORTYPE		UseGraphicBuffer(OMX_U32 nPortIndex, const sp<GraphicBuffer>& piGraphicBuffer, IOMX::buffer_id * phBuffer);

	OMX_S32				NativeWindow_CancelBuffer(voIOMXPortBuffer * pBuffer);
	OMX_S32				NativeWindow_QueueBuffer(voIOMXPortBuffer * pBuffer);
	OMX_S32				NativeWindow_SetCrop(OMX_S32 nLeft, OMX_S32 nTop, OMX_S32 nRight, OMX_S32 nBottom);
	voIOMXPortBuffer *	NativeWindow_PrepareBuffer(voIOMXPortBuffer * pBuffers, OMX_U32 nBuffers);
#endif	// _ICS

protected:
	// different OS version implement differently, 
	// we can refer to "\frameworks\base\media\libstagefright\OMXClient.cpp"
	virtual OMX_ERRORTYPE		Connect();

	virtual OMX_ERRORTYPE		SetHeader(OMX_PTR pHeader, OMX_U32 nSize);

	virtual OMX_ERRORTYPE		CreateComponent();
	virtual OMX_ERRORTYPE		CreatePorts();
	virtual OMX_ERRORTYPE		CreateVideoRenderer(OMX_S32 nNumOfOpBuffers = -1);

	virtual CvoIOMXPort *		GetPort(OMX_U32 nIndex);

	virtual OMX_ERRORTYPE		Loaded2Idle();
	virtual OMX_ERRORTYPE		Idle2Executing();
	virtual OMX_ERRORTYPE		Executing2Idle();
	virtual OMX_ERRORTYPE		Idle2Loaded();
	virtual OMX_ERRORTYPE		Flush();
	virtual OMX_ERRORTYPE		UpdateCrop();

	OMX_BOOL					IsFlushing() {return m_bFlushing;}

	virtual OMX_U32				EventHandle();

	virtual void				OnEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2);
	virtual void				ProcEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2);

private:
	void						OnMessage(const omx_message &msg);

protected:
	IOMX::node_id					m_hNode;
	sp<IOMX>						m_piOMX;

#if defined (_GB) || defined (_FRY)
	sp<Surface>						m_piSurface;
	sp<IOMXRenderer>				m_piOMXRenderer;		// actually, it is wrapper for libstagefrighthw.so
#endif	// _GB, _FRY

#ifdef _ICS
	sp<ANativeWindow>				m_piNativeWindow;
	voCOMXThreadMutex				m_tmNativeWindow;
#endif	// _ICS

	OMX_VERSIONTYPE					m_uMainVersion;

	voCOMXThreadMutex				m_tmLock;
	voCOMXThreadMutex				m_tmStatus;

	char							m_szCompName[OMX_MAX_STRINGNAME_SIZE];
	char							m_szDevName[2048];
	char							m_szBoardPlatform[PROPERTY_VALUE_MAX];
	VO_S32							m_nVersionNumber[3];

	COMP_TYPE						m_nType;
	OMX_STATETYPE					m_eState;
	COMP_TRANS_STATUS				m_eTransStatus;

	OMX_BOOL						m_bOMXError;
	OMX_BOOL						m_bFlushing;

	voCOMXThreadMutex				m_tmPortSettingsChanging;
	OMX_BOOL						m_bPortSettingsChanging;

	CvoIOMXInPort *					m_pInPort;
	CvoIOMXOutPort *				m_pOutPort;

	OMX_VIDEO_CODINGTYPE			m_eCodec;

	OMX_U8 *						m_pHeadData;
	OMX_U32							m_nHeadData;

	OMX_U32							m_nWidth;
	OMX_U32							m_nHeight;

	OMX_U32							m_nOriginalLeft;
	OMX_U32							m_nOriginalTop;

protected:
	voOMXThreadHandle				m_hEventThread;
	OMX_U32							m_nEventThreadID;
	OMX_BOOL						m_bEventThreadStop;

	voCOMXTaskQueue					m_tqEvent;
	voCOMXThreadSemaphore			m_tsEvent;
};

#endif	//__CvoIOMXComponent_H__
