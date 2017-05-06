	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompSource.cpp

	Contains:	voCOMXCompSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXMemory.h"
#include "voCOMXCompSource.h"
#include "voOMX_Index.h"
#include "voPlayReady.h"
#include "voWidevine.h"

#define LOG_TAG "voCOMXCompSource"
#include "voLog.h"

typedef struct
{
	IVOFileBasedDRM*		pIDrm;
	OMX_VO_DRM_FILEBASED_IO	sIO;
	OMX_S32					nFD;
} OMX_VO_DRM_FILEBASED_HANDLE;

voCOMXCompSource::voCOMXCompSource(OMX_COMPONENTTYPE * pComponent)
	: voCOMXBaseComponent (pComponent)
	, m_pPlayReadyAPI(NULL)
	, m_pPlayReadyDecryptionInfo(NULL)
	, m_pIFileBasedDrm(NULL)
	, m_pIVOWideVineDRM(NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCompSource::~voCOMXCompSource(void)
{
	if(m_pPlayReadyDecryptionInfo)
		voOMXMemFree(m_pPlayReadyDecryptionInfo);

	if(m_pPlayReadyAPI)
		voOMXMemFree(m_pPlayReadyAPI);
}

OMX_ERRORTYPE voCOMXCompSource::SetNewState (OMX_STATETYPE newState)
{
	if (m_tsState.Waiting ())
		m_tsState.Up ();

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
		return OMX_ErrorNone;
	else if (m_sTrans == COMP_TRANSSTATE_IdleToLoaded || m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
		return OMX_ErrorNone;

	return voCOMXBaseComponent::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXCompSource::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											  OMX_IN  OMX_INDEXTYPE nIndex,
											  OMX_IN  OMX_PTR pComponentParameterStructure)
{
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	switch (nIndex)
	{
	case OMX_VO_IndexPlayReadyAPI:
		{
			OMX_VO_DRM_PLAYREADY_API* pAPI = (OMX_VO_DRM_PLAYREADY_API*)pComponentParameterStructure;
			if(!pAPI)
			{
				VOLOGE("OMX_VO_IndexPlayReadyAPI pComponentParameterStructure is NULL");
				return OMX_ErrorBadParameter;
			}

			if(!m_pPlayReadyAPI)
				m_pPlayReadyAPI = (OMX_VO_DRM_PLAYREADY_API*)voOMXMemAlloc(sizeof(OMX_VO_DRM_PLAYREADY_API));
			if(!m_pPlayReadyAPI)
				return OMX_ErrorInsufficientResources;

			VOLOGI("OMX_VO_IndexPlayReadyAPI: APIs 0x%08X, Initialize 0x%08X, Destory 0x%08X, Get_License_Challenge 0x%08X, Set_License_Response 0x%08X, Commit 0x%08X, Init_Decrypt 0x%08X, Decrypt 0x%08X", 
				(int)pAPI, (int)pAPI->Initialize, (int)pAPI->Destory, (int)pAPI->Get_License_Challenge, (int)pAPI->Set_License_Response, (int)pAPI->Commit, (int)pAPI->Init_Decrypt, (int)pAPI->Decrypt);

			m_pPlayReadyAPI->Initialize = pAPI->Initialize;
			m_pPlayReadyAPI->Destory = pAPI->Destory;
			m_pPlayReadyAPI->Get_License_Challenge = pAPI->Get_License_Challenge;
			m_pPlayReadyAPI->Set_License_Response = pAPI->Set_License_Response;
			m_pPlayReadyAPI->Commit = pAPI->Commit;
			m_pPlayReadyAPI->Init_Decrypt = pAPI->Init_Decrypt;
			m_pPlayReadyAPI->Decrypt = pAPI->Decrypt;
		}
		break;

	case OMX_VO_IndexFileBasedDRMInterface:
		{
			m_pIFileBasedDrm = (IVOFileBasedDRM*)pComponentParameterStructure;
			VOLOGI("OMX_VO_IndexFileBasedDRMInterface 0x%08X", (int)m_pIFileBasedDrm);
		}
		break;

	case OMX_VO_IndexDRMWidevine:
		{
			m_pIVOWideVineDRM = (IVOWidevineDRM*)pComponentParameterStructure;
			VOLOGI("OMX_VO_IndexDRMWidevine 0x%08X", (int)m_pIVOWideVineDRM);
		}
		break;

	default:
		errType = voCOMXBaseComponent::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

VO_U32 voCOMXCompSource::voOMXSourceDrmCallBack(VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved)
{
	voCOMXCompSource* pCompSource = (voCOMXCompSource*)pUserData;
	return pCompSource->OnDrmCallBack(nFlag, pParam, nReserved);
}

OMX_U32 voCOMXCompSource::OnDrmCallBack(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	if(OMX_VO_SOURCEDRM_FLAG_DRMINFO == nFlag)
	{
		OMX_VO_SOURCEDRM_INFO * pDrmInfo = (OMX_VO_SOURCEDRM_INFO *)pParam;
		if(OMX_VO_DRMTYPE_PlayReady == pDrmInfo->nType)
		{
			if(!m_pPlayReadyAPI)
			{
				VOLOGE ("m_pPlayReadyAPI is NULL");
				return OMX_VO_ERR_DRM_ERRORSTATE;
			}

			if(m_pPlayReadyDecryptionInfo)
				voOMXMemFree(m_pPlayReadyDecryptionInfo);

			VO_PLAYREADY_INFO* pPlayReadyInfo = (VO_PLAYREADY_INFO*)pDrmInfo->pDrmInfo;
			if(pDrmInfo->nReserved[0])
			{
				void * hPlayReadyDrm = m_pPlayReadyAPI->Initialize(pPlayReadyInfo->pDrmHeader, pPlayReadyInfo->nDrmHeader);
				if(!hPlayReadyDrm)
				{
					VOLOGE ("Initialize return NULL");
					return VO_ERR_OUTOF_MEMORY;
				}

				VO_U32 nRC = 2;
				while(nRC == 2)
					nRC = PlayReady_RequestResponse(hPlayReadyDrm, (VO_GET_LICENSE_RESPONSE)pDrmInfo->nReserved[0], (VO_PTR)pDrmInfo->nReserved[1]);

				m_pPlayReadyAPI->Destory(hPlayReadyDrm);
				if(nRC)
				{
					VOLOGE ("GetLicenseResponse Error 0x%08X", (int)nRC);
					return OMX_VO_ERR_DRM_BADDRMINFO;
				}
			}

			VO_U32 nDecryptionInfo = 0;
			int nRes = m_pPlayReadyAPI->Commit(pPlayReadyInfo->pDrmHeader, pPlayReadyInfo->nDrmHeader, NULL, (unsigned int*)&nDecryptionInfo);
			if(nDecryptionInfo <= 0)
			{
				VOLOGE ("Commit fail nRes 0x%08X", (int)nRes);
				return OMX_VO_ERR_DRM_BADDRMINFO;
			}

			m_pPlayReadyDecryptionInfo = (OMX_U8*)voOMXMemAlloc(nDecryptionInfo);
			if(!m_pPlayReadyDecryptionInfo)
			{
				VOLOGE ("allocate m_pPlayReadyDecryptionInfo fail %d", (int)nDecryptionInfo);
				return OMX_VO_ERR_DRM_UNDEFINED;
			}

			nRes = m_pPlayReadyAPI->Commit(pPlayReadyInfo->pDrmHeader, pPlayReadyInfo->nDrmHeader, m_pPlayReadyDecryptionInfo, (unsigned int*)&nDecryptionInfo);
			VOLOGI("PlayReady Commit return %d", nRes);

			return OMX_VO_ERR_DRM_OK;
		}
		else if (OMX_VO_DRMTYPE_Widevine == pDrmInfo->nType)
		{
			if(!m_pIVOWideVineDRM)
			{
				VOLOGE ("Widevine m_pIVODRMWideVine is NULL");
				return OMX_VO_ERR_DRM_ERRORSTATE;
			}

			VO_Widevine_INFO* pWidevineInfo = (VO_Widevine_INFO*)pDrmInfo->pDrmInfo;
			VOWidewineDrmBuffer header(pWidevineInfo->pHeadData, pWidevineInfo->nHeaderLen);

			VO_U32 nRC = m_pIVOWideVineDRM->initializeDecryptUnit(pWidevineInfo->nDecryptID, &header);
			if(nRC != Widevine_DRM_NO_ERROR)
			{
				VOLOGE("Widevine initializeDecryptUnit fails: nRC=%d", (int)nRC);
				return OMX_VO_ERR_DRM_BADDRMINFO;
			}

			return OMX_VO_ERR_DRM_OK;
		}
		

		return OMX_VO_ERR_DRM_MODULENOTFOUND;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_DRMDATA == nFlag)
	{
		OMX_VO_SOURCEDRM_DATA * pDrmData = (OMX_VO_SOURCEDRM_DATA *)pParam;

		if(OMX_VO_DRMDATATYPE_PACKETDATA == pDrmData->sDataInfo.nDataType)
		{
			if(m_pPlayReadyAPI)
			{
				if(!m_pPlayReadyAPI || !m_pPlayReadyDecryptionInfo)
					return OMX_VO_ERR_DRM_ERRORSTATE;

				int nRes = m_pPlayReadyAPI->Init_Decrypt(m_pPlayReadyDecryptionInfo, NULL, 0);
				VOLOGR("PlayReady Init_Decrypt return %d", nRes);

				OMX_VO_PLAYREADY_AES_CTX sAesCtx;
				VO_PLAYREADY_AESCTR_INFO* pAesCtrInfo = (VO_PLAYREADY_AESCTR_INFO*)pDrmData->sDataInfo.pInfo;
				sAesCtx.qwBlockOffset = pAesCtrInfo->nBlockOffset;
				sAesCtx.ucByteOffset = pAesCtrInfo->btByteOffset;
				// LG PlayReady Engine will request such buffer order, East 20110727
				VO_U8* p = (VO_U8*)&(sAesCtx.qwInitializationVector); 
				for(VO_S32 i = 7; i >= 0; i--)
				{
					*p = pAesCtrInfo->btInitializationVector[i];
					p++;
				}

				nRes = m_pPlayReadyAPI->Decrypt(m_pPlayReadyDecryptionInfo, &sAesCtx, pDrmData->pData, pDrmData->nSize);
				VOLOGR("PlayReady Decrypt return %d", nRes);

				return OMX_VO_ERR_DRM_OK;

			}
			else if(m_pIVOWideVineDRM)
			{
				VO_U32 nRC;
				VO_BYTE *IVdata = (VO_BYTE *)malloc(16);
				memset(IVdata, 0, 16);

				VO_U32	dec_payloadDataLength = (pDrmData->nSize>>4)<<4;

				if(pDrmData->nSize==dec_payloadDataLength)// payloadDataLength is multiple of 16
				{	
					VOWidewineDrmBuffer IV(IVdata,16);
					VOWidewineDrmBuffer encBuf(pDrmData->pData, pDrmData->nSize);
					VOWidewineDrmBuffer decBuffer;
					decBuffer.length = pDrmData->nSize;
					decBuffer.data = pDrmData->pData;
				//	decBuffer.data = (VO_BYTE *)malloc(pDrmData->nSize);
					VOWidewineDrmBuffer* pDecBuffer = &decBuffer;

					nRC = m_pIVOWideVineDRM->decrypt(0, &encBuf, &pDecBuffer, &IV);
					if(nRC != Widevine_DRM_NO_ERROR)
					{
						VOLOGE("Widevine decrpt len=%d, fails: %d", (int)pDrmData->nSize,(int)nRC);
						free(IVdata);
				//		free(decBuffer.data);
						return OMX_VO_ERR_DRM_UNDEFINED;
					}
					memcpy(pDrmData->pData, decBuffer.data, decBuffer.length);
					free(IVdata);
				//	free(decBuffer.data);
					return OMX_VO_ERR_DRM_OK;
				}
				else
				{
					VO_BYTE *pPayload = pDrmData->pData;
					// step 1
					memset(IVdata, 0x00, 16);
					VOWidewineDrmBuffer IVBuf_Step1(IVdata,16);
					VOWidewineDrmBuffer EncBuf_Step1(pPayload, dec_payloadDataLength - 16);
					VOWidewineDrmBuffer DecBuf_Step1;
					DecBuf_Step1.length = dec_payloadDataLength - 16;
					DecBuf_Step1.data = (VO_BYTE *)malloc(dec_payloadDataLength - 16);
					VOWidewineDrmBuffer *pDecBuf_Step1 = &DecBuf_Step1;

					nRC = m_pIVOWideVineDRM->decrypt(0, &EncBuf_Step1, &pDecBuf_Step1, &IVBuf_Step1);
					if(nRC != Widevine_DRM_NO_ERROR)
					{
						VOLOGE("Widevine decrpt 1 fails: %d", (int)nRC);
						free(IVdata);
						free(DecBuf_Step1.data);
						return OMX_VO_ERR_DRM_UNDEFINED;
					}
	
					// step 2
					memset(IVdata, 0x00, 16);
					memcpy(IVdata, ( pPayload + dec_payloadDataLength ), pDrmData->nSize - dec_payloadDataLength );
					VOWidewineDrmBuffer IVBuf_Step2(IVdata,16);
					VOWidewineDrmBuffer EncBuf_Step2(pPayload + dec_payloadDataLength - 16, 16);
					VOWidewineDrmBuffer DecBuf_Step2;
					DecBuf_Step2.length = 16;
					DecBuf_Step2.data = (VO_BYTE *)malloc(16);
					memset(DecBuf_Step2.data, 0, DecBuf_Step2.length);
					VOWidewineDrmBuffer *pDecBuf_Step2 = &DecBuf_Step2;
					nRC = m_pIVOWideVineDRM->decrypt(0, &EncBuf_Step2, &pDecBuf_Step2, &IVBuf_Step2);
					if(nRC != Widevine_DRM_NO_ERROR)
					{
						VOLOGE("Widevine decrpt 2 fails: %d", (int)nRC);
						free(IVdata);
						free(DecBuf_Step1.data);
						free(DecBuf_Step2.data);
						return OMX_VO_ERR_DRM_UNDEFINED;
					}

					// step 3
					memset(IVdata, 0x00, 16);
					memcpy(IVdata, ( pPayload + dec_payloadDataLength - 32 ), 16 );
					VOWidewineDrmBuffer IVBuf_Step3(IVdata,16);
					VO_BYTE tmp[16];
					memcpy(tmp, DecBuf_Step2.data, 16);
					memcpy(tmp, ( pPayload + dec_payloadDataLength) , pDrmData->nSize - dec_payloadDataLength );
					VOWidewineDrmBuffer EncBuf_Step3(tmp, 16);
					VOWidewineDrmBuffer DecBuf_Step3;
					DecBuf_Step3.length = 16;
					DecBuf_Step3.data = (VO_BYTE *)malloc(16);
					memset(DecBuf_Step3.data, 0, DecBuf_Step3.length);
					VOWidewineDrmBuffer *pDecBuf_Step3 = &DecBuf_Step3;
					nRC = m_pIVOWideVineDRM->decrypt(0, &EncBuf_Step3, &pDecBuf_Step3, &IVBuf_Step3);
					if(nRC != Widevine_DRM_NO_ERROR)
					{
						VOLOGE("Widevine decrpt 3 fails: %d", (int)nRC);
						free(IVdata);
						free(DecBuf_Step1.data);
						free(DecBuf_Step2.data);
						free(DecBuf_Step3.data);
						return OMX_VO_ERR_DRM_UNDEFINED;
					}
					// replace with decrypted data
					memcpy(pDrmData->pData, DecBuf_Step1.data, DecBuf_Step1.length);
					memcpy(pDrmData->pData+DecBuf_Step1.length, DecBuf_Step3.data, DecBuf_Step3.length);
					memcpy(pDrmData->pData+dec_payloadDataLength, DecBuf_Step2.data, pDrmData->nSize - dec_payloadDataLength);
					free(IVdata);
					free(DecBuf_Step1.data);
					free(DecBuf_Step2.data);
					free(DecBuf_Step3.data);
					return OMX_VO_ERR_DRM_OK;
				}				
			}
			else
				return OMX_VO_ERR_DRM_ERRORSTATE;			
		}
		else
			return OMX_VO_ERR_DRM_BADPARAMETER;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_FLUSH == nFlag)
	{
		//reserved
		return OMX_VO_ERR_DRM_OK;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_ISSUPPORTED == nFlag)
	{
		if(*((OMX_VO_DRM_TYPE*)pParam) == OMX_VO_DRMTYPE_PlayReady && m_pPlayReadyAPI)
			return OMX_VO_ERR_DRM_OK;
	}

	return OMX_VO_ERR_DRM_BADPARAMETER;
}

VO_U32 voCOMXCompSource::PlayReady_RequestResponse(VO_PTR hPlayReadyDrm, VO_GET_LICENSE_RESPONSE fGetLicenseResponse, VO_PTR pGetLicenseResponseUserData)
{
	// get challenge resource length
	VO_U32 nChallenge = 0, nUrl = 0, nHttpHeader = 0;
	VO_U32 nRC = m_pPlayReadyAPI->Get_License_Challenge(hPlayReadyDrm, NULL, (unsigned int*)&nChallenge, NULL, (unsigned int*)&nUrl, NULL, (unsigned int*)&nHttpHeader);
	VOLOGI ("nRC 0x%08X, nChallenge %d, nUrl %d, nHttpHeader %d", (int)nRC, (int)nChallenge, (int)nUrl, (int)nHttpHeader);
	if(nRC)
		return 1;

	// allocate challenge resource
	VO_PBYTE pChallenge = NULL;
	VO_PCHAR pcUrl = NULL, pcHttpHeader = NULL;
	if(nChallenge > 0)
		pChallenge = (VO_PBYTE)voOMXMemAlloc(nChallenge);
	if(nUrl > 0)
		pcUrl = (VO_PCHAR)voOMXMemAlloc(nUrl);
	if(nHttpHeader > 0)
		pcHttpHeader = (VO_PCHAR)voOMXMemAlloc(nHttpHeader);

	VO_U32 nRet = 1;
	if((nChallenge <= 0 || pChallenge) && (nUrl <= 0 || pcUrl) && (nHttpHeader <= 0 || pcHttpHeader))
	{
		// get challenge content
		nRC = m_pPlayReadyAPI->Get_License_Challenge(hPlayReadyDrm, pChallenge, (unsigned int*)&nChallenge, pcUrl, (unsigned int*)&nUrl, pcHttpHeader, (unsigned int*)&nHttpHeader);
		VOLOGI ("nRC 0x%08X, nChallenge %d, nUrl %d, nHttpHeader %d", (int)nRC, (int)nChallenge, (int)nUrl, (int)nHttpHeader);

		if(!nRC)
		{
			// get response from license server
			VO_PCHAR pcMimetype = NULL;
			VO_PBYTE pResponse = NULL;
			VO_U32 nResponse = 0;
			nRC = fGetLicenseResponse(pChallenge, nChallenge, pcUrl, nUrl, pcHttpHeader, nHttpHeader, &pcMimetype, &pResponse, &nResponse, NULL, pGetLicenseResponseUserData);
			VOLOGI ("VO_GET_LICENSE_RESPONSE nRC 0x%08X, pcMimetype %s, pResponse 0x%08X, nResponse %d", 
				(int)nRC, (char*)pcMimetype, (int)pResponse, (int)nResponse);

			if(!nRC)
			{
				nRC = m_pPlayReadyAPI->Set_License_Response(hPlayReadyDrm, pcMimetype, pResponse, nResponse);
				VOLOGI ("Set_License_Response nRC 0x%08X", (int)nRC);

				if(nRC == 0)
					nRet = 0;
				else if(nRC == 2)	// next request
					nRet = 2;
			}
		}
	}

	if(pChallenge)
		voOMXMemFree(pChallenge);
	if(pcUrl)
		voOMXMemFree(pcUrl);
	if(pcHttpHeader)
		voOMXMemFree(pcHttpHeader);

	return nRet;
}

VO_PTR voCOMXCompSource::FileBasedDrm_Open(VO_FILE_SOURCE * pSource)
{
	if(!pSource)
		return NULL;

	OMX_VO_DRM_FILEBASED_HANDLE* pDrmHandle = new OMX_VO_DRM_FILEBASED_HANDLE;
	if(!pDrmHandle)
		return NULL;

	pDrmHandle->pIDrm = (IVOFileBasedDRM*)pSource->nReserve;
	pDrmHandle->nFD = -1;

	int nRet = -1;
	if(pSource->nFlag == VO_FILE_TYPE_NAME)
		nRet = pDrmHandle->pIDrm->Open(&pDrmHandle->sIO, (const char*)pSource->pSource);
	else if(pSource->nFlag == VO_FILE_TYPE_ID)
	{
#ifdef _LINUX
		pDrmHandle->nFD = dup((int)pSource->pSource);
		nRet = pDrmHandle->pIDrm->Open(&pDrmHandle->sIO, pDrmHandle->nFD);
#endif	// _LINUX
	}

	if(nRet != 0)
	{
		delete pDrmHandle;
		return NULL;
	}

	return pDrmHandle;
}

VO_S32 voCOMXCompSource::FileBasedDrm_Read(VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	OMX_VO_DRM_FILEBASED_HANDLE* pDrmHandle = (OMX_VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	return pDrmHandle->pIDrm->Read(&pDrmHandle->sIO, pBuffer, uSize);
}

VO_S32 voCOMXCompSource::FileBasedDrm_Write(VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	return 0;
}

VO_S32 voCOMXCompSource::FileBasedDrm_Flush(VO_PTR pHandle)
{
	return 0;
}

VO_S64 voCOMXCompSource::FileBasedDrm_Seek(VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag)
{
	OMX_VO_DRM_FILEBASED_HANDLE* pDrmHandle = (OMX_VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	OMX_VO_DRM_FILEBASED_SEEKMODE nSeekMode;
	if (uFlag == VO_FILE_BEGIN)
		nSeekMode = OMX_VO_DRM_FILEBASED_SEEK_SET;
	else if (uFlag == VO_FILE_CURRENT)
		nSeekMode = OMX_VO_DRM_FILEBASED_SEEK_CUR;
	else
		nSeekMode = OMX_VO_DRM_FILEBASED_SEEK_END;

	VO_S64 nRet = pDrmHandle->pIDrm->Seek(&pDrmHandle->sIO, (OMX_U32)nPos, nSeekMode);
	if(nRet != 0)
		return -1;

	return pDrmHandle->pIDrm->Tell(&pDrmHandle->sIO);
}

VO_S64 voCOMXCompSource::FileBasedDrm_Size(VO_PTR pHandle)
{
	OMX_VO_DRM_FILEBASED_HANDLE* pDrmHandle = (OMX_VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	// this API should return current actual file size instead of complete file size
//	return pDrmHandle->pIDrm->Size(&pDrmHandle->sIO);
	int nOldPos = pDrmHandle->pIDrm->Tell(&pDrmHandle->sIO);
	if(-1 == nOldPos)
		return -1;

	int nRet = pDrmHandle->pIDrm->Seek(&pDrmHandle->sIO, 0, OMX_VO_DRM_FILEBASED_SEEK_END);
	if(-1 == nRet)
		return -1;

	nRet = pDrmHandle->pIDrm->Tell(&pDrmHandle->sIO);
	// restore position
	pDrmHandle->pIDrm->Seek(&pDrmHandle->sIO, nOldPos, OMX_VO_DRM_FILEBASED_SEEK_SET);
	
	return nRet;
}

VO_S64 voCOMXCompSource::FileBasedDrm_Save(VO_PTR pHandle)
{
	return 0;
}

VO_S32 voCOMXCompSource::FileBasedDrm_Close(VO_PTR pHandle)
{
	OMX_VO_DRM_FILEBASED_HANDLE* pDrmHandle = (OMX_VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	pDrmHandle->pIDrm->Close(&pDrmHandle->sIO);
#ifdef _LINUX
	if(-1 != pDrmHandle->nFD)
		close(pDrmHandle->nFD);
#endif	// _LINUX
	delete pDrmHandle;

	return 0;
}
