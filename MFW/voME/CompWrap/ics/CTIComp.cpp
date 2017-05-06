	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIComp.cpp

	Contains:	CTIComp class file

	Written by:	David1
	Time: 2011/11/29

*******************************************************************************/

#include <string.h>
#include <voOMX_Index.h>

#include "voOMXOSFun.h"
#include "CTIComp.h"
#include "CTIPort.h"
#include "CFileSource.h"
#include "fVideoHeadDataInfo.h"

#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CTIComp"
#endif
#include "voLog.h"

using namespace android;

struct TICompObserver : public BnOMXObserver {

    TICompObserver() : m_plistner(NULL) {}

    void setListner(CTIComp* target) {
        m_plistner = target;
    }

    // from IOMXObserver
    void onMessage(const omx_message &msg) {

        if (m_plistner) { 
		  voCOMXAutoLock lock(&m_plistner->m_lock);
          m_plistner->on_message(msg);
        }
    }

protected:
    virtual ~TICompObserver() {}

private:
    CTIComp*	m_plistner;
    TICompObserver(const TICompObserver &);
    TICompObserver &operator=(const TICompObserver &);
};


CTIComp::CTIComp(VO_CODEC_INIT_USERDATA * pUserData)
	: CBaseComp(pUserData)
	, m_pListener(NULL)
	, m_pnode(NULL)
	, m_pdrmengine(NULL)
	, m_nflags(0) 
	//, m_nflags(kUseSecureInputBuffers)
	, m_bInited(OMX_FALSE)
{
	if (pUserData) 
		m_pListener =(VOMP_LISTENERINFO*)pUserData->reserved2;
}

CTIComp::~CTIComp()
{
	if (m_pomx.get() && m_pnode) 
		m_pomx->freeNode(m_pnode);
}

OMX_ERRORTYPE CTIComp::SetCompName(OMX_STRING pCompName) {
	VOLOGI("component name: %s", pCompName);
	strcpy(m_szCompName, pCompName);
	m_nColorType = VO_COLOR_YUV_PLANAR420_NV12;   //
	return OMX_ErrorNone;
}

OMX_ERRORTYPE CTIComp::EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
{
	return m_pInputPort->EmptyBufferDone(pBuffer);
}

OMX_ERRORTYPE CTIComp::FillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
{
	m_semBufferFilled.Up();
	return m_pOutputPort->FillBufferDone(pBuffer);
}

OMX_ERRORTYPE CTIComp::ResetPort(int nIndex)
{
	VOLOGI("The port is %d, m_pOutputPort %08X", nIndex,(int)m_pOutputPort);

	OMX_PARAM_PORTDEFINITIONTYPE def;
	GetPortDefinition(&def, 1);

	//VOLOGI("buffer count: %d, min: %d, w x h = %d x %d", def.nBufferCountActual, def.nBufferCountMin, def.format.video.nFrameWidth, def.format.video.nFrameHeight);
	m_nVideoWidth =  def.format.video.nFrameWidth;
	m_nVideoHeight = def.format.video.nFrameHeight;

	if (nIndex != 1 || m_pOutputPort == NULL)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	VOLOGI("disable port: %d", nIndex);
	errType = SendCommand(OMX_CommandPortDisable, nIndex, NULL);
	if (errType != OMX_ErrorNone)
		VOLOGE("SendCommand OMX_CommandPortDisable failed 0X%08X.", errType);


	errType = m_pOutputPort->FreeBuffer();
	if (errType != OMX_ErrorNone)
		VOLOGE("FreeBuffer failed 0X%08X.", errType);

	if (WaitCommandDone() != VO_ERR_NONE)
	{
		VOLOGE("It handle command OMX_CommandPortDisable return error!");
		return OMX_ErrorInvalidState;
	}
	
	m_bSendMsg = OMX_TRUE;
	VOLOGI("enable port: %d", nIndex);
	errType = SendCommand(OMX_CommandPortEnable, nIndex, NULL);
	if (errType != OMX_ErrorNone)
		VOLOGE("SendCommand OMX_CommandPortEnable failed 0X%08X.", errType);

	((CTIPort*)m_pOutputPort)->ReconnectNativeWindow(&def);
	errType = m_pOutputPort->AllocBuffer(&def);
	if (errType != OMX_ErrorNone)
		VOLOGE("AllocBuffer failed 0X%08X.", errType);

	if (WaitCommandDone() != VO_ERR_NONE)
	{
		VOLOGE("It handle command OMX_CommandPortEnable return error!");
		return OMX_ErrorInvalidState;
	}


	ResetCrop(&def);
	errType = m_pOutputPort->StartBuffer();
	if (errType != OMX_ErrorNone)
		VOLOGE("StartBuffer failed 0X%08X.", errType);

	m_bResetOutputPort = OMX_TRUE;
	VOLOGI("Reset port done %08X", errType);
	return errType;
}

void CTIComp::ResetCrop(OMX_PTR ptr)
{
	m_pOutputPort->SetCrop((OMX_PARAM_PORTDEFINITIONTYPE*) ptr);
}

VO_U32 CTIComp::SetVideoRenderSurface(VO_PTR pSuf) {

	m_pnativewindow = (ANativeWindow*) pSuf;
	return 0;
}

VO_U32 CTIComp::SetVideoInputData(VO_CODECBUFFER * pInput)
{
	voCOMXAutoLock lock(&m_muxStatus);

	if (m_pInputPort == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	//currently, if the first sample get from source is sequence header, should skip it;
	if (pInput->Length<128)
		return VO_ERR_NONE;
	
	if (m_pBufferList == NULL)
		m_pBufferList = new CWrapBufferList();

	m_pBufferList->addBuffer(pInput);
	return VO_ERR_NONE;
}

VO_U32 CTIComp::GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock(&m_muxStatus);

	//if (m_pOutputPort == NULL)
		//return VO_ERR_NOT_IMPLEMENT;

	VO_U32	nRC = VO_ERR_NONE;
	if (!m_bHeadDone && m_pHeadData != NULL)
	{
		VO_CODECBUFFER headBuff;
		headBuff.Buffer = m_pHeadData;
		headBuff.Length = m_nHeadSize;
		headBuff.Time = 0;

		if (m_bInited == OMX_FALSE) {
			OMX_ERRORTYPE errType = OMX_ErrorInvalidState;
			if (Load2Idle() == OMX_ErrorNone) 
				errType = Load2Executing();

			if (errType == OMX_ErrorNone)
				m_bInited = OMX_TRUE;
		}

		nRC = m_pInputPort->SetVideoInputData(&headBuff, m_pHeadData, m_nHeadSize, VO_TRUE);
		m_bHeadDone = VO_TRUE;
	}

	VO_CODECBUFFER* pInputBuffer = NULL;
	pInputBuffer = m_pBufferList->getBuffer();

	if (m_pInputPort->GetEmptyNum() > 0 && pInputBuffer != NULL)
	{	
		nRC = m_pInputPort->SetVideoInputData(pInputBuffer, NULL, 0, VO_FALSE);
		if (nRC == VO_ERR_NONE)
		{
			pInputBuffer = NULL;
			m_pBufferList->removeBuffer();
		}		
	}

	m_pOutputPort->FillOutputBuffer();
	nRC = m_pOutputPort->GetVideoOutputData(pOutBuffer, pOutInfo);
	if (nRC == VO_ERR_RETRY)
	{
		if (pInputBuffer == NULL)
			return VO_ERR_INPUT_BUFFER_SMALL;

		pOutInfo->Flag = VO_VIDEO_OUTPUT_MORE;
	}

	if (pInputBuffer != NULL)
		pOutInfo->Flag = VO_VIDEO_OUTPUT_MORE;
	else
		pOutInfo->Flag = 0;
	
	return nRC;
}

OMX_ERRORTYPE CTIComp::SendCommand(OMX_COMMANDTYPE nCmd, OMX_U32 nParam, OMX_PTR pData)
{
	if (m_pomx.get() == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType =(OMX_ERRORTYPE)m_pomx->sendCommand(m_pnode, nCmd, nParam);
	return errType;
}

OMX_ERRORTYPE CTIComp::GetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pParam, size_t size)
{
	if (m_pomx.get() == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = (OMX_ERRORTYPE)m_pomx->getParameter(m_pnode, nIndex, pParam, size);
	return errType;
}

OMX_ERRORTYPE CTIComp::SetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pParam, size_t size)
{
	if (m_pomx.get() == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType =(OMX_ERRORTYPE)m_pomx->setParameter(m_pnode, nIndex, pParam, size);
	return errType;
}

OMX_ERRORTYPE CTIComp::GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pConfig, size_t size)
{
	if (m_pomx.get() == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType =(OMX_ERRORTYPE)m_pomx->getConfig(m_pnode, nIndex, pConfig, size);
	return errType;
}

OMX_ERRORTYPE CTIComp::SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pConfig, size_t size)
{
	if (m_pomx.get() == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType =(OMX_ERRORTYPE)m_pomx->setConfig(m_pnode, nIndex, pConfig, size);
	return errType;
}

OMX_ERRORTYPE CTIComp::GetPortDefinition(OMX_PTR pParam, OMX_U32 index) {

	if (pParam == NULL) return OMX_ErrorBadParameter;

	OMX_PARAM_PORTDEFINITIONTYPE *pdef = (OMX_PARAM_PORTDEFINITIONTYPE*)pParam;
	InitOMXParams(pdef);
	pdef->nPortIndex = index;
	OMX_ERRORTYPE errType = GetParameter(OMX_IndexParamPortDefinition, pdef, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	if (errType != OMX_ErrorNone) 
	  return OMX_ErrorUndefined;

	return errType;
}

OMX_ERRORTYPE	CTIComp::GetState(OMX_STATETYPE *pst) {

	if (m_pomx.get() == NULL)
		return OMX_ErrorNotReady;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (pst != NULL)
		errType = (OMX_ERRORTYPE)m_pomx->getState(m_pnode, pst);
	else
		errType = (OMX_ERRORTYPE)m_pomx->getState(m_pnode, &m_sState);

	return errType;
}

OMX_ERRORTYPE CTIComp::CreatePorts(void)
{
	voCOMXAutoLock lock(&m_muxStatus);

	if (m_pInputPort != NULL)
		return OMX_ErrorNone;

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_U32	nPorts = 0;
	OMX_PORT_PARAM_TYPE	pt;
	size_t size = sizeof(OMX_PORT_PARAM_TYPE); 
	InitOMXParams(&pt);
	errType = GetParameter(OMX_IndexParamAudioInit, &pt, size);
	if (errType == OMX_ErrorNone)
		nPorts += pt.nPorts;

	InitOMXParams(&pt);
	errType = GetParameter(OMX_IndexParamVideoInit, &pt, size);
	if (errType == OMX_ErrorNone)
		nPorts += pt.nPorts;

	//VOLOGI("total ports: %d", nPorts);
	OMX_PARAM_PORTDEFINITIONTYPE def;
	if (nPorts == 0) {
		for (VO_U32 ii = 0 ; ii < 16 ; ii++)
		{
			VOLOGI("gonna get portdef....");
			errType = GetPortDefinition(&def, ii);
			if (errType != OMX_ErrorNone)
			{
				break;
			}

			switch (def.eDomain)
			{
			case OMX_PortDomainAudio: 
				nPorts ++;
				break;
			case OMX_PortDomainVideo:
				nPorts ++;
				break;
			case OMX_PortDomainImage:
				nPorts ++;
				break;
			case OMX_PortDomainOther:
				nPorts ++;
				break;
			default:
				break;
			}
		}
	}


	for (OMX_U32 i = 0; i < nPorts; i++) {
		def.nPortIndex = i;
		errType = GetPortDefinition(&def, i);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE("It could not get param OMX_IndexParamPortDefinition. return error %08X!", errType);
			return errType;
		}

		if (def.eDir == OMX_DirInput && m_pInputPort == NULL) {
			m_pInputPort = new CTIPort(this, i, m_pomx, m_pnode, m_pnativewindow);
		} else if (def.eDir == OMX_DirOutput && m_pOutputPort == NULL) {
			m_pOutputPort = new CTIPort(this, i, m_pomx, m_pnode, m_pnativewindow);
		}
	}

	if (m_pInputPort)
		m_pInputPort->SetFlag(m_nflags);

	return OMX_ErrorNone;
}


OMX_ERRORTYPE CTIComp::Load2Idle(void)
{
	OMX_ERRORTYPE		errType = OMX_ErrorNone;

	GetState();
	errType = SetComponentRole();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("Set Component Role fails: 0X%8X", errType);
		return errType;
	}

	errType = CreatePorts();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("It could not create the ports! Return %08X", errType);
		return errType;
	}

	if (m_pInputPort != NULL && m_nCodecType == VO_COMPWRAP_CODEC_VideoDec)
	{
		errType = m_pInputPort->SetVideoInfo((VO_VIDEO_CODINGTYPE)m_nCoding, m_nColorType, m_nVideoWidth, m_nVideoHeight);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE("It could not set input video info! Return %08X", errType);
			return errType;
		}
	}

	if (m_pOutputPort != NULL && m_nCodecType == VO_COMPWRAP_CODEC_VideoDec)
	{
		errType = m_pOutputPort->SetVideoInfo((VO_VIDEO_CODINGTYPE)m_nCoding, m_nColorType, m_nVideoWidth, m_nVideoHeight);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE("It could not set output video info! Return %08X", errType);
			return errType;
		}
	}

	m_pInputPort->GetDefinition();
	m_pOutputPort->GetDefinition();
	m_bSendMsg = OMX_TRUE;
	errType = SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("It send command OMX_StateIdle return error %08X!", errType);
		return errType;
	}

	GetState();

	if (m_pInputPort != NULL) 
		m_pInputPort->AllocBuffer();

	if (m_pOutputPort != NULL)
		m_pOutputPort->AllocBuffer();

	if (WaitCommandDone() != VO_ERR_NONE)
	{
		VOLOGE("It handle command OMX_StateIdle return error!");
		return OMX_ErrorInvalidState;
	}

	GetState();
	if (m_sState != OMX_StateIdle)
	{
		VOLOGE("The current state %d was not OMX_StateIdle!", m_sState);
		return OMX_ErrorInvalidState;
	}

	return errType;
}

OMX_ERRORTYPE CTIComp::Load2Executing(void) {

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	if (m_pnode == NULL)
	{
		VOLOGE ("There was no component created!");
		return OMX_ErrorInvalidState;
	}

	OMX_STATETYPE	state;
	errType = GetState(&state);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not get the state. Result %08X", errType);
		return errType;
	}

	if (state != OMX_StateIdle && state != OMX_StatePause && state != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was wrong!", state);
		return OMX_ErrorInvalidState;
	}

	if (state == OMX_StateExecuting)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandStateSet, OMX_StateExecuting return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandStateSet, OMX_StateExecuting return error!");
		return OMX_ErrorInvalidState;
	}

	errType = GetState();

	if (m_sState != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was not OMX_StateExecuting!", state);
		return OMX_ErrorInvalidState;
	}

	if (m_pInputPort != NULL)
		m_pInputPort->StartBuffer ();
	if (m_pOutputPort != NULL)
		m_pOutputPort->StartBuffer ();

	return errType;
}


OMX_ERRORTYPE CTIComp::Start (void)
{
	voCOMXAutoLock lock (&m_muxStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pomx.get() == NULL)
		connect();

	errType = GetState();
	if (errType == OMX_ErrorNone && m_sState == OMX_StatePause)
		Load2Executing();

	VOLOGI("start over....");
	return errType;
}


OMX_ERRORTYPE CTIComp::Pause (void)
{
	voCOMXAutoLock lock (&m_muxStatus);
	VOLOGI ("MAAA Pause");
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pnode == NULL)
	{
		VOLOGE ("There was no component created!");
		return OMX_ErrorInvalidState;
	}

	OMX_STATETYPE	state;
	errType = GetState(&state);
	VOLOGI("state: %d", state);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not get the state. Result %08X", errType);
		return errType;
	}

	if (state != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was wrong!", state);
		return OMX_ErrorInvalidState;
	}

	if (state == OMX_StatePause)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StatePause, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandStateSet, OMX_StateExecuting return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandStateSet, OMX_StateExecuting return error!");
		return OMX_ErrorInvalidState;
	}

	errType = GetState();
	VOLOGI("state: %d", m_sState);
	if (m_sState != OMX_StatePause)
	{
		VOLOGE ("The current state %d was not OMX_StateExecuting!", state);
		return OMX_ErrorInvalidState;
	}

	return errType;
}


OMX_ERRORTYPE CTIComp::Stop (void)
{
	voCOMXAutoLock lock (&m_muxStatus);
	VOLOGI ("MAAA Stop");

	if (m_pnode == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	OMX_STATETYPE	state;
	errType = GetState(&state);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It could not get the state. Result %08X", errType);
		return errType;
	}

	if (state != OMX_StateIdle && state != OMX_StatePause && state != OMX_StateExecuting)
	{
		VOLOGE ("The current state %d was wrong!", state);
		return OMX_ErrorInvalidState;
	}

	if (state == OMX_StateIdle)
		return OMX_ErrorNone;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand (OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("It send command OMX_CommandStateSet, OMX_StateIdle return error %08X!", errType);
		return errType;
	}

	if (m_pInputPort != NULL)
		m_pInputPort->Stop ();
	if (m_pOutputPort != NULL)
		m_pOutputPort->Stop ();

	if (WaitCommandDone () != VO_ERR_NONE)
	{
		VOLOGE ("It handle command OMX_CommandStateSet, OMX_StateIdle return error!");
		return OMX_ErrorInvalidState;
	}

	errType = GetState();
	if (m_sState != OMX_StateIdle)
	{
		VOLOGE ("The current state %d was not OMX_StateIdle!", state);
		return OMX_ErrorInvalidState;
	}

	return errType;
}

OMX_ERRORTYPE CTIComp::Flush(void) {

	voCOMXAutoLock lock(&m_muxStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_pnode == NULL)
		return OMX_ErrorInvalidState;

	m_bSendMsg = OMX_TRUE;
	errType = SendCommand(OMX_CommandFlush, OMX_ALL, NULL);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("It send command OMX_CommandFlush return error %08X!", errType);
		return errType;
	}

	if (WaitCommandDone() != VO_ERR_NONE)
	{
		VOLOGE("It handle command OMX_CommandFlush return error!");
		return OMX_ErrorInvalidState;
	}

	VOLOGI(" [ Input ] Full %d, Free %d, [ Output ] Full %d, Free %d",(int)m_pInputPort->GetFullNum(),(int)m_pInputPort->GetEmptyNum(),(int)m_pOutputPort->GetFullNum(),(int)m_pOutputPort->GetEmptyNum());

	m_pOutputPort->ReturnAllBuffers();

	return errType;
}

OMX_ERRORTYPE CTIComp::connect() {
	
	m_pmps = IMediaDeathNotifier::getMediaPlayerService();
	if (m_pmps.get() == NULL) {
		VOLOGE("failed to get MediaPlayerService!");
		return OMX_ErrorNotImplemented;
	} else {
		VOLOGI("Succeed to get service: %p", m_pmps.get());
	}

	m_pomx = m_pmps->getOMX();

	if (m_pomx.get() == NULL) {
		VOLOGI("failed to get IOMX!");
		return OMX_ErrorNotImplemented;
	} else {
		VOLOGI("Succeed to get IOMX: %p", m_pomx.get());
	}

	if (m_nflags & kUseSecureInputBuffers)
		SetCompName((OMX_STRING)"OMX.TI.DUCATI1.VIDEO.DECODER.secure");
	else 
		SetCompName((OMX_STRING)"OMX.TI.DUCATI1.VIDEO.DECODER");

	sp<TICompObserver> observer = new TICompObserver;
	observer->setListner(this);

	OMX_ERRORTYPE errType = (OMX_ERRORTYPE)m_pomx->allocateNode(m_szCompName, observer, &m_pnode);
	if (m_pnode == NULL) {
		VOLOGE("It could not create the component!");
	} else {
		VOLOGI("Succeed to create component %s : %p", m_szCompName, m_pnode);
	}

	return errType;
}

OMX_ERRORTYPE CTIComp::SetComponentRole() {

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_nCodecType != VO_COMPWRAP_CODEC_VideoDec)
		return errType;

	char szVideoDecRole[128];

	//VOLOGI("codec: %d", m_nCoding);
	if (m_nCoding == VO_VIDEO_CodingH263)
		strcpy(szVideoDecRole, "video_decoder.h263");
	else if (m_nCoding == VO_VIDEO_CodingMPEG4)
		strcpy(szVideoDecRole, "video_decoder.mpeg4");
	else if (m_nCoding == VO_VIDEO_CodingH264)
		strcpy(szVideoDecRole, "video_decoder.avc");
	else
	{
		VOLOGE("m_nCoding=0X%x",(unsigned int)m_nCoding);
		strcpy(szVideoDecRole, "video_decoder.unknown");
	}

	OMX_STRING	role =(OMX_STRING)szVideoDecRole;

	if (role != NULL) {
		OMX_PARAM_COMPONENTROLETYPE roleParams;
		InitOMXParams(&roleParams);

		strncpy((char *)roleParams.cRole, role, OMX_MAX_STRINGNAME_SIZE - 1);
		roleParams.cRole[OMX_MAX_STRINGNAME_SIZE - 1] = '\0';

		VOLOGI("role: %s", roleParams.cRole);
		status_t err = SetParameter(OMX_IndexParamStandardComponentRole, &roleParams, sizeof(roleParams));
		if (err != OK) {
			LOGE("Failed to set standard component role '%s' with error: %d", role, err);
			errType = OMX_ErrorHardware;
		}
	}

	return errType;
}

VO_U32 CTIComp::GetSourceParam(VO_U32 paramID, VO_PTR pData)
{
	VO_U32 err= VO_ERR_NONE;
	if (paramID == VO_PID_VIDEO_ParamVideoAvc)
	{
		VO_CODECBUFFER headBuff;
		headBuff.Buffer = m_pHeadData;
		headBuff.Length = m_nHeadSize;
		headBuff.Time = 0;
		VO_VIDEO_PARAM_AVCTYPE avcInfo;
		memset(&avcInfo, 0, sizeof(avcInfo));
		if (getVIDEO_PARAM_AVCTYPE_H264(&headBuff, &avcInfo) != VO_ERR_NONE)
			return OMX_ErrorNotImplemented; 

		OMX_VIDEO_PARAM_AVCTYPE* pavc = (OMX_VIDEO_PARAM_AVCTYPE*) pData;
		pavc->nRefFrames = avcInfo.nRefFrames;
		switch (avcInfo.eProfile) {

			case VO_VIDEO_AVCProfileBaseline:
				pavc->eProfile = OMX_VIDEO_AVCProfileBaseline;
				break;
			case VO_VIDEO_AVCProfileMain:
				pavc->eProfile = OMX_VIDEO_AVCProfileMain;
				break;
			case VO_VIDEO_AVCProfileExtended:
				pavc->eProfile = OMX_VIDEO_AVCProfileExtended;
				break;
			case VO_VIDEO_AVCProfileHigh:
				pavc->eProfile = OMX_VIDEO_AVCProfileHigh;
				break;
			case VO_VIDEO_AVCProfileHigh10:
				pavc->eProfile = OMX_VIDEO_AVCProfileHigh10;
				break;
			case VO_VIDEO_AVCProfileHigh422:
				pavc->eProfile = OMX_VIDEO_AVCProfileHigh422;
				break;
			case VO_VIDEO_AVCProfileHigh444:
				pavc->eProfile = OMX_VIDEO_AVCProfileHigh444;
				break;

			default: break;
		}

		switch (avcInfo.eLevel) {
			case VO_VIDEO_AVCLevel1:
				pavc->eLevel = OMX_VIDEO_AVCLevel1; 
				break;
			case VO_VIDEO_AVCLevel1b:
				pavc->eLevel = OMX_VIDEO_AVCLevel1b; 
				break;
			case VO_VIDEO_AVCLevel11:
				pavc->eLevel = OMX_VIDEO_AVCLevel11; 
				break;
			case VO_VIDEO_AVCLevel12:
				pavc->eLevel = OMX_VIDEO_AVCLevel12; 
				break;
			case VO_VIDEO_AVCLevel13:
				pavc->eLevel = OMX_VIDEO_AVCLevel13; 
				break;
			case VO_VIDEO_AVCLevel2:
				pavc->eLevel = OMX_VIDEO_AVCLevel2; 
				break;
			case VO_VIDEO_AVCLevel21:
				pavc->eLevel = OMX_VIDEO_AVCLevel21; 
				break;
			case VO_VIDEO_AVCLevel22:
				pavc->eLevel = OMX_VIDEO_AVCLevel22; 
				break;
			case VO_VIDEO_AVCLevel3:
				pavc->eLevel = OMX_VIDEO_AVCLevel3; 
				break;
			case VO_VIDEO_AVCLevel31:
				pavc->eLevel = OMX_VIDEO_AVCLevel31; 
				break;
			case VO_VIDEO_AVCLevel32:
				pavc->eLevel = OMX_VIDEO_AVCLevel32; 
				break;
			case VO_VIDEO_AVCLevel4:
				pavc->eLevel = OMX_VIDEO_AVCLevel4; 
				break;
			case VO_VIDEO_AVCLevel41:
				pavc->eLevel = OMX_VIDEO_AVCLevel41; 
				break;
			case VO_VIDEO_AVCLevel42:
				pavc->eLevel = OMX_VIDEO_AVCLevel42; 
				break;
			case VO_VIDEO_AVCLevel5:
				pavc->eLevel = OMX_VIDEO_AVCLevel5; 
				break;
			case VO_VIDEO_AVCLevel51:
				pavc->eLevel = OMX_VIDEO_AVCLevel51; 
				break;

			default: 
				return OMX_ErrorNotImplemented;
		}
	}

	return err;
}

void CTIComp::on_message(const omx_message &msg) {

	switch (msg.type) {
		case omx_message::EVENT:
			{
				EventHandler(msg.u.event_data.event, msg.u.event_data.data1, msg.u.event_data.data2, NULL);

				break;
			}

		case omx_message::EMPTY_BUFFER_DONE:
			{
				OMX_BUFFERHEADERTYPE* buffer = (OMX_BUFFERHEADERTYPE*)msg.u.extended_buffer_data.buffer;
				EmptyBufferDone(buffer);
				break;
			}

		case omx_message::FILL_BUFFER_DONE:
			{
				OMX_BUFFERHEADERTYPE* buffer = (OMX_BUFFERHEADERTYPE*)msg.u.extended_buffer_data.buffer;
				FillBufferDone(buffer);

				break;
			}

		default:
			{
				//CHECK(!"should not be here.");
				break;
			}
	}
}

