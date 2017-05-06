#include "DRM_Verimatrix_AES128_mac.h"
#include "openssl/rsa.h"
#include "openssl/engine.h"
#include "openssl/ssl.h"
#include "openssl/aes.h"
#include "voLog.h"
#include "voHLSDRM.h"
#if (defined _LINUX_ANDROID) || (defined _MAC_OS)
#include <sys/types.h>
#include <unistd.h>
#endif // _LINUX_ANDROID
#ifdef _IOS
#include <sys/types.h>
#include <unistd.h>
#endif
#include "voOSFunc.h"
#include "AES_CBC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


VO_U32 encryptHandshakeString(VO_BYTE* Data, VO_U32* uSize)
{
	unsigned char key[] = "L44T0sqrGSqgOu1";
	unsigned char iv[] = "0000000000000000";

	unsigned char pPublicKey[464] = { 0xeb, 0x6c, 0x00, 0x3d, 0x8f, 0x43, 0x6d, 0xe0, 0xb3, 0xc7, 0xf3, 0x58, 0x60, 0xdb, 0xce, 0x16, 0x40, 0x90, 0xe3, 0x48, 0x56, 0x34, 0x09, 0xce, 0x0c, 0xa2, 0xee, 0xce, 0x60, 0x64, 0xaf, 0xfd, 0x7b, 0x48, 0x51, 0x61, 0x8b, 0x7a, 0x55, 0x2f, 0x47, 0x12, 0x68, 0x95, 0xd5, 0x9b, 0xc6, 0x5f, 0x5e, 0xd7, 0xa2, 0x22, 0xb0, 0x78, 0x89, 0x1b, 0x30, 0xdc, 0x28, 0xd4, 0xf3, 0x11, 0x43, 0xd0, 0x18, 0xe0, 0x47, 0x76, 0x59, 0x45, 0x09, 0x9c, 0xd4, 0x72, 0x4f, 0xdb, 0xd7, 0x6b, 0x51, 0xd5, 0xf6, 0x5a, 0xa2, 0x71, 0x92, 0x32, 0xe8, 0xd5, 0x8b, 0xcf, 0xe9, 0x77, 0xe3, 0x01, 0x9e, 0x7c, 0x1e, 0x5f, 0xda, 0x22, 0x1f, 0xe1, 0x7b, 0x44, 0x82, 0x28, 0x6b, 0x4f, 0x81, 0x31, 0xc6, 0x28, 0xf0, 0x23, 0xd6, 0x9c, 0x4e, 0xf8, 0xee, 0x5d, 0x81, 0x63, 0xb2, 0x19, 0x9f, 0x70, 0x38, 0xf8, 0xa1, 0x11, 0x58, 0xfe, 0x29, 0xdd, 0x0f, 0x99, 0x31, 0x4f, 0x2a, 0x1b, 0xce, 0x16, 0xf7, 0x2f, 0xac, 0x08, 0x1a, 0x96, 0x4b, 0x4f, 0x93, 0xdb, 0x65, 0xf6, 0x5a, 0x1f, 0x36, 0xcd, 0x5f, 0xe6, 0xdf, 0x91, 0x80, 0x0d, 0x38, 0x17, 0x9a, 0xe3, 0x07, 0x70, 0xdb, 0xb7, 0x17, 0x73, 0x91, 0x25, 0x98, 0x7f, 0xc8, 0xa8, 0xac, 0x27, 0xee, 0x46, 0xa5, 0x41, 0x7d, 0x3d, 0x48, 0x88, 0x11, 0x46, 0xca, 0xc2, 0xe3, 0xe1, 0xa7, 0xd1, 0x1d, 0x3a, 0x89, 0x30, 0x43, 0x79, 0x78, 0x8e, 0x8b, 0x5a, 0x93, 0x77, 0x88, 0x11, 0x28, 0xb5, 0xf8, 0x08, 0x8f, 0x9b, 0xc8, 0xbf, 0xaf, 0xfd, 0xee, 0x4e, 0xbb, 0xa2, 0xcf, 0x12, 0x8d, 0xa7, 0x5e, 0x3c, 0xdb, 0x4d, 0xf2, 0x0a, 0x15, 0xcb, 0x97, 0x17, 0x5d, 0x14, 0x6b, 0x3e, 0x73, 0xce, 0xa5, 0xe9, 0xb7, 0x9e, 0xd7, 0xc2, 0x76, 0xb4, 0x34, 0x5f, 0xa6, 0xce, 0x7b, 0x6d, 0x87, 0x0a, 0x21, 0x77, 0x23, 0xbe, 0x5b, 0x22, 0x5c, 0x78, 0xb9, 0xdc, 0xd8, 0x23, 0x6e, 0x89, 0xb3, 0x18, 0x23, 0xd1, 0x4c, 0x4d, 0x38, 0xf3, 0x68, 0x93, 0x3b, 0xda, 0xb1, 0xc1, 0xe5, 0xe7, 0x31, 0x27, 0x56, 0x23, 0xda, 0x26, 0xf3, 0xbd, 0x2b, 0xcf, 0xe0, 0xf1, 0xb9, 0x73, 0xac, 0xe2, 0x26, 0xfd, 0x6c, 0x28, 0xbc, 0x24, 0x9a, 0xde, 0x84, 0xf2, 0xef, 0x25, 0x59, 0x95, 0x72, 0x3e, 0x42, 0x4c, 0x85, 0xf1, 0x9b, 0x7b, 0x98, 0x89, 0xd5, 0xaf, 0x03, 0x8f, 0x9d, 0xf8, 0xe5, 0x03, 0xa7, 0x9e, 0x0c, 0xf8, 0x3d, 0x26, 0x20, 0x48, 0x87, 0x99, 0xe7, 0x06, 0x6d, 0xdc, 0x89, 0x49, 0x88, 0x52, 0x25, 0xdd, 0x43, 0xd5, 0x57, 0x7e, 0xbc, 0x09, 0xab, 0xd1, 0x09, 0xde, 0x86, 0x80, 0x25, 0x7f, 0x11, 0xad, 0xef, 0x87, 0x8d, 0xe8, 0xb7, 0x9b, 0x1e, 0x20, 0x7c, 0x88, 0x85, 0x3e, 0x11, 0xae, 0xe7, 0xc4, 0x93, 0x7f, 0x4c, 0x7a, 0x8b, 0x29, 0xcb, 0xac, 0x8e, 0xa4, 0x94, 0xc7, 0xfd, 0xf5, 0xb6, 0xf0, 0xc4, 0xeb, 0x27, 0xe2, 0x28, 0x31, 0xb4, 0x2e, 0x3d, 0x32, 0xa2, 0x6d, 0x7c, 0x49, 0x4a, 0xa4, 0xea, 0x26, 0x10, 0x31, 0x59, 0x47, 0x51, 0x3a, 0x8a, 0x13, 0x2f, 0x49, 0x15, 0xbf, 0x56, 0x89, 0x04, 0x34, 0x2e, 0x17, 0xb3, 0xaf, 0x2b, 0xd8, 0x42, 0x47, 0x48, 0x1c, 0x4e, 0xa5, 0x45, 0x7e, 0xf6, 0x8e, 0x61, 0x4b, 0xb7, 0xcd, 0xef, 0x3d };

	CAES_CBC aAES128;
	aAES128.setKey(key, 16);
	aAES128.setIV(iv, 16);

	VO_U32 uSizeOutput = 0;
	aAES128.decryptData(pPublicKey, 464, pPublicKey, &uSizeOutput, VO_TRUE);

	BIO *bpo = BIO_new_mem_buf((void *)pPublicKey, -1);
	RSA *pubKey = PEM_read_bio_RSA_PUBKEY(bpo, 0, 0, 0);
	if (NULL == pubKey) {
		BIO_free(bpo);
		VOLOGE("Public Key NULL");
		return VO_ERR_DRM2_BASE;
	}

	char szHandshakeBase[] = { 0xe6, 0xe1, 0xe0, 0xe2, 0x98, 0x99, 0x99, 0x9e, 0x99, 0x9e, 0xc1, 0xc6, 0xc0, 0xcd, 0xc5, 0xc3, 0xdf, 0xde, 0xd3, 0x99, 0x98, 0x98, 0x99, 0x99, 0x98, 0xc7, 0xc8, 0xc1, 0xc0, 0xc2, 0xde, 0xcd, 0xc5, 0xc6, 0x98, 0x99, 0x99, 0x98, 0x99, 0x9e, 0x98, 0x99, 0x9e, 0x98, 0x0};
	for (int i = 0; i < strlen(szHandshakeBase); i++)
		szHandshakeBase[i] = szHandshakeBase[i] ^ 0xAA;

#if defined _WIN32
	DWORD iPID = GetCurrentThreadId();
#elif defined _LINUX_ANDROID || defined _MAC_OS
	pid_t iPID = getpid();
#endif

	char szHandshake[256] = { 0 };
	sprintf(szHandshake, "%s%d", szHandshakeBase, iPID);
	//VOLOGR("PID %d, szHandshake %s", iPID, szHandshake);

	int iRetSize = RSA_public_encrypt(RSA_PKCS1_OAEP_PADDING, (const unsigned char *)szHandshake, Data, pubKey, RSA_PKCS1_OAEP_PADDING);
	BIO_free(bpo);
	RSA_free(pubKey);

	if (iRetSize == -1) {
		VOLOGE("RSA_public_encrypt error: %s\n", ERR_error_string(ERR_get_error(), NULL));
		ERR_free_strings();
		return VO_ERR_DRM2_BASE;
	}

	*uSize = iRetSize;

	return VO_ERR_DRM2_OK;
}


ViewRightWebClient::VRWebClientError_t CDRM_Verimatrix_AES128::CheckDeviceVCASStatus(void)
{
        if(b_init)
                return ViewRightWebClient::VR_Success;
	m_bExit = VO_FALSE;
        VOLOGI("+cb");
        int time_before = voOS_GetSysTime();
        ViewRightWebClient::VRWebClientError_t commErr;
        m_cbVerify.HTTP_Callback(m_cbVerify.hHandle, VO_SOURCE2_CALLBACKIDBASE_DRM, NULL);
        VOLOGI("-cb m_szVCASBootAddress %s", m_szVCASBootAddress);

        VOLOGI("current m_szPackPath is %s",m_szPackPath);

#ifdef _VOLOG_INFO
	const char* pVersion = ViewRightWebClient::GetInstance()->GetVersion();
	VOLOGI("VR version: %s", pVersion);
#endif //_VOLOG_INFO

#ifdef _LINUX_ANDROID
        unsigned char szPassWord[32] = {0};
        strcpy((char*)szPassWord, "b3sT4tt3Mp7@S3cuR!7Y");
        obfuscate_pw((char*)szPassWord);
        VOLOGI("%s", m_szID);
        commErr = m_pVRWClient->SetUniqueIdentifier(m_szID, (char*)szPassWord);
        VOLOGI("2");
        if (ViewRightWebClient::VR_Success != commErr)
        {
                VOLOGE("SetUniqueIdentifier %d", commErr);
                return commErr;
        }

        VOLOGI("3");
        obfuscate_pw2((char*)szPassWord);
        VOLOGI("4");
        commErr = m_pVRWClient->SetUniqueIdentifier(m_szID, (char*)szPassWord);
        if (ViewRightWebClient::VR_Success != commErr)
        {
                VOLOGE("SetUniqueIdentifier %d", commErr);
                return commErr;
        }
        VOLOGI("5");
#endif

#ifdef _LINUX_ANDROID
        VO_CHAR *p = strstr(m_szPackPath + strlen(m_szPackPath) - 7, "/lib");
        if (!p)
                return ViewRightWebClient::BadStoreIntegrity;

        p++;
        VO_CHAR szStoreDir[512] = {0};
        strncpy(szStoreDir, m_szPackPath, p - m_szPackPath);
#endif

        /*
        memset(m_szVCASBootAddress,0,64);
        char * path = "public-ott-nodrm.verimatrix.com:80";
        strncpy(m_szVCASBootAddress,path,strlen(path));
        */
#ifdef _MAC_OS
        VOLOGI("current store path %s bootaddress %s",m_szPackPath,m_szVCASBootAddress);
        ViewRightWebClient::GetInstance()->SetVCASCommunicationHandlerSettings(m_szVCASBootAddress, m_szPackPath);
        VOLOGI("SetVCASCommunicationHandlerSettings %d", commErr);
#endif

#ifdef _LINUX_ANDROID
        VOLOGI("current store path %s bootaddress %s",szStoreDir,m_szVCASBootAddress);
        ViewRightWebClient::GetInstance()->SetVCASCommunicationHandlerSettings(m_szVCASBootAddress, szStoreDir);
        VOLOGI("SetVCASCommunicationHandlerSettings %d", commErr);
#endif

#ifdef _VOLOG_INFO
        ViewRightWebClient::GetInstance()->SetLogging(true);
#endif //_VOLOG_INFO

        commErr = ViewRightWebClient::GetInstance()->IsDeviceProvisioned();
        VOLOGI("IsDeviceProvisioned %d", commErr);
        if ( commErr == ViewRightWebClient::VR_Success )
        {
                // Device already provisioned - Check the connection
                commErr = ViewRightWebClient::GetInstance()->CheckVCASConnection();
                VOLOGI("CheckVCASConnection %d", commErr);
                if ( commErr != ViewRightWebClient::CertificateExpired )
                {
                        // We either succeeded or failed due to a connection problem - return error code and see what to do
                        // Typically if this happens, you may want to re-provision by calling ConnectAndProvisionDevice
                        // unless this error was NoConnect or a network related issue. return (int) commErr;
                }
        }
        // At this point the device is either not provisioned or has an expired certificate¡­
        // So, we need to provision¡­
        commErr = ViewRightWebClient::GetInstance()->ConnectAndProvisionDevice();
        VOLOGI("ConnectAndProvisionDevice %d", commErr);
        if ( commErr != ViewRightWebClient::VR_Success )
        {
                // Device did not provision successfully¡­ return the error code to determine what to do next
                return commErr;
        }
        // Provisioning succeeded - Check the connection
        commErr = ViewRightWebClient::GetInstance()->CheckVCASConnection();
        if(commErr == ViewRightWebClient::VR_Success)
                b_init = true;
        VOLOGI("CheckVCASConnection %d", commErr);
        int time_after = voOS_GetSysTime();
        VOLOGI("Init Drm server cost time %d",time_after - time_before);
        return commErr;
}

CDRM_Verimatrix_AES128::CDRM_Verimatrix_AES128(void)
        : m_pVRWClient(NULL)
        , m_eContent(CONTENT_LINK_NEW_SEVER)
{
        VOLOGI("CDRM_Verimatrix_AES128");

        memset(m_szID, 0, 64);
        memset(m_szVCASBootAddress, 0, 64);
        b_init = false;
        ViewRightWebClient::VRWebClientError_t commErr = ViewRightWebClient::GetInstance()->InitializeSSL();
        VOLOGI("InitializeSSL %d current m_szVCASBootAddress is %s", commErr,m_szVCASBootAddress);

        VO_BYTE bufHandshake[1024] = {0};
        VO_U32 uSizeHandshake = 0;
        VO_U32 uRet = encryptHandshakeString(bufHandshake, &uSizeHandshake);
        if (uRet)
        {
                VOLOGE("encryptHandshakeString %d", uRet);
                //return ViewRightWebClient::GeneralError;
        }

        VOLOGI("before VerifyHandshake");
        commErr = ViewRightWebClient::GetInstance()->VerifyHandshake(bufHandshake, uSizeHandshake);
        m_pVRWClient = ViewRightWebClient::GetInstance();
        VOLOGI("after VerifyHandshake %d",commErr);
        VOLOGI("VerifyHandshake %d size %d", commErr, uSizeHandshake);
        char* pUID = NULL;
        int iUID = 0;
        commErr = ViewRightWebClient::GetInstance()->GetUniqueIdentifier(&pUID, &iUID);
        VOLOGI("------------------------------------GetUniqueIdentifier %d %s %d", commErr, pUID, iUID);
        strncpy(m_szID,pUID,iUID);
        VOLOGI("m_szID is %s",m_szID);
}

CDRM_Verimatrix_AES128::~CDRM_Verimatrix_AES128(void)
{
}


VO_U32 CDRM_Verimatrix_AES128::Init(VO_DRM_OPENPARAM *pParam)
{
        strcpy(m_drmCallback.szDRMTYPE, "CDRM_Verimatrix_AES128");

        return CDRM::Init(pParam);
}

VO_U32 CDRM_Verimatrix_AES128::Uninit()
{
        if (m_pVRWClient)
        {
                delete m_pVRWClient;
                m_pVRWClient = NULL;
        }

        return VO_ERR_DRM2_OK;
}

VO_U32 CDRM_Verimatrix_AES128::GetParameter(VO_U32 uID, VO_PTR pParam)
{
        switch (uID)
        {
        case VO_PID_DRM2_UNIQUE_IDENTIFIER:
        {
                if (NULL == pParam)
                {
                        VOLOGE("empty pointor");
                        return VO_ERR_DRM2_BADPARAMETER;
                }

                VOLOGI("VO_PID_DRM2_UNIQUE_IDENTIFIER %s",m_szID);

                *(VO_PCHAR** )pParam = (VO_PCHAR *)m_szID;
        }
        break;

        default:
                return VO_ERR_NOT_IMPLEMENT;
        }

        return VO_ERR_DRM2_OK;
}


VO_U32 CDRM_Verimatrix_AES128::SetParameter(VO_U32 uID, VO_PTR pParam)
{
        VOLOGI("uID %x", uID);

        switch (uID)
        {

        case VO_PID_DRM2_UNIQUE_IDENTIFIER:
        {
                if (NULL == pParam)
                {
                        VOLOGE("VO_PID_DRM2_UNIQUE_IDENTIFIER NULL");
                        return VO_ERR_DRM2_BADPARAMETER;
                }

                VO_CHAR* pInfo = (VO_CHAR *)pParam;

                VOLOGR("%s", pInfo);

                strcpy(m_szID, pInfo);

                return VO_ERR_DRM2_OK;
        }
        break;
        case VO_PID_SOURCE2_DOHTTPVERIFICATION:
        {
                if (NULL == pParam)
                {
                        VOLOGE("VO_PID_SOURCE2_DOHTTPVERIFICATION NULL");
                        return VO_ERR_DRM2_BADPARAMETER;
                }

                VO_SOURCE2_VERIFICATIONINFO* pInfo = (VO_SOURCE2_VERIFICATIONINFO *)pParam;

                VOLOGI("Size %d", pInfo->uDataSize);
                if (pInfo->uDataSize > 0 && strncmp(m_szVCASBootAddress, (char*)(pInfo->pData), pInfo->uDataSize) != 0)
                {
                        strncpy(m_szVCASBootAddress, (char*)(pInfo->pData), pInfo->uDataSize);
                        VOLOGI("%s", m_szVCASBootAddress);

                        m_eContent = CONTENT_LINK_NEW_SEVER;
                }

                return VO_ERR_NONE;
        }
        break;

        default:
                break;;
        }

        return CDRM::SetParameter(uID, pParam);
}

VO_U32 CDRM_Verimatrix_AES128::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
        switch (nFlag)
        {
        case VO_DRM_FLAG_DRMINFO:
        {
                VOLOGI(" ------------VO_DRM_FLAG_DRMINFO:");
                if (VO_DRM2SRC_MEDIAFORMAT == eSourceFormat || VO_DRM2SRC_RAWFILE == eSourceFormat)
                {
                        VOLOGW("!DRM Module");
                        return VO_ERR_DRM2_MODULENOTFOUND;
                }

                if(!b_init)
                {
                        ViewRightWebClient::VRWebClientError_t eRet = CheckDeviceVCASStatus();
                        VOLOGI("CheckDeviceVCASStatus %d", eRet);
                }

                memset(&m_DCI, 0, sizeof(Chunk_DRM_INFO) * 3);

                m_DCI[0].uIdentifer = 0xFFFFFFFF;
                m_DCI[1].uIdentifer = 0xFFFFFFFF;
                m_DCI[2].uIdentifer = 0xFFFFFFFF;
        }
        break;

        case VO_DRM_FLAG_DRMDATA:
        {
                VOLOGI(" ------------VO_DRM_FLAG_DRMDATA:");
                if (NULL == pParam)
                {
                        VOLOGE("VO_DRM2_DATA is NULL");
                        return VO_ERR_DRM2_BADPARAMETER;
                }

                voCAutoLock lock(&m_lock);

                VO_DRM2_DATA * pDRMData = static_cast<VO_DRM2_DATA *>(pParam);
                switch (pDRMData->sDataInfo.nDataType)
                {
                case VO_DRM2DATATYPE_CHUNK_BEGIN:
                {
                        VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pDRMData->sDataInfo.pInfo);
                        if (NULL == pDRMHLSProcess)
                        {
                                VOLOGE("VO_DRM2_HSL_PROCESS_INFO is NULL");
                                return VO_ERR_DRM2_BADPARAMETER;
                        }

                        VOLOGI("szCurURL %s", pDRMHLSProcess->szCurURL);
                        VOLOGI("szKeyString %s", pDRMHLSProcess->szKeyString);
                        VOLOGI("uSequenceNum %d", pDRMHLSProcess->uSequenceNum);

                        VO_U32 uIndex = 0xFFFFFFFF;
                        for (int i = 0; i < 3; i++)
                        {
                                if (m_DCI[i].uIdentifer == 0xFFFFFFFF)
                                {
                                        uIndex = i;
                                        break;
                                }
                        }

                        if (0xFFFFFFFF == uIndex)
                        {
                                VOLOGE("No chunk info");
                                return VO_ERR_DRM2_ERRORSTATE;
                        }

                        m_DCI[uIndex].uIdentifer = pDRMData->nReserved[2];

                        memset(m_DCI[uIndex].szKey, 0, 2048);
                        memset(m_DCI[uIndex].IV, 0, 16);

                        VO_U32 uRet = getKeyURI_HLS(pDRMHLSProcess->szCurURL, pDRMHLSProcess->szKeyString, m_DCI[uIndex].szKey, NULL);
                        if (0 != uRet)
                        {
                                VOLOGE("!getKeyURI 0x%x", uRet);
                                return uRet;
                        }

                        if ( !IsEncrypted() )
                                break;

                        uRet = getIV_HLS(pDRMHLSProcess->szKeyString, pDRMHLSProcess->uSequenceNum, m_DCI[uIndex].IV, NULL);
                        if (0 != uRet)
                        {
                                VOLOGE("!getIV 0x%x", uRet);
                                return uRet;
                        }
                }
                break;

                case VO_DRM2DATATYPE_CHUNK_PROCESSING:
                {
                        VOLOGI(" ------------VO_DRM2DATATYPE_CHUNK_PROCESSING:");
                        VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pDRMData->sDataInfo.pInfo);
                        if (NULL == pDRMHLSProcess)
                        {
                                VOLOGE("VO_DRM2_HSL_PROCESS_INFO is NULL");
                                return VO_ERR_DRM2_BADPARAMETER;
                        }

                        if (m_bExit)
                        {
                                VOLOGW("exiting");
                                return VO_ERR_DRM2_ERRORSTATE;
                        }

                        VO_U32 uIndex = 0xFFFFFFFF;
                        for (int i = 0; i < 3; i++)
                        {
                                if (pDRMData->nReserved[2] == m_DCI[i].uIdentifer)
                                {
                                        uIndex = i;
                                        break;
                                }
                        }

                        unsigned char IV[16] = {0};
                        if (pDRMData->nSize >= 16)
                                memcpy(IV, pDRMData->pData + pDRMData->nSize - 16, 16);

                        //VOLOGI("IV 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", m_DCI[uIndex].IV[0], m_DCI[uIndex].IV[1], m_DCI[uIndex].IV[2], m_DCI[uIndex].IV[3], m_DCI[uIndex].IV[4], m_DCI[uIndex].IV[5], m_DCI[uIndex].IV[6], m_DCI[uIndex].IV[7], m_DCI[uIndex].IV[8], m_DCI[uIndex].IV[9], m_DCI[uIndex].IV[10], m_DCI[uIndex].IV[11], m_DCI[uIndex].IV[12], m_DCI[uIndex].IV[13], m_DCI[uIndex].IV[14], m_DCI[uIndex].IV[15]);

                        unsigned long ulDes = pDRMData->nSize;
                        int len = strlen(m_DCI[uIndex].szKey);
                        VOLOGI("before decrypt");
                        if(len == 0)
                        {
                                VOLOGI("clear stream");
                                break;
                        }
                        if( b_init == false && len != 0)
                        {
                                VOLOGI("server init failed return");
                                return VO_ERR_DRM2_BADPARAMETER;
                        }
                        int time_before = voOS_GetSysTime();
                        ViewRightWebClient::VRWebClientError_t err = ViewRightWebClient::GetInstance()->Decrypt(pDRMData->pData, ulDes, strstr(pDRMHLSProcess->szKeyString, "METHOD=AES-128") ? "AES-128" : "NONE", m_DCI[uIndex].szKey, m_DCI[uIndex].IV);
                        int time_after = voOS_GetSysTime();
                        VOLOGI("after decrypt cost %d",time_after - time_before);
                        //sprintf(sz, "%s %d VerifyHandshake %d\n", __FUNCTION__, __LINE__, eRet);
                        if(err != ViewRightWebClient::VR_Success && err != ViewRightWebClient::SecurityError)
                        {
                                VOLOGE("Decrypt %d", err);
                                return VO_ERR_DRM2_BADPARAMETER;
                        }
                        //VOLOGI("pData %p, nSize %d, dS %d", pDRMData->pData, pDRMData->nSize, ulDes);

                        *pDRMData->pnDstSize = ulDes;

                        memcpy(m_DCI[uIndex].IV, IV, 16);
                }
                break;

                case VO_DRM2DATATYPE_CHUNK_END:
                {
                        for (int i = 0; i < 3; i++)
                        {
                                if (pDRMData->nReserved[2] == m_DCI[i].uIdentifer)
                                {
                                        m_DCI[i].uIdentifer = 0xFFFFFFFF;
                                        break;
                                }
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

