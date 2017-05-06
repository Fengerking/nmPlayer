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
#include <stdio.h>
#include <string.h>

#ifdef LINUX
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX

#define LOG_TAG "CVOMEMetadataRetriever"

#include "CVOMEMetadataRetriever.h"
#include "CVOWPlayer.h"

#include "voOMXOSFun.h"

#include "voLog.h"

#include "vojnilog.h"

// ----------------------------------------------------------------------------

CVOMEMetadataRetriever::CVOMEMetadataRetriever ()
	: m_nMode (0)
	, m_nFD (-1)
	, m_hFile (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_pMetadata (NULL)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nDuration(0)
{

     strcpy(m_szVideoFormat, "");
     strcpy(m_szAudioFormat, "");
     strcpy(m_szFPS, "");

	strcpy (m_szFile, "");
}

CVOMEMetadataRetriever::~CVOMEMetadataRetriever ()
{
	if (m_hFile != NULL)
		fclose (m_hFile);
	m_hFile = NULL;

	if (m_pMetadata != NULL)
		delete m_pMetadata;
}


int CVOMEMetadataRetriever::setDataSource(const char *url)
{
	strcpy (m_szFile, url);
	return 0;
}

int CVOMEMetadataRetriever::setDataSource(int fd, int64_t offset, int64_t length)
{
//	m_nFD = fd;
#ifdef LINUX
	if (m_hFile != NULL)
		fclose (m_hFile);

	m_nFD = dup(fd); 
	if (m_nFD > 0)
	{
		m_hFile = fdopen(m_nFD, "r");
		if (m_hFile == NULL)
		{
			VOLOGE ("fopen error. ID %d,  %s", errno, strerror(errno));
		}
	}
	else
	{
		VOLOGE ("dup error. ID %d,  %s", errno, strerror(errno));
		m_nFD = fd;
	}
#endif // LINUX
	m_nOffset = offset;
	m_nLength = length;

//	VOLOGI ("File ID %d, %d, %d", m_nFD, (int)m_nOffset, (int)m_nLength);

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

VideoFrame* CVOMEMetadataRetriever::captureFrame()
{
	int nRC = 0;
	CVOWPlayer player;

	if (player.Init () < 0)
		return NULL;

	if (strlen (m_szFile) > 0)
	{
		nRC = player.SetSource (m_szFile);
	}
	else
	{
		nRC = player.SetSource (m_nFD, m_nOffset, m_nLength);
	}
	if (nRC < 0)
	{
		player.Uninit ();
		return NULL;
	}

	int nPos = 0;
	int nWidth = 160;
	int nHeight = 120;
	uint32_t nDuration = 0;
	
	uint32_t nOutWidth  = nWidth;
	uint32_t nOutHeight = nHeight;

	VO_VIDEO_BUFFER * pVideoBuffer = NULL;
	VO_VIDEO_FORMAT * pVideoFormat = NULL;
	nRC = player.CaptureFrame (nPos, nWidth, nHeight, VO_COLOR_RGB565_PACKED, 
							   &pVideoBuffer, &pVideoFormat, &nDuration, &nOutWidth, &nOutHeight);
	if (nRC < 0 || pVideoBuffer == NULL)
	{
		player.Uninit ();
		return NULL;
	}

	VideoFrame * pVideoFrame = NULL;

	pVideoFrame = new VideoFrame ();
	pVideoFrame->mWidth = nWidth;
	pVideoFrame->mHeight = nHeight;
	pVideoFrame->mDisplayWidth = nWidth;
	pVideoFrame->mDisplayHeight = nHeight;
	pVideoFrame->mSize = nWidth * nHeight * 2;

	pVideoFrame->mData = new unsigned char[pVideoFrame->mSize];
	memset (pVideoFrame->mData, 0, pVideoFrame->mSize);

	int nOffset = (nHeight - pVideoFormat->Height) * nWidth + nWidth - pVideoFormat->Width;
	for (int i = 0; i < pVideoFormat->Height; i++)
		memcpy(pVideoFrame->mData + nOffset + i * nWidth * 2, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i,  pVideoFormat->Width * 2);
	
	player.Uninit ();

	return pVideoFrame;
}

VideoFrame* CVOMEMetadataRetriever::captureFrame2(int nWidth, int nHeight, uint8_t* out_pBuffer, uint32_t* out_npDuration, uint32_t* out_nWidth, uint32_t* out_nHeight)
{     
	int nRC = 0;
	CVOWPlayer player;

	if (player.Init () < 0)
		return NULL;

	if (strlen (m_szFile) > 0)
	{
		nRC = player.SetSource (m_szFile);
	}
	else
	{
		nRC = player.SetSource (m_nFD, m_nOffset, m_nLength);
	}
	if (nRC < 0)
	{
		player.Uninit ();
		return NULL;
	}

	int nPos = 0;


	VO_VIDEO_BUFFER * pVideoBuffer = NULL;
	VO_VIDEO_FORMAT * pVideoFormat = NULL;
	nRC = player.CaptureFrame (nPos, nWidth, nHeight, VO_COLOR_RGB565_PACKED, 
							   &pVideoBuffer, &pVideoFormat, out_npDuration, out_nWidth, out_nHeight);


 player.GetDuration ((int *)out_npDuration);
     	  
     	  
    if (pVideoFormat)
	 {
     	  *out_nWidth  = pVideoFormat->Width;
     	  *out_nHeight = pVideoFormat->Height;

		 
          JNILOGI2("CF Width : %d Height %d", 	pVideoFormat->Width,  pVideoFormat->Height);
     }
    else
	{
         *out_nWidth  =0;
     	  *out_nHeight = 0;
     	  
      	 JNILOGE("pVideoFormat is NULL");
    	}



	
	if (nRC < 0 || pVideoBuffer == NULL)
	{
		player.Uninit ();
		return NULL;
	}

	VideoFrame * pVideoFrame = NULL;

	pVideoFrame = new VideoFrame ();
	pVideoFrame->mWidth = nWidth;
	pVideoFrame->mHeight = nHeight;
	pVideoFrame->mDisplayWidth = nWidth;
	pVideoFrame->mDisplayHeight = nHeight;
	pVideoFrame->mSize = nWidth * nHeight * 2;

//	pVideoFrame->mData = new unsigned char[pVideoFrame->mSize];
	memset (out_pBuffer, 0, pVideoFrame->mSize);

	int nOffset = (nHeight - pVideoFormat->Height) * nWidth + nWidth - pVideoFormat->Width;
	for (int i = 0; i < pVideoFormat->Height; i++)
		memcpy(out_pBuffer + nOffset + i * nWidth * 2, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i,  pVideoFormat->Width * 2);
	
	player.Uninit ();

	return pVideoFrame;
}


MediaAlbumArt* CVOMEMetadataRetriever::extractAlbumArt()
{
	MediaAlbumArt * pAlbum = NULL;

	if (m_pMetadata == NULL)
	{
		m_pMetadata = new CVOMEMetadata ();
		if (m_pMetadata == NULL)
			return NULL;

		if (strlen (m_szFile) > 0)
			m_pMetadata->setDataSource (m_szFile);
		else
			m_pMetadata->setDataSource (m_nFD, m_nOffset, m_nLength);
	}

	unsigned char * pBuffer = NULL;
	int				nBuffSize = 0;

	m_pMetadata->GetAlbumArt (&pBuffer, &nBuffSize);
	if (pBuffer != NULL)
	{
		pAlbum = new MediaAlbumArt ();
		pAlbum->mSize = nBuffSize;
		pAlbum->mData = new unsigned char[pAlbum->mSize];
		memcpy (pAlbum->mData, pBuffer, nBuffSize);
	}

	return pAlbum;
}

const char * CVOMEMetadataRetriever::extractMetadata(int keyCode)
{
	if (m_pMetadata == NULL)
	{
		m_pMetadata = new CVOMEMetadata ();
		if (m_pMetadata == NULL)
			return NULL;

		if (strlen (m_szFile) > 0)
			m_pMetadata->setDataSource (m_szFile);
		else
			m_pMetadata->setDataSource (m_nFD, m_nOffset, m_nLength);
	}

	const char * pKeyText = m_pMetadata->GetMetaDataText (keyCode);

	return pKeyText;
}

void CVOMEMetadataRetriever::extractMetadata()
{

	
	CVOMEMetadata *pMetadata = new CVOMEMetadata ();
	if (pMetadata == NULL)
		return;

      JNILOGI(m_szFile);
      
	if (strlen (m_szFile) > 0)
		pMetadata->setDataSource (m_szFile);
	else
		pMetadata->setDataSource (m_nFD, m_nOffset, m_nLength);
	


   char szTmp[1024];

	strcpy(szTmp,  pMetadata->GetMetaDataText (METADATA_KEY_VIDEO_WIDTH));
	m_nWidth = ::atoi(szTmp);

	strcpy(szTmp,  pMetadata->GetMetaDataText (METADATA_KEY_VIDEO_HEIGHT));
	m_nHeight = ::atoi(szTmp);


	strcpy(szTmp,  pMetadata->GetMetaDataText (METADATA_KEY_DURATION));
	m_nDuration= ::atoi(szTmp);

	
	JNILOGI("Before get Video format");
	strcpy(m_szVideoFormat,  pMetadata->GetVideoFormat());	
	JNILOGI("before get Audio format");
	strcpy(m_szAudioFormat,  pMetadata->GetAudioFormat());
	JNILOGI("before get frame rate");	
//	strcpy(m_szFPS,  pMetadata->GetMetaDataText (METADATA_KEY_FRAME_RATE));

   delete pMetadata;
   JNILOGI("DONE");
	return ;
}

