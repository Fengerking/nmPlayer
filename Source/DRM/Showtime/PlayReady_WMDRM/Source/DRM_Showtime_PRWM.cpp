#include "DRM_Showtime_PRWM.h"
#include "voHLSDRM.h"
#include "voWMDRM.h"
#include "drm_auth.h"
#include "HTTPs_Transaction.h"
#include "voLog.h"
#include "voPlayReady.h"


int32_t drm_wrapper_authenticate(void *context)
{
	unsigned char nonce[DRM_AUTH_NONCE_LENGTH];
	unsigned char challenge[DRM_AUTH_CHALLENGE_LENGTH];
	unsigned char response[DRM_AUTH_RESPONSE_LENGTH];
	int nonceLen, challengeLen, responseLen;
	DrmAgent_Error Err;

	if (context == NULL)
		return VO_ERR_DRM2_ERRORSTATE;

	memset(nonce, 0, sizeof(nonce));
	memset(challenge, 0, sizeof(challenge));
	memset(response, 0, sizeof(response));

	nonceLen = DRM_AUTH_NONCE_LENGTH;
	DRM_AGENT_AUTHENTICATION_SETUP(nonce, nonceLen, Err);
	if (Err != DRMAPI_SUCCESS)
	{
		VOLOGE("Authentication Setup operation failed!\n");
		return VO_ERR_DRM2_ERRORSTATE;
	}

	challengeLen = DRM_AUTH_CHALLENGE_LENGTH;
	if (DrmAgent_AuthenticationSetup(context, nonce, DRM_AUTH_NONCE_LENGTH, challenge, &challengeLen) != DRMAPI_SUCCESS)
	{
		VOLOGE("Authentication Setup operation failed!\n");
		return VO_ERR_DRM2_ERRORSTATE;
	}

	responseLen = DRM_AUTH_RESPONSE_LENGTH;
	DRM_AGENT_AUTHENTICATION_COMPLETE(nonce, nonceLen, challenge, challengeLen, response, responseLen, Err);
	if (Err != DRMAPI_SUCCESS)
	{
		VOLOGE("DRM_AGENT_AUTHENTICATION_COMPLETE failed!\n");
		return VO_ERR_DRM2_ERRORSTATE;
	}

	if (DrmAgent_AuthenticationComplete(context, response, responseLen) != DRMAPI_SUCCESS)
	{
		VOLOGE("DrmAgent_AuthenticationComplete failed!\n");
		return VO_ERR_DRM2_ERRORSTATE;
	}

	return VO_ERR_DRM2_OK;
}

CDRM_Showtime_PRWM::CDRM_Showtime_PRWM(void)
: m_Context(NULL)
, m_Handle(NULL)
, m_pBuffer(NULL)
{
	memset(m_szSID, 0, 256);
	memset(m_szURLServer, 0, 1024);

	m_pBuffer = new VO_BYTE[1920 * 1080 * 3 / 2];
}


CDRM_Showtime_PRWM::~CDRM_Showtime_PRWM(void)
{
	Uninit();

	if (m_pBuffer)
		delete []m_pBuffer;
}


VO_U32 CDRM_Showtime_PRWM::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_Showtime_PRWM");

	return CDRMStreaming::Init(pParam);
}


VO_U32 CDRM_Showtime_PRWM::Uninit()
{
	voCAutoLock lock( &m_lock );

	if (m_Context)
	{
		DrmAgent_Rights_Delete(m_Context, m_Handle);

		DrmAgent_Close(m_Context, m_Handle);
		m_Handle = NULL;

		DrmAgent_FreeContext(m_Context);
		m_Context = NULL;
	}

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM_Showtime_PRWM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	VOLOGR("uID %x", uID);

	switch (uID)
	{
	case VO_PID_SOURCE2_DOHTTPVERIFICATION:
		{
			if (NULL == pParam)
				return VO_ERR_DRM2_BADPARAMETER;

			VO_SOURCE2_VERIFICATIONINFO* pInfo = (VO_SOURCE2_VERIFICATIONINFO *)pParam;

			if (pInfo->uDataSize > 0)
			{
				VOLOGR("%s", pInfo->pData);
				VO_CHAR *pInput = (VO_CHAR *)pInfo->pData;

				VO_CHAR *p = strstr(pInput, "\r\n");
				if (p)
				{
					strncpy(m_szURLServer, (const char*)pInfo->pData, p - pInput);

					p +=  strlen("\r\n");

					strncpy( m_szSID, p, pInfo->uDataSize - (p - pInput) );
				}
			}

			//strcpy(m_szSID, "SID=k8k80a0ri06ikf5sgq25tr320jmamut0");

			return VO_ERR_DRM2_OK;
		}
		break;

	default:
		break;
	}

	return CDRMStreaming::SetParameter(uID, pParam);
}

VO_U32 CDRM_Showtime_PRWM::VO_Rights_Acquire()
{
	int iLenChallenge = 0;
	DrmAgent_Error iError = DrmAgent_Rights_Generate_Challenge(m_Context, m_Handle, DrmGenerateChallenge_Flag_PlayReady, NULL, NULL, &iLenChallenge);
	if (iError != DRMAPI_E_BUFFER_TOO_SMALL) {
		VOLOGE("DrmAgent_Rights_Generate_Challenge -0x%x", -iError);
		return VO_ERR_DRM2_BASE;
	}

	unsigned char* pChallenge = new unsigned char[iLenChallenge];
	iError = DrmAgent_Rights_Generate_Challenge(m_Context, m_Handle, DrmGenerateChallenge_Flag_PlayReady, NULL, pChallenge, &iLenChallenge);
	if (iError != DRMAPI_SUCCESS) {
		VOLOGE("DrmAgent_Rights_Generate_Challenge -0x%x", -iError);

		delete []pChallenge;
		return VO_ERR_DRM2_BASE;
	}

	//-
	CHTTP_Transaction *pHTTPTransaction = NULL;
	if ( strstr(m_szURLServer, "https://") )
		pHTTPTransaction = new CHTTPs_Transaction;
	else
		pHTTPTransaction = new CHTTP_Transaction;

	RequestInfo request;
	VO_CHAR szExtendHeaders[1024] = {0};

	memset( &request, 0, sizeof(RequestInfo) );
	request.method			= HTTP_POST;
	request.szURL			= m_szURLServer;
	request.szBody			= (VO_CHAR*)pChallenge;
	sprintf(szExtendHeaders, "Content-Length: %d\r\nContent-Type: text/xml; charset=utf-8\r\nUser-Agent: AuthenTec DRM Agent\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\"\r\nDRM-Version: 2.0\r\nPragma: no-cache\r\nmsprdrm_server_rediret_compat: false\r\nmsprdrm_server_exception_compat: false\r\nX-CustomData: %s\r\n", iLenChallenge, m_szSID);
	request.szExtendHeaders = szExtendHeaders;		

	VO_S32 iRet = pHTTPTransaction->Start(&request);

	delete []pChallenge;
	if (iRet) {
		VOLOGE("HTTP Start");

		delete pHTTPTransaction;
		return VO_ERR_DRM2_BASE;
	}

	VO_CHAR* szRespose = NULL;
	iRet = pHTTPTransaction->getResposeInfo(&szRespose);
	if (iRet) {
		VOLOGE("HTTP getResposeInfo");

		delete pHTTPTransaction;
		return VO_ERR_DRM2_BASE;
	}

	VO_CHAR *p1 = strstr(szRespose , "Content-Length: ");

	long long ll = 0;
	if (p1)
	{
		p1 += strlen("Content-Length: ");

		while (*p1 == ' ')
			p1++;

		if (p1)
			sscanf(p1 , "%lld\r\n" , &ll);
	}

	int iLenResponse = ll;
	if (iLenResponse <= 0)
	{
		VOLOGE("license len error");

		delete pHTTPTransaction;
		return VO_ERR_DRM2_BASE;
	}

	unsigned char* pResponse = new unsigned char[iLenResponse];
	iRet = 0;
	do  {
		iRet = pHTTPTransaction->ReadData((VO_CHAR*)pResponse + iLenResponse - ll, ll);
		ll -= iRet;
	} while (iRet > 0 && ll);

	// Assume the license response from the server is returned in the ¡¯response¡¯ buffer.
	// Now input this response to the DRM Agent.
	iError = DrmAgent_Data_Handle(m_Context, pResponse, iLenResponse, DT_WMDRM_License_Response, NULL, NULL, NULL, NULL);

	delete []pResponse;
	delete pHTTPTransaction;
	if (iError != DRMAPI_SUCCESS) {
		VOLOGE("DrmAgent_Data_Handle -0x%x", -iError);

		return VO_ERR_DRM2_BASE;
	}

	return VO_ERR_DRM2_OK;
}


VO_U32 CDRM_Showtime_PRWM::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	voCAutoLock lock(&m_lock);

	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			DrmAgent_Error iError = 0;

			DrmAgent_Bool bSecure = DrmAgent_IsSecureDevice();
			if (!bSecure) {
				VOLOGW("the device is not secure");
				SendEvent(VO_EVENTID_DRM2_ERR_NOT_SECURE, 0, 0);
			}

			if (NULL == m_Context)
			{
				VO_CHAR *p = strstr(m_szPackPath + strlen(m_szPackPath) - 7, "/lib");
				if (!p)
					return VO_ERR_DRM2_BASE;

				p++;
				VO_CHAR szStoreDir[512] = {0};
				strncpy(szStoreDir, m_szPackPath, p - m_szPackPath);
				VOLOGR("szStoreDir %s", szStoreDir);

				DrmAgent_InitParams init_params = {0};

				init_params.AppId				= (char *)"SampleMediaPlayer";
				init_params.Path				= szStoreDir;// /sdcard/Authentec/ 

				strcat(szStoreDir, "Authentec/");
				init_params.PathKeysAndCerts	= szStoreDir;
				//init_params.LogLevel			= DrmLogLevel_Detail;
				//init_params.LogOutput			= DrmLogOutput_Console;
				//init_params.DeviceID			= "356468764843469";

				m_Context = DrmAgent_CreateContext(&init_params, NULL, &iError);
				if (!m_Context || DRMAPI_SUCCESS != iError)
				{
					VOLOGE("DrmAgent_CreateContext -0x%x", -iError);
					SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
					return VO_ERR_DRM2_BASE;
				}
				else if (drm_wrapper_authenticate(m_Context) != 0)
				{
					VOLOGE("drm_wrapper_authenticate fail");
					SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);

					m_Context = NULL;
					return VO_ERR_DRM2_BASE;
				}
			}


			switch (eSourceFormat)
			{
			case VO_DRM2SRC_MEDIAFORMAT:
				{
					VO_DRM2_INFO * pDrmInfo = (VO_DRM2_INFO *)pParam;
					VO_WMDRM_INFO *pWMDRMInfo = (VO_WMDRM_INFO *)pDrmInfo->pDrmInfo;
					pWMDRMInfo->pExtendedContentEncryptionObject += 4;
					pWMDRMInfo->nExtendedContentEncryptionObject -= 4;

					iError = DrmAgent_AV_Open(m_Context, NULL, pWMDRMInfo->pExtendedContentEncryptionObject, pWMDRMInfo->nExtendedContentEncryptionObject, &m_Handle);
					if (DRMAPI_SUCCESS != iError) {
						VOLOGE("DrmAgent_AV_Open -0x%x", -iError);
						SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;

			case VO_DRM2SRC_CHUNK_SSTR:
				{
					VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);
					VOLOGR("Manifest %p %d", pDRMInfo->pManifestData, pDRMInfo->uSizeManifest);

					VO_BYTE *pDrmHeader = NULL;
					VO_U32 nDrmHeader = 0;
					VO_U32 uRet = getProtectionHeader_SSTR((VO_CHAR *)pDRMInfo->pManifestData, pDRMInfo->uSizeManifest, &pDrmHeader, &nDrmHeader, VO_TRUE);
					if (uRet) {
						delete []pDrmHeader;
						VOLOGE("!getProtectionHeader_SSTR");
						return uRet;
					}

					if ( !IsEncrypted() )
						return VO_ERR_DRM2_OK;

					iError = DrmAgent_AV_Open(m_Context, NULL, pDrmHeader, nDrmHeader, &m_Handle);

					delete []pDrmHeader;
					if (DRMAPI_SUCCESS != iError) {
						VOLOGE("DrmAgent_AV_Open -0x%x", -iError);
						SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;

			default:
				return 0;
			}

			EDrmAgent_RightsStatus rightsstatus = RST_LAST;
			DrmAgent_Constraints constraints;
			iError = DrmAgent_Rights_Verify(m_Context, m_Handle, Perm_Play, &rightsstatus, &constraints);
			if (DRMAPI_SUCCESS != iError) {
				VOLOGE("DrmAgent_Rights_Verify -0x%x", -iError);
				SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
				return VO_ERR_DRM2_BASE;
			}

			if (RST_Valid != rightsstatus)
			{
				VOLOGR("+cb");
				m_cbVerify.HTTP_Callback(m_cbVerify.hHandle, VO_SOURCE2_CALLBACKIDBASE_DRM, NULL);
				VOLOGR("-cb");

				if (strlen(m_szSID) == 0)
				{
					VOLOGR("type authentec without sid");

					iError = DrmAgent_Rights_Acquire(m_Context, m_Handle, NULL);
					if (DRMAPI_SUCCESS != iError) {
						VOLOGE("DrmAgent_Rights_Acquire -0x%x", -iError);
						SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
						return VO_ERR_DRM2_BASE;
					}
				}
				else
				{
					VOLOGR("type https with sid");

					if (VO_Rights_Acquire() != 0)
					{
						VOLOGE("VO_Rights_Acquire");
						SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);

						return VO_ERR_DRM2_BASE;
					}
				}
			}

			iError = DrmAgent_Rights_Verify(m_Context, m_Handle, Perm_Play, &rightsstatus, NULL);
			if (DRMAPI_SUCCESS != iError) {
				VOLOGE("DrmAgent_Rights_Verify -0x%x", -iError);
				SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
				return VO_ERR_DRM2_BASE;
			}
			else if (RST_Valid != rightsstatus)
			{
				VOLOGW("Still no right");
				return VO_ERR_DRM2_BASE;
			}

			iError = DrmAgent_AV_FinalizeInit(m_Context, m_Handle);
			if (DRMAPI_SUCCESS != iError) {
				VOLOGE("DrmAgent_AV_FinalizeInit -0x%x", -iError);

				if (DRMAPI_E_POLICY_CHECK_FAIL ==  iError)
				{
					SendEvent(VO_EVENTID_DRM2_ERR_POLICY_FAIL, 0, 0);
				}
				else
				{
					SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
				}

				return VO_ERR_DRM2_BASE;
			}

			return VO_ERR_DRM2_OK;

		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			VO_DRM2_DATA * pDRMData = static_cast<VO_DRM2_DATA *>(pParam);

			switch (pDRMData->sDataInfo.nDataType)
			{
			case VO_DRM2DATATYPE_CHUNK_BEGIN:
			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
			case VO_DRM2DATATYPE_CHUNK_END:
				break;

			case VO_DRM2DATATYPE_MEDIASAMPLE:
				{
					if ( !IsEncrypted() )
						return VO_ERR_DRM2_OK;

					if (!m_Context)
						return VO_ERR_DRM2_ERRORSTATE;

					VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO * pInfo = (VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO *)pDRMData->sDataInfo.pInfo;
					VOLOGR("Data pointer: %p , Size: %d , Box pointer: %p , Box Size: %d , SampleIndex: %d , TrackID: %d", 
						pDRMData->pData, pDRMData->nSize, pInfo->pSampleEncryptionBox, pInfo->uSampleEncryptionBoxSize, pInfo->uSampleIndex, pInfo->uTrackId);

					long unsigned n = pDRMData->nSize + 1024;

					DrmAgent_DecryptCtx_PIFF Ctx;
					Ctx.InBuf			= pDRMData->pData;
					Ctx.InBufLen		= pDRMData->nSize;
					Ctx.OutBuf			= m_pBuffer;
					Ctx.OutBufLen		= &n;
					Ctx.SampleEncBox	= pInfo->pSampleEncryptionBox;
					Ctx.SampleEncBoxLen	= pInfo->uSampleEncryptionBoxSize;
					Ctx.SampleIndex		= pInfo->uSampleIndex;
					Ctx.TrackID			= pInfo->uTrackId;

					DrmAgent_Error iError = DrmAgent_AV_DecryptPacket_WithCtx(m_Context, m_Handle, CType_PIFF, &Ctx);
					if (DRMAPI_SUCCESS != iError) {
						VOLOGE("DrmAgent_AV_DecryptPacket_WithCtx -0x%x", -iError);
						SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
						return VO_ERR_DRM2_BASE;
					}

					if (n != pDRMData->nSize)
					{
						VOLOGW("Bufer is not enough");
					}
					else
					{
						memcpy(pDRMData->pData, m_pBuffer, n);
					}
				}
				break;

			case VO_DRM2DATATYPE_PACKETDATA:
				{
					if (!m_Context)
						return VO_ERR_DRM2_ERRORSTATE;

					unsigned n = pDRMData->nSize + 1024;

					DrmAgent_DecryptCtx_WMDRM_ASF Ctx;
					Ctx.AsfPayload		= pDRMData->pData;
					Ctx.AsfPayloadLen	= pDRMData->nSize;
					Ctx.OutBuf			= m_pBuffer;
					Ctx.OutBufLen		= &n;

					DrmAgent_Error iError = DrmAgent_AV_DecryptPacket_WithCtx(m_Context, m_Handle, CType_WMDRM_ASF, &Ctx);
					if (DRMAPI_SUCCESS != iError) {
						VOLOGE("DrmAgent_AV_DecryptPacket_WithCtx -0x%x", -iError);
						SendEvent(VO_EVENTID_SOURCE2_ERR_DRMFAIL, 0, 0);
						return VO_ERR_DRM2_BASE;
					}

					if (n != pDRMData->nSize)
					{
						VOLOGW("Bufer is not enough");
					}
					else
					{
						memcpy(pDRMData->pData, m_pBuffer, n);
					}
				}
				break;

			default:
				return VO_ERR_NOT_IMPLEMENT;
			}
		}
		break;

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}