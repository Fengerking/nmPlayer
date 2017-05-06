#include <Shlobj.h>
#include <iepmapi.h>
#include "DRM_Verimatrix_AES128_Win.h"
#include "openssl/rsa.h"
#include "openssl/engine.h"
#include "openssl/ssl.h"
#include "AES_CBC.h"
#include "voHLSDRM.h"
#include "voOSFunc.h"
#include "voLog.h"

#pragma comment(lib, "Iepmapi.lib")

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CDRM_Verimatrix_AES128::CDRM_Verimatrix_AES128(void)
: m_pVRWClient(NULL)
, m_eContent(CONTENT_LINK_NEW_SEVER)
{
	VOLOGI("CDRM_Verimatrix_AES128");
	m_bINIT = 1;

	memset(m_szID, 0, 64);
	memset(m_szVCASBootAddress, 0, 64);

	b_init = 0;
}

CDRM_Verimatrix_AES128::~CDRM_Verimatrix_AES128(void)
{
}

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



int CheckEncrypted_HLS(char* pKeyString)
{
	if (NULL == pKeyString || strlen(pKeyString) == 0)
	{
		return VO_OSMP_DRM_ERR_UNENCRYPT;//clr
	}

	VO_CHAR * p = strstr(pKeyString, "METHOD=");
	if (NULL == p)
	{
		VOLOGE("NO METHOD=");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	p += strlen("METHOD=");
	if (memcmp(p, "NONE", strlen("NONE") ) == 0)
	{
		return VO_OSMP_DRM_ERR_UNENCRYPT;//clr
	}
	else if (memcmp(p, "AES-128", strlen("AES-128") ) != 0)
	{
		VOLOGE("NOT AES-128");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	return VO_OSMP_DRM_ERR_NONE;
}


VO_U32 getKeyURI_HLS(VO_CHAR* pCurURL, VO_CHAR* pKeyString, VO_CHAR* pKeyURI, VO_PTR pReserved)
{
	if (NULL == pKeyString || strlen(pKeyString) == 0)
		return VO_OSMP_DRM_ERR_NONE;

	VO_CHAR * p = strstr(pKeyString, "METHOD=");
	if (NULL == p)
	{
		VOLOGE("NO METHOD=");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	p += strlen("METHOD=");
	if (memcmp(p, "NONE", strlen("NONE") ) == 0)
	{
		return VO_OSMP_DRM_ERR_UNENCRYPT;
	}
	else if (memcmp(p, "AES-128", strlen("AES-128") ) != 0)
	{
		VOLOGE("NOT AES-128");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	//m_bEncrypted = VO_TRUE;

	p = strstr(pKeyString , "URI=");
	if (NULL == p)
	{
		VOLOGE("NO URI=");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}
	p += strlen("URI=\"");

	VO_CHAR* pEnd = strchr(p, '\"');

	VO_S32 iLen = 0;
	if (pEnd)
		iLen = pEnd - p;
	else
		iLen = strlen(p);

	VO_CHAR szURL[2048] = {0};
	memcpy(szURL, p, iLen);

	VO_U32 uRet = GetTheAbsolutePath(pKeyURI, szURL, pCurURL);
	if (0 != uRet)
	{
		VOLOGE("!getKeyURI 0x%x", uRet);
		return uRet;
	}

	return VO_OSMP_DRM_ERR_NONE;
}

VO_VOID str2IV_HLS(VO_PBYTE ptr_key , VO_CHAR * pStr)
{
	if (NULL == ptr_key || NULL == pStr)
	{
		VOLOGE("empty pointor");
		return;
	}

	VO_CHAR szIV[33];
	memset(szIV, '0', 33);

	VO_CHAR *p = szIV + 32 - strlen(pStr);
	strcpy(p, pStr);

	for (VO_S32 i = 0; i < 16; i++)
	{
		VO_CHAR temp[3];

		temp[0] = szIV[2*i];
		temp[1] = szIV[2*i + 1];
		temp[2] = '\0';

		int value;

		sscanf(temp, "%x", &value);

		ptr_key[i] = value;
	}
}

VO_U32 getIV_HLS(VO_CHAR* pKeyString, VO_U32 uSequenceNum, VO_BYTE* pIV, VO_PTR pReserved)
{
	if (NULL == pKeyString || NULL == pIV)
	{
		VOLOGE("empty pointor");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	VO_CHAR * pBuffer = strstr(pKeyString , "IV=");

	VO_CHAR Buffer[1024] = {0};
	if (pBuffer)
	{
		pBuffer += strlen( "IV=" );
		VO_CHAR * pEnd = strchr(pBuffer, ',');
		if (pEnd)
		{
			strncpy(Buffer, pBuffer, pEnd - pBuffer);
		}
		else
		{
			strcpy(Buffer, pBuffer);
		}

		pBuffer = Buffer;
	}

	if (pBuffer)
	{
		if ( '0' == pBuffer[0] &&
			('x' == pBuffer[1] || 'X' == pBuffer[1]) )
		{
			pBuffer += strlen("0x");

			str2IV_HLS(pIV, pBuffer);
		}
		else
		{
			strcpy( (char *)pIV, pBuffer );
		}
	}
	else
	{
		pIV[15] = (VO_BYTE)((uSequenceNum)&0x000000ff);
		pIV[14] = (VO_BYTE)(((uSequenceNum)>>8)&0x000000ff);
		pIV[13] = (VO_BYTE)(((uSequenceNum)>>16)&0x000000ff);
		pIV[12] = (VO_BYTE)(((uSequenceNum)>>24)&0x000000ff);
	}

	return VO_OSMP_DRM_ERR_NONE;
}

int CDRM_Verimatrix_AES128::processURI(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcURI, char* pDesURI, int* piDesURI)
{
	voCAutoLock lock(&m_lock);
	voCAutoLock s_lock(&m_lock_shake);
	VOLOGI("here into process uri %s",szSrcURI);
	memset(&m_DCI, 0, sizeof(Chunk_DRM_INFO) * 3);

	m_DCI[0].uIdentifer = 0xFFFFFFFF;
	m_DCI[1].uIdentifer = 0xFFFFFFFF;
	m_DCI[2].uIdentifer = 0xFFFFFFFF;

#ifdef _VOLOG_INFO
	const char* pVersion = ViewRightWebClient::GetInstance()->GetVersion();
	VOLOGI("VR version: %s", pVersion);
#endif //_VOLOG_INFO

	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;

}

int CDRM_Verimatrix_AES128::GetUDID()
{
	VOLOGI("here into get udid ");
	ViewRightWebClient::VRWebClientError_t commErr = ViewRightWebClient::GetInstance()->InitializeSSL();
	VOLOGI("InitializeSSL %d current m_szVCASBootAddress is %s", commErr,m_szVCASBootAddress);

	VO_BYTE bufHandshake[1024] = {0};
	VO_U32 uSizeHandshake = 0;
	VO_U32 uRet = encryptHandshakeString(bufHandshake, &uSizeHandshake);
	if (uRet)
	{
		VOLOGE("encryptHandshakeString %d", uRet);
		return VO_OSMP_DRM_ERR_STATUS;
	}

	VOLOGI("before VerifyHandshake");
	commErr = ViewRightWebClient::GetInstance()->VerifyHandshake(bufHandshake, uSizeHandshake);
	VOLOGI("after VerifyHandshake %d",commErr);
	VOLOGI("VerifyHandshake %d size %d", commErr, uSizeHandshake);
	char* pUID = NULL;
	int iUID = 0;
	commErr = ViewRightWebClient::GetInstance()->GetUniqueIdentifier(&pUID, &iUID);
	VOLOGI("------------------------------------GetUniqueIdentifier %d %s %d", commErr, pUID, iUID);
	strncpy(m_szID,pUID,iUID);
	VOLOGI("m_szID is %s",m_szID);
	return VO_OSMP_DRM_ERR_NONE;
}

ViewRightWebClient::VRWebClientError_t CDRM_Verimatrix_AES128::CheckDeviceVCASStatus(void)
{
	voCAutoLock s_lock(&m_lock_shake);
	ViewRightWebClient::VRWebClientError_t commErr = ViewRightWebClient::VR_Success;
	b_init = 1;

	PWSTR pSzFolderPath = NULL;
	HRESULT result = IEGetWriteableFolderPath(FOLDERID_InternetCache, &pSzFolderPath);
	if (S_OK != result)
	{
		VOLOGW("IEGetWriteableFolderPath %ld", result);

		result = SHGetKnownFolderPath(FOLDERID_InternetCache, 0, NULL, &pSzFolderPath);
		if (S_OK != result)
		{
			VOLOGE("FOLDERID_InternetCache %ld", result);
			return ViewRightWebClient::BadWriteStore;
		}
	}

	VO_TCHAR szWPath[2048] = {0};
	vostrcat(szWPath, pSzFolderPath);
	CoTaskMemFree(pSzFolderPath);

	vostrcat(szWPath, TEXT("\\"));

	VOLOGI("before set communication handler setting m_szVCASBootAddress %s, pPackPath %ls", m_szVCASBootAddress, szWPath);
	commErr = ViewRightWebClient::GetInstance()->SetVCASCommunicationHandlerSettings(m_szVCASBootAddress, szWPath);
	VOLOGI("SetVCASCommunicationHandlerSettings %d", commErr);

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
	// At this point the device is either not provisioned or has an expired certificate?-
	// So, we need to provision?-
	commErr = ViewRightWebClient::GetInstance()->ConnectAndProvisionDevice();
	VOLOGI("ConnectAndProvisionDevice %d", commErr);
	if ( commErr != ViewRightWebClient::VR_Success )
	{
		b_init = 3;
		// Device did not provision successfully?- return the error code to determine what to do next
		return commErr;
	}
	// Provisioning succeeded - Check the connection
	commErr = ViewRightWebClient::GetInstance()->CheckVCASConnection();
	if(commErr == ViewRightWebClient::VR_Success)
		b_init = 2;
	else 
		b_init = 3;
	VOLOGI("CheckVCASConnection %d", commErr);
	return commErr;
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

			VOLOGI("%s", pInfo);

			//strcpy(m_szID, pInfo);
			//m_szID[strlen(m_szID) - 1] = 0;
			strcpy(m_szID, "0800270078EC");

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

			if(pInfo->uDataSize > 0)
			{
				if(b_init == 0)
				{
					VOLOGI("never do init go to init");
					memset(m_szVCASBootAddress,0,64);
					strncpy(m_szVCASBootAddress, (char*)(pInfo->pData), pInfo->uDataSize);
					VOLOGI("%s   data length %d", m_szVCASBootAddress,pInfo->uDataSize);
					CheckDeviceVCASStatus();
					return VO_ERR_NONE;
				}						
				if(b_init == 1)
				{
					VOLOGI("current status is in init return without do anything");
					return VO_ERR_NONE;
				}
				VOLOGI("before server %s size %d",m_szVCASBootAddress,strlen(m_szVCASBootAddress));
				if(strcmp(m_szVCASBootAddress, (char*)(pInfo->pData)) != 0 )
				{
					memset(m_szVCASBootAddress,0,64);
					int size = pInfo->uDataSize;
					if (size > 64)
						size = 64;
					strncpy(m_szVCASBootAddress, (char*)(pInfo->pData), size);
					VOLOGI("%s   data length %d", m_szVCASBootAddress,pInfo->uDataSize);
					VOLOGI("current server is different from before ");
					CheckDeviceVCASStatus();
					return VO_ERR_NONE;

				}
				else
				{
					if(b_init == 2)
					{
						VOLOGI("the server is the same and already init success");
						return VO_ERR_NONE;
					}
					if(b_init == 3)
					{
						VOLOGI("the server is the same but before init failed ->  to init");
						CheckDeviceVCASStatus();
						return VO_ERR_NONE;
					}
				}
			}
			return VO_ERR_NONE;
		}
		break;
	case VO_PID_DRM2_PackagePath:
		{
			if (pParam)
			{
				VOLOGR("%s", pParam);

				VO_CHAR	szPackPath[1024];

				strcpy(szPackPath, (char*)pParam);

				size_t newsize = strlen(szPackPath) + 1;

				wchar_t * wcstring = new wchar_t[newsize];

				size_t convertedChars = 0;
				mbstowcs_s(&convertedChars, wcstring, newsize, szPackPath, _TRUNCATE);

				VOLOGINIT(wcstring);

				delete[] wcstring;
			}
		}

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}
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


int CDRM_Verimatrix_AES128::beginSegment(int identifer, void* pAdditionalInfo)
{
	voCAutoLock lock(&m_lock);
	voCAutoLock s_lock(&m_lock_shake);

	VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pAdditionalInfo);
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

	if (0xFFFFFFFF == uIndex || 0xFFFFFFFF == identifer)
	{
		VOLOGE("No chunk info");
		return VO_ERR_DRM2_ERRORSTATE;
	}


	m_DCI[uIndex].uIdentifer = identifer;
	VOLOGI("Current identifer is %d",identifer);

	memset(m_DCI[uIndex].szKey, 0, 2048);
	memset(m_DCI[uIndex].IV, 0, 16);
	VO_U32 uRet = getKeyURI_HLS(pDRMHLSProcess->szCurURL, pDRMHLSProcess->szKeyString, m_DCI[uIndex].szKey, NULL);
	if (0 != uRet && uRet != VO_OSMP_DRM_ERR_UNENCRYPT)
	{
		VOLOGE("!getKeyURI 0x%x", uRet);
		return uRet;
	}


	uRet = getIV_HLS(pDRMHLSProcess->szKeyString, pDRMHLSProcess->uSequenceNum, m_DCI[uIndex].IV, NULL);
	if (0 != uRet)
	{
		VOLOGE("!getIV 0x%x", uRet);
		return uRet;
	}
	return VO_ERR_DRM2_OK;

}


int CDRM_Verimatrix_AES128::decryptSegment(int identifer, long long llPosition, unsigned char* pData, int* piSize, bool bLastBlock, void* pAdditionalInfo)
{
	VOLOGI("here into derrypto segment");
	voCAutoLock lock(&m_lock);
	voCAutoLock s_lock(&m_lock_shake);

	VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pAdditionalInfo);
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
		if (identifer == m_DCI[i].uIdentifer)
		{
			uIndex = i;
			break;
		}
	}
	VOLOGI("current identifer is %d  index %d",identifer,uIndex);
	VOLOGI("Current key %s",m_DCI[uIndex].szKey);

	unsigned char IV[16] = {0};
	if (*piSize >= 16)
		memcpy(IV, pData + *piSize - 16, 16);

	//VOLOGI("IV 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", m_DCI[uIndex].IV[0], m_DCI[uIndex].IV[1], m_DCI[uIndex].IV[2], m_DCI[uIndex].IV[3], m_DCI[uIndex].IV[4], m_DCI[uIndex].IV[5], m_DCI[uIndex].IV[6], m_DCI[uIndex].IV[7], m_DCI[uIndex].IV[8], m_DCI[uIndex].IV[9], m_DCI[uIndex].IV[10], m_DCI[uIndex].IV[11], m_DCI[uIndex].IV[12], m_DCI[uIndex].IV[13], m_DCI[uIndex].IV[14], m_DCI[uIndex].IV[15]);

	unsigned long ulDes = *piSize;
	VOLOGI("current decrypt status is %d",b_init);
	if( b_init !=2 )
	{
		int len = strlen(m_DCI[uIndex].szKey);
		VOLOGI("current key len is %d",len);
		if(!len)
			return VO_ERR_DRM2_OK;
		else
			return VO_ERR_DRM2_BADPARAMETER;
	}

	ViewRightWebClient::VRWebClientError_t err = ViewRightWebClient::VR_Success;
	// VOLOGI("current key %s iv %d",m_DCI[uIndex].szKey, m_DCI[uIndex].IV);
	err = ViewRightWebClient::GetInstance()->Decrypt(pData, ulDes, strstr(pDRMHLSProcess->szKeyString, "METHOD=AES-128") ? "AES-128" : "NONE", m_DCI[uIndex].szKey, m_DCI[uIndex].IV);
	VOLOGI("current decrypt %d",err);
	//sprintf(sz, "%s %d VerifyHandshake %d\n", __FUNCTION__, __LINE__, eRet);
	if(err != ViewRightWebClient::VR_Success)
	{
		VOLOGE("Decrypt %d", err);
		return VO_ERR_DRM2_BADPARAMETER;
	}
	//        VOLOGI("pData %p, nSize %d, dS %d", pDRMData->pData, pDRMData->nSize, ulDes);

	*piSize = ulDes;

	memcpy(m_DCI[uIndex].IV, IV, 16);
	return VO_ERR_DRM2_OK;
}

int CDRM_Verimatrix_AES128::decryptSegment(int identifer, long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo)
{
	voCAutoLock s_lock(&m_lock_shake);
	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
}

int CDRM_Verimatrix_AES128::endSegment(int identifer)
{

	voCAutoLock lock(&m_lock);
	voCAutoLock s_lock(&m_lock_shake);

	for (int i = 0; i < 3; i++)
	{
		if (identifer == m_DCI[i].uIdentifer)
		{
			m_DCI[i].uIdentifer = 0xFFFFFFFF;
			break;
		}
	}

	return VO_ERR_DRM2_OK;
}
