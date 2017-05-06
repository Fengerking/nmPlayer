	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voMidi.h"
#include "voOSFunc.h"
#include "voAAC.h"
#include "voStreaming.h"
#include "CFileSource.h"

#include "voMetaData.h"
#include "voPD.h"
#include "voRealFile.h"
#include "voASF.h"
#include "voAudioFile.h"
#include "voMKV.h"
#include "voTS.h"
#include "voMP4.h"
#include "voLiveSource.h"
#include "fVideoHeadDataInfo.h"
#define LOG_TAG "CFileSource"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETFILEREADAPI) (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

const int AAC_SampRateTab[12] = {
    96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

CFileSource::CFileSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: CBaseSource (hInst, nFormat, pMemOP, pFileOP, pDrmCB)
	, m_hFile (NULL)
	, m_bForceClosed (VO_FALSE)
	, m_pbOutputConfigData (NULL)
	, m_bPlayClearestVideo (VO_FALSE)
	, m_pClearestVideoData (NULL)
	, m_nClearestVideoSize (0)
	, m_nWVC1Decoder(1)
	, m_bAVC2NAL (VO_TRUE)
	, m_bIsAVC (VO_FALSE)
	, m_nVideoTrack (-1)
	, m_nAudioTrack (-1)
	, m_pHeadData (NULL)
	, m_nHeadSize (0)
	, m_nNalLen (4)
	, m_nNalWord (0X01000000)
	, m_pVideoData (NULL)
	, m_nVideoSize (0)
	, m_nFrameSize (0)
	, m_bAudioAACAVC (VO_FALSE)
	, m_pAudioData (NULL)
	, m_nAudioSize (0)
	, m_nThumbFrameNum(0)
	, m_bIsHEVC(VO_FALSE)
{
	memset (&m_Source, 0, sizeof (VO_FILE_SOURCE));

	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE;
	m_paramOpen.pSource = &m_Source;
	m_paramOpen.pSourceOP = pFileOP;
	m_paramOpen.pMemOP = pMemOP;
	m_paramOpen.pDrmCB = pDrmCB;

	m_nFormat = nFormat;
}

CFileSource::~CFileSource ()
{
	CloseSource ();

	if (m_pbOutputConfigData != NULL)
		delete []m_pbOutputConfigData;
	if (m_pClearestVideoData != NULL)
		delete []m_pClearestVideoData;
	if (m_pHeadData != NULL)
		delete []m_pHeadData;
	if (m_pVideoData != NULL)
		delete []m_pVideoData;
	if (m_pAudioData != NULL)
		delete []m_pAudioData;
}

VO_U32 CFileSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	if (m_hFile != NULL)
		CloseSource ();

	voCAutoLock lock (&m_csRead);

	m_bForceClosed = VO_FALSE;

	if (LoadLib (m_hInst) == 0)
		return VO_ERR_FAILED;

	m_paramOpen.pLibOP = m_pLibOP;

	m_Source.nFlag = nType;
	m_Source.pSource = pSource;
	m_Source.nOffset = nOffset;
	m_Source.nLength = nLength;
	m_Source.nMode = VO_FILE_READ_ONLY;
#ifdef _MAC_OS
	m_Source.nReserve = (unsigned long)m_pIFileBasedDrm;
#else
	m_Source.nReserve = (VO_U32)m_pIFileBasedDrm;
#endif

	if(m_bForThumbnail)
		m_paramOpen.nFlag |= VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL;

	VO_U32 nRC = m_funFileRead.Open (&m_hFile, &m_paramOpen);

	if (m_bForceClosed)
		return VO_ERR_FAILED;

	if(VO_ERR_SOURCE_OK != nRC)
	{
		VOLOGE("m_funFileRead.Open failed. 0X%08X", (unsigned int)nRC);
		return nRC;
	}

	if (m_nFormat == VO_FILE_FFAUDIO_MIDI)
	{
		VO_TCHAR		szPath[1024];
		VO_CHAR *	pPath = NULL;
		if (m_pConfig != NULL)
			pPath = m_pConfig->GetItemText ((char*)"Source_File_MIDI", (char*)"ConfigFile");
		if (pPath != NULL)
		{
#ifdef _WIN32
			memset (szPath, 0, 1024 * sizeof (VO_TCHAR));
			MultiByteToWideChar (CP_ACP, 0, pPath, -1, szPath, 1024);
#else
			strcpy (szPath, pPath);
#endif // _WIN32
		}
		else
		{
			voOS_GetModuleFileName (m_hInst, (VO_PTCHAR)szPath, 1024);
			VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
			if (pPos != NULL)
			{
				*(pPos) = 0;
				vostrcat (szPath, _T("\\voMidi.cfg"));
			}
			else
			{
				pPos = vostrrchr (szPath, _T('/'));
				if (pPos != NULL)
					*(pPos) = 0;
				vostrcat (szPath, _T("/voMidi.cfg"));
			}
		}

		SetSourceParam (VO_PID_MIDI_PATCHDIR, szPath);
	}

	nRC = m_funFileRead.GetSourceInfo (m_hFile, &m_filInfo);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.GetSourceInfo failed. 0X%08X", (unsigned int)nRC);
		return nRC;
	}

	AllocOutputConfigDatas();

	VO_SOURCE_TRACKINFO	trkInfo;
	for(VO_U32 i = 0; i < m_filInfo.Tracks; i++)
	{
		nRC = m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
		if (trkInfo.Duration > m_nDuration)
			m_nDuration = trkInfo.Duration;

		if(VO_SOURCE_TT_VIDEO == trkInfo.Type && m_nVideoTrack == -1)
		{
			m_nVideoTrack = i;

			if(VO_VIDEO_CodingH264 == trkInfo.Codec && m_bAVC2NAL)
			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				if (nRC == VO_ERR_NONE && memcmp((char*)&nFourCC, (char*)"AVC1", 4) == 0)
				{
					m_bIsAVC = VO_TRUE;
					nRC = GetTrackParam (i, VO_PID_SOURCE_MAXSAMPLESIZE, &m_nFrameSize);
				}
			}
			else if(VO_VIDEO_CodingH265 == trkInfo.Codec )
			{
				nRC = GetTrackParam (i, VO_PID_SOURCE_MAXSAMPLESIZE, &m_nFrameSize);
			}
		}
		else if(VO_SOURCE_TT_AUDIO == trkInfo.Type && m_nAudioTrack == -1)
		{
			m_nAudioTrack = i;

			if(VO_AUDIO_CodingAAC == trkInfo.Codec && m_bAudioAACAVC == (VO_BOOL)VOAAC_ADTS)
			{
				m_bAudioAACAVC = VO_FALSE;
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				if (nFourCC == 0XA106 || nFourCC == 0XFF)
				{
					m_funFileRead.GetTrackInfo (m_hFile, i, (VO_SOURCE_TRACKINFO *)&m_sAudioTrackInfo);

					m_bAudioAACAVC = VO_TRUE;
					GetTrackParam (i, VO_PID_SOURCE_MAXSAMPLESIZE, &m_nAudioSize);
					if (m_pAudioData == NULL)
						m_pAudioData = new VO_BYTE[m_nAudioSize * 2];
				}
			}
		}
	}

	/*
	// East write sample code to support multi-track
	// If you not use first A/V track, please use following code
	// un-prepare tracks
	VO_BOOL bPrepare = VO_FALSE;
	SetSourceParam(VO_PID_SOURCE_PREPARETRACKS, &bPrepare);

	VO_SOURCE_SELTRACK sSelTrack;
	// reset select tracks
	sSelTrack.bInUsed = VO_FALSE;
	sSelTrack.nIndex = VO_ALL;
	SetSourceParam(VO_PID_SOURCE_SELTRACK, &sSelTrack);

	// re-select tracks
	sSelTrack.bInUsed = VO_TRUE;
	if(m_nVideoTrack != -1)
	{
		sSelTrack.nIndex = m_nVideoTrack;
		SetSourceParam(VO_PID_SOURCE_SELTRACK, &sSelTrack);
	}

	if(m_nAudioTrack != -1)
	{
		sSelTrack.nIndex = m_nAudioTrack;
		SetSourceParam(VO_PID_SOURCE_SELTRACK, &sSelTrack);
	}

	// prepare tracks
	bPrepare = VO_TRUE;
	SetSourceParam(VO_PID_SOURCE_PREPARETRACKS, &bPrepare);
	*/

	nRC = CheckCodecLegal();
	if(VO_ERR_NONE != nRC)
		return nRC;

	return VO_ERR_NONE;
}

VO_U32 CFileSource::CloseSource (void)
{
	if(m_bForceClosed)
		return 0;

	voCAutoLock lock (&m_csRead);
	m_bForceClosed = VO_TRUE;

	if (m_hFile != NULL)
	{
		m_funFileRead.Close (m_hFile);
		m_hFile = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32	CFileSource::Start(void)
{
	if(m_hFile != NULL)
	{
		m_funFileRead.SetSourceParam(m_hFile , VO_PID_COMMON_START , 0);
	}

	return CBaseSource::Start();
}

VO_U32	CFileSource::Pause(void)
{
	if(m_hFile != NULL)
	{
		m_funFileRead.SetSourceParam(m_hFile , VO_PID_COMMON_PAUSE , 0);
	}

	return CBaseSource::Pause();
}

VO_U32	CFileSource::Stop(void)
{
	if(m_hFile != NULL)
	{
		m_funFileRead.SetSourceParam(m_hFile , VO_PID_COMMON_STOP , 0);
	}

	return CBaseSource::Stop();
}

VO_U32 CFileSource::GetSourceInfo (VO_PTR pSourceInfo)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_SOURCE_INFO * pFileInfo = (VO_SOURCE_INFO *)pSourceInfo;

	return 	m_funFileRead.GetSourceInfo (m_hFile, pFileInfo);
}

VO_U32 CFileSource::GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackInfo (m_hFile, nTrack, (VO_SOURCE_TRACKINFO *)pTrackInfo);
	if(VO_ERR_NONE != nRC)
		return nRC;

	VO_SOURCE_TRACKINFO * pTrack = (VO_SOURCE_TRACKINFO *)pTrackInfo;

	if (m_bAVC2NAL && m_bIsAVC && (int)nTrack == m_nVideoTrack)
	{
		if(pTrack->HeadData && pTrack->HeadSize > 0)
		{
			seq_parameter_set_rbsp_t seqParSet;
			VO_PBYTE pHeaderCopy = new VO_BYTE[pTrack->HeadSize];
			memcpy(pHeaderCopy, pTrack->HeadData, pTrack->HeadSize);
			if(-1 != ProcessSPS_AVC(pHeaderCopy, pTrack->HeadSize, &seqParSet) && ConvertHeadData(pTrack->HeadData, pTrack->HeadSize))
			{
				pTrack->HeadData = m_pHeadData;
				pTrack->HeadSize = m_nHeadSize;
			}
			delete [] pHeaderCopy;
		}
	}
	else if(m_bAVC2NAL && m_bIsHEVC && (int)nTrack == m_nVideoTrack)
	{
		
		if(pTrack->HeadData && pTrack->HeadSize > 0)
		{
			VO_BYTE *p = pTrack->HeadData;
 
			if(p != NULL && (( p[0] == 0 &&p[1] == 0 &&p[2] ==0 &&p[3] == 1) || ( p[0] == 0 &&p[1] == 0 &&p[2] ==1)))
			{
				m_bAVC2NAL = VO_FALSE;
			}

			if(m_bAVC2NAL == VO_TRUE && ConvertHEVCHeadData(pTrack->HeadData, pTrack->HeadSize) == VO_TRUE)
			{
				pTrack->HeadData = m_pHeadData;
				pTrack->HeadSize = m_nHeadSize;
			}
		}
	}

	if (m_bAudioAACAVC && (int)nTrack == m_nAudioTrack)
	{
		VO_SOURCE_TRACKINFO * pTrack = (VO_SOURCE_TRACKINFO *)pTrackInfo;
		pTrack->HeadData = NULL;
		pTrack->HeadSize = 0;
	}

	return nRC;
}

VO_U32 CFileSource::GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackParam (m_hFile, nTrack, VO_PID_AUDIO_FORMAT, pAudioFormat);

	//permit 8 bit!!
	if(8 != pAudioFormat->SampleBits && 24 != pAudioFormat->SampleBits && 32 != pAudioFormat->SampleBits)
		pAudioFormat->SampleBits = 16;
	
	return nRC;
}

VO_U32 CFileSource::GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackParam (m_hFile, nTrack, VO_PID_VIDEO_FORMAT, pVideoFormat);

	return nRC;
}

VO_U32 CFileSource::GetFrameType (VO_U32 nTrack)
{
	return 0;
}

VO_U32 CFileSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;
	if (nTrack < 0 || nTrack >= m_filInfo.Tracks)
		return VO_ERR_INVALID_ARG;

	voCAutoLock lock (&m_csRead);

	VO_U32 nRC = VO_ERR_NONE;
	VO_SOURCE_SAMPLE* pSample = (VO_SOURCE_SAMPLE*)pTrackData;
	
	if (IsOutputConfigData(nTrack))
	{
		SetOutputConfigData(nTrack, VO_FALSE);

		VO_SOURCE_TRACKINFO trkInfo;
		nRC = GetTrackInfo (nTrack, &trkInfo);
		if (nRC == VO_ERR_NONE)
		{
			if (trkInfo.Type == VO_SOURCE_TT_AUDIO)
			{
				if (trkInfo.Codec == VO_AUDIO_CodingWMA || trkInfo.Codec == VO_AUDIO_CodingADPCM)
				{
					trkInfo.HeadSize = 0;
					nRC = GetTrackParam (nTrack, VO_PID_SOURCE_WAVEFORMATEX, &trkInfo.HeadData);
					if (nRC == VO_ERR_NONE)
					{
						VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)trkInfo.HeadData;
						trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
					}
				}
				// OGG we need use sequence header as first sample, East 20110915
				if (trkInfo.Codec == VO_AUDIO_CodingQCELP13 || trkInfo.Codec == VO_AUDIO_CodingEVRC || trkInfo.Codec == VO_AUDIO_CodingRA || trkInfo.Codec == VO_AUDIO_CodingPCM)
					trkInfo.HeadSize = 0;
			}
			else if (trkInfo.Type == VO_SOURCE_TT_VIDEO)
			{
				
				if (trkInfo.Codec == VO_VIDEO_CodingWMV || trkInfo.Codec == VO_VIDEO_CodingVC1)
				{
					
					// Fix the Acer WMV3 can not get the thumnail. It is Finally that find the key reason 
#if !defined (_ACER)
					VO_U32 nFourCC = 0;
					nRC = GetTrackParam (nTrack, VO_PID_SOURCE_CODECCC, &nFourCC);	

					if(memcmp((char*)&nFourCC, (char*)"WVC1", 4) != 0 || m_nWVC1Decoder != 1)	// if WVC1 and QCM hardware decoder, use sequence data directly, East 20110408
					{
						nRC = GetTrackParam (nTrack, VO_PID_SOURCE_BITMAPINFOHEADER, &trkInfo.HeadData);
						if (nRC == VO_ERR_NONE)
						{
							VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)trkInfo.HeadData;
							trkInfo.HeadSize = pBmpInfo->biSize;
						}
					}
#endif 					
				}				
				//Real Decoder don't want to get sequence header twice!!
				else if (trkInfo.Codec == VO_VIDEO_CodingRV)
				{
					trkInfo.HeadSize = 0;
				}
			}
		
			if (trkInfo.HeadSize > 0)
			{
				pSample->Buffer = trkInfo.HeadData;
				pSample->Size = 0X80000000 + trkInfo.HeadSize;
				pSample->Time = 0;
				pSample->Duration = 0;
				pSample->Flag = VO_SOURCE_SAMPLE_FLAG_CODECCONFIG;

				return VO_ERR_NONE;
			}
		}
	}

	if(m_bPlayClearestVideo && (int)nTrack == m_nVideoTrack)
	{
		VO_S32 nFrames = m_nThumbFrameNum>0 ? 1: 20;
		nRC = SetTrackParam(m_nVideoTrack, VO_PID_SOURCE_THUMBNAILSCANRANGE, &nFrames);
		if(nRC!= VO_ERR_NONE)
		{
			VOLOGE("Set VO_PID_SOURCE_THUMBNAILSCANRANGE fail 0X%X", (unsigned int)nRC);
			return nRC;
		}

		nRC = GetTrackParam(m_nVideoTrack, VO_PID_SOURCE_GETTHUMBNAIL, pSample);
		m_nThumbFrameNum++;
	}
	else
	{
		// modified by gtxia 2011-3-24
		// for fixing the issue that some a sample length is zero, it will cause the SF abort with hardware codec 
		do{
			nRC = m_funFileRead.GetSample (m_hFile, nTrack, pSample);
		} while(pSample->Size == 0 && nRC == VO_ERR_NONE);
	}
	
	VOLOGR ("Size %d, Time %d, Track %d Result 0X%08X", (pSample->Size & 0X7FFFFFFF), (int)pSample->Time, nTrack, nRC);

	if (m_bAVC2NAL && (int)nTrack == m_nVideoTrack && (m_bIsAVC || m_bIsHEVC))
	{
		if (nRC == VO_ERR_NONE)
		{
			// if error frame, we will not send to video decoder since some HW decoder will crash, East 20110708
			if(VO_TRUE == ConvertData (pSample->Buffer, pSample->Size & 0X7FFFFFFF))
			{
				if (m_pVideoData != NULL)
				{
					pSample->Buffer = m_pVideoData;
					pSample->Size = m_nVideoSize;
				}
			}
			else
				nRC = VO_ERR_RETRY;
		}
	}
	
	if (m_bAudioAACAVC && (int)nTrack == m_nAudioTrack)
	{
		unsigned int nHeadSize = m_nAudioSize;
		Config2ADTSHeader (m_sAudioTrackInfo.HeadData, m_sAudioTrackInfo.HeadSize, pSample->Size & 0X7FFFFFFF, m_pAudioData, &nHeadSize);
		memcpy (m_pAudioData + nHeadSize, pSample->Buffer, pSample->Size & 0X7FFFFFFF);
		pSample->Size += nHeadSize;
		pSample->Buffer = m_pAudioData;
	}

	return nRC;
}

VO_U32 CFileSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	voCAutoLock lock (&m_csRead);

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.SetPos (m_hFile, nTrack, pPos);

	if (nRC != VO_ERR_NONE)
		VOLOGE ("m_funFileRead.SetPos, Pos %d, Result 0X%08X", (int)*pPos, (unsigned int)nRC);

	return nRC;
}

VO_U32 CFileSource::GetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funFileRead.GetSourceParam (m_hFile, nID, pValue);
}

VO_U32 CFileSource::SetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funFileRead.SetSourceParam (m_hFile, nID, pValue);
}

VO_U32 CFileSource::GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackParam (m_hFile, nTrack, nID, pValue);

	if (nID == VO_PID_SOURCE_CODECCC)
	{
		if (m_bAVC2NAL && m_bIsAVC && (int)nTrack == m_nVideoTrack)
		{
			VO_U32	nFourCC = 0;
			nFourCC = *(VO_U32 *)pValue;

			if(memcmp((char*)&nFourCC, (char*)"AVC1", 4) == 0)
				memcpy(pValue, (char*)"H264", 4);
		}
		else if (m_bAudioAACAVC && (int)nTrack == m_nAudioTrack)
		{
			*(VO_U32 *)pValue = 0xF7;
		}
	}

	return nRC;
}

VO_U32 CFileSource::SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funFileRead.SetTrackParam (m_hFile, nTrack, nID, pValue);
}

VO_BOOL CFileSource::ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (nHeadSize < 12)
		return VO_TRUE;

	if (m_pHeadData != NULL)
		return VO_TRUE;

	char* pData = (char *)pHeadData;
	int numOfPictureParameterSets;
	int configurationVersion = pData[0];
	int AVCProfileIndication = pData[1];
	int profile_compatibility = pData[2];
	int AVCLevelIndication  = pData[3];
	configurationVersion = configurationVersion;
	AVCProfileIndication = AVCProfileIndication;
	profile_compatibility = profile_compatibility;
	AVCLevelIndication = AVCLevelIndication;
	m_nNalLen =  (pData[4]&0x03)+1;
	int nNalLen = m_nNalLen;
	if (m_nNalLen == 3)
		m_nNalWord = 0X010000;
	if (m_nNalLen < 3)
	{
		m_pVideoData = new VO_BYTE[512 + m_nFrameSize];
		nNalLen = 4;
	}

	m_pHeadData = new VO_BYTE[512 + m_nFrameSize];
	m_nHeadSize = 0;

	int i = 0;
	int numOfSequenceParameterSets = pData[5]&0x1f;

	VO_U8 * pBuffer = (VO_U8*)pData+6;
	for (i=0; i< numOfSequenceParameterSets; i++)
	{
		int sequenceParameterSetLength = (pBuffer[0]<<8)|pBuffer[1];
		pBuffer+=2;

		memcpy (m_pHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
		m_nHeadSize += nNalLen;

		memcpy (m_pHeadData + m_nHeadSize, pBuffer, sequenceParameterSetLength);
		m_nHeadSize += sequenceParameterSetLength;

		pBuffer += sequenceParameterSetLength;
	}

	numOfPictureParameterSets = *pBuffer++;
	for (i=0; i< numOfPictureParameterSets; i++)
	{
		int pictureParameterSetLength = (pBuffer[0]<<8)|pBuffer[1];
		pBuffer+=2;

		memcpy (m_pHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
		m_nHeadSize += nNalLen;

		memcpy (m_pHeadData + m_nHeadSize, pBuffer, pictureParameterSetLength);
		m_nHeadSize += pictureParameterSetLength;

		pBuffer += pictureParameterSetLength;
	}

	return VO_TRUE;
}

VO_BOOL     CFileSource::ConvertHEVCHeadData(VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (nHeadSize < 12)
		return VO_TRUE;

	if (m_pHeadData != NULL)
		return VO_TRUE;

	VO_PBYTE pData = pHeadData;
	m_nNalLen =  (pData[21]&0x03)+1;
	VO_U32 nNalLen = m_nNalLen;
	m_nFrameSize = (m_nFrameSize > nHeadSize) ? m_nFrameSize : nHeadSize;

	if (m_nNalLen == 3)
		m_nNalWord = 0X010000;
	if (m_nNalLen < 3)
	{
		m_pVideoData = new VO_BYTE[512 + m_nFrameSize];
		nNalLen = 4;
	}

	VOLOGI("m_nFrameSize : %d " , m_nFrameSize);
	m_pHeadData = new VO_BYTE[512 + m_nFrameSize];
	m_nHeadSize = 0;

	VO_U8 numOfArrays = pData[22];
	pData += 23;
	if(numOfArrays)
	{
		for(int arrNum = 0; arrNum < numOfArrays; arrNum++)
		{
			VO_U8 nal_type = 0;
			nal_type = pData[0]&0x3F;
			pData += 1;
			switch(nal_type)
			{
			case 33://sps
				{

					VO_U16 numOfSequenceParameterSets = 0;
					numOfSequenceParameterSets = ((numOfSequenceParameterSets|pData[0]) << 8)|pData[1];
					pData += 2;
					for(int i = 0; i < numOfSequenceParameterSets; i++)
					{
						memcpy (m_pHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
						m_nHeadSize += nNalLen;
						VO_U16 sequenceParameterSetLength = pData[0];
						sequenceParameterSetLength = (sequenceParameterSetLength << 8)|pData[1];
						pData += 2;
						memcpy (m_pHeadData + m_nHeadSize, pData, sequenceParameterSetLength);
						m_nHeadSize += sequenceParameterSetLength;
						pData += sequenceParameterSetLength;

					}
				}
				break;
			case 34://pps
				{

					VO_U16 numofPictureParameterSets = pData[0];
					numofPictureParameterSets = (numofPictureParameterSets << 8)|pData[1];
					pData += 2;

					for(int i = 0; i < numofPictureParameterSets; i++)
					{
						memcpy (m_pHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
						m_nHeadSize += nNalLen;
						VO_U16 pictureParameterSetLength = pData[0];
						pictureParameterSetLength = (pictureParameterSetLength << 8)|pData[1];
						pData += 2;
						memcpy (m_pHeadData + m_nHeadSize, pData, pictureParameterSetLength);
						m_nHeadSize += pictureParameterSetLength;
						pData += pictureParameterSetLength;
					}
				}
				break;
			case 32: //aps
				{
					VO_U16 numofAdaptationParameterSets = pData[0];
					numofAdaptationParameterSets = (numofAdaptationParameterSets << 8)|pData[1];
					pData += 2;

					for(int i = 0; i < numofAdaptationParameterSets; i++)
					{
						memcpy (m_pHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
						m_nHeadSize += nNalLen;
						VO_U16 adaptationParameterSetLength = pData[0];
						adaptationParameterSetLength = (adaptationParameterSetLength << 8)|pData[1];
						pData += 2;
						memcpy (m_pHeadData + m_nHeadSize, pData, adaptationParameterSetLength);
						m_nHeadSize += adaptationParameterSetLength;
						pData += adaptationParameterSetLength;
					}
				}
				break;
			default://just skip the data block
				{
					VO_U16 numofskippingParameter = pData[0];
					numofskippingParameter = (numofskippingParameter << 8)|pData[1];
					pData += 2;
					for(int i = 0; i < numofskippingParameter; i++)
					{
						VO_U16 adaptationParameterSetLength = pData[0];
						adaptationParameterSetLength = (adaptationParameterSetLength << 8)|pData[1];
						pData += 2;
						pData += adaptationParameterSetLength;
					}

				}
				break;
			}
		}
	}
	return VO_TRUE;
}

VO_BOOL CFileSource::ConvertData (VO_PBYTE pData, VO_U32 nSize)
{
	if (m_pHeadData == NULL)
		return VO_TRUE;

	VO_PBYTE pBuffer = pData;
	VO_U32	 nFrameLen = 0;

	m_nVideoSize = 0;

	int i = 0;
	while (pBuffer - pData + m_nNalLen < nSize)
	{
		nFrameLen = *pBuffer++;
		for (i = 0; i < (int)m_nNalLen - 1; i++)
		{
			nFrameLen = nFrameLen << 8;
			nFrameLen += *pBuffer++;
		}

		if(nFrameLen > nSize)
			return VO_FALSE;

		if (m_nNalLen == 3 || m_nNalLen == 4)
		{
			memcpy ((pBuffer - m_nNalLen), &m_nNalWord, m_nNalLen);
		}
		else
		{
			memcpy (m_pVideoData + m_nVideoSize, &m_nNalWord, 4);
			m_nVideoSize += 4;
			memcpy (m_pVideoData + m_nVideoSize, pBuffer, nFrameLen);
			m_nVideoSize += nFrameLen;
		}

		pBuffer += nFrameLen;
	}

	return VO_TRUE;
}

int CFileSource::Config2ADTSHeader(/*in*/	unsigned char *config, 
		    					  /*in*/	unsigned int  conlen,
	  							  /*in*/	unsigned int  framelen,
	  							  /*in&out*/unsigned char *adtsbuf,
	  							  /*in&out*/unsigned int  *adtslen)
{
	int object, sampIdx, sampFreq, chanNum;
 
	if(config == NULL || conlen < 2 || adtsbuf == NULL || *adtslen < 7)
		return -1;
 
	object = config[0] >> 3;
	if(object > 5)
	{
		return -2;
	}
 
	if(object == 0) object += 1;
 
	object--;
 
	sampIdx = ((config[0] & 7) << 1) | (config[1] >> 7);
	if(sampIdx == 0x0f)
	{
		int idx;
 
		if(conlen < 5)
			return -3;
 
		sampFreq = ((config[1]&0x7f) << 17) | (config[2] << 9) | ((config[3] << 1)) | (config[4] >> 7);
 
		for (idx = 0; idx < 12; idx++) {
			if (sampFreq == AAC_SampRateTab[idx]) {
				sampIdx = idx;
				break;
			}
		}
 
		if (idx == 12)
			return -4;
 
		chanNum = (config[4]&0x78) >> 3;
	}
	else
	{
		chanNum = (config[1]&0x78) >> 3;
	}
 
	if(chanNum > 7)
		return -5;
 
	adtsbuf[0] = 0xFF; adtsbuf[1] = 0xF9;
 
	adtsbuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));
 
	framelen += 7;
 
	if(framelen > 0x1FFF)
		return -6;
 
	adtsbuf[3] = (chanNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
	adtsbuf[5] = ((framelen & 7) << 5) | 0x1F;
	adtsbuf[6] = 0xFC;	
 
	*adtslen = 7;
 
	return 0;
}


VO_U32 CFileSource::LoadLib (VO_HANDLE hInst)
{
#ifdef _SYMBIAN_LIB
	if (m_nFormat == VO_FILE_FFMOVIE_REAL)
		voGetRealReadAPI(&m_funFileRead, 0);
	else
		return 0;
#else
#ifdef _LIB
	if(m_nFormat == VO_FILE_FFSTREAMING_HTTPPD)
		voGetPDReadAPI(&m_funFileRead, 0);
	else if (m_nFormat == VO_FILE_FFMOVIE_REAL)
		voGetRealReadAPI(&m_funFileRead, 0);
	else
		return 0;
#else	//_LIB
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_nFormat == VO_FILE_FFMOVIE_AVI)
	{
		strcpy (m_szCfgItem, "Source_File_AVI");
		vostrcpy (m_szDllFile, _T("voAVIFR"));
		vostrcpy (m_szAPIName, _T("voGetAVIReadAPI"));

	}
	else if (m_nFormat == VO_FILE_FFMOVIE_MP4 || m_nFormat == VO_FILE_FFMOVIE_MOV)
	{
		strcpy (m_szCfgItem, "Source_File_MP4");
		vostrcpy (m_szDllFile, _T("voMP4FR"));
		vostrcpy (m_szAPIName, _T("voGetMP4ReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_ASF)
	{
		strcpy (m_szCfgItem, "Source_File_ASF");
		vostrcpy (m_szDllFile, _T("voASFFR"));
		vostrcpy (m_szAPIName, _T("voGetASFReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_MPG)
	{
		strcpy (m_szCfgItem, "Source_File_MPG");
		vostrcpy (m_szDllFile, _T("voMPGFR"));
		vostrcpy (m_szAPIName, _T("voGetMPGReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_TS)
	{
		strcpy (m_szCfgItem, "Source_File_TS");
		vostrcpy (m_szDllFile, _T("voTsParser"));
		vostrcpy (m_szAPIName, _T("voGetMTVReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_FLV)
	{
		strcpy (m_szCfgItem, "Source_File_FLV");
		vostrcpy (m_szDllFile, _T("voFLVFR"));
		vostrcpy (m_szAPIName, _T("voGetFLVReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_REAL)
	{
		strcpy (m_szCfgItem, "Source_File_REAL");
		vostrcpy (m_szDllFile, _T("voRealFR"));
		vostrcpy (m_szAPIName, _T("voGetRealReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFAUDIO_OGG)
	{
		strcpy (m_szCfgItem, "Source_File_OGG");
		vostrcpy (m_szDllFile, _T("voOGGFR"));
		vostrcpy (m_szAPIName, _T("voGetOGGReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFAUDIO_MIDI)
	{
		strcpy (m_szCfgItem, "Source_File_MIDI");
		vostrcpy (m_szDllFile, _T("voMidiFR"));
		vostrcpy (m_szAPIName, _T("voGetMidiReadAPI"));
	}
	/*	else if (m_nFormat == VO_FILE_FFAUDIO_AAC || m_nFormat == VO_FILE_FFAUDIO_AMR ||
	m_nFormat == VO_FILE_FFAUDIO_AWB || m_nFormat == VO_FILE_FFAUDIO_MP3 ||
	m_nFormat == VO_FILE_FFAUDIO_QCP || m_nFormat == VO_FILE_FFAUDIO_WAV)
	{
	strcpy (m_szCfgItem, "Source_File_AUDIO");
	vostrcpy (m_szDllFile, _T("voAudioFR"));
	vostrcpy (m_szAPIName, _T("voGetAudioReadAPI"));
	}*/
	else if (m_nFormat == VO_FILE_FFSTREAMING_RTSP || m_nFormat == VO_FILE_FFAPPLICATION_SDP)
	{
		strcpy (m_szCfgItem, "Source_Live_RTSP");
		vostrcpy (m_szDllFile, _T("voSrcRTSP"));
		vostrcpy (m_szAPIName, _T("voGetRTSPReadAPI"));
	}
	else if (m_nFormat == VO_FILE_FFSTREAMING_HTTPPD)
	{
		strcpy (m_szCfgItem, "Source_Live_HTTPPD");
		if (m_LiveSrcType == VO_LIVESRC_FT_WMS)
		{
			vostrcpy (m_szDllFile, _T("voSrcWMSP"));
			vostrcpy (m_szAPIName, _T("voGetWMSPReadAPI"));
		}
		else
		{
			vostrcpy (m_szDllFile, _T("voSrcPD"));
			vostrcpy (m_szAPIName, _T("voGetPDReadAPI"));
		}
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_CMMB)
	{
		strcpy (m_szCfgItem, "Source_File_CMMB");
		vostrcpy (m_szDllFile, _T("voCMMBParser"));
		vostrcpy (m_szAPIName, _T("voGetMTVReadAPI"));
	}
	else if( m_nFormat == VO_FILE_FFMOVIE_MKV )
	{
		strcpy(m_szCfgItem,"Source_File_MKV");
		vostrcpy( m_szDllFile , _T("voMKVFR") );
		vostrcpy( m_szAPIName , _T("voGetMKVReadAPI") );
	}
	else if( m_nFormat == VO_FILE_FFVIDEO_H264)
	{
		strcpy(m_szCfgItem,"Source_File_H264");
		vostrcpy( m_szDllFile , _T("voRawDataFR") );
		vostrcpy( m_szAPIName , _T("voGetH264RawDataParserAPI") );
	}
	else if( m_nFormat == VO_FILE_FFAUDIO_PCM)
	{
		strcpy(m_szCfgItem,"Source_File_PCM");
		vostrcpy( m_szDllFile , _T("voRawDataFR") );
		vostrcpy( m_szAPIName , _T("voGetPCMRawDataParserAPI") );
	}
	else
	{
		strcpy (m_szCfgItem, "Source_File_AUDIO");
		vostrcpy (m_szDllFile, _T("voAudioFR"));
		vostrcpy (m_szAPIName, _T("voGetAudioReadAPI"));
	}

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");
	}

#if defined _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#elif defined __SYMBIAN32__
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".dll"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#elif defined _MAC_OS
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".dylib"));
	
	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);	
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

#if defined __SYMBIAN32__
	m_pAPIEntry = (VO_PTR) m_cLibrary.Lookup(53);
#endif // _WIN32

	VOGETFILEREADAPI pAPI = (VOGETFILEREADAPI) m_pAPIEntry;
	pAPI (&m_funFileRead, 0);
#endif	//_LIB
#endif // _SYMBIAN_LIB

	return 1;
}

// check if codec can be supported
VO_U32 CFileSource::CheckCodecLegal()
{
	VO_U32 nRC = VO_ERR_NONE;
	VO_SOURCE_TRACKINFO	trkInfo;
	for(VO_U32 i = 0; i < m_filInfo.Tracks; i++)
	{
		nRC = m_funFileRead.GetTrackInfo(m_hFile, i, &trkInfo);
		if(VO_SOURCE_TT_VIDEO == trkInfo.Type || VO_SOURCE_TT_RTSP_VIDEO == trkInfo.Type)
		{
			VO_VIDEO_PROFILELEVEL pl;
			memset(&pl, 0, sizeof(pl));
			VO_U32 nRC = GetTrackParam(i, VO_PID_SOURCE_VIDEOPROFILELEVEL, &pl);
			if(VO_ERR_NONE == nRC)
			{
				VOLOGI("VO_PID_SOURCE_VIDEOPROFILELEVEL profile %d, level %d", pl.Profile, pl.Level);
			}

			if(m_pConfig != NULL)
			{
				char szCfgItem[128];
				if (trkInfo.Codec== VO_VIDEO_CodingMPEG4)
					strcpy (szCfgItem, "Video_Dec_MPEG4");
				else if (trkInfo.Codec== VO_VIDEO_CodingH263)
					strcpy (szCfgItem, "Video_Dec_H263");
				else if (trkInfo.Codec== VO_VIDEO_CodingS263)
					strcpy (szCfgItem, "Video_Dec_S263");
				else if (trkInfo.Codec== VO_VIDEO_CodingH264)
					strcpy (szCfgItem, "Video_Dec_H264");
				else if (trkInfo.Codec== VO_VIDEO_CodingWMV)
					strcpy (szCfgItem, "Video_Dec_WMV");
				else if (trkInfo.Codec== VO_VIDEO_CodingVC1)
					strcpy (szCfgItem, "Video_Dec_VC1");
				else if (trkInfo.Codec== VO_VIDEO_CodingRV)
					strcpy (szCfgItem, "Video_Dec_REAL");
				else if (trkInfo.Codec== VO_VIDEO_CodingMPEG2)
					strcpy (szCfgItem, "Video_Dec_MPEG2");
				else if (trkInfo.Codec== VO_VIDEO_CodingMJPEG)
					strcpy (szCfgItem, "Video_Dec_MJPEG");
				else if (trkInfo.Codec== VO_VIDEO_CodingDIVX)
					strcpy (szCfgItem, "Video_Dec_DIVX");
				else if (trkInfo.Codec== VO_VIDEO_CodingVP6)
					strcpy (szCfgItem, "Video_Dec_VP6");
				else if (trkInfo.Codec== VO_VIDEO_CodingVP8)
					strcpy (szCfgItem, "Video_Dec_VP8");
				else if(trkInfo.Codec== VO_VIDEO_CodingH265)
					strcpy (szCfgItem, "Video_Dec_H265");

				// check the max video size.
				VO_S32 nMaxWidth = m_pConfig->GetItemValue (szCfgItem, (char*)"MaxWidth", 0);
				VO_S32 nMaxHeight = m_pConfig->GetItemValue (szCfgItem,(char*)"MaxHeight", 0);
#if defined(_ACER_A110)
				VO_U32 nVMaxBitrate = m_pConfig->GetItemValue (szCfgItem,(char*)"VMaxBitrate", 0);
#endif
				if(nMaxWidth > 0 && nMaxHeight > 0)
				{
					VO_VIDEO_FORMAT fmtVideo;
					memset (&fmtVideo, 0, sizeof (VO_VIDEO_FORMAT));
					nRC = m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
#if defined(_ACER_A110)
					VO_U32 nVideoBitrate = 0;
					nRC = m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_SOURCE_BITRATE, &nVideoBitrate);
					
					VOLOGI("get the nVideoBitrate from file is %d\n", nVideoBitrate);
					VOLOGI("fmtVideo.Width = %d, fmtVideo.Height = %d\n", fmtVideo.Width, fmtVideo.Height);
					if (((fmtVideo.Width >= nMaxWidth || fmtVideo.Height >= nMaxHeight) && nVideoBitrate*8 > nVMaxBitrate) || (fmtVideo.Width > nMaxWidth || fmtVideo.Height > nMaxHeight))
					{
						VOLOGI ("Unsupported width %d, height %d, maxwidth %d, maxheight %d", (int)fmtVideo.Width, (int)fmtVideo.Height, (int)nMaxWidth, (int)nMaxHeight);
						VOLOGI ("Unsupported Bitrate %d(byte/s), nVMaxbitrate %d(bit/s)", (int)nVideoBitrate, (int)nVMaxBitrate);
						return VO_ERR_SOURCE_CODECUNSUPPORT;
					}
#else
					if (fmtVideo.Width > nMaxWidth || fmtVideo.Height > nMaxHeight)
					{
						VOLOGE ("Unsupported width %d, height %d, maxwidth %d, maxheight %d", (int)fmtVideo.Width, (int)fmtVideo.Height, (int)nMaxWidth, (int)nMaxHeight);
						return VO_ERR_SOURCE_CODECUNSUPPORT;
					}
#endif
				}

				//check RTSP resolution control
				if( VO_SOURCE_TT_RTSP_VIDEO == trkInfo.Type)
				{
					nMaxWidth = m_pConfig->GetItemValue ((char*)"RTSP_Resolution_Ctrl", (char*)"MaxWidth", 0);
					nMaxHeight = m_pConfig->GetItemValue ((char*)"RTSP_Resolution_Ctrl", (char*)"MaxHeight", 0);
					if(nMaxWidth > 0 && nMaxHeight > 0)
					{
						VO_VIDEO_FORMAT fmtVideo;
						memset (&fmtVideo, 0, sizeof (VO_VIDEO_FORMAT));
						nRC = m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
						if (fmtVideo.Width > nMaxWidth || fmtVideo.Height > nMaxHeight)
						{
							VOLOGE ("Unsupported RTSP resolution width %d, height %d, maxwidth %d, maxheight %d", (int)fmtVideo.Width, (int)fmtVideo.Height, (int)nMaxWidth, (int)nMaxHeight);
							return VO_ERR_SOURCE_CODECUNSUPPORT;
						}
					}
				}

				//check the max video bitrate.
				VO_S32 nMaxBitrate = m_pConfig->GetItemValue(szCfgItem, (char*)"MaxBitrate", 0);
				if(nMaxBitrate > 0)
				{
					VO_U32 nBitrate = 0;
					nRC = GetTrackParam(i, VO_PID_SOURCE_BITRATE, &nBitrate);
					if(VO_ERR_NONE == nRC && nBitrate > 0)
					{
						// History reason, VO_PID_SOURCE_BITRATE is for Byte/Sec
						if(nBitrate * 8 > (VO_U32)nMaxBitrate)
						{
							VOLOGE ("Unsupported bitrate %d(byte/s), maxbitrate %d(bit/s)", (int)nBitrate, (int)nMaxBitrate);
							return VO_ERR_SOURCE_CODECUNSUPPORT;
						}
					}
					else
					{
						// Can not get track bitrate, use file bitrate
						if(VO_SOURCE_TYPE_RTSP == m_nSourceType)
						{
							VOStreamingInfo sStreamingInfo;
							nRC = GetSourceParam(VOID_STREAMING_INFO, &sStreamingInfo);
							if(VO_ERR_NONE == nRC && sStreamingInfo.clip_bitrate * 1024LL > nMaxBitrate)
							{
								VOLOGE ("Unsupported bitrate %d(kbit/s), maxbitrate %d(bit/s)", (int)sStreamingInfo.clip_bitrate, (int)nMaxBitrate);
								return VO_ERR_SOURCE_CODECUNSUPPORT;
							}
						}
						else
						{
							VO_S64 llFileSize = 0;
							nRC = GetSourceParam(VO_PID_SOURCE_FILESIZE, &llFileSize);
							if(VO_ERR_NONE == nRC && m_filInfo.Duration > 0 && llFileSize * 8000 / m_filInfo.Duration > nMaxBitrate)
							{
								VOLOGE ("Unsupported filesize %lld, duration %d maxbitrate %d(bit/s)", llFileSize, (int)m_filInfo.Duration, (int)nMaxBitrate);
								return VO_ERR_SOURCE_CODECUNSUPPORT;
							}
						}
					}
				}
			}

			if(VO_VIDEO_CodingH264 == trkInfo.Codec)
			{
				if(m_bAVC2NAL)
				{
					VO_U32 nFourCC = 0;
					nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
					if (nRC == VO_ERR_NONE && memcmp((char*)&nFourCC, (char*)"AVC1", 4) == 0)
					{
						if (trkInfo.HeadSize  > 6)
						{
							//0x1	BaseLine
							//0x2	Main
							//0x4	High
							//0x8	Extend
							//0xF	All
							int nProfile = 0;
							if (m_pConfig != NULL)
							{
								char * pProfile = m_pConfig->GetItemText ((char*)"Video_Dec_H264", (char*)"Profile", (char*)"BaseLine");
								if (pProfile != NULL)
								{
									if (!strcmp (pProfile, "BaseLine"))
										nProfile = 0x1;
									else if (!strcmp (pProfile, "Main"))
										nProfile = 0x2;
									else if (!strcmp (pProfile, "High"))
										nProfile = 0x4;
									else if (!strcmp (pProfile, "Extent"))
										nProfile = 0x8;
									else if (!strcmp (pProfile, "All"))
										nProfile = 0xF;
								}
							}

							// we only supports baseline profile. (0X42 BaseLine, 0X4D Main, 0X58 extent 0X64 High)
							if(trkInfo.HeadData[1] == 0x4d && !(nProfile & 0x2))
								return VO_ERR_SOURCE_CODECUNSUPPORT;
							else if(trkInfo.HeadData[1] == 0x58 && !(nProfile & 0x8))
								return VO_ERR_SOURCE_CODECUNSUPPORT;
							else if(trkInfo.HeadData[1] == 0x64 && !(nProfile & 0x4))
								return VO_ERR_SOURCE_CODECUNSUPPORT;
						}
					}
				}
			}
			else if(VO_VIDEO_CodingWMV == trkInfo.Codec || VO_VIDEO_CodingVC1 == trkInfo.Codec)

			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				if(memcmp((char*)"MPG4", (char*)&nFourCC, 4) == 0)
				{
					VOLOGE ("The WMV codec %04X was not support!", (unsigned int)nFourCC);
					return VO_ERR_SOURCE_CODECUNSUPPORT;
				}
			}
			else if(VO_VIDEO_CodingRV == trkInfo.Codec)
			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				//RV10(RV5, RV6), RV20(RV7), we block them, East, 2010/05/25
				if((memcmp((char*)"01VR", (char*)&nFourCC, 4) == 0) || (memcmp((char*)"02VR", (char*)&nFourCC, 4) == 0))
				{
					VOLOGE ("The RV codec %04X was not support!", (unsigned int)nFourCC);
					return VO_ERR_SOURCE_CODECUNSUPPORT;
				}
			}
			else if(VO_VIDEO_CodingH265 == trkInfo.Codec)
			{
				m_bIsHEVC = VO_TRUE;
			}
		}
		else if(VO_SOURCE_TT_AUDIO == trkInfo.Type || VO_SOURCE_TT_RTSP_AUDIO == trkInfo.Type)
		{
			if(VO_AUDIO_CodingRA == trkInfo.Codec)
			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				//Now only 'raac', 'racp' and 'cook' are supported, East, 2010/06/01
				if((memcmp((char*)"caar", (char*)&nFourCC, 4) != 0)&& (memcmp((char*)"pcar", (char*)&nFourCC, 4) != 0) && (memcmp((char*)"kooc", (char*)&nFourCC, 4) != 0)
					&& 0x2004 != nFourCC && 0x2006 != nFourCC && 0x2007 != nFourCC )
				{
					VOLOGE ("The RA codec %04X was not support!", (unsigned int)nFourCC);
					return VO_ERR_SOURCE_CODECUNSUPPORT;
				}

				VO_AUDIO_FORMAT fmtAudio;
				memset(&fmtAudio, 0, sizeof(fmtAudio));
				nRC = GetAudioFormat(i, &fmtAudio);
				if(fmtAudio.Channels > 2)
				{
					VOLOGE ("The RA codec multi channels %d was not support!", (int)fmtAudio.Channels);
					return VO_ERR_SOURCE_CODECUNSUPPORT;
				}
			}
			else if(VO_AUDIO_CodingWMA == trkInfo.Codec)
			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				//Speech and RT Voice not supported now, East, 2010/08/10
				//0x000B Microsoft Windows Media RT Voice 
				if(0xA == nFourCC || 0xB == nFourCC)
				{
					VOLOGE ("The WMA codec %04X was not support!", (unsigned int)nFourCC);
					return VO_ERR_SOURCE_CODECUNSUPPORT;
				}
			}
			else if(VO_AUDIO_CodingAAC == trkInfo.Codec)
			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				if(nFourCC == 0XA106 || nFourCC == 0XFF)
				{
					VO_PBYTE pHeadData = trkInfo.HeadData;
					VO_U32 nHeadSize = trkInfo.HeadSize;
					if(!pHeadData || !nHeadSize)
					{
						VO_SOURCE_SAMPLE sample;
						memset(&sample, 0, sizeof(sample));
						nRC = GetTrackParam(i, VO_PID_SOURCE_FIRSTFRAME, &sample);
						if(nRC == VO_ERR_NONE)
						{
							pHeadData = sample.Buffer;
							nHeadSize = sample.Size & 0x7FFFFFFF;
						}
					}

					if(nHeadSize > 2 && pHeadData[0] == 0xFF && (pHeadData[1] & 0xF0) == 0xF0 && (pHeadData[2] >> 6) == 0)
					{
						VOLOGE ("The AAC main profile was not support!");
						return VO_ERR_SOURCE_CODECUNSUPPORT;
					}
				}
			}
		}
	}
	return VO_ERR_NONE;
}

VO_U32 CFileSource::GetNearKeyframe(VO_SOURCE_NEARKEYFRAME* pNearKeyframe)
{
	if(m_nVideoTrack < 0)
		return VO_ERR_NOT_IMPLEMENT;

	return GetTrackParam(m_nVideoTrack, VO_PID_SOURCE_NEARKEYFRAME, pNearKeyframe);
}

VO_BOOL CFileSource::AllocOutputConfigDatas()
{
	if(m_pbOutputConfigData)
		delete [] m_pbOutputConfigData;

	m_pbOutputConfigData = new VO_BOOL[m_filInfo.Tracks];
	if(!m_pbOutputConfigData)
		return VO_FALSE;

	for (VO_U32 i = 0; i < m_filInfo.Tracks; i++)
		m_pbOutputConfigData[i] = VO_TRUE;

	return VO_TRUE;
}

VO_BOOL CFileSource::IsOutputConfigData(VO_U32 nTrack)
{
	if(!m_pbOutputConfigData)
		return VO_FALSE;

	return m_pbOutputConfigData[nTrack];
}

VO_U32 CFileSource::SetOutputConfigData(VO_U32 nTrack, VO_BOOL bOutputConfigData)
{
	if(!m_pbOutputConfigData)
		return VO_ERR_OUTOF_MEMORY;

	m_pbOutputConfigData[nTrack] = bOutputConfigData;
	return VO_ERR_NONE;
}

char* CFileSource::GetMetaDataText (int nIndex)
{
	//voCOMXAutoLock lock (&mMutex);

	if (m_hFile == NULL)
		return NULL;

	memset (m_szMetaData, 0, sizeof (m_szMetaData));
	memset (&m_strMetaText, 0, sizeof (MetaDataString));

	switch (nIndex)
	{
	case METADATA_KEY_CD_TRACK_NUMBER:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_ALBUM:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_ALBUM, &m_strMetaText);
		break;

	case METADATA_KEY_ARTIST:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_ARTIST, &m_strMetaText);
		break;

	case METADATA_KEY_AUTHOR:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_OWNER, &m_strMetaText);
		break;

	case METADATA_KEY_COMPOSER:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_COMPOSER, &m_strMetaText);
		break;

	case METADATA_KEY_DATE:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_YEAR, &m_strMetaText);
		break;

	case METADATA_KEY_GENRE:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_GENRE, &m_strMetaText);
		break;

	case METADATA_KEY_TITLE:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_TITLE, &m_strMetaText);
		break;

	case METADATA_KEY_YEAR:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_DURATION:
		{
			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);
			//sprintf (m_szMetaData, "%d", filInfo.Duration);
		}
		break;

	case METADATA_KEY_NUM_TRACKS:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_IS_DRM_CRIPPLED:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_CONDUCTOR, &m_strMetaText);
		break;

	case METADATA_KEY_CODEC:
		{
			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);

			VO_SOURCE_TRACKINFO	trkInfo;
			for (VO_U32 i = 0; i < filInfo.Tracks; i++)
			{
				m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
				if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
				{
					strcat (m_szMetaData, "Audio ");

					char szAudio[128];
					if (trkInfo.Codec == VO_AUDIO_CodingPCM)
						strcpy (szAudio, "PCM");
					else if (trkInfo.Codec == VO_AUDIO_CodingADPCM)
						strcpy (szAudio, "ADPCM");
					else if (trkInfo.Codec == VO_AUDIO_CodingAMRNB)
						strcpy (szAudio, "AMR_NB");
					else if (trkInfo.Codec == VO_AUDIO_CodingAMRWB)
						strcpy (szAudio, "AMR_WB");
					else if (trkInfo.Codec == VO_AUDIO_CodingAMRWBP)
						strcpy (szAudio, "AMR_WB+");
					else if (trkInfo.Codec == VO_AUDIO_CodingQCELP13)
						strcpy (szAudio, "QCELP13");
					else if (trkInfo.Codec == VO_AUDIO_CodingEVRC)
						strcpy (szAudio, "EVRC");
					else if (trkInfo.Codec == VO_AUDIO_CodingAAC)
						strcpy (szAudio, "AAC");
					else if (trkInfo.Codec == VO_AUDIO_CodingAC3)
						strcpy (szAudio, "AC3");
					else if (trkInfo.Codec == VO_AUDIO_CodingFLAC)
						strcpy (szAudio, "FLAC");
					else if (trkInfo.Codec == VO_AUDIO_CodingMP3)
						strcpy (szAudio, "MP3");
					else if (trkInfo.Codec == VO_AUDIO_CodingOGG)
						strcpy (szAudio, "OGG");
					else if (trkInfo.Codec == VO_AUDIO_CodingWMA)
						strcpy (szAudio, "WMA");
					else if (trkInfo.Codec == VO_AUDIO_CodingRA)
						strcpy (szAudio, "RealAudio");
					else if (trkInfo.Codec == VO_AUDIO_CodingMIDI)
						strcpy (szAudio, "MIDI");
					else
						strcpy (szAudio, "Unknown");

					strcat (m_szMetaData, szAudio);
					strcat (m_szMetaData, " ");
				}
				else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
				{
					strcat (m_szMetaData, "Video ");

					char szVideo[128];
					if (trkInfo.Codec == VO_VIDEO_CodingMPEG2)
						strcpy (szVideo, "MPEG2");
					else if (trkInfo.Codec == VO_VIDEO_CodingH263)
						strcpy (szVideo, "H263");
					else if (trkInfo.Codec == VO_VIDEO_CodingS263)
						strcpy (szVideo, "S263");
					else if (trkInfo.Codec == VO_VIDEO_CodingMPEG4)
						strcpy (szVideo, "MPEG4");
					else if (trkInfo.Codec == VO_VIDEO_CodingH264)
						strcpy (szVideo, "H264");
					else if (trkInfo.Codec == VO_VIDEO_CodingWMV)
						strcpy (szVideo, "WMV");
					else if (trkInfo.Codec == VO_VIDEO_CodingRV)
						strcpy (szVideo, "RealVideo");
					else if (trkInfo.Codec == VO_VIDEO_CodingMJPEG)
						strcpy (szVideo, "MJPEG");
					else if (trkInfo.Codec == VO_VIDEO_CodingDIVX)
						strcpy (szVideo, "DIVX");
					else if (trkInfo.Codec == VO_VIDEO_CodingVP6)
						strcpy (szVideo, "VP6");
					else
						strcpy (szVideo, "Unknown");

					strcat (m_szMetaData, szVideo);
					strcat (m_szMetaData, " ");
				}
			}
		}
		break;

	case METADATA_KEY_RATING:
		{
			strcpy (m_szMetaData, "0");

			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);

			VO_SOURCE_TRACKINFO	trkInfo;
			for (VO_U32 i = 0; i < filInfo.Tracks; i++)
			{
				m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
				if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
				{
					VO_AUDIO_FORMAT fmtAudio;
					m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_AUDIO_FORMAT, &fmtAudio);
					//sprintf (m_szMetaData, "%d", fmtAudio.SampleRate);
				}
			}
		}
		break;

	case METADATA_KEY_COMMENT:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_COMMENT, &m_strMetaText);
		break;

	case METADATA_KEY_COPYRIGHT:
		m_funFileRead.GetSourceParam (m_hFile, VO_PID_METADATA_COPYRIGHT, &m_strMetaText);
		break;

	case METADATA_KEY_BIT_RATE:
		{
			VO_S64 llFileSize = 0;
			m_funFileRead.GetSourceParam (m_hFile, VO_PID_SOURCE_FILESIZE, &llFileSize);

			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);
			if (filInfo.Duration == 0)
			{
				strcpy (m_szMetaData, "0");
			}
			else
			{
				VO_U32 uBitRate = (llFileSize * 8) / (filInfo.Duration / 1000);
				//sprintf (m_szMetaData, "%d", uBitRate);
			}
		}
		break;

	case METADATA_KEY_FRAME_RATE:
		{
			strcpy (m_szMetaData, "0");

			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);

			VO_SOURCE_TRACKINFO	trkInfo;
			for (VO_U32 i = 0; i < filInfo.Tracks; i++)
			{
				m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
				if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
				{
					VO_U32 uFrameTime = 0;
					m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_SOURCE_FRAMETIME, &uFrameTime);
// 					if (uFrameTime > 0)
// 						sprintf (m_szMetaData, "%d", 100000 / uFrameTime);	//unit is 1/100ms
				}
			}
		}
		break;

// 	case METADATA_KEY_VIDEO_FORMAT:
// 		{
// 			if (m_nFileFormat == VO_FILE_FFUNKNOWN)
// 				m_nFileFormat = GetFileFormat (true);
// 
// 			if (m_nFileFormat == VO_FILE_FFMOVIE_MP4)
// 				strcpy (m_szMetaData, "MP4 (3GP)");
// 			else if (m_nFileFormat == VO_FILE_FFMOVIE_ASF)
// 				strcpy (m_szMetaData, "WMV (ASF)");
// 			else if (m_nFileFormat == VO_FILE_FFMOVIE_AVI)
// 				strcpy (m_szMetaData, "AVI");
// 			else if (m_nFileFormat == VO_FILE_FFMOVIE_REAL)
// 				strcpy (m_szMetaData, "REAL");
// 			else if (m_nFileFormat == VO_FILE_FFMOVIE_MPG)
// 				strcpy (m_szMetaData, "MPG");
// 			else
// 				strcpy (m_szMetaData, "Unknown");
// 		}
// 		break;

	case METADATA_KEY_VIDEO_HEIGHT:
		{
			strcpy (m_szMetaData, "0");

			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);

			VO_SOURCE_TRACKINFO	trkInfo;
			for (VO_U32 i = 0; i < filInfo.Tracks; i++)
			{
				m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
				if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
				{
					VO_VIDEO_FORMAT fmtVideo;
					m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
					//sprintf (m_szMetaData, "%d", fmtVideo.Height);
				}
			}
		}
		break;

	case METADATA_KEY_VIDEO_WIDTH:
		{
			strcpy (m_szMetaData, "0");

			VO_SOURCE_INFO	filInfo;
			m_funFileRead.GetSourceInfo (m_hFile, &filInfo);

			VO_SOURCE_TRACKINFO	trkInfo;
			for (VO_U32 i = 0; i < filInfo.Tracks; i++)
			{
				m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
				if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
				{
					VO_VIDEO_FORMAT fmtVideo;
					m_funFileRead.GetTrackParam (m_hFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
					//sprintf (m_szMetaData, "%d", fmtVideo.Width);
				}
			}
		}
		break;

	default:
		break;
	}

	if (m_strMetaText.pBuffer != NULL)
	{
		if ((m_strMetaText.dwFlags & 0XFF) == VO_METADATA_TE_ANSI)
		{
			const unsigned char* src = (const unsigned char *)m_strMetaText.pBuffer;
			char* dest = m_szMetaData;
			unsigned int uch;
			for (VO_U32 i = 0; i < m_strMetaText.dwBufferSize; i++)
			{
				uch = *src++;
				if (uch & 0x80)
				{
					*dest++ = (uch >> 6) | 0xc0;
					*dest++ = (uch & 0x3f) | 0x80;
				}
				else
				{
					*dest++ = uch;
				}
			}
		}
		else
		{
			memcpy (m_szMetaData, m_strMetaText.pBuffer, m_strMetaText.dwBufferSize);
		}
	}

	if (strlen (m_szMetaData) > 0)
		return m_szMetaData;
	else
		return NULL;
}