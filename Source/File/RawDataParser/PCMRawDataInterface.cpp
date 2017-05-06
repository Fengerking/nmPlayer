	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		PCMRawDataInterface.h

	Contains:	PCM raw data interface file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2012-02-02		Rodney		Create file

*******************************************************************************/


#include "PCMRawData.h"
#include "PCMRawDataInterface.h"

#define LOG_TAG "PCMRawDataInterface"
#include "voLog.h"

#ifdef __cplusplus
extern "C" {
#endif

    VO_U32 VO_API voFRPCMOpen(VO_PTR* ppHandle, VO_SOURCE_OPENPARAM* pParam)
    {
	    if(VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE != (pParam->nFlag & 0xFF) && 
		  (VO_SOURCE_OPENPARAM_FLAG_OPENPD != (pParam->nFlag & 0xFF)) &&
		  (VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL != (pParam->nFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)))
	    {
		    VOLOGE("invalid file open flags!!");
		    return VO_ERR_INVALID_ARG;
	    }

	    if(VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR != (pParam->nFlag & 0xFF00))
	    {
		    VOLOGE("invalid file operator flags!!");
		    return VO_ERR_INVALID_ARG;
	    }

	    CPCMRawData* pReader = new CPCMRawData((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    if(!pReader)
	    {
		    VOLOGE("failed to create PCM raw data file reader!!");
		    return VO_ERR_OUTOF_MEMORY;
	    }

	    VO_U32 rc = pReader->Load(pParam->nFlag, (VO_FILE_SOURCE*)pParam->pSource);
	    //only OK/DRM can be playback
	    if(VO_ERR_SOURCE_OK != rc && VO_ERR_SOURCE_CONTENTENCRYPT != rc)
	    {
		    VOLOGE("failed to load file parser: 0x%08X!!", rc);
		    delete pReader;
		    return rc;
	    }
	    *ppHandle = pReader;

	    return rc;
    }

    VO_U32 VO_API voFRPCMGetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CPCMRawData* pReader = (CPCMRawData*)pHandle;

	    return pReader->GetTrackInfo(pTrackInfo);
    }

    VO_U32 VO_API voFRPCMGetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

		CPCMRawData *pRawDataParser = (CPCMRawData*)pHandle;

		return pRawDataParser->GetSample(pSample);
    }

    VO_U32 VO_API voFRPCMSetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64* pPos)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CPCMRawData* pReader = (CPCMRawData*)pHandle;

	    return pReader->SetPos(pPos);
    }

    VO_U32 VO_API voFRPCMSetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CPCMRawData* pReader = (CPCMRawData*)pHandle;

	    return pReader->SetParameter(uID, pParam);
    }

    VO_U32 VO_API voFRPCMSetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CPCMRawData* pReader = (CPCMRawData*)pHandle;

		return pReader->SetTrackParameter(uID, pParam);
    }

    VO_U32 VO_API voFRPCMGetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CPCMRawData* pReader = (CPCMRawData*)pHandle;

	    return pReader->GetTrackParameter(uID, pParam);
    }


#ifdef __cplusplus
}
#endif
