#include "CFileFormatCheck.h"
#include "fMacros.h"
#include "CID3MetaData.h"
#include "CASFMetaData.h"
#include "CMP4MetaData.h"

VO_U32 g_dwFRModuleID = VO_INDEX_SRC_ID3;

#if defined __cplusplus
extern "C" {
#endif

VO_U32 VO_API voMDOpen(VO_PTR* ppHandle, VO_SOURCE_OPENPARAM* pParam)
{
	if(!(pParam->nFlag & VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR))
		return VO_ERR_INVALID_ARG;

	//check file format
	CFileFormatCheck checker((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
	VO_FILE_FORMAT ff = checker.GetFileFormat((VO_FILE_SOURCE*)pParam->pSource, FLAG_CHECK_METADATA);

	//MP3, AAC(ID3)
	//ASF
	//MP4
	CBaseMetaData* pMD = VO_NULL;
	if(VO_FILE_FFAUDIO_MP3 == ff || VO_FILE_FFAUDIO_AAC == ff)
		pMD = new CID3MetaData((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
	else if(VO_FILE_FFMOVIE_ASF == ff)
		pMD = new CASFMetaData((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
	else if(VO_FILE_FFMOVIE_MP4 == ff)
		pMD = new CMP4MetaData((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
	else
		return VO_ERR_METADATA_NOMETADATA;

	if(!pMD)
		return VO_ERR_OUTOF_MEMORY;

	VO_U32 rc = pMD->Load((VO_FILE_SOURCE*)pParam->pSource);

	if(VO_ERR_METADATA_OK != rc)
	{
		delete pMD;
		return rc;
	}

	*ppHandle = pMD;
	return VO_ERR_METADATA_OK;
}

VO_U32 VO_API voMDClose(VO_PTR pHandle)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseMetaData* pMD = (CBaseMetaData*)pHandle;
	VO_U32 rc = pMD->Close();
	delete pMD;
	return rc;
}

VO_U32 VO_API voMDSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseMetaData* pMD = (CBaseMetaData*)pHandle;
	return pMD->SetParameter(uID, pParam);
}

VO_U32 VO_API voMDGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseMetaData* pMD = (CBaseMetaData*)pHandle;
	return pMD->GetParameter(uID, pParam);
}

VO_S32 VO_API voGetMetaDataReadAPI(VO_METADATA_READAPI* pReadHandle, VO_U32 uFlag)
{
	pReadHandle->Open = voMDOpen;
	pReadHandle->Close = voMDClose;
	pReadHandle->SetParam = voMDSetParam;
	pReadHandle->GetParam = voMDGetParam;

	return VO_ERR_METADATA_OK;
}
#if defined __cplusplus
}
#endif

