#include "CWMDRM.h"

//#include <drmsal.h>
#include <drmcommon.h>
#include <drmtypes.h>
#include <drmhds.h>
#include <drmmanager.h>

#include <stdio.h>
#include <tchar.h>
#include <memory.h>

#define STORE_FILE	L"C:\\wmdrmpd\\VDRM.hds"

#define CHECK_WMDRM_STATE	if(!m_pDrmContext)\
	return VO_ERR_DRM_ERRORSTATE;

#define CHECK_WMDRM_STATE_2	if(!m_pDecryptContext)\
	return VO_ERR_DRM_ERRORSTATE;

DRM_RESULT DRMOutputLevelsCallback(IN const DRM_VOID* f_pvOutputLevelsData, IN DRM_DWORD f_dwCallbackType, IN const DRM_VOID* f_pv)
{
	CWMDRM* pDrm = (CWMDRM*)f_pv;

	return (DRM_RESULT)pDrm->OutputLevelsCallback((const VO_PTR)f_pvOutputLevelsData, f_dwCallbackType);
}

CWMDRM::CWMDRM()
	: m_pDrmContext(NULL)
	, m_pDecryptContext(NULL)
{
}

CWMDRM::~CWMDRM()
{
	Close();
}

VO_U32 CWMDRM::Open()
{
	Close();

	DRM_CONST_STRING drmStoreFile;
	drmStoreFile.pwszString = (const DRM_WCHAR *)STORE_FILE;
	drmStoreFile.cchString = wcslen(STORE_FILE);

	m_pDrmContext = new DRM_MANAGER_CONTEXT;
	if(!m_pDrmContext)
		return VO_ERR_OUTOF_MEMORY;

	memset(m_pDrmContext, 0, sizeof(DRM_MANAGER_CONTEXT));

	DRM_RESULT result = DRM_MGR_Initialize(m_pDrmContext, &drmStoreFile);
	if(DRM_SUCCESS != result)
	{
		delete m_pDrmContext;
		m_pDrmContext = NULL;

		return result;
	}

	return VO_ERR_DRM_OK;
}

VO_U32 CWMDRM::Close()
{
	if(m_pDecryptContext)
	{
		delete m_pDecryptContext;
		m_pDecryptContext = NULL;
	}

	if(m_pDrmContext)
	{
		DRM_MGR_Uninitialize(m_pDrmContext);

		delete m_pDrmContext;
		m_pDrmContext = NULL;
	}

	return VO_ERR_DRM_OK;
}

VO_U32 CWMDRM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CWMDRM::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CWMDRM::SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo)
{
	CHECK_WMDRM_STATE

	VO_WMDRM_INFO* pInfo = (VO_WMDRM_INFO*)pDrmInfo;
	if(!pInfo || (!pInfo->pContentEncryptionObject && !pInfo->pExtendedContentEncryptionObject))
		return VO_ERR_DRM_BADPARAMETER;

	//we will use extended content encryption object if it is occur in file
	if(pInfo->pExtendedContentEncryptionObject)
	{
		DRM_RESULT result = DRM_MGR_SetV2Header(m_pDrmContext, pInfo->pExtendedContentEncryptionObject, pInfo->nExtendedContentEncryptionObject);
		if(DRM_SUCCESS != result)
			return result;
	}
	else
	{	
		VO_PBYTE p = pInfo->pContentEncryptionObject;

		VO_U32 cbSecretData;
		memcpy(&cbSecretData, p, 4);
		p += 4;

		VO_BYTE pbSecretData[64];
		memcpy(pbSecretData, p, cbSecretData);
		p += cbSecretData;

		// Protection Type Length
		VO_U32 cbProtectionType;
		memcpy(&cbProtectionType, p, 4);
		p += 4;
		p += cbProtectionType;

		VO_U32 cbKeyID;
		memcpy(&cbKeyID, p, 4);
		p += 4;

		VO_BYTE pbKeyID[64];
		memcpy(pbKeyID, p, cbKeyID);
		p += cbKeyID;

		VO_U32 cbURL;
		memcpy(&cbURL, p, 4);
		p += 4;

		VO_BYTE pbURL[512];
		memcpy(pbURL, p, cbURL);
		p += cbURL;

		DRM_RESULT result = DRM_MGR_SetV1Header(m_pDrmContext, pbKeyID, cbKeyID, pbSecretData, cbSecretData, pbURL, cbURL);
		if(DRM_SUCCESS != result)
			return result;
	}

	return CheckLicense();
}

VO_U32 CWMDRM::GetDrmFormat(VO_DRM_FORMAT* pDrmFormat)
{
	return VO_ERR_DRM_OK;
}

VO_U32 CWMDRM::Commit()
{
	CHECK_WMDRM_STATE

	DRM_RESULT result = DRM_MGR_Commit(m_pDrmContext);
	return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
}

VO_U32 CWMDRM::DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize)
{
	CHECK_WMDRM_STATE_2

	//if size less than 15, we regard it as not encrypted data
	//for example, padding data
	//East, 2010/04/08
	if(nSize < 15)
		return VO_ERR_DRM_OK;

	DRM_RESULT result = DRM_MGR_InitDecrypt(m_pDecryptContext, pData + nSize - 15, nSize);
	if(DRM_SUCCESS != result)
		return result;

	result = DRM_MGR_Decrypt(m_pDecryptContext, pData, nSize);
	if(DRM_SUCCESS != result)
		return result;

	return VO_ERR_DRM_OK;
}

VO_U32 CWMDRM::DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CWMDRM::CheckLicense()
{
	#define REQ_RIGHT g_dstrWMDRM_RIGHT_PLAYBACK
//	#define REQ_RIGHT g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY
//	#define REQ_RIGHT g_dstrWMDRM_RIGHT_COPY_TO_CD

	CHECK_WMDRM_STATE

	if(m_pDecryptContext)
	{
		delete m_pDecryptContext;
		m_pDecryptContext = NULL;
	}

	const DRM_CONST_STRING* rgpdstrRights[] = { &REQ_RIGHT };
	DRM_LICENSE_STATE_DATA rgStateData[NO_OF(rgpdstrRights)];
	DRM_RESULT result = DRM_MGR_GetLicenseData(m_pDrmContext, rgpdstrRights, rgStateData, NO_OF(rgpdstrRights));
	if(DRM_SUCCESS != result)
		return result;

	m_pDecryptContext = new DRM_MANAGER_DECRYPT_CONTEXT;
	memset(m_pDecryptContext, 0, sizeof(DRM_MANAGER_DECRYPT_CONTEXT));

	result = DRM_MGR_Bind(m_pDrmContext, rgpdstrRights, NO_OF(rgpdstrRights), 
		(DRMPFNOUTPUTLEVELSCALLBACK)DRMOutputLevelsCallback, this, 
		m_pDecryptContext);

	if(DRM_SUCCESS != result)
	{
		delete m_pDecryptContext;
		m_pDecryptContext = NULL;

		return result;
	}

	return VO_ERR_DRM_OK;
}

VO_S32 CWMDRM::OutputLevelsCallback(const VO_PTR pOutputLevelsData, VO_U32 nCallbackType)
{
	switch(nCallbackType)
	{
	case DRM_PLAY_OPL_CALLBACK:
		{
			if(!pOutputLevelsData)
				return DRM_E_INVALIDARG;

			//pOutputLevelsData: DRM_PLAY_OPL*
			DRM_PLAY_OPL* pPlayOpl = (DRM_PLAY_OPL*)pOutputLevelsData;
		}
		break;

	case DRM_COPY_OPL_CALLBACK:
		{
			if(!pOutputLevelsData)
				return DRM_E_INVALIDARG;

			//pOutputLevelsData: DRM_COPY_OPL*
			DRM_COPY_OPL* pCopyOpl = (DRM_COPY_OPL*)pOutputLevelsData;
		}
		break;

	case DRM_INCLUSION_LIST_CALLBACK:
		{
			if(!pOutputLevelsData)
				return DRM_E_INVALIDARG;

			//pOutputLevelsData: DRM_INCLUSION_LIST_CALLBACK_STRUCT*
		}
		break;

	default:	//unknown callback type
		break;
	}

	return DRM_SUCCESS;
}
