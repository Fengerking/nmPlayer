	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompBaseChain.h

	Contains:	voCOMXCompBaseChain header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXCompBaseChain_H__
#define __voCOMXCompBaseChain_H__

#include <OMX_Types.h>
#include "vomeAPI.h"

#include "voCOMXBaseObject.h"

#include "voCOMXCompList.h"
#include "voCOMXCfgComp.h"

#include "voCOMXBaseBox.h"

#define	VOMAX_BOX_NUM	32
#define	VOMAX_TIMEOUT	30000

class voCOMXBaseGraph;
class voCOMXFLOGraph;

// wrapper for whatever critical section we have
class voCOMXCompBaseChain : public voCOMXBaseObject
{
friend class voCOMXBaseGraph;
friend class voCOMXFLOGraph;

public:
    voCOMXCompBaseChain(voCOMXBaseGraph * pGraph, voCOMXCompList * pCompLisst,
						voCOMXCfgCore * pCfgCore,  voCOMXCfgComp * pCfgComp);
    virtual ~voCOMXCompBaseChain(void);

	virtual void			SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData);

	virtual	voCOMXBaseBox *	AddSource (VOME_SOURCECONTENTTYPE * pSource);
	virtual	voCOMXBaseBox *	AddClock (void);

	virtual voCOMXBaseBox *	AddBox (OMX_STRING pCompName);
	virtual voCOMXBaseBox *	FindBox (OMX_COMPONENTTYPE * pComponent);
	virtual OMX_ERRORTYPE 	RemoveBox (voCOMXBaseBox * pBox);
	virtual OMX_ERRORTYPE 	RemoveComp (OMX_COMPONENTTYPE * pComp);

	virtual OMX_ERRORTYPE	ReleaseBoxes (void);

	virtual OMX_ERRORTYPE	ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
												 OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel);
	virtual OMX_ERRORTYPE 	Connect (voCOMXBaseHole * pOutput, voCOMXBaseHole * pInput, OMX_BOOL bTunnel);

	virtual	OMX_ERRORTYPE	Run (OMX_BOOL bStartBuffer = OMX_FALSE);
	virtual	OMX_ERRORTYPE	Pause (void);
	virtual	OMX_ERRORTYPE	Stop (void);
	virtual	OMX_ERRORTYPE	Flush (void);
	virtual	OMX_ERRORTYPE	FlushAll (void);

	virtual OMX_S32			GetDuration (void);
	virtual OMX_S32			GetPos (void);
	virtual OMX_S32			SetPos (OMX_S32 nPos);

	virtual OMX_U32			EndOfStream (voCOMXBaseBox * pBox, voCOMXBaseHole * pHole);

	virtual OMX_BOOL		IsFlushing (void) {return m_bFlushing;}
	virtual OMX_BOOL		IsSeeking (void) {return m_bSeeking;}

	OMX_ERRORTYPE			ReconfigPort(voCOMXBaseHole* pHole, OMX_BOOL bUseThread);
	OMX_ERRORTYPE			ReconfigPortB(voCOMXBaseHole* pHole);

	static OMX_U32			ReconfigPortProc(OMX_PTR pParam);
	OMX_U32					ReconfigPortProcB();

	OMX_ERRORTYPE			DisablePort(voCOMXBaseHole * pHole);
	OMX_ERRORTYPE			EnablePort(voCOMXBaseHole * pHole);
	OMX_ERRORTYPE			ShutdownVideoDecoder(OMX_STATETYPE eStateBeforeCS);
	OMX_ERRORTYPE			RecreateVideoDecoder(OMX_STATETYPE eStateBeforeCS);

	OMX_ERRORTYPE			ChangeSurface(VOMEUpdateSurfaceCallBack fCallBack, OMX_PTR pUserData);

	OMX_ERRORTYPE 			ShutdownAudioDecoder(OMX_STATETYPE eStateBeforeST);
	OMX_ERRORTYPE 			RecreateAudioDecoder(OMX_STATETYPE eStateBeforeST);
	OMX_ERRORTYPE			SelectTrack(OMX_S32 nIndex,VOME_TRACKTYPE nType);

	voCOMXBaseGraph *		GetGraph (void) {return m_pGraph;}
	voCOMXBaseBox **		GetBoxList (void) {return &m_aBox[0];}

	virtual OMX_ERRORTYPE	SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack);
	OMX_CALLBACKTYPE *		GetCompCallBack (void) {return m_pClientCB;}
	voCOMXThreadMutex *		GetMutexCallBack (void) {return &m_mutCallBack;}

protected:
	virtual OMX_ERRORTYPE	SendBoxCommand (VOBOX_Command nCmd);
	virtual OMX_ERRORTYPE	CheckBoxStatus (VOBOX_Statue sStatus, OMX_U32 nTimeOut = VOMAX_TIMEOUT);
	virtual OMX_ERRORTYPE	CheckPortsStatus (PORT_TRANS_STATUS nStatus, OMX_U32 nTimeOut = VOMAX_TIMEOUT);
	virtual OMX_ERRORTYPE	CheckBoxPortSettingChanged (OMX_U32 nTimeOut = VOMAX_TIMEOUT);

protected:
	voCOMXBaseGraph *			m_pGraph;
	voCOMXCompList *			m_pCompList;
	voCOMXCfgCore *				m_pCfgCore;
	voCOMXCfgComp *				m_pCfgComp;

	OMX_U32						m_nBoxs;
	voCOMXBaseBox *				m_aBox[VOMAX_BOX_NUM];
	voCOMXBaseBox *				m_pSeekBox;
	voCOMXBaseBox *				m_pClockBox;
	voCOMXBaseBox *				m_pVideoDec;
	voCOMXBaseBox *				m_pAudioDec;
	voCOMXBaseBox *				m_pVideoSink;
	voCOMXBaseBox *				m_pAudioSink;
	voCOMXBaseBox *				m_pSource;

	OMX_U32						m_nSourceType;

	OMX_BOOL					m_bEndOfAudio;
	OMX_BOOL					m_bEndOfVideo;
	OMX_BOOL					m_bEndOfSource;

	OMX_CALLBACKTYPE			m_CallBack;
	OMX_STATETYPE				m_sState;
	OMX_BOOL					m_bFlushing;
	OMX_BOOL					m_bSeeking;
	OMX_BOOL					m_bSeeked;
	OMX_BOOL					m_bFlushed;
	OMX_BOOL					m_bSeekVideoRender;
	voCOMXBaseHole*				m_pReconfigPortHole;

	voCOMXThreadMutex			m_mutCallBack;
	voCOMXThreadMutex			m_mutStatus;

	OMX_CALLBACKTYPE *			m_pClientCB;

	OMX_VO_SOURCEDRM_CALLBACK *	m_pDrmCB;
	OMX_VO_LIB_OPERATOR *		m_pLibOP;
	OMX_STRING					m_pWorkPath;

	VOMECallBack				m_pCallBack;
	OMX_PTR						m_pUserData;

	//add by MaTao for seek position after stop
	OMX_U32						mnCurPos;
	OMX_U32						m_nExitPos;
public:
	static OMX_ERRORTYPE voOMXEventHandler (OMX_IN OMX_HANDLETYPE hComponent,
											OMX_IN OMX_PTR pAppData,
											OMX_IN OMX_EVENTTYPE eEvent,
											OMX_IN OMX_U32 nData1,
											OMX_IN OMX_U32 nData2,
											OMX_IN OMX_PTR pEventData);

	static OMX_ERRORTYPE voOMXEmptyBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_IN OMX_PTR pAppData,
												OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

	static OMX_ERRORTYPE voOMXFillBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_IN OMX_PTR pAppData,
												OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
};

#endif //__voCOMXCompBaseChain_H__
