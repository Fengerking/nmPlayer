	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCSourceBuffer.cpp

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif

#include <string.h>
#include "voType.h"
#include "voIndex.h"
#include "vompCSourceBuffer.h"

int vompReadAudio (void * pUserData, VOMP_BUFFERTYPE * pBuffer)
{
	vompCSourceBuffer * pData = (vompCSourceBuffer *)pUserData;

	return pData->ReadAudioBuffer (pBuffer);
}

int	vompReadVideo (void * pUserData, VOMP_BUFFERTYPE * pBuffer)
{
	vompCSourceBuffer * pData = (vompCSourceBuffer *)pUserData;

	return pData->ReadVideoBuffer (pBuffer);
}

vompCSourceBuffer::vompCSourceBuffer(void)
	: m_pAudioBuffer(NULL)
	, m_nAudioFileSize(0)
	, m_pAudioStartData(NULL)
	, m_pAudioNextData(NULL)
	, m_llAudioTime(0)
	, m_pVideoBuffer(NULL)
	, m_nVideoFileSize(0)
	, m_pVideoStartData(NULL)
	, m_pVideoNextData(NULL)
	, m_llVideoTime(0)
{
#ifdef _WIN32
	DWORD dwRead = 0;
	HANDLE hFile = CreateFile(_T("E:\\Media_Samples\\SD_CARD\\AAC_LC\\336029.aac"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		m_nAudioFileSize = GetFileSize (hFile, NULL);
		m_pAudioBuffer = new BYTE[m_nAudioFileSize];
		ReadFile (hFile, m_pAudioBuffer, m_nAudioFileSize, &dwRead, NULL);
		CloseHandle (hFile);

		m_pAudioStartData = m_pAudioBuffer;
		m_pAudioNextData = m_pAudioBuffer;
		m_llAudioTime = 0;
	}

	hFile = CreateFile(_T("E:\\Media_Samples\\H264\\akiyo_cif_30fps_384k.264"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		m_nVideoFileSize = GetFileSize (hFile, NULL);
		m_pVideoBuffer = new BYTE[m_nVideoFileSize];
		ReadFile (hFile, m_pVideoBuffer, m_nVideoFileSize, &dwRead, NULL);
		CloseHandle (hFile);

		m_pVideoStartData = m_pVideoBuffer;
		m_pVideoNextData = m_pVideoBuffer;
		m_llVideoTime = 0;
	}
#endif
}

vompCSourceBuffer::~vompCSourceBuffer(void)
{
	if(m_pVideoBuffer)
		delete []m_pVideoBuffer;

	if(m_pAudioBuffer)
		delete []m_pAudioBuffer;
}

int vompCSourceBuffer::ReadAudioBuffer (VOMP_BUFFERTYPE * pBuffer)
{
	if(!m_pAudioBuffer)
		return 0;

	static VO_U32 dwAudioSyncWord = 0X4050F1FF;
//	static DWORD dwAudioSyncWord = 0X4054F1FF;

	while (true)
	{
		if (m_pAudioNextData - m_pAudioBuffer >= m_nAudioFileSize)
		{
			m_pAudioStartData = m_pAudioBuffer;
			m_pAudioNextData = m_pAudioBuffer;
		}

		if (!memcmp (m_pAudioNextData, &dwAudioSyncWord, 4))
		{
			if (m_pAudioNextData - m_pAudioStartData > 32)
				break;
		}

		m_pAudioNextData++;
	}

	pBuffer->nSize = m_pAudioNextData - m_pAudioStartData;
	pBuffer->pBuffer = m_pAudioStartData;
	pBuffer->llTime = m_llAudioTime;

	m_pAudioStartData = m_pAudioNextData;
	m_llAudioTime += 20;

	return 0;
}

int vompCSourceBuffer::ReadVideoBuffer (VOMP_BUFFERTYPE * pBuffer)
{
	if(!m_pVideoBuffer)
		return 0;

	static VO_U32 dwVideoSyncWord = 0X01000000;

	while (true)
	{
		if (m_pVideoNextData - m_pVideoBuffer >= m_nVideoFileSize)
		{
			m_pVideoStartData = m_pVideoBuffer;
			m_pVideoNextData = m_pVideoBuffer;
		}

		if (!memcmp (m_pVideoNextData, &dwVideoSyncWord, 4))
		{
			if (m_pVideoNextData - m_pVideoStartData > 32)
				break;
		}

		m_pVideoNextData++;
	}

	pBuffer->nSize = m_pVideoNextData - m_pVideoStartData;
	pBuffer->pBuffer = m_pVideoStartData;
	pBuffer->llTime = m_llVideoTime;

	m_pVideoStartData = m_pVideoNextData;
	m_llVideoTime += 40;

	return 0;
}
