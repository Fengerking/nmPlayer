	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseGraph.h

	Contains:	voCOMXBaseGraph header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXBaseGraph_H__
#define __voCOMXBaseGraph_H__

#include <OMX_Types.h>
#include <vomeAPI.h>

#include "voCOMXBaseObject.h"
#include "voOMXThread.h"
#include "voCOMXThreadSemaphore.h"
#include "voCOMXTaskQueue.h"

#include "voCOMXCfgCore.h"
#include "voCOMXCfgComp.h"
#include "voCOMXCompList.h"
#include "voCOMXCompBaseChain.h"

#define _VO_NDK_CheckLicense

#ifdef _VO_NDK_CheckLicense
#include "voVideo.h"
typedef VO_U32 (VO_API * VOCHECKDLLINIT) (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_PTR pCheckBuffer);
typedef VO_U32 (VO_API * VOCHECKDLLCHECKIMAGE) (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pFormat);
typedef VO_U32 (VO_API * VOCHECKDLLUNINIT) (VO_PTR hCheck);

typedef struct _LICENSEBUFFER {
	unsigned char *	m_pSource[4];
	unsigned char *	m_pOutput[4];
	unsigned char *	m_pKey[4];
} LicenseBuffer, * PLicenseBuffer;

#endif // _VO_NDK_CheckLicense

#define GRAPH_MESSAGE_TYPE_CALLBACK			0x80000000

typedef struct GRAPH_MESSAGE_TYPE {
	OMX_U32 uType;
	union {
		struct
		{
			OMX_U32			uType;
			OMX_PTR			pData1;
			OMX_PTR			pData2;
		} callback;
		struct
		{
			voCOMXBaseBox*	pBox;
			OMX_U32			nPort;
		} reconfig_port;
	} sInfo;
} GRAPH_MESSAGE_TYPE;

// wrapper for whatever critical section we have
class voCOMXBaseGraph : public voCOMXBaseObject
{
friend class voCOMXCompBaseChain;

public:
    voCOMXBaseGraph(void);
    virtual ~voCOMXBaseGraph(void);

	virtual OMX_ERRORTYPE	Init (OMX_PTR hInst);
	virtual void			SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData);
	virtual OMX_ERRORTYPE	SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay);
	virtual OMX_ERRORTYPE	GetParam (OMX_S32 nID, OMX_PTR pValue);
	virtual OMX_ERRORTYPE	SetParam (OMX_S32 nID, OMX_PTR pValue);

	virtual OMX_ERRORTYPE 	Playback (VOME_SOURCECONTENTTYPE * pSource);
	virtual OMX_ERRORTYPE 	Record (VOME_RECORDERFILETYPE * pRecFile);
	virtual OMX_ERRORTYPE 	Snapshot (VOME_RECORDERIMAGETYPE * pRecImage);
	virtual OMX_ERRORTYPE 	Transcode (VOME_TRANSCODETYPE * pTranscode);
	virtual OMX_ERRORTYPE 	Close (void);

	virtual OMX_ERRORTYPE 	Run (void);
	virtual OMX_ERRORTYPE 	Pause (void);
	virtual OMX_ERRORTYPE 	Stop (void);
	virtual OMX_ERRORTYPE 	GetStatus (VOMESTATUS * pStatus);

	virtual OMX_ERRORTYPE 	GetDuration (OMX_S32 * pDuration);
	virtual OMX_ERRORTYPE 	GetCurPos (OMX_S32 * pCurPos);
	virtual OMX_ERRORTYPE 	SetCurPos (OMX_S32 nCurPos);

	virtual OMX_ERRORTYPE 	SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack);
	virtual OMX_ERRORTYPE 	EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex);
	virtual OMX_ERRORTYPE 	GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles);
	virtual OMX_ERRORTYPE 	GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames);
	virtual OMX_ERRORTYPE 	AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	virtual OMX_ERRORTYPE 	RemoveComponent (OMX_COMPONENTTYPE * pComponent);
	virtual OMX_ERRORTYPE 	ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
											OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel);
	virtual OMX_ERRORTYPE 	GetComponentByName (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	virtual OMX_ERRORTYPE 	GetComponentByIndex (OMX_U32 nIndex, OMX_COMPONENTTYPE ** ppComponent);

	virtual OMX_ERRORTYPE	SaveGraph (OMX_STRING pFile);
	virtual OMX_ERRORTYPE	OpenGraph (OMX_STRING pFile);

	virtual OMX_ERRORTYPE	NotifyCallbackMsg (OMX_U32 nType, OMX_PTR pData1, OMX_PTR pData2);

protected:
	virtual voCOMXBaseBox *	TryConnectAudioDec (voCOMXBaseHole * pOutputHole, OMX_U32 nTimes, OMX_BOOL& bExit);
	virtual voCOMXBaseBox *	TryConnectAudioEnc (voCOMXBaseHole * pOutputHole, OMX_AUDIO_CODINGTYPE nCoding, OMX_U32 nBitrate, OMX_VO_FILE_FORMATTYPE nFormat, OMX_U32 nTimes, OMX_BOOL& bExit);
	virtual voCOMXBaseBox *	TryConnectVideoDec (voCOMXBaseHole * pOutputHole, OMX_U32 nTimes, OMX_BOOL& bExit);
	virtual voCOMXBaseBox *	TryConnectVideoEnc (voCOMXBaseHole * pOutputHole, OMX_VIDEO_CODINGTYPE nCoding, OMX_U32 nBitrate, OMX_U32 xFramerate, OMX_U32 nQuality, OMX_U32 nTimes, OMX_BOOL& bExit);
	virtual voCOMXBaseBox * TryConnectVideoRsz (voCOMXBaseHole * pOutputHole, OMX_U32 nWidth, OMX_U32 nHeight);

	virtual voCOMXBaseHole*	AddEffectComp (voCOMXBaseHole * pOutputHole, OMX_BOOL bAudioEffect = OMX_TRUE);

	OMX_BOOL				InitLicenseCheck (void);
	OMX_STRING				GetWorkPath (void) {return m_pWorkPath;}
	OMX_PTR					GetInstance (void) {return m_hInstance;}

protected:
	voCOMXThreadMutex		m_tmState;
	voCOMXBaseConfig *		m_pConfig;

	voCOMXCfgCore			m_cfgCore;
	voCOMXCfgComp			m_cfgComp;
	voCOMXCompList			m_cmpList;

	VOMESTATUS				m_sStatus;
	voCOMXCompBaseChain *	m_pChain;
	OMX_VO_DISPLAYAREATYPE	m_dispArea;
	OMX_BOOL				m_bInit;
	OMX_STRING				m_pWorkPath;
	OMX_STRING				m_pCompCfgFile;
	OMX_STRING				m_pLicenseText;
	OMX_VO_LIB_OPERATOR		m_libOP;
	OMX_PTR					m_hInstance;
	OMX_VO_LIB_OPERATOR *	m_pExtLibOP;

	OMX_S32					m_nSeekTime;
	OMX_S32					m_nSeekPos;

	OMX_S32					m_nStartRunTime;

	OMX_BOOL				m_bTunnelMode;
	OMX_U32					m_nPlayFlag;

	OMX_U32					m_nVideoWidth;
	OMX_U32					m_nVideoHeight;
	OMX_S32					m_nVideoRotation;

#ifdef _VO_NDK_CheckLicense
#ifdef _WIN32
	HMODULE					m_hVidDec;
#else
	OMX_PTR					m_hVidDec;
#endif // _WIN32
	VO_PTR					m_hLcsCheck;
	VOCHECKDLLUNINIT		m_fUninitCheck;
	VOCHECKDLLCHECKIMAGE	m_fCheckImage;
#endif // _VO_NDK_CheckLicense

protected:
	VOMECallBack				m_pCallBack;
	OMX_PTR						m_pUserData;
	CP_PIPETYPE *				m_pFilePipe;

	OMX_VO_CHECKRENDERBUFFERTYPE	m_cbVideoRender;
	OMX_VO_CHECKRENDERBUFFERTYPE	m_cbAudioRender;

	voCOMXTaskQueue			m_tqMessage;
	voCOMXThreadSemaphore	m_tsMessage;
	voCOMXThreadMutex		m_tmLipOP;

	voOMXThreadHandle		m_hMsgThread;
	OMX_U32					m_nMsgThreadID;
	OMX_BOOL				m_bMsgThreadStop;

public:
	static OMX_U32	voGraphMessageHandleProc (OMX_PTR pParam);
	static OMX_PTR	OMX_mmLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag);
	static OMX_PTR	OMX_mmGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag);
	static OMX_S32	OMX_mmFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag);


protected:
	virtual OMX_U32	voGraphMessageHandle (void);

};

#endif //__voCOMXBaseGraph_H__
