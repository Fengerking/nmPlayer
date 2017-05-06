	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEMetadataRetriever.cpp

	Contains:	CVOMEMetadataRetriever class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-08-31		JBF			Create file

*******************************************************************************/
#include "CVOMEMetadataRetriever.h"

#undef LOG_TAG
#define LOG_TAG "CVOMEMetadataRetriever"
#include "voLog.h"
#include "voCMutex.h"
voCMutex	g_mutex;

CVOMEMetadataRetriever::CVOMEMetadataRetriever ()
	: m_nMode (0)
	, m_nFD (-1)
	, m_nOffset (0)
	, m_nLength (0)
	, m_pMetadata (NULL)
	, m_pThumbnail (NULL)
	, m_bInitThumbnailError(false)
	, mpDRMAPI(NULL)
	, m_pFilePipe (NULL)
	, m_pFileBasedDrmAPI(NULL)
{
	strcpy (m_szFile, "");
}

CVOMEMetadataRetriever::~CVOMEMetadataRetriever ()
{
	UninitMetadata();
	UninitThumbnail();

#ifdef LINUX
	if(m_nFD != -1)
	{
#ifdef LINUX
		close(m_nFD);
#endif	//LINUX
		m_nFD = -1;
	}
#endif //LINUX
}


int CVOMEMetadataRetriever::setDataSource(const char *url)
{
	strcpy (m_szFile, url);

	return 0;
}

int CVOMEMetadataRetriever::setDataSource(int fd, int64_t offset, int64_t length)
{
#ifdef LINUX
	if(m_nFD != -1)
		close(m_nFD);

	m_nFD = dup(fd); 
	if (m_nFD <= 0)
	{
		VOLOGE ("dup error. ID %d,  %s", errno, strerror(errno));
		m_nFD = fd;
	}
#endif // LINUX
	m_nOffset = offset;
	m_nLength = length;

	return 0;
}

int CVOMEMetadataRetriever::setMode(int mode)
{
	if (mode < 0 || mode > 3)
		return -1;

	m_nMode = mode;

	return 0;
}

int CVOMEMetadataRetriever::getMode(int* mode) const
{
	*mode = m_nMode;

	return 0;
}

void dumpdata(VO_PBYTE data, VO_U32	size)
{
	FILE *input;
	char filename[64];
	sprintf(filename, "/data/local/stony.raw");
	VOLOGI("filename : %s , size: %d" , filename , (int)size);
	input = fopen(filename,"wb");
	VOLOGI("fopen suc handle: %p error: %d" , input , errno);
	int wrt = fwrite(data, size,1,input);
	VOLOGI("fwrite suc size wrt: %d" , wrt);
	fclose(input);
}

VideoFrame* CVOMEMetadataRetriever::getFrameAtTime(const VOA_GETFRAMEATTIME_PARAM* pParam)
{
	voCAutoLock lock(&g_mutex);

	if(!InitThumbnail())
		return NULL;

	m_pThumbnail->setForbidBlackFrame((pParam->nFlags & VOA_GETFRAMEATTIME_FLAG_FORBIDBLACKFRAME) != 0);

	int nWidth = 0, nHeight = 0;
	if(pParam->nFlags & VOA_GETFRAMEATTIME_FLAG_SPECIFYRESOLUTION)
	{
		nWidth = pParam->nWidth;
		nHeight = pParam->nHeight;
	}
	else if(pParam->nFlags & VOA_GETFRAMEATTIME_FLAG_SPECIFYRSIZELIMIT)
	{
		m_pThumbnail->getOrigResolution(&nWidth, &nHeight);
		if ((nWidth > pParam->nWidth) || (nHeight > pParam->nHeight))
		{
			nWidth = pParam->nWidth;
			nHeight = pParam->nHeight;
		}
	}
	else
		m_pThumbnail->getOrigResolution(&nWidth, &nHeight);	// use original resolution

	m_pThumbnail->setResolution(nWidth, nHeight);
	m_pThumbnail->setPos(pParam->nPosition, (pParam->nFlags & 0xFF));
	m_pThumbnail->setColor(VO_COLOR_RGB565_PACKED);

	VO_VIDEO_BUFFER * pVideoBuffer = NULL;
	VO_VIDEO_FORMAT * pVideoFormat = NULL;
	int nRotation = 0;
	int nRC = m_pThumbnail->capture(&pVideoBuffer, &pVideoFormat, &nRotation);
	if(nRC < 0 || pVideoBuffer == NULL)
		return NULL;

	VideoFrame * pVideoFrame = new VideoFrame();
	pVideoFrame->mWidth = nWidth;
	pVideoFrame->mHeight = nHeight;
	pVideoFrame->mDisplayWidth = nWidth;
	pVideoFrame->mDisplayHeight = nHeight;
	pVideoFrame->mSize = nWidth * nHeight * 2;
	pVideoFrame->mData = new unsigned char[pVideoFrame->mSize];
	memset(pVideoFrame->mData, 0, pVideoFrame->mSize);
	int nOffset = (nHeight - pVideoFormat->Height) * nWidth * 2 + nWidth - pVideoFormat->Width;
	for(int i = 0; i < pVideoFormat->Height; i++)
		memcpy(pVideoFrame->mData + nOffset + i * nWidth * 2, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i,  pVideoFormat->Width * 2);
#ifdef _GingerBread
	pVideoFrame->mRotationAngle = nRotation;
#endif	//_GingerBread

	//dump thumbnail data 
#if 0
	VOLOGI("dump thumbnail data ============= for debug purpose data size: %d" ,pVideoFrame->mSize);
	dumpdata(pVideoFrame->mData, pVideoFrame->mSize);
#endif

	return pVideoFrame;
}

MediaAlbumArt* CVOMEMetadataRetriever::extractAlbumArt()
{
	if(!InitMetadata())
		return NULL;

	unsigned char * pBuffer = NULL;
	int nBuffSize = 0;
	m_pMetadata->GetAlbumArt(&pBuffer, &nBuffSize);
	if(!pBuffer)
		return NULL;

	MediaAlbumArt * pAlbum = new MediaAlbumArt();
	if(pAlbum)
	{
		pAlbum->mSize = nBuffSize;
		pAlbum->mData = new unsigned char[pAlbum->mSize];
		memcpy(pAlbum->mData, pBuffer, nBuffSize);
	}
	return pAlbum;
}

const char * CVOMEMetadataRetriever::extractMetadata(int keyCode)
{
	if(!InitMetadata(keyCode))
		return NULL;

	return m_pMetadata->GetMetaDataText (keyCode);
}


int CVOMEMetadataRetriever::SetFilePipe (void * pFilePipe)
{
	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;

	return 0;
}

bool CVOMEMetadataRetriever::InitMetadata(int keyCode)
{
	if(m_pMetadata)
		return true;

	m_pMetadata = new CVOMEMetadata();
	if(m_pMetadata == NULL)
		return false;

	if(m_pFileBasedDrmAPI)
	{
		VOLOGI("Set File Based DRM");
		m_pMetadata->SetFileBasedDrm(&m_FileOpt, (void*)m_pFileBasedDrmAPI);
	}
	else
	{
		VOLOGI("Set the file pipe %p", m_pFilePipe);
		m_pMetadata->SetFilePipe(m_pFilePipe);
	}


	int nRC;
	if (strlen (m_szFile) > 0)
		nRC = m_pMetadata->setDataSource(m_szFile);
	else
		nRC = m_pMetadata->setDataSource(m_nFD, m_nOffset, m_nLength);

	if ((METADATA_KEY_IS_DRM_CRIPPLED != keyCode) && (KEY_PARAMETER_LGEDIVX_DIVX_DRM_FILE != keyCode) && (VO_ERR_NONE != nRC))
		return false;

	return true;
}

void CVOMEMetadataRetriever::UninitMetadata()
{
	if(m_pMetadata)
	{
		delete m_pMetadata;
		m_pMetadata = NULL;
	}
}

bool CVOMEMetadataRetriever::InitThumbnail()
{
	if(m_pThumbnail)
		return true;

	if(m_bInitThumbnailError)
		return false;

	m_pThumbnail = new CVOWThumbnail;
	if(!m_pThumbnail)
		return false;

	if(m_pFileBasedDrmAPI)
	{
		VOLOGI("Set File Based DRM");
		m_pThumbnail->SetFileBasedDrm(&m_FileOpt, (void*)m_pFileBasedDrmAPI);
	}else if (mpDRMAPI)
		m_pThumbnail->SetDrmApi(mpDRMAPI);

	VOLOGI("Set the file pipe %p", m_pFilePipe);
	m_pThumbnail->SetFilePipe(m_pFilePipe);

	if(m_pThumbnail->Init() >= 0)
	{
		int nRC = 0;
		if(strlen(m_szFile) > 0)
			nRC = m_pThumbnail->SetSource(m_szFile, NULL);
		else
			nRC = m_pThumbnail->SetSource(m_nFD, m_nOffset, m_nLength);
		if(nRC >= 0)
		{
			nRC = m_pThumbnail->prepareForCapture();
			if(nRC >= 0)
				return true;
		}
	}

	m_bInitThumbnailError = true;
	UninitThumbnail();
	return false;
}

void CVOMEMetadataRetriever::UninitThumbnail()
{
	if(m_pThumbnail)
	{
		m_pThumbnail->Uninit();

		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}
}

int CVOMEMetadataRetriever::SetFileBasedDrmApi(void *pDrmApi)
{
	m_pFileBasedDrmAPI = (IVOFileBasedDRM*)pDrmApi;
	m_FileOpt.Open = FileBasedDrm_Open;
	m_FileOpt.Read = FileBasedDrm_Read;
	m_FileOpt.Write = FileBasedDrm_Write;
	m_FileOpt.Seek = FileBasedDrm_Seek;
	m_FileOpt.Flush = FileBasedDrm_Flush;
	m_FileOpt.Size = FileBasedDrm_Size;
	m_FileOpt.Close = FileBasedDrm_Close;
	return 0;
}

typedef struct
{
	IVOFileBasedDRM*		pIDrm;
	OMX_VO_DRM_FILEBASED_IO	sIO;
	VO_S32					nFD;
} VO_DRM_FILEBASED_HANDLE;

VO_PTR CVOMEMetadataRetriever::FileBasedDrm_Open(VO_FILE_SOURCE * pSource)
{
	if(!pSource)
		return NULL;

	VO_DRM_FILEBASED_HANDLE* pDrmHandle = new VO_DRM_FILEBASED_HANDLE;
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

VO_S32 CVOMEMetadataRetriever::FileBasedDrm_Read(VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	VO_DRM_FILEBASED_HANDLE* pDrmHandle = (VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	return pDrmHandle->pIDrm->Read(&pDrmHandle->sIO, pBuffer, uSize);
}

VO_S32 CVOMEMetadataRetriever::FileBasedDrm_Write(VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	return 0;
}

VO_S32 CVOMEMetadataRetriever::FileBasedDrm_Flush(VO_PTR pHandle)
{
	return 0;
}

VO_S64 CVOMEMetadataRetriever::FileBasedDrm_Seek(VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag)
{
	VO_DRM_FILEBASED_HANDLE* pDrmHandle = (VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	OMX_VO_DRM_FILEBASED_SEEKMODE nSeekMode;
	if (uFlag == VO_FILE_BEGIN)
		nSeekMode = OMX_VO_DRM_FILEBASED_SEEK_SET;
	else if (uFlag == VO_FILE_CURRENT)
		nSeekMode = OMX_VO_DRM_FILEBASED_SEEK_CUR;
	else
		nSeekMode = OMX_VO_DRM_FILEBASED_SEEK_END;

	int nRet = pDrmHandle->pIDrm->Seek(&pDrmHandle->sIO,(int)nPos, nSeekMode);
	if(nRet != 0)
		return -1;

	return pDrmHandle->pIDrm->Tell(&pDrmHandle->sIO);
}

VO_S64 CVOMEMetadataRetriever::FileBasedDrm_Size(VO_PTR pHandle)
{
	VO_DRM_FILEBASED_HANDLE* pDrmHandle = (VO_DRM_FILEBASED_HANDLE*)pHandle;
	if(!pDrmHandle)
		return -1;

	return pDrmHandle->pIDrm->Size(&pDrmHandle->sIO);
}

VO_S64 CVOMEMetadataRetriever::FileBasedDrm_Save(VO_PTR pHandle)
{
	return 0;
}

VO_S32 CVOMEMetadataRetriever::FileBasedDrm_Close(VO_PTR pHandle)
{
	VO_DRM_FILEBASED_HANDLE* pDrmHandle = (VO_DRM_FILEBASED_HANDLE*)pHandle;
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
