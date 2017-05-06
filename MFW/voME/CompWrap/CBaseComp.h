	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseComp.h

	Contains:	CBaseComp header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __CBaseComp_H__
#define __CBaseComp_H__

#include <OMX_Component.h>

#include "voVideo.h"
#include "voAudio.h"

#include "voCBaseThread.h"

#include "COMXLoader.h"
#include "CBasePort.h"
#include "CBaseSource.h"
#include "voCSemaphore.h"

typedef enum {
	VO_COMPWRAP_CODEC_VideoDec		= 1,
	VO_COMPWRAP_CODEC_AudioDec		= 2,
	VO_COMPWRAP_CODEC_VideoEnc		= 3,
	VO_COMPWRAP_CODEC_AudioEnc		= 4,
	VO_COMPWRAP_RENDER_VIDEO		= 5,
	VO_COMPWRAP_CODEC_Max			= VO_MAX_ENUM_VALUE
} VO_COMPWRAP_CODECTYPE;

#define		VOCOMP_EVENT_COMMAND		(('v'<<24) + ('o' << 16) + ('c'<<8) + 'm' + 1)
#define		VOCOMP_EVENT_RESETPORT		1001


typedef struct OMX_TI_PARAM_BUFFERPREANNOUNCE
{
	OMX_U32 nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32 nPortIndex;
	OMX_BOOL bEnabled;
}OMX_TI_PARAM_BUFFERPREANNOUNCE;
#define OMX_TI_IndexParamBufferPreAnnouncement  0x7F000055
#define OMX_TI_IndexParam2DBufferAllocDimension	0x7F000057

class BufferElem
{
public:
	BufferElem():m_pBuffer(NULL), m_pNext(NULL){}
	~BufferElem()
	{
		if(m_pBuffer)
		{
			if(m_pBuffer->Buffer)
				free(m_pBuffer->Buffer);
			free(m_pBuffer);
		}
	}

	VO_CODECBUFFER * m_pBuffer;
	BufferElem*		m_pNext;
};

class CWrapBufferList
{
public:
	CWrapBufferList():m_pHead(NULL), m_pTail(NULL){}
	~CWrapBufferList()
	{
		resetList();
	}
public:
	VO_U32 addBuffer(VO_CODECBUFFER * pBuffer)
	{
		if(NULL == pBuffer)
			return VO_ERR_INVALID_ARG;

		BufferElem* pElem = new BufferElem();
		pElem->m_pBuffer = (VO_CODECBUFFER*)malloc(sizeof(VO_CODECBUFFER));
		pElem->m_pBuffer->Buffer = (VO_PBYTE)malloc(pBuffer->Length);
		pElem->m_pBuffer->Length = pBuffer->Length;
		pElem->m_pBuffer->Time	 = pBuffer->Time;
		memcpy(pElem->m_pBuffer->Buffer, pBuffer->Buffer, pBuffer->Length);

		if(m_pHead == NULL)
		{
			m_pHead = m_pTail = pElem;
		}
		else
		{
			m_pTail->m_pNext = pElem;
			m_pTail = pElem;
		}
		return VO_ERR_NONE;
	}
	VO_CODECBUFFER*	getBuffer()
	{
		if(m_pHead)
			return m_pHead->m_pBuffer;
		return NULL;
	}
	VO_U32 removeBuffer()
	{
		if(m_pHead == NULL)
			return VO_ERR_NONE;
		BufferElem* pHead = m_pHead;
		m_pHead = m_pHead->m_pNext;
		delete pHead;

		return VO_ERR_NONE;
	}
	void resetList()
	{
		BufferElem* pElem = m_pHead;
		while(pElem)
		{
			m_pHead = m_pHead->m_pNext;
			delete pElem;
			pElem = m_pHead;
		}
	}

protected:
	BufferElem*		m_pHead;
	BufferElem*		m_pTail;
};


// wrapper for whatever critical section we have
class CBaseComp
{
public:
    CBaseComp(VO_CODEC_INIT_USERDATA * pUserData);
    virtual ~CBaseComp(void);

	virtual OMX_ERRORTYPE		SetCoreFile (OMX_STRING pCoreFile);
	virtual OMX_ERRORTYPE		SetCompName (OMX_STRING pCompName);
	virtual VO_CHAR *			GetCompName(){return m_szCompName;}

	virtual OMX_ERRORTYPE		Start (void);
	virtual OMX_ERRORTYPE		Pause (void);
	virtual OMX_ERRORTYPE		Stop (void);
	virtual OMX_ERRORTYPE		Flush (void);
	virtual OMX_ERRORTYPE		ResetPort (int nIndex);

	virtual int					onNotifyEvent (int nMsg, int nV1, int nV2);
	virtual int					postCommandEvent (int nMsg, int nV1 = 0, int nV2 = 0, int nDelayTime = -1);

	virtual VO_U32				SetVideoInputData(VO_CODECBUFFER * pInput);
	virtual VO_U32				GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);

	virtual VO_U32				SetAudioInputData(VO_CODECBUFFER * pInput);
	virtual VO_U32				GetAudioOutputData(VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);

	virtual VO_U32				SetVideoRenderData(VO_VIDEO_BUFFER * pInput);
	virtual VO_U32				SetVideoRenderSurface(VO_PTR pSuf);
	virtual VO_VOID				SetThumbnailMode(VO_BOOL bThumbnailMode){m_bThumbnailMode = bThumbnailMode;};

	virtual OMX_ERRORTYPE		SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);

	virtual OMX_ERRORTYPE		SetCodecCoding (VO_COMPWRAP_CODECTYPE nCodecType, VO_U32 nCoding);
	virtual OMX_ERRORTYPE		SetVideoSize (VO_U32 nWidth, VO_U32 nHeight);
	virtual OMX_ERRORTYPE		SetAudioFormat (VO_U32 nSampleRate, VO_U32 nChannels, VO_U32 nSampleBits);
	virtual OMX_ERRORTYPE		SetHeader (OMX_PTR pHeader, OMX_U32 nSize);
	
	OMX_ERRORTYPE				SendCommand (OMX_COMMANDTYPE nCmd, OMX_U32 nParam, OMX_PTR pData);
	OMX_ERRORTYPE				GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	OMX_ERRORTYPE				SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	OMX_ERRORTYPE				GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);
	OMX_ERRORTYPE				SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);

	OMX_COMPONENTTYPE *			GetComponentHandle (void) {return m_pComponent;}
	VO_COMPWRAP_CODECTYPE		GetComWrapType(){return m_nCodecType;}
	VO_BOOL						GetThumbMode(){return m_bThumbnailMode;}
	VO_CHAR*					GetComponentName(){return m_szCompName;}
	VO_U32						GetSourceParam(VO_U32 paramID, VO_PTR pData);
	VO_U32						GetDecParam(VO_U32 nparam, VO_PTR pData);

protected:
	virtual OMX_ERRORTYPE		CreateComponent(void);
	virtual OMX_ERRORTYPE		CreatePorts (void);
	OMX_ERRORTYPE				SetComponentRole();

	VO_U32						WaitCommandDone (void);
	VO_U32						SetSurfaceToVideoRender();
	
protected:
	voCOMXThreadMutex			m_muxStatus;
	VO_CHAR						m_szCoreFile[256];
	VO_CHAR						m_szCompName[256];
	COMXLoader *				m_pCore;
	OMX_COMPONENTTYPE *			m_pComponent;

	OMX_S8						m_szName[128];
	OMX_VERSIONTYPE				m_verMain;
	OMX_VERSIONTYPE				m_verSpec;
	OMX_UUIDTYPE				m_uuidType;
	OMX_STATETYPE				m_sState;
	OMX_BOOL					m_bSendMsg;

	OMX_CALLBACKTYPE			m_CallBack;

	CBasePort *					m_pInputPort;
	CBasePort *					m_pOutputPort;

	VO_PBYTE					m_pHeadData;
	VO_U32						m_nHeadSize;
	VO_BOOL						m_bHeadDone;

	VO_COMPWRAP_CODECTYPE		m_nCodecType;
	VO_U32						m_nCoding;

	VO_U32						m_nVideoWidth;
	VO_U32						m_nVideoHeight;
	VO_IV_COLORTYPE				m_nColorType;

	OMX_U32						m_nSampleRate;
	OMX_U32						m_nChannels;
	OMX_U32						m_nSampleBits;

	CWrapBufferList*			m_pBufferList;
	VO_CODECBUFFER *			m_pInputBuffer;

	OMX_BOOL					m_bResetOutputPort;
	voCBaseThread *				m_pMsgThread;
	VO_BOOL						m_bThumbnailMode;

	CBaseSource*				m_pSource;
	VO_S32						m_nAudioTrack;
	VO_S32						m_nVideoTrack;

	voCSemaphore				m_semBufferFilled;

	VO_PTR						mpSurface;
public:
	virtual OMX_ERRORTYPE		EventHandler (OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	virtual OMX_ERRORTYPE		EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE		FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);

	static OMX_ERRORTYPE		voOMXEventHandler (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_IN OMX_PTR pAppData,
													OMX_IN OMX_EVENTTYPE eEvent,
													OMX_IN OMX_U32 nData1,
													OMX_IN OMX_U32 nData2,
													OMX_IN OMX_PTR pEventData);
	static OMX_ERRORTYPE		voOMXEmptyBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_IN OMX_PTR pAppData,
													OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE		voOMXFillBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_IN OMX_PTR pAppData,
													OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
};

class CBaseCompEvent : public voCBaseEventItem
{
public:
    CBaseCompEvent(CBaseComp * pComp, int (CBaseComp::* method)(int, int, int),
					    int nType, int nMsg = 0, int nV1 = 0, int nV2 = 0)
		: voCBaseEventItem (nType, nMsg, nV1, nV2)
	{
		m_pComp = pComp;
		m_fMethod = method;
    }

    virtual ~CBaseCompEvent()
	{
	}

    virtual void Fire (void) 
	{
        (m_pComp->*m_fMethod)(m_nMsg, m_nV1, m_nV2);
    }

protected:
    CBaseComp *			m_pComp;
    int (CBaseComp::*	m_fMethod) (int, int, int);
};

#endif //__CBaseComp_H__
