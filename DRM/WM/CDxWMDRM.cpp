#include "CDxWMDRM.h"

/* Discretix integration Start */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DxDrmDebugApi.h"
/* Discretix integration End */

#define LOG_TAG "DxWMDRM"

#include "voLog.h"

CDxWMDRM::CDxWMDRM()
	:drmStream(NULL)
{
}

CDxWMDRM::~CDxWMDRM()
{
	Close();
}

VO_U32 CDxWMDRM::Open()
{
	Close();
	
	lastResult = DX_SUCCESS;
	drmFileOpened = DX_FALSE;
	clientInitialized = DX_TRUE;
	fileIsDrmProtected = DX_FALSE;
	drmStream = NULL;
	
	  /*Loading the Drm config file*/
	VOLOGI("DX:  DxLoadConfigFile(\"/data/DxDrm/DxDrmConfig.txt\") ");
  lastResult = DxLoadConfigFile("/data/DxDrm/DxDrmConfig.txt");
  if(lastResult != DX_SUCCESS)
  {
  	VOLOGI("DX: ERROR - Discretix configuration file not found");
  }

	/*Initialize Drm Client*/
	VOLOGI("DX: DxDrmClient_Init()");
  lastResult = DxDrmClient_Init();
  if(lastResult == DX_SUCCESS)
  {
      clientInitialized = DX_TRUE;
      VOLOGI("DX: Discretix client intialized successfully");
  }
  else 
  {
  		VOLOGI("DX: ERROR - Discretix client initialization failed with error: %d",(int)lastResult);
  		return lastResult;
  }
  /* Discretix integration End */

	return VO_ERR_DRM_OK;
}


VO_U32 CDxWMDRM::Close()
{
	if(drmFileOpened == DX_TRUE && drmStream != NULL)
	{
		VOLOGI("DX: DxDrmFile_HandleConsumptionEvent(drmStream, DX_EVENT_STOP)" );
		DxDrmFile_HandleConsumptionEvent(drmStream, DX_EVENT_STOP);
	
	  VOLOGI("DX: DxDrmStream_Close(&(drmStream))");
		DxDrmStream_Close(&(drmStream));
	}
	
	if(clientInitialized == DX_TRUE)
	{
		VOLOGI("DX: DxTerminateConfigFile()");
		DxTerminateConfigFile();
	
	  VOLOGI("DX: DxDrmClient_Terminate()");
		DxDrmClient_Terminate();
	}
	
	return VO_ERR_DRM_OK;
}



VO_U32 CDxWMDRM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CDxWMDRM::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CDxWMDRM::SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo)
{
	EDxDrmStatus result = DX_DRM_SUCCESS;
	
	VOLOGI("DX: Opening drm stream ( DxDrmOpenStream)");
	
	if(clientInitialized != DX_TRUE)
  {
    VOLOGI("DX: ERROR - Discretix DRM client not initialized");
    return VO_ERR_DRM_ERRORSTATE;
  }
	
	
	VO_WMDRM_INFO* pInfo = (VO_WMDRM_INFO*)pDrmInfo;
	if(!pInfo || (!pInfo->pContentEncryptionObject && !pInfo->pExtendedContentEncryptionObject))
		return VO_ERR_DRM_BADPARAMETER;

	//we will use extended content encryption object if it is occur in file
	if(pInfo->pExtendedContentEncryptionObject)
	{
	//		for(int i=0; i<pInfo->nExtendedContentEncryptionObject; i++)
	//		{
	//			VOLOGI("DX:0x%x" , pInfo->pExtendedContentEncryptionObject[i]);
	//		}
			
			VOLOGI("DX: DxDrmClient_OpenDrmStreamFromData(&drmStream,pInfo->pExtendedContentEncryptionObject,pInfo->nExtendedContentEncryptionObject )");
		  result = DxDrmClient_OpenDrmStreamFromData(&drmStream,pInfo->pExtendedContentEncryptionObject,pInfo->nExtendedContentEncryptionObject );	
  		if(result != DX_DRM_SUCCESS)
  		{
    			VOLOGI("DX: ERROR - error while opening stream from data use ExtendedContentEncryptionObject" );
    			goto error;
  		}
	}
	else
	{	
  		//may be problem stony
	//		for(int i=0; i<pInfo->nContentEncryptionObject; i++)
	//		{
	//			VOLOGI("DX:0x%x" , pInfo->pContentEncryptionObject[i]);
	//		}
			
			VOLOGI("DX: DxDrmClient_OpenDrmStreamFromData(&drmStream,pInfo->pContentEncryptionObject,pInfo->nContentEncryptionObject )");
			result = DxDrmClient_OpenDrmStreamFromData(&drmStream,pInfo->pContentEncryptionObject,pInfo->nContentEncryptionObject );	
  		if(result != DX_DRM_SUCCESS)
  		{
    			VOLOGI("DX: ERROR - error while opening stream from data use ContentEncryptionObject" );
    			goto error;
  		}
	}
	
	VOLOGI("DX: stream opened successfully");
	
	/*setting use intent for the drm file*/
  /*this operation checks if we can use the file according to the intent specified*/
  /*It will check for valid license*/
  
  VOLOGI("DX: DxDrmStream_SetIntent(drmStream, DX_INTENT_AUTO_PLAY, DX_AUTO_NO_UI)");
  result = DxDrmStream_SetIntent(drmStream, DX_INTENT_AUTO_PLAY, DX_AUTO_NO_UI);

  if(result != DX_DRM_SUCCESS)
  {
    VOLOGI("DX: ERROR - opening stream failed because there are no rights (license) to play the content " );
    goto error;
  }
  
  VOLOGI("DX: playback rights found");
  
  return VO_ERR_DRM_OK;
	
error:
	VOLOGI("DX: error occur, so close drm stream");
  drmFileOpened = DX_FALSE;
  VOLOGI("DX: DxDrmStream_Close(&(drmStream))");
  DxDrmStream_Close(&(drmStream));
  
  return VO_ERR_DRM_BADDRMINFO;

//	return CheckLicense();
}

VO_U32 CDxWMDRM::GetDrmFormat(VO_DRM_FORMAT* pDrmFormat)
{
	return VO_ERR_DRM_OK;
}

VO_U32 CDxWMDRM::CheckCopyRightResolution(VO_U32 nWidth, VO_U32 nHeight)
{
	return VO_ERR_DRM_OK;
}

	
VO_U32 CDxWMDRM::Commit()
{	
	EDxDrmStatus result = DX_DRM_SUCCESS;
	drmFileOpened = DX_FALSE;
	
	/*starting consumption of the file - notifying the drm that the file is being used*/
	VOLOGI("DX: DxDrmFile_HandleConsumptionEvent(drmStream, DX_EVENT_START)");
  result = DxDrmFile_HandleConsumptionEvent(drmStream, DX_EVENT_START);
  if(result != DX_DRM_SUCCESS)
  {
    VOLOGI("DX: Content consumption failed" );
    goto csmfail;
  }

  VOLOGI("DX: Stream was opened and is ready for playback");
  drmFileOpened = DX_TRUE;
  
  return VO_ERR_DRM_OK;
  
csmfail:
	
	drmFileOpened = DX_FALSE;
	VOLOGI("DX: DxDrmStream_Close(&(drmStream))");
  DxDrmStream_Close(&(drmStream));
  
  return VO_ERR_DRM_ERRORSTATE;
}

VO_U32 CDxWMDRM::DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize)
{
	EDxDrmStatus result = DX_DRM_SUCCESS;
	//if size less than 15, we regard it as not encrypted data
	//for example, padding data
	//East, 2010/04/08
	if(nSize < 15)
		return VO_ERR_DRM_OK;
		
	/* Discretix integration Start */
 // unsigned long output = 0;
  
  VO_U32 dwStreamNum = pDataInfo->nReserved[0];

  if (drmFileOpened != DX_TRUE)
	{
	  	VOLOGI("DX: ERROR content is encrypted but DRM stream is not opened");
	  	return VO_ERR_DRM_ERRORSTATE;
	}
	
	VOLOGI("DxDrmStream_DecryptWMPayload in put data:0x%02x size: %d stream number is: %d", pData, (int)nSize, dwStreamNum);
	
	/*
	for(int i=0; i < (nSize / 10); i++)
	{
		VOLOGI("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", pData[i*5+0] , pData[i*5+1] ,pData[i*5+2] ,pData[i*5+3] ,pData[i*5+4]);
	}
	*/
	/*
	VOLOGI("DX: DxDrmStream_ProcessPacket(drmStream,1 ,pData, nSize, pData, &nSize)");
	result = DxDrmStream_ProcessPacket(drmStream,1 ,pData, nSize, pData, &output);
  if ( result!= DX_DRM_SUCCESS)
	{
	  	VOLOGI( "DX: ERROR - packet decryption failed result: %d , nSize: %d" , (int)result , output);
	  	return VO_ERR_DRM_BADDRMDATA;
	}
	
	nSize = output;
	
	VOLOGI("DxDrmStream_ProcessPacket out put data: 0x%x  ,size: %d" , pData, (int)nSize);
	*/
	
	result = DxDrmStream_DecryptWMPayload(drmStream,pData, nSize, dwStreamNum);
	if ( result != DX_DRM_SUCCESS)
	{
			VOLOGI( "DX: ERROR - packet decryption failed result: %d " , (int)result);
	  	return VO_ERR_DRM_BADDRMDATA;
	}
	
	VOLOGI("DxDrmStream_DecryptWMPayload out put data:0x%02x size: %d stream number is: %d", pData, (int)nSize, dwStreamNum);
	
	/*
	for(int i=0; i < (nSize / 10); i++)
	{
		VOLOGI("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", pData[i*5+0] , pData[i*5+1] ,pData[i*5+2] ,pData[i*5+3] ,pData[i*5+4]);
	}
	*/
  
  /* Discretix integration End */


	return VO_ERR_DRM_OK;
}

VO_U32 CDxWMDRM::DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CDxWMDRM::CheckLicense()
{
	return VO_ERR_DRM_OK;
}
