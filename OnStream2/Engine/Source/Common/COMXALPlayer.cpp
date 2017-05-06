/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
*																		*
************************************************************************/
/*******************************************************************************
File:		COMXALPlayer.cpp

Contains:	VisualOn OpenMAX AL player class file

Written by:	Jim Lin

Change History (most recent first):
2012-03-24		Jim			Create file
*******************************************************************************/

#include <string.h>
#include <assert.h>
#include <memory.h>
#include "COMXALPlayer.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "voVideoParser.h"

#include <dlfcn.h>

#ifndef _IOS
#include <jni.h>
#include "CJniEnvUtil.h"
#include "CJavaParcelWrapOSMP.h"
#endif

#ifdef _IOS
#include "voLoadLibControl.h"
#endif

#define __OSMP "_OSMP"

COMXALPlayer::COMXALPlayer()
	: COSBasePlayer ()
	, m_pEngn(NULL)
	, m_pOSMPLListener(NULL)
{	
	m_pEngn = new COMXALEngine;

	memset(&m_NP_ReadBufFunc, 0, sizeof(VONP_READBUFFER_FUNC));
	m_NP_ReadBufFunc.pUserData	= this;
	m_NP_ReadBufFunc.ReadAudio	= OnNPReadAudio;
	m_NP_ReadBufFunc.ReadVideo	= OnNPReadVideo;

	memset(&m_NP_SendBufFunc, 0, sizeof(VONP_SENDBUFFER_FUNC));
	memset(&m_NPListener, 0, sizeof(VONP_LISTENERINFO));
	memset(&m_cbVOLOG, 0, sizeof(VO_LOG_PRINT_CB));

	m_sLibFunc.pUserData = this;
	m_sLibFunc.LoadLib = vomtLoadLib;
	m_sLibFunc.GetAddress = vomtGetAddress;
	m_sLibFunc.FreeLib = vomtFreeLib;

	m_pOSLibFunc = (VOOSMP_LIB_FUNC*)(&m_sLibFunc);

	m_NPListener.pListener	= OnNPListener;
	m_NPListener.pUserData	= this;

	m_nSEICount = 300;
}

COMXALPlayer::~COMXALPlayer()
{
	if(m_pEngn)
	{
		delete m_pEngn;
		m_pEngn = NULL;
	}
}


int COMXALPlayer::Init()
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	nRet = m_pEngn->Init(&m_sLibFunc);

	return ReturnCode_NP2OSMP(nRet);
}

int COMXALPlayer::Uninit()
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	nRet = m_pEngn->Uninit();

	COSBasePlayer::Uninit();

	return ReturnCode_NP2OSMP(nRet);	
}

int COMXALPlayer::SetDataSource(void* pSource, int nFlag)
{	
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;
	int	nmFlag = 0;

	nRet = COSBasePlayer::SetDataSource (pSource, nFlag);
	if(nRet) return nRet;

	m_nChanging = 1;

	if(nFlag & VOOSMP_FLAG_SOURCE_URL)
		nmFlag |= VONP_FLAG_SOURCE_URL;

	if(nFlag & VOOSMP_FLAG_SOURCE_READBUFFER)
		nmFlag |= VONP_FLAG_SOURCE_READBUFFER;

	if(nFlag & VOOSMP_FLAG_SOURCE_SENDBUFFER)
		nmFlag |= VONP_FLAG_SOURCE_SENDBUFFER;

	if(nmFlag & VONP_FLAG_SOURCE_READBUFFER)
	{
		m_pOSReadBufFunc = (VOOSMP_READBUFFER_FUNC*)pSource;
		pSource = (void*)&m_NP_ReadBufFunc;
	}

	nRet = m_pEngn->Open(pSource, nmFlag);

	m_nChanging = 0;

	return ReturnCode_NP2OSMP(nRet);	
}

int COMXALPlayer::OnNPReadAudio(void * pUserData, VONP_BUFFERTYPE * pBuffer)
{
	COMXALPlayer* pPlayer = (COMXALPlayer*)pUserData;

	if(!pPlayer)
		return VOOSMP_ERR_Pointer;

	int nRet = pPlayer->doNPReadAudio(pBuffer);

	return nRet;
}

int COMXALPlayer::OnNPReadVideo(void * pUserData, VONP_BUFFERTYPE * pBuffer)
{
	COMXALPlayer* pPlayer = (COMXALPlayer*)pUserData;

	if(!pPlayer)
		return VOOSMP_ERR_Pointer;

	int nRet = pPlayer->doNPReadVideo(pBuffer);

	return nRet;
}

int COMXALPlayer::doNPReadAudio(VONP_BUFFERTYPE* pBuffer)
{
	int nRet = VOOSMP_ERR_None;

	VOOSMP_BUFFERTYPE buf;
	memset(&buf, 0, sizeof(VOOSMP_BUFFERTYPE));
	nRet = COSBasePlayer::doReadAudio(&buf);

	Buffer_OSMP2NP(&buf, pBuffer, VOOSMP_SS_AUDIO);

	return ReturnCode_OSMP2NP(nRet);
}

int COMXALPlayer::doNPReadVideo(VONP_BUFFERTYPE* pBuffer)
{
	int nRet = VOOSMP_ERR_None;	

	VOOSMP_BUFFERTYPE buf;
	memset(&buf, 0, sizeof(VOOSMP_BUFFERTYPE));
	nRet = COSBasePlayer::doReadVideo(&buf);

	Buffer_OSMP2NP(&buf, pBuffer, VOOSMP_SS_VIDEO);

	return ReturnCode_OSMP2NP(nRet);	
}

int COMXALPlayer::OnOSMPSendData(void * pUserData, int nSSType, VOOSMP_BUFFERTYPE* pBuffer)
{
	COMXALPlayer* pPlayer = (COMXALPlayer*)pUserData;

	if(!pPlayer)
		return VOOSMP_ERR_Pointer;

	return pPlayer->doOSMPSendData(nSSType, pBuffer);
}

int COMXALPlayer::doOSMPSendData(int nSSType, VOOSMP_BUFFERTYPE* pBuffer)
{
	int nRet = VONP_ERR_Retry;

	if(m_NP_SendBufFunc.pUserData && m_NP_SendBufFunc.SendData)
	{
		int nType = StreamType_OSMP2NP(nSSType);

		VONP_BUFFERTYPE buf;
		memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
		Buffer_OSMP2NP(pBuffer, &buf, nSSType);

		nRet = m_NP_SendBufFunc.SendData(m_NP_SendBufFunc.pUserData, nType, &buf);
	}

	return ReturnCode_NP2OSMP(nRet);
}

int COMXALPlayer::GetAudioData (unsigned char * pData, unsigned int nSize, int * pFilled)
{
	*pFilled = 0;

	VONP_BUFFERTYPE buf;
	VONP_BUFFERTYPE *pBuf = NULL;
	memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
	buf.pBuffer = pData;
	buf.nSize = nSize;

	pBuf = &buf;

	int nRC =  m_pEngn->GetParam(VONP_PID_AUDIO_BUFFER, &pBuf);
	if (nRC == 0)
		*pFilled = pBuf->nSize;

	if(nRC != VOOSMP_ERR_None)
		voOS_Sleep (2);

	return ReturnCode_NP2OSMP(nRC);
}


int COMXALPlayer::Run(void)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	nRet = COSBasePlayer::Run ();
	if(nRet) return nRet;	

	m_nChanging = 1;
	nRet = m_pEngn->SetParam(VONP_PID_LISTENER, &m_NPListener);

	nRet = m_pEngn->Run();

	m_nChanging = 0;

	return ReturnCode_NP2OSMP(nRet);		
}

int	COMXALPlayer::Pause(void)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	nRet = COSBasePlayer::Pause ();
	if(nRet) return nRet;

	m_nChanging = 1;

	nRet = m_pEngn->Pause();

	m_nChanging = 0;

	return ReturnCode_NP2OSMP(nRet);	
}

int COMXALPlayer::Stop(void)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	m_nChanging = 1;
	nRet = m_pEngn->Stop();
	m_nChanging = 0;

	nRet = COSBasePlayer::Stop ();
	if(nRet) return nRet;	

	return ReturnCode_NP2OSMP(nRet);		
}

int COMXALPlayer::Close(void)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;
    
	int nRet = VONP_ERR_None;
    
	m_nChanging = 1;
	nRet = m_pEngn->Close();
	m_nChanging = 0;
    
	return ReturnCode_NP2OSMP(nRet);
}

int COMXALPlayer::Flush(void)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	//nRet = m_pEngn->Flush();

	return ReturnCode_NP2OSMP(nRet);		
}

int COMXALPlayer::GetStatus(int* pStatus)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_None;

	int nVal;
	nRet = m_pEngn->GetParam(VONP_PID_STATUS, &nVal);

	if(nVal == VONP_STATUS_INIT)
		*pStatus = VOOSMP_STATUS_INIT;
	else if(nVal == VONP_STATUS_LOADING)
		*pStatus = VOOSMP_STATUS_LOADING;
	else if(nVal == VONP_STATUS_RUNNING)
		*pStatus = VOOSMP_STATUS_RUNNING;
	else if(nVal == VONP_STATUS_PAUSED)
		*pStatus = VOOSMP_STATUS_PAUSED;
	else if(nVal == VONP_STATUS_STOPPED)
		*pStatus = VOOSMP_STATUS_STOPPED;
	else if(nVal == VONP_STATUS_MAX)
		*pStatus = VOOSMP_STATUS_MAX;

	return ReturnCode_NP2OSMP(nRet);	
}

int COMXALPlayer::GetDuration(int* pDuration)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	return VOOSMP_ERR_Implement;
}

int COMXALPlayer::GetPos(int* pCurPos)
{	
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = m_pEngn->GetPos();
	*pCurPos = nRet;
	return nRet==-1?VONP_ERR_Unknown:VONP_ERR_None;
}

int COMXALPlayer::SetPos(int nCurPos)
{
	COSBasePlayer::SetPos (nCurPos);

	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	return m_pEngn->SetPos(nCurPos);
}

int	COMXALPlayer::GetParam(int nID, void* pValue)
{	
	if(nID == VOOSMP_PID_PLAYER_TYPE)
	{
		*((int*)pValue) = VOOSMP_OMXAL_PLAYER;
		return VOOSMP_ERR_None;
	}

	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_ParamID;

	switch(nID)
	{
	case VOOSMP_PID_AUDIO_VOLUME:
		{
			nRet = m_pEngn->GetParam(VONP_PID_AUDIO_VOLUME, pValue); 
		}
		break;

	case VOOSMP_PID_FUNC_LIB:
		{
			nID = VONP_PID_FUNC_LIB;
		}
		break;

	case VOOSMP_PID_STATUS:
		return GetStatus((int*)pValue);

	case VOOSMP_PID_LISTENER:
		{
			nID = VONP_PID_LISTENER;
		}
		break;

	case VOOSMP_PID_SENDBUFFERFUNC:
		{
			memset(&m_NP_SendBufFunc, 0, sizeof(VONP_SENDBUFFER_FUNC));
			nRet = m_pEngn->GetParam(VONP_PID_SENDBUFFERFUNC, (void*)&m_NP_SendBufFunc); 

			VOOSMP_SENDBUFFER_FUNC* pFunc = (VOOSMP_SENDBUFFER_FUNC*)pValue;
			pFunc->pUserData	= this;
			pFunc->SendData		= OnOSMPSendData;
		}
		break;
	case VOOSMP_PID_CPU_INFO:
		{
			VONP_CPU_INFO sCPUInfo;
			memset(&sCPUInfo, 0, sizeof(VONP_CPU_INFO));
			nRet = m_pEngn->GetParam(VONP_PID_CPU_INFO, (void*)&sCPUInfo); 
			
			VOOSMP_CPU_INFO *pCPUInfo = (VOOSMP_CPU_INFO *)pValue;
			pCPUInfo->nCoreCount   = sCPUInfo.nCoreCount;
			pCPUInfo->nCPUType	   = sCPUInfo.nCPUType;
			pCPUInfo->nFrequency   = sCPUInfo.nFrequency;
			pCPUInfo->llReserved   = sCPUInfo.llReserved;
		}
		break;
	case VOOSMP_PID_AUDIO_FORMAT:
		{
			VONP_AUDIO_FORMAT mAudioFormat;
			memset(&mAudioFormat, 0, sizeof(VONP_AUDIO_FORMAT));
			nRet = m_pEngn->GetParam(VONP_PID_AUDIO_FORMAT, (void*)&mAudioFormat); 

			VOOSMP_AUDIO_FORMAT *pAudioFormat = (VOOSMP_AUDIO_FORMAT *)pValue;

			pAudioFormat->SampleRate = mAudioFormat.nSampleRate;
			pAudioFormat->Channels = mAudioFormat.nChannels;
			pAudioFormat->SampleBits = mAudioFormat.nSampleBits;
		}
        break;
	default:
        {
            return COSBasePlayer::GetParam(nID, pValue);
        }
		//VOLOGI("Param ID not support");
		break;
	}

	return ReturnCode_NP2OSMP(nRet);	
}

int	COMXALPlayer::SetParam(int nID, void* pValue)
{
	if(COSBasePlayer::SetParam(nID, pValue) == VOOSMP_ERR_None)
		return VOOSMP_ERR_None;
	
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	int nRet = VONP_ERR_ParamID;

	switch(nID)
	{
	case VOOSMP_PID_AUDIO_VOLUME:
		{
			nRet = m_pEngn->SetParam(VONP_PID_AUDIO_VOLUME, pValue); 
		}
		break;

	case VOOSMP_PID_PLAYER_PATH:
		{
			VOLOGI("+Set VOOSMP_PID_PLAYER_PATH");
			if(pValue)
			{
				strcpy(m_szPathLib, (char *)pValue);
				nRet = m_pEngn->SetParam(VONP_PID_FUNC_LIB, &m_sLibFunc);
				nRet = m_pEngn->SetParam(VONP_PID_PLAYER_PATH, m_szPathLib);
			}
			VOLOGI("+Set VOOSMP_PID_PLAYER_PATH, Load addr %x", (int)m_sLibFunc.LoadLib);
		}
		break;
	case VOOSMP_PID_FUNC_LIB:
		{
			VOLOGI("+Set VOOSMP_PID_FUNC_LIB");
			if(pValue)
			{
				memset(&m_sLibFunc, 0, sizeof(VONP_LIB_FUNC));
				LibOP_OSMP2NP((VOOSMP_LIB_FUNC*)pValue, &m_sLibFunc);
				nRet = m_pEngn->SetParam(VONP_PID_FUNC_LIB, &m_sLibFunc);
				VOLOGI("+Set VOOSMP_PID_FUNC_LIB, Load addr %x", (int)m_sLibFunc.LoadLib);
			}
		}
		break;
	case VOOSMP_PID_MFW_CFG_FILE:
		nRet = m_pEngn->SetParam(VONP_PID_MFW_CFG_FILE, pValue);				                
		break;
	case VOOSMP_PID_STATUS:
		return GetStatus((int*)pValue);

	case VOOSMP_PID_AUDIO_STREAM_ONOFF:
		nRet = m_pEngn->SetParam(VONP_PID_AUDIO_STREAM_ONOFF, pValue);
		break;
	case VOOSMP_PID_VIDEO_STREAM_ONOFF:
		nRet = m_pEngn->SetParam(VONP_PID_VIDEO_STREAM_ONOFF, pValue);
		break;
	case VOOSMP_PID_LISTENER:
		{
			m_pOSMPLListener = (VOOSMP_LISTENERINFO*)pValue;
			nRet = m_pEngn->SetParam(VONP_PID_LISTENER, &m_NPListener);
		}
		break;

	case VOOSMP_PID_SENDBUFFERFUNC:
		{
			if(pValue)
			{
				memset(&m_NP_SendBufFunc, 0, sizeof(VONP_SENDBUFFER_FUNC));
				nRet = m_pEngn->GetParam(VONP_PID_SENDBUFFERFUNC, (void*)&m_NP_SendBufFunc); 

				VOOSMP_SENDBUFFER_FUNC* pFunc = (VOOSMP_SENDBUFFER_FUNC*)pValue;
				pFunc->pUserData	= this;
				pFunc->SendData		= OnOSMPSendData;
			}
		}
		break;

	case VOOSMP_PID_AUDIO_REND_BUFFER_TIME:
		{

		}
		break;
	case VOOSMP_PID_COMMON_LOGFUNC:
		{
			if(pValue)
				memcpy(&m_cbVOLOG, pValue, sizeof(VO_LOG_PRINT_CB));
			nRet = m_pEngn->SetParam(VONP_PID_COMMON_LOGFUNC, &m_cbVOLOG); 
		}
		break;
	case VOOSMP_PID_COMMAND_STRING:
		{
			nRet = m_pEngn->SetParam(VONP_PID_COMMAND_STRING, pValue); 
		}
		break;
	case VOOSMP_PID_VIEW_ACTIVE:
		{
			nRet = m_pEngn->SetParam(VONP_PID_VIEW_ACTIVE, pValue); 
		}
		break;
	case VOOSMP_PID_APPLICATION_SUSPEND:
        {
            nRet = m_pEngn->SetParam(VONP_PID_APPLICATION_SUSPEND, pValue);
        }
        break;
    case VOOSMP_PID_APPLICATION_RESUME:
        {
            nRet = m_pEngn->SetParam(VONP_PID_APPLICATION_RESUME, pValue);
        }
        break;
	default:
		//VOLOGI("Param ID not support");
		break;
	}

	return ReturnCode_NP2OSMP(nRet);		
}

int COMXALPlayer::SetView(void* pView)
{
	if(!m_pEngn)
		return VOOSMP_ERR_Pointer;

	return m_pEngn->SetView(pView);
}

int COMXALPlayer::OnNPListener(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	COMXALPlayer* pPlayer = (COMXALPlayer*)pUserData;

	if(!pPlayer)
		return VOOSMP_ERR_Pointer;

	voCAutoLock lock( &pPlayer->m_EventLock );

	int nEvent = pPlayer->ChangEventIDEvent(nID, pParam1, pParam2);

	return pPlayer->HandleEvent(nEvent, pParam1, pParam2);
}

int COMXALPlayer::ChangEventIDEvent (unsigned int nID, void * pParam1, void * pParam2)
{
	int nEvent = nID;

	switch (nID)
	{
	case VONP_CB_Error:
		{
			nEvent = VOOSMP_CB_Error;
		}
		break;
	case VONP_CB_PlayComplete:
		{
			nEvent = VOOSMP_CB_PlayComplete;
		}
		break;
	case VONP_CB_StartBuff:
		{
			nEvent = VOOSMP_CB_VideoStartBuff;
		}
		break;
	case VONP_CB_StopBuff:
		{
			m_bSeeking = VO_FALSE;
			nEvent = VOOSMP_CB_VideoStopBuff;
		}
		break;
	case VONP_CB_SeekComplete:
		{
			m_bSeeking = VO_FALSE;
			nEvent = VOOSMP_CB_SeekComplete;
		}
		break;
	case VONP_CB_VideoSizeChanged:
		{
			nEvent = VOOSMP_CB_VideoSizeChanged;
		}
		break;
	case VONP_CB_Codec_NotSupport:
		{
			nEvent = VOOSMP_CB_CodecNotSupport;
		}
		break;
	case VONP_CB_VideoAspectRatio:
		{
			nEvent = VOOSMP_CB_VideoAspectRatio;
		}
		break;
    case VONP_CB_VideoRenderStart:
        {
            nEvent = VOOSMP_CB_VideoRenderStart;
        }
        break;
    case VONP_CB_NativePlayerError:
        {
            nEvent = VOOSMP_CB_NativePlayerFailed;
        }
            break;
	default:
		break;
	}

	return nEvent;
}

int COMXALPlayer::HandleEvent(int nID, void * pParam1, void * pParam2)
{
	int nRet = VONP_ERR_None;

	COSBasePlayer::HandleEvent(nID, pParam1, pParam2);

	VOLOGI("OMX AL event %x", nID);
    
#ifndef _IOS
	CJniEnvUtil	env(m_JavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jclass clazz = env.getEnv()->GetObjectClass(m_jOSmpEng);
	if (clazz == NULL) 
	{
		return VOOSMP_ERR_Unknown;
	}

	jmethodID eventCallBack = env.getEnv()->GetMethodID(clazz, "osmpCallBack", "(IIILjava/lang/Object;)V");
	if (eventCallBack == NULL)
	{
		return VOOSMP_ERR_Unknown;
	}

	int nParam1	= pParam1?*((int*)pParam1):0;
	int nParam2	= pParam2?*((int*)pParam2):0;

	jobject jobj = NULL;
	if(VOOSMP_CB_SEI_INFO == nID)
	{
		if(pParam1!= NULL)
		{
			VOOSMP_SEI_INFO* pInfo = (VOOSMP_SEI_INFO*)pParam1;
			if(pInfo->nFlag == VOOSMP_FLAG_SEI_PIC_TIMING)
			{
				VOOSMP_SEI_PIC_TIMING* pPicTimingInfo = (VOOSMP_SEI_PIC_TIMING*)pInfo->pInfo;
				int size = sizeof(VOOSMP_SEI_PIC_TIMING)/sizeof(int);
				jintArray arr = env.getEnv()->NewIntArray(size);
				env.getEnv()->SetIntArrayRegion(arr, 0, size, (const jint*)pPicTimingInfo);

				jobj = arr;
				nParam1 = VOOSMP_FLAG_SEI_PIC_TIMING;
				if(pInfo!=NULL && jobj==NULL)
					VOLOGE("VOOSMP_FLAG_SEI_EVENT_PIC_TIMING pInfo!=NULL && jobj==NULL");
				VOLOGI("VOOSMP_FLAG_SEI_EVENT_PIC_TIMING pInfo=%d && jobj=%d",(int)pInfo, (int)jobj);
			}
			if(pInfo->nFlag == VO_SEI_USER_DATA_UNREGISTERED)
			{
				CJavaParcelWrap	javaParcel(env.getEnv());
				VO_H264_USERDATA_Params* pInfo2 = (VO_H264_USERDATA_Params*)pInfo->pInfo;
				javaParcel.writeInt32(pInfo2->count);
				if(pInfo2->count>255)
					VOLOGE("VO_SEI_USER_DATA_UNREGISTERED pInfo->count>255");
				int all=0;
				for(int i=0;i<(int)(pInfo2->count)  ;i++)
				{
					all+=pInfo2->size[i];
					javaParcel.writeInt32(pInfo2->size[i]);
				}
				javaParcel.write(pInfo2->buffer, (VO_U32) all);

				jobj = javaParcel.getParcel();
				nParam1 = VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED;
				if(pInfo!=NULL && jobj==NULL)
					VOLOGE("VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED pInfo!=NULL && jobj==NULL");
				VOLOGI("VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED pInfo=%d && jobj=%d all=%d",(int)pInfo, (int)jobj,all);
			}
		}
	}
	else if(VOOSMP_CB_Metadata_Arrive == nID)
	{
		if(VOOSMP_SS_IMAGE == nParam1)
		{
			CJavaParcelWrap	javaParcel(env.getEnv());
			//	VOLOGI ("CloseCaption GetSubtileSample 555!");
			CJavaParcelWrap::getParcelFromSubtitleInfo((voSubtitleInfo*)nParam2, &javaParcel);
			//	VOLOGI ("CloseCaption GetSubtileSample 777!");
			jobj = javaParcel.getParcel();
		}
	}


	env.getEnv()->CallVoidMethod(m_jOSmpEng, eventCallBack, nID, nParam1, nParam2, jobj);
#endif
    
	return ReturnCode_OSMP2NP(nRet);
}


int COMXALPlayer::ReturnCode_NP2OSMP(unsigned int nRC)
{
	int nRet = nRC; 

	switch(nRC)
	{
	case VONP_ERR_None:
		nRet = VOOSMP_ERR_None;
		break;
	case VONP_ERR_EOS:
		nRet = VOOSMP_ERR_EOS;
		break;
	case VONP_ERR_Retry:
		nRet = VOOSMP_ERR_Retry;
		break;
	case VONP_ERR_VideoCodec:
		nRet = VOOSMP_ERR_Video;
		break;
	case VONP_ERR_AudioCodec:
		nRet = VOOSMP_ERR_Audio;
		break;
	case VONP_ERR_OutMemory:
		nRet = VOOSMP_ERR_OutMemory;
		break;
	case VONP_ERR_Pointer:
		nRet = VOOSMP_ERR_Pointer;
		break;
	case VONP_ERR_ParamID:
		nRet = VOOSMP_ERR_ParamID;
		break;
	case VONP_ERR_Status:
		nRet = VOOSMP_ERR_Status;
		break;
	case VONP_ERR_Implement:
		nRet = VOOSMP_ERR_Implement;
		break;
	case VONP_ERR_Format_Changed:
		nRet = VOOSMP_ERR_FormatChange;
		break;
	case VONP_ERR_SmallSize:
		nRet =VOOSMP_ERR_SmallSize;
		break;
	case VONP_ERR_WaitTime:
		nRet = VOOSMP_ERR_WaitTime;
		break;
	case VONP_ERR_Unknown:
		nRet = VOOSMP_ERR_Unknown;
		break;
	case VONP_ERR_Audio_No_Now:
		nRet = VOOSMP_ERR_Audio_No_Now;
		break;
	case VONP_ERR_Video_No_Now:
		nRet = VOOSMP_ERR_Video_No_Now;
		break;
	case VONP_ERR_FLush_Buffer:
		nRet = VOOSMP_ERR_FLush_Buffer;
		break;
	default:
		{
			//VOLOGI("Loss return node");
			nRet = VOOSMP_ERR_Unknown;
		}
		break;
	}

	return nRet;
}

int COMXALPlayer::ReturnCode_OSMP2NP(unsigned int nRC)
{
	int nRet = nRC; 

	switch(nRC)
	{
	case VOOSMP_ERR_None:
		nRet = VONP_ERR_None;
		break;
	case VOOSMP_ERR_EOS:
		nRet = VONP_ERR_EOS;
		break;
	case VOOSMP_ERR_Retry:
		nRet = VONP_ERR_Retry;
		break;
	case VOOSMP_ERR_Video:
		nRet = VONP_ERR_VideoCodec;
		break;
	case VOOSMP_ERR_Audio:
		nRet = VONP_ERR_AudioCodec;
		break;
	case VOOSMP_ERR_OutMemory:
		nRet = VONP_ERR_OutMemory;
		break;
	case VOOSMP_ERR_Pointer:
		nRet = VONP_ERR_Pointer;
		break;
	case VOOSMP_ERR_ParamID:
		nRet = VONP_ERR_ParamID;
		break;
	case VOOSMP_ERR_Status:
		nRet = VONP_ERR_Status;
		break;
	case VOOSMP_ERR_Implement:
		nRet = VONP_ERR_Implement;
		break;
	case VOOSMP_ERR_SmallSize:
		nRet = VONP_ERR_WaitTime;
		break;
	case VOOSMP_ERR_WaitTime:
		nRet = VONP_ERR_WaitTime;
		break;
	case VOOSMP_ERR_Unknown:
		nRet = VONP_ERR_Unknown;
		break;
	case VOOSMP_ERR_Audio_No_Now:
		nRet = VONP_ERR_Audio_No_Now;
		break;
	case VOOSMP_ERR_Video_No_Now:
		nRet = VONP_ERR_Video_No_Now;
		break;
	case VOOSMP_ERR_FLush_Buffer:
		nRet = VONP_ERR_FLush_Buffer;
		break;
	default:
		{
			//VOLOGI("Loss return node");
			nRet = VONP_ERR_Unknown;
		}
		break;
	}

	return nRet;
}


int COMXALPlayer::BufferFlag_OSMP2NP(int nFlag)
{
	int mnFlag = 0;

	if(VOOSMP_FLAG_BUFFER_KEYFRAME & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_KEYFRAME;

	if(VOOSMP_FLAG_BUFFER_NEW_PROGRAM & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_NEW_PROGRAM;

	if(VOOSMP_FLAG_BUFFER_NEW_FORMAT & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_NEW_FORMAT;

	if(VOOSMP_FLAG_BUFFER_HEADDATA & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_HEADDATA;

	if(VOOSMP_FLAG_BUFFER_DROP_FRAME & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_DROP_FRAME;

	if(VOOSMP_FLAG_BUFFER_DELAY_TO_DROP & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_DELAY_TO_DROP;

	if(VOOSMP_FLAG_BUFFER_TIMESTAMP_RESET & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_TIMESTAMP_RESET;

	if(VOOSMP_FLAG_BUFFER_FRAME_DECODE_ONLY & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_DECODER_ONLY;

	return mnFlag;
}


int COMXALPlayer::StreamType_OSMP2NP(int nType)
{
	if(VOOSMP_SS_AUDIO == nType)
		return VONP_SS_Audio;
	else if(VOOSMP_SS_VIDEO == nType)
		return VONP_SS_Video;
	else if(VOOSMP_SS_TS == nType)
		return VONP_SS_TS;
	else if(VOOSMP_SS_MFS == nType)
		return VONP_SS_MFS;
	else if(VOOSMP_SS_MAX == nType)
		return VONP_SS_MAX;

	assert(0);
	return VONP_SS_MAX;
}


int COMXALPlayer::Buffer_OSMP2NP(VOOSMP_BUFFERTYPE* pOSMP, VONP_BUFFERTYPE* pNP, int nSSType)
{
	pNP->nSize		= pOSMP->nSize;
	pNP->pBuffer		= pOSMP->pBuffer;
	pNP->llTime		= pOSMP->llTime;
	pNP->nFlag		= BufferFlag_OSMP2NP(pOSMP->nFlag);
	pNP->pData		= pOSMP->pData;
	pNP->llReserve	= pOSMP->llReserve;

	VOOSMP_BUFFERTYPE*	sBuf = pOSMP;
	VONP_BUFFERTYPE*	dBuf = pNP;

	//dBuf->nFlag = 0;

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_KEYFRAME) == VOOSMP_FLAG_BUFFER_KEYFRAME)
	{
		//dBuf->nFlag |= VOMP_FLAG_BUFFER_KEYFRAME;
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) == VOOSMP_FLAG_BUFFER_NEW_PROGRAM)
	{
		//dBuf->nFlag |= VOMP_FLAG_BUFFER_NEW_PROGRAM;

		if(sBuf->pData)
		{
			VOOSMP_BUFFER_FORMAT *pOSTackInfo = (VOOSMP_BUFFER_FORMAT *)sBuf->pData;
			if(nSSType == VOOSMP_SS_AUDIO)
			{
				m_sAudioTrackInfo.nStreamType = VONP_SS_Audio;
				//here has issue
				m_sAudioTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sAudioTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sAudioTrackInfo.sFormat.audio.nSampleRate = pOSTackInfo->sFormat.audio.SampleRate;
				m_sAudioTrackInfo.sFormat.audio.nChannels = pOSTackInfo->sFormat.audio.Channels;  
				m_sAudioTrackInfo.sFormat.audio.nSampleBits =  pOSTackInfo->sFormat.audio.SampleBits;

				m_sAudioTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sAudioTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sAudioTrackInfo;
			}
			else if(nSSType == VOOSMP_SS_VIDEO)
			{
				m_sVideoTrackInfo.nStreamType = VONP_SS_Video;
				//here has issue
				m_sVideoTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sVideoTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sVideoTrackInfo.sFormat.video.nWidth = pOSTackInfo->sFormat.video.Width;
				m_sVideoTrackInfo.sFormat.video.nHeight = pOSTackInfo->sFormat.video.Height;  
				m_sVideoTrackInfo.sFormat.video.nType =  pOSTackInfo->sFormat.video.Type;

				m_sVideoTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sVideoTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sVideoTrackInfo;
			}			
		}
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) == VOOSMP_FLAG_BUFFER_NEW_FORMAT)
	{
		if(sBuf->pData)
		{
			VOOSMP_BUFFER_FORMAT *pOSTackInfo = (VOOSMP_BUFFER_FORMAT *)sBuf->pData;
			if(nSSType == VOOSMP_SS_AUDIO)
			{
				m_sAudioTrackInfo.nStreamType = VONP_SS_Audio;
				// has issue
				m_sAudioTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sAudioTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sAudioTrackInfo.sFormat.audio.nSampleRate = pOSTackInfo->sFormat.audio.SampleRate;
				m_sAudioTrackInfo.sFormat.audio.nChannels = pOSTackInfo->sFormat.audio.Channels;  
				m_sAudioTrackInfo.sFormat.audio.nSampleBits =  pOSTackInfo->sFormat.audio.SampleBits;

				m_sAudioTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sAudioTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sAudioTrackInfo;
			}
			else if(nSSType == VOOSMP_SS_VIDEO)
			{
				m_sVideoTrackInfo.nStreamType = VONP_SS_Video;
				m_sVideoTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sVideoTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sVideoTrackInfo.sFormat.video.nWidth = pOSTackInfo->sFormat.video.Width;
				m_sVideoTrackInfo.sFormat.video.nHeight = pOSTackInfo->sFormat.video.Height;  
				m_sVideoTrackInfo.sFormat.video.nType =  pOSTackInfo->sFormat.video.Type;

				m_sVideoTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sVideoTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sVideoTrackInfo;
			}			
		}
	}

	return VONP_ERR_None;
}

int COMXALPlayer::LibOP_OSMP2NP(VOOSMP_LIB_FUNC* pOSMP, VONP_LIB_FUNC* pNP)
{
	pNP->pUserData	= pOSMP->pUserData;
	pNP->LoadLib		= pOSMP->LoadLib;
	pNP->GetAddress	= pOSMP->GetAddress;
	pNP->FreeLib		= pOSMP->FreeLib;

	return VONP_ERR_None;
}

int COMXALPlayer::Listener_OSMP2NP(VOOSMP_LISTENERINFO* pOSMP, VONP_LISTENERINFO* pNP)
{
	pNP->pUserData	= pOSMP->pUserData;
	pNP->pListener	= pOSMP->pListener;

	return VONP_ERR_None;
}

#ifndef _IOS
int COMXALPlayer::SetJavaVM(void *pJavaVM, void* obj)
{
	m_JavaVM	= (JavaVM *)pJavaVM;
	m_jOSmpEng	= (jobject)obj;

	if(m_pEngn)
		m_pEngn->SetParam(VONP_PID_JAVAENV, pJavaVM); 

	return VOOSMP_ERR_None;	
}
#endif

void * COMXALPlayer::vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
#ifdef _IOS
    voGetModuleRealAPI pFunction = (voGetModuleRealAPI)voGetModuleAdapterFunc(pFuncName);
    
    if (pFunction) {
        return pFunction();
    }
    
    return NULL;
#else
	void * ptr = dlsym (hLib, pFuncName);

	VOLOGI( "Func Name: %s  %p" , pFuncName , ptr );
	return ptr;
#endif
}

int COMXALPlayer::vomtFreeLib (void * pUserData, void * hLib, int nFlag)
{
	VOLOGI ("Free Lib: %0xd", (unsigned int) hLib);

#ifndef _IOS
	dlclose (hLib);
#endif
    
	return 0;
}

void * COMXALPlayer::vomtLoadLib (void * pUserData, char * pLibName, int nFlag)
{
#ifdef _IOS
    VOLOGI("Load lib name: %s", pLibName);
    // Return a no-zero value to continue load on iOS
    return (void *)1;
#endif
    
	COMXALPlayer *pPlayer = (COMXALPlayer *)pUserData;

	char szFile[1024];
	char tempFile[1024];

	strcpy (szFile, pPlayer->m_szPathLib);
	//	strcat (szFile, "lib/lib");

	memset(tempFile, 0, sizeof(tempFile));
	strcpy(tempFile, "/data/local/tmp/OnStreamPlayer/lib/");
	if (strstr(pLibName, "lib") == 0)
	{
		strcat(szFile, "lib");
		strcat(tempFile, "lib");
	}

	strcat(szFile, pLibName); // use non-NEON routines and call v6 lib
#ifdef __OSMP
	if( strstr( szFile , ".so" ))
	{
		memset(szFile, 0, 1024);
		strcpy (szFile, pPlayer->m_szPathLib);
		if(!strstr(pLibName, "libvo"))
		{
			strcat(szFile, "lib");
		}
		int n = (int)(strstr(pLibName, ".so") - pLibName);
		strncat(szFile, pLibName, n);
	}
	strcat(szFile, __OSMP);
#endif
	strcat(tempFile, pLibName);


	if( strstr( szFile , ".so" ) == 0 )
	{
		strcat( szFile , ".so" );
		strcat( tempFile, ".so" );
	}

		
	int n;
	if(pPlayer->m_nPrefixEnable)
	{
		char PreFile[1024];
		char *p = strstr(szFile, "libvo");

		if(p)
		{
			p = p + 5;
			n = (int)(p - szFile);
			memset(PreFile, 0, 1024);
			strncpy(PreFile, szFile, n);
			strcat(PreFile, pPlayer->m_cPrefix);
			strcat(PreFile, p);
			strcpy(szFile, PreFile);
		}

		p = strstr(tempFile, "libvo");
		if(p)
		{
			p = p + 5;
			n = (int)(p - tempFile);
			memset(PreFile, 0, 1024);
			strncpy(PreFile, tempFile, n);
			strcat(PreFile, pPlayer->m_cPrefix);
			strcat(PreFile, p);

			strcpy(tempFile, PreFile);
		}
	}

	if(pPlayer->m_nSuffixEnable)
	{
		char SufFile[1024];
		char *p = strstr(szFile, ".so");

		if(p)
		{
			n = (int)(p - szFile);
			memset(SufFile, 0, 1024);
			strncpy(SufFile, szFile, n);
			strcat(SufFile, pPlayer->m_cSuffix);
			strcat(SufFile, p);
			strcpy(szFile, SufFile);
		}

		p = strstr(tempFile, ".so");
		if(p)
		{
			n = (int)(p - tempFile);
			memset(SufFile, 0, 1024);
			strncpy(SufFile, tempFile, n);
			strcat(SufFile, pPlayer->m_cSuffix);
			strcat(SufFile, p);

			strcpy(tempFile, SufFile);
		}
	}

	VOLOGI("Load lib name: %s",szFile);

	void * hDll = NULL;
	
	if(voOS_EnableDebugMode(0))
	{
		hDll = dlopen (tempFile, RTLD_NOW);
	}

	if (hDll == NULL)
	{
		hDll = dlopen (szFile, RTLD_NOW);

		if (hDll == NULL)
		{
			strcpy (szFile, pPlayer->m_szPathLib);
			strcat (szFile, "lib/");
			strcat (szFile, pLibName);

			if( strstr( szFile , ".so" ) == 0 )
				strcat( szFile , ".so" );

			hDll = dlopen (szFile, RTLD_NOW);
		}
		VOLOGI ("Load Library File %s, Handle %p", szFile, hDll);
	}else
	{
		VOLOGI ("Load Library File %s, Handle %p", tempFile, hDll);
	}

	return hDll;
}
