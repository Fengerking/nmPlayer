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

#define LOG_TAG "CVOMEMetadataRetriever"

#include "CVOMEMetadataRetriever.h"
#include "CVOWPlayer.h"

#include "voLog.h"

// ----------------------------------------------------------------------------

CVOMEMetadataRetriever::CVOMEMetadataRetriever ()
	: m_nMode (0)
	, m_nFD (-1)
	, m_hFile (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_pMetadata (NULL)
{
	strcpy (m_szFile, "");
}

CVOMEMetadataRetriever::~CVOMEMetadataRetriever ()
{
#ifdef LINUX
	if(m_nFD != -1)
	{
#ifdef LINUX
		close(m_nFD);
#endif	//LINUX
		m_nFD = -1;
	}
#endif //LINUX

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

	if(m_nFD != -1)
		close(m_nFD);

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

	VO_VIDEO_BUFFER * pVideoBuffer = NULL;
	VO_VIDEO_FORMAT * pVideoFormat = NULL;
	nRC = player.CaptureFrame (nPos, nWidth, nHeight, VO_COLOR_RGB565_PACKED, 
							   &pVideoBuffer, &pVideoFormat);
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
		memcpy (pVideoFrame->mData + nOffset + i * nWidth * 2, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i,  pVideoFormat->Width * 2);
	
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



