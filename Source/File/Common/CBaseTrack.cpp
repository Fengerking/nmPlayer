/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseTrack.cpp

Contains:	CBaseTrack class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CBaseTrack.h"
#include "fMacros.h"
#include "fCodec.h"
#include "fCC.h"
#include "fVideoHeadDataInfo.h"
#include "vome/OMX_Video.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseTrack::CBaseTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, VO_MEM_OPERATOR* pMemOp)
	: CvoBaseMemOpr(pMemOp)
	, m_nType(nType)
	, m_btStreamNum(btStreamNum)
	, m_dwDuration(dwDuration)
	, m_bInUsed(VO_FALSE)
	, m_bEndOfStream(VO_FALSE)
	, m_PlayMode(VO_SOURCE_PM_PLAY)
	, m_pSampleData(VO_NULL)
    , m_pThumbnailBuffer(0)
    , m_ThumbnailBufferSize(0)
    , m_ThumbnailBufferUsedSize(0)
    , m_ThumbnallScanCount(0)
	, m_pSeqHeadData (NULL)
	, m_nHeadSize (0)
	, m_nNalLen (4)
	, m_nNalWord (0X01000000)
	, m_pVideoData (NULL)
	, m_nVideoSize (0)
	, m_llLastTimeStamp(0)
{
	memset(m_strLanguage,0,16*sizeof(VO_CHAR));
}

CBaseTrack::~CBaseTrack()
{
	Uninit();

	if (m_pSeqHeadData != NULL)
		delete []m_pSeqHeadData;
	if (m_pVideoData != NULL)
		delete []m_pVideoData;
}

VO_U32 CBaseTrack::Init(VO_U32 nSourceOpenFlags)
{
	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::Uninit()
{
    if( m_pThumbnailBuffer )
        delete []m_pThumbnailBuffer;

    m_pThumbnailBuffer = 0;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::Prepare()
{
	if(!m_pSampleData)
	{
		VO_U32 dwMaxSampleSize = 0;
		GetMaxSampleSize(&dwMaxSampleSize);

		m_pSampleData = NEW_BUFFER(dwMaxSampleSize);
	}

	return m_pSampleData ? VO_ERR_SOURCE_OK : VO_ERR_OUTOF_MEMORY;
}

VO_U32 CBaseTrack::Unprepare()
{
	SAFE_MEM_FREE(m_pSampleData);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_SOURCE_THUMBNAILSCANRANGE:
		{
			if(VOTT_VIDEO != m_nType || !pParam)
				return VO_ERR_NOT_IMPLEMENT;

			m_ThumbnallScanCount = *(VO_S32 *)pParam;

			if( !m_pThumbnailBuffer )
			{
				VO_U32 size = 0;
				GetMaxSampleSize( &size );

				m_pThumbnailBuffer = new VO_BYTE[size];
				m_ThumbnailBufferSize = size;
				m_ThumbnailBufferUsedSize = 0;
			}
			return VO_ERR_SOURCE_OK;
		}
		break;
	default:
		break;
	}
	
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VO_PID_COMMON_HEADDATA:
		return GetHeadData((VO_CODECBUFFER*)pParam);

	case VO_PID_SOURCE_MAXSAMPLESIZE:
		return GetMaxSampleSize((VO_U32*)pParam);

	case VO_PID_SOURCE_CODECCC:
		return GetCodecCC((VO_U32*)pParam);

	case VO_PID_SOURCE_FIRSTFRAME:
		return GetFirstFrame((VO_SOURCE_SAMPLE*)pParam);

	case VO_PID_SOURCE_NEXTKEYFRAME:
		return GetNextKeyFrame((VO_SOURCE_SAMPLE*)pParam);

	case VO_PID_AUDIO_FORMAT:
		return GetAudioFormat((VO_AUDIO_FORMAT*)pParam);

	case VO_PID_VIDEO_FORMAT:
		{
			VO_VIDEO_FORMAT *pVideoFormat = (VO_VIDEO_FORMAT*)pParam;
			VO_U32 nRet = GetVideoFormat(pVideoFormat);
			if (nRet != VO_ERR_NONE)
				return nRet;

			GetVideoResolution(pVideoFormat);

			return VO_ERR_NONE;
		}

	case VO_PID_SOURCE_BITRATE:
		return GetBitrate((VO_U32*)pParam);

	case VO_PID_SOURCE_FRAMENUM:
		return GetFrameNum((VO_U32*)pParam);

	case VO_PID_SOURCE_FRAMETIME:
		return GetFrameTime((VO_U32*)pParam);

	case VO_PID_SOURCE_WAVEFORMATEX:
		return GetWaveFormatEx((VO_WAVEFORMATEX**)pParam);

	case VO_PID_SOURCE_BITMAPINFOHEADER:
		return GetBitmapInfoHeader((VO_BITMAPINFOHEADER**)pParam);

	case VO_PID_SOURCE_NEARKEYFRAME:
		return GetNearKeyFrame(((VO_SOURCE_NEARKEYFRAME*)pParam)->Time, &((VO_SOURCE_NEARKEYFRAME*)pParam)->PreviousKeyframeTime, &((VO_SOURCE_NEARKEYFRAME*)pParam)->NextKeyframeTime);

	case VO_PID_SOURCE_VIDEOPROFILELEVEL:
		return GetVideoProfileLevel((VO_VIDEO_PROFILELEVEL*)pParam);

	case VO_PID_SOURCE_ISINTERLACE:
		return IsVideoInterlace((VO_BOOL*)pParam);

	case VO_PID_VIDEO_S3D:
		{
			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;

			VO_U32 uFourcc = 0;
			VO_U32 nRes = GetCodecCC(&uFourcc);
			if(VO_ERR_SOURCE_OK != nRes)
				return nRes;

			if (fCodecGetVideoCodec(uFourcc) == VO_VIDEO_CodingH264)
			{
				VO_CODECBUFFER codecbuf = {0};
				nRes = GetHeadData(&codecbuf);
				if(VO_ERR_SOURCE_OK == nRes &&  codecbuf.Length) {
					if (FOURCC_avc1 == uFourcc || FOURCC_AVC1 == uFourcc)
					{
						if (ConvertHeadData(codecbuf.Buffer, codecbuf.Length))
						{
							codecbuf.Buffer = m_pSeqHeadData;
							codecbuf.Length = m_nHeadSize;
						}
					}
					
					if (getS3D_H264(&codecbuf, (VO_S3D_params*)pParam) == 0)
						return VO_ERR_SOURCE_OK;
				}

				VO_SOURCE_SAMPLE sample = {0};
				sample.Flag = VO_SOURCE_SAMPLE_FLAG_NOFRAMESKIPED;
				nRes = GetFirstFrame(&sample);
				if (VO_ERR_NONE != nRes || 0 == sample.Size)
					return nRes ? nRes : VO_ERR_BASE;

				VO_PBYTE pCopy = new VO_BYTE[sample.Size & 0X7FFFFFFF];
				memcpy(pCopy, sample.Buffer, sample.Size & 0X7FFFFFFF);

				codecbuf.Buffer = pCopy;
				codecbuf.Length = sample.Size & 0X7FFFFFFF;
				
				nRes = getS3D_H264(&codecbuf, (VO_S3D_params*)pParam);
				if (nRes == 0)
				{
					delete []pCopy;
					return VO_ERR_SOURCE_OK;
				}

				if (FOURCC_avc1 == uFourcc || FOURCC_AVC1 == uFourcc)
				{
					if(ConvertData (pCopy, sample.Size & 0X7FFFFFFF))
					{
						if (m_pVideoData != NULL)
						{
							codecbuf.Buffer	= m_pVideoData;
							codecbuf.Length	= m_nVideoSize;
						}
						else
						{
							codecbuf.Buffer = pCopy;
							codecbuf.Length = sample.Size & 0X7FFFFFFF;			
						}
					}
					else
					{
						delete []pCopy;
						return VO_ERR_BASE;
					}
				}

				nRes = getS3D_H264(&codecbuf, (VO_S3D_params*)pParam);

				delete []pCopy;
				return nRes ? VO_ERR_SOURCE_BASE : VO_ERR_SOURCE_OK;
			}
		}
		break;

	case VO_PID_VIDEO_ParamVideoAvc:
		{
			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;

			VO_U32 uFourcc = 0;
			VO_U32 nRes = GetCodecCC(&uFourcc);
			if(VO_ERR_SOURCE_OK != nRes)
				return nRes;

			if (fCodecGetVideoCodec(uFourcc) == VO_VIDEO_CodingH264)
			{
				VO_CODECBUFFER codecbuf = {0};
				nRes = GetHeadData(&codecbuf);
				if(VO_ERR_SOURCE_OK == nRes &&  codecbuf.Length) {
					if (getVIDEO_PARAM_AVCTYPE_H264(&codecbuf, (VO_VIDEO_PARAM_AVCTYPE*)pParam) == 0)
						return VO_ERR_SOURCE_OK;
				}

				VO_SOURCE_SAMPLE sample = {0};
				nRes = GetFirstFrame(&sample);
				if (VO_ERR_NONE != nRes || 0 == sample.Size)
					return nRes ? nRes : VO_ERR_BASE;

				codecbuf.Buffer = sample.Buffer;
				codecbuf.Length = sample.Size;

				return getVIDEO_PARAM_AVCTYPE_H264(&codecbuf, (VO_VIDEO_PARAM_AVCTYPE*)pParam);
			}
		}
        break;
    case VO_PID_SOURCE_GETTHUMBNAIL:
        {
            if(VOTT_VIDEO != m_nType)
                return VO_ERR_NOT_IMPLEMENT;

            VO_SOURCE_SAMPLE * ptr_sample = (VO_SOURCE_SAMPLE*)pParam;

            for( VO_S32 i = 0 ; i < m_ThumbnallScanCount ; i++ )
            {
                VO_SOURCE_SAMPLE sample;
				sample.Time = 0;
				VO_U32 ret = 0;
				while(1)
				{
					ret = GetSampleN( &sample );
					if ((VO_ERR_SOURCE_OK != ret) || (sample.Size & 0x80000000))
					{
						break;
					}
				}
                if(VO_ERR_SOURCE_OK == ret)
				{
					VO_U32 sample_size = (sample.Size & 0x7FFFFFFF);
					if( sample_size > (VO_U32)m_ThumbnailBufferUsedSize && sample_size <= (VO_U32)m_ThumbnailBufferSize )
					{
						memcpy( m_pThumbnailBuffer , sample.Buffer , sample_size );
						m_ThumbnailBufferUsedSize = sample_size;
					}
				} 
            }

            ptr_sample->Buffer = m_pThumbnailBuffer;
            ptr_sample->Size = m_ThumbnailBufferUsedSize;

            return VO_ERR_SOURCE_OK;
        }
        break;
	case VO_PID_VIDEO_MAXENCODERFRAMESIZE:
		{
			return VO_ERR_SOURCE_OK;
		}
		break;
	case VO_PID_SOURCE_TRACK_LANGUAGE:
		{
			if (pParam)
			{
				*(VO_CHAR **)pParam = m_strLanguage;
			}
			return VO_ERR_SOURCE_OK;
		}
		break;
	case VO_PID_VIDEO_GETTHUMBNAIL:
		{
			return GetThumbNail((VO_SOURCE_THUMBNAILINFO*)pParam);
		}
		break;
		
	default:

		break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseTrack::GetInfo(VO_SOURCE_TRACKINFO* pTrackInfo)
{
	if(!pTrackInfo)
		return VO_ERR_INVALID_ARG;

	pTrackInfo->Start = 0;
	pTrackInfo->Duration = GetDuration();

	if(VOTT_VIDEO == m_nType)
		pTrackInfo->Type = VO_SOURCE_TT_VIDEO;
	else if(VOTT_AUDIO == m_nType)
		pTrackInfo->Type = VO_SOURCE_TT_AUDIO;
	else if(VOTT_TEXT == m_nType)
		pTrackInfo->Type = VO_SOURCE_TT_SUBTITLE;

	VO_U32 nRes = GetCodec(&pTrackInfo->Codec);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	VO_CODECBUFFER cb = {0};
	if(VOTT_VIDEO == m_nType && pTrackInfo->Codec == VO_VIDEO_CodingMPEG4)//for mpeg4 short header
	{
		VO_VIDEO_HEADDATAINFO  Info;
		VO_U32 nRes;

		nRes = GetHeadData(&cb);
		if(VO_ERR_SOURCE_OK != nRes)
			return nRes;
		if (cb.Length) //default is MPEG4 if no head data
		{
			//modify by leon :issue 7396
			/* nRes = getResolution_Mpeg4(&cb, &Info);*/
			nRes =Mpeg4VideoInfo(&cb, &Info);
			if(VO_ERR_DEC_MPEG4_HEADER == nRes)
			{
				pTrackInfo->Codec = VO_VIDEO_CodingH263;
				pTrackInfo->HeadSize = 0;

				return VO_ERR_SOURCE_OK;
			}

			if (IsCorrectSequenceHeader(cb.Buffer, cb.Length, NULL) == VO_FALSE)
			{
				pTrackInfo->HeadSize = 0;

				return VO_ERR_SOURCE_OK;
			}
		}
	}

	memset( &cb, 0, sizeof(VO_CODECBUFFER) );
	nRes = GetHeadData(&cb);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	pTrackInfo->HeadData = cb.Buffer;
	pTrackInfo->HeadSize = cb.Length;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	return (VO_SOURCE_PM_PLAY == m_PlayMode) ? GetSampleN(pSample) : GetSampleK(pSample);
}

VO_U32 CBaseTrack::SetPos(VO_S64* pPos)
{
	return (VO_SOURCE_PM_PLAY == m_PlayMode) ? SetPosN(pPos) : SetPosK(pPos);
}

VO_U32 CBaseTrack::GetCodec(VO_U32* pCodec)
{
	VO_U32 dwFcc = 0;
	VO_U32 nRes = GetCodecCC(&dwFcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	if(pCodec)
	{
		if(VOTT_VIDEO == m_nType)
			*pCodec = fCodecGetVideoCodec(dwFcc);
		else if(VOTT_AUDIO == m_nType)
			*pCodec = fCodecGetAudioCodec((VO_U16)(dwFcc));
		else
			*pCodec = dwFcc;
	}	

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::SetDuration(VO_U32 dwDuration)
{
	m_dwDuration = dwDuration;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::SetPlayMode(VO_SOURCE_PLAYMODE PlayMode)
{
	m_PlayMode = PlayMode;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::SetInUsed(VO_BOOL bInUsed)
{
	m_bInUsed = bInUsed;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::SetEndOfStream(VO_BOOL bEndOfStream)
{
	m_bEndOfStream = bEndOfStream;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CBaseTrack::GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader)
{
	VO_U32 uFourcc = 0;
	VO_U32 nRes = GetCodecCC(&uFourcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	if (fCodecGetVideoCodec(uFourcc) ==  VO_VIDEO_CodingWMV)
	{
		VO_CODECBUFFER codecbuf = {0};
		VO_S32 nRes = GetHeadData(&codecbuf);
		if(VO_ERR_SOURCE_OK != nRes)
			return nRes;

		*ppBitmapInfoHeader = (VO_BITMAPINFOHEADER*)codecbuf.Buffer;
		if (codecbuf.Length >= (*ppBitmapInfoHeader)->biSize)
			return 0;

	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseTrack::FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_U32 dwSize)
{
	if(dwSize < 3)
		return -1;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwSize - 2;
	while(pHead < pTail)
	{
		if(pHead[0])
		{
			pHead += 2;
			continue;
		}

		if(pHead == pBuffer || *(pHead - 1))	//previous byte is not 0x00
		{
			//0x00 found
			if(pHead[1])
			{
				pHead += 2;
				continue;
			}

			//0x0000 found
			pHead++;	//point to second 0x00
		}

		while(!pHead[1] && pHead < pTail)
			pHead++;

		if(1 != pHead[1])
		{
			pHead += 2;
			continue;
		}

		return (pHead - pBuffer - 1);
	}

	if(pHead > pTail)
		return -1;

	if(*(pHead - 1))
		return -1;

	if(pHead[0])
		return -1;

	if(1 != pHead[1])
		return -1;

	return (pHead - pBuffer - 1);
}

VO_BOOL CBaseTrack::GetVideoHeadDataFromBuffer(VO_U32 nCodec, VO_PBYTE pBuffer, VO_U32 nBufferSize, VO_U32* pdwPos, VO_U32* pdwSize)
{
	if(!pBuffer || nBufferSize <= 0)
		return VO_FALSE;

	if(VO_VIDEO_CodingMPEG4 == nCodec)
	{
		VO_U32 dwPESHeaderPos = 0xffffffff;
		VO_U32 i = 0;
		VO_U32 dwSeqHeadPos = 0;
		VO_U32 dwSeqHeadSize = 0;
		while(i < nBufferSize - 3)
		{
			dwPESHeaderPos = FindPESHeaderInBuffer(pBuffer + i, nBufferSize - i);
			if(0xffffffff == dwPESHeaderPos)
				break;

			i += (dwPESHeaderPos + 3);
			if(i >= nBufferSize)
				break;

			if(0xB6 == pBuffer[i])
			{
				dwSeqHeadSize = i - 3;
				break;
			}

			i++;
		}

		if(dwSeqHeadPos != 0xffffffff)
		{
			if(pdwPos)
				*pdwPos = dwSeqHeadPos;
			if(pdwSize)
				*pdwSize = dwSeqHeadSize;

			return VO_TRUE;
		}
	}
	else if(VO_VIDEO_CodingH264 == nCodec)
	{
		VO_U32 dwPESHeaderPos = 0xffffffff;
		VO_U32 i = 0;
		VO_U32 dwSeqHeadPos = -1;
		VO_U32 dwSeqHeadSize = 0;
		while(i < nBufferSize - 3)
		{
			dwPESHeaderPos = FindPESHeaderInBuffer(pBuffer + i, nBufferSize - i);
			if(0xffffffff == dwPESHeaderPos)
				break;

			i += (dwPESHeaderPos + 3);
			if(i >= nBufferSize)
				break;

			if (dwSeqHeadPos == 0xffffffff) //find start point: dwSeqHeadPos
			{
				if(0x7 == (pBuffer[i] & 0x1F) || 0x8 == (pBuffer[i] & 0x1F))
				{
					dwSeqHeadPos = i - 3;
					dwSeqHeadSize = nBufferSize - (i - 3);
				}
			} 
			else //find end point: dwSeqHeadSize
			{
				if(pBuffer[i] == 0x9 || 0x7 == (pBuffer[i] & 0x1F) || (pBuffer[i] & 0x1F) == 0x08)
				{
					dwSeqHeadSize = i - 3 - dwSeqHeadPos - 1;
				}
				else// if( (pBuffer[i] & 0x1F) <= 5 )
				{
					dwSeqHeadSize = i - 3 - dwSeqHeadPos - 1;
					break;
				}
			}

			i++;
		}

		if(dwSeqHeadPos != 0xffffffff)
		{
			if(pdwPos)
				*pdwPos		= dwSeqHeadPos - 1;
			if(pdwSize)
				*pdwSize	= dwSeqHeadSize + 1;

			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

VO_U32 CBaseTrack::GetVideoResolution(VO_VIDEO_FORMAT* pInfo)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32 uFourcc = 0;
	VO_U32 nRes = GetCodecCC(&uFourcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	switch ( fCodecGetVideoCodec(uFourcc) )
	{
	case VO_VIDEO_CodingH263:
		{
			VO_CODECBUFFER codecbuf = {0};
			VO_VIDEO_HEADDATAINFO headdatainfo = {0};

			nRes = GetHeadData(&codecbuf);
			if (VO_ERR_SOURCE_OK == nRes && codecbuf.Length)
			{
				if (getResolution_H263(&codecbuf, &headdatainfo) == 0)
				{
					pInfo->Width	= headdatainfo.Width;
					pInfo->Height	= headdatainfo.Height;

					return VO_ERR_SOURCE_OK;
				}
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			//10/19/2011,modify by leon, if file has only one key, it will return less than zero.
			if (VO_ERR_NONE != nRes || 0 >= sample.Size)
			//if (VO_ERR_NONE != nRes || 0 == sample.Size)
			//10/19/2011,modify by leon, if file has only one key, it will return less than zero.
				return nRes ? nRes : VO_ERR_BASE;
			
			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			nRes = getResolution_H263(&codecbuf, &headdatainfo);
			if ( 0 == nRes)
			{
				pInfo->Width	= headdatainfo.Width;
				pInfo->Height	= headdatainfo.Height;
			}

			return nRes;
		}
	case VO_VIDEO_CodingMPEG4:
		{
			VO_CODECBUFFER codecbuf = {0};
			VO_VIDEO_HEADDATAINFO headdatainfo = {0};

			nRes = GetHeadData(&codecbuf);
			if (VO_ERR_SOURCE_OK == nRes && codecbuf.Length)
			{
				if (getResolution_Mpeg4(&codecbuf, &headdatainfo) == 0)
				{
					pInfo->Width	= headdatainfo.Width;
					pInfo->Height	= headdatainfo.Height;

					return VO_ERR_SOURCE_OK;
				}
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			nRes = getResolution_Mpeg4(&codecbuf, &headdatainfo);
			if ( 0 == nRes)
			{
				pInfo->Width	= headdatainfo.Width;
				pInfo->Height	= headdatainfo.Height;
			}

			return nRes;
		}
	case VO_VIDEO_CodingH264:
		{
			VO_CODECBUFFER codecbuf = {0};
			VO_VIDEO_HEADDATAINFO headdatainfo = {0};

			nRes = GetHeadData(&codecbuf);
			if (VO_ERR_SOURCE_OK == nRes && codecbuf.Length)
			{
				if (getResolution_H264(&codecbuf, &headdatainfo) == 0)
				{
					pInfo->Width	= headdatainfo.Width;
					pInfo->Height	= headdatainfo.Height;

					return VO_ERR_SOURCE_OK;
				}
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			nRes = getResolution_H264(&codecbuf, &headdatainfo);
			if ( 0 == nRes)
			{
				pInfo->Width	= headdatainfo.Width;
				pInfo->Height	= headdatainfo.Height;
			}

			return nRes;
		}
	case VO_VIDEO_CodingWMV:
	case VO_VIDEO_CodingVC1:
		{
	
			VO_BITMAPINFOHEADER* pBitmapInfoHeader = NULL;
			nRes = GetBitmapInfoHeader(&pBitmapInfoHeader);

			if(nRes != VO_ERR_NONE)
				return VO_ERR_BASE;

			pInfo->Width = pBitmapInfoHeader->biWidth;
			pInfo->Height = pBitmapInfoHeader->biHeight;
			
			return VO_ERR_SOURCE_OK;
		}
	case VO_VIDEO_CodingDIVX:
	case VO_VIDEO_CodingMPEG2:
	case VO_VIDEO_CodingS263:
	case VO_VIDEO_CodingRV:
	case VO_VIDEO_CodingMJPEG:
	case VO_VIDEO_CodingVP6:
	case VO_VIDEO_CodingVP8:
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}
}

VO_U32 CBaseTrack::GetVideoProfileLevel(VO_VIDEO_PROFILELEVEL* pInfo)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	pInfo->Profile	= VO_VIDEO_PROFILETYPE_MAX;
	pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

	VO_U32 uFourcc = 0;
	VO_U32 nRes = GetCodecCC(&uFourcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	switch(uFourcc)//DivX
	{
	case FOURCC_DIV3:
	case FOURCC_DIV4:
		{
			pInfo->Profile	= VO_VIDEO_DivX311;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_DIVX:
	case FOURCC_divx:
		{
			pInfo->Profile	= VO_VIDEO_DivX4;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_DIV5:
	case FOURCC_DX50:
		{
			pInfo->Profile	= VO_VIDEO_DivX5;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_DIV6:
		{
			pInfo->Profile	= VO_VIDEO_DivX6;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_xvid:
		{
			return VO_ERR_INVALID_ARG;
		}
	}

	switch ( fCodecGetVideoCodec(uFourcc) )
	{
	case VO_VIDEO_CodingMPEG4:
		{
			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes && codecbuf.Length) {
				if (getProfileLevel_Mpeg4(&codecbuf, pInfo) == 0)
					return VO_ERR_SOURCE_OK;
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			return getProfileLevel_Mpeg4(&codecbuf, pInfo);
		}
	case VO_VIDEO_CodingH264:
		{
			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes &&  codecbuf.Length) {
				if (getProfileLevel_H264(&codecbuf, pInfo) == 0)
					return VO_ERR_SOURCE_OK;
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			return getProfileLevel_H264(&codecbuf, pInfo);
		}
	case VO_VIDEO_CodingWMV:
		{
			if(FOURCC_WMV1 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormat7;
			else if(FOURCC_WMV2 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormat8;
			else
				pInfo->Profile = VO_VIDEO_PROFILETYPE_MAX;
			pInfo->Level = VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case VO_VIDEO_CodingVC1:
		{
			if(FOURCC_WMV3 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormat9;
			else if(FOURCC_WVC1 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormatWVC1;
			else
				pInfo->Profile = VO_VIDEO_PROFILETYPE_MAX;

			pInfo->Level = VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}

	case VO_VIDEO_CodingDIVX:
		//break;
	case VO_VIDEO_CodingH263:
		// TODO implement them
	case VO_VIDEO_CodingMPEG2:
	case VO_VIDEO_CodingS263:
	case VO_VIDEO_CodingRV:
	case VO_VIDEO_CodingMJPEG:
	case VO_VIDEO_CodingVP6:
	case VO_VIDEO_CodingVP8:
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}
}

VO_U32 CBaseTrack::IsVideoInterlace(VO_BOOL* bInterlace)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32 uFourcc = 0;
	VO_U32 nRes = GetCodecCC(&uFourcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	switch ( fCodecGetVideoCodec(uFourcc) )
	{
	case VO_VIDEO_CodingMPEG4:
		{
			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes && codecbuf.Length) {
				if (IsInterlace_Mpeg4(&codecbuf, bInterlace) == 0)
					return VO_ERR_SOURCE_OK;
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			return IsInterlace_Mpeg4(&codecbuf, bInterlace);
		}

	case VO_VIDEO_CodingH264:
		{
			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes &&  codecbuf.Length) {
				if (IsInterlace_H264(&codecbuf, bInterlace) == 0)
					return VO_ERR_SOURCE_OK;
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			return IsInterlace_H264(&codecbuf, bInterlace);
		}
	case VO_VIDEO_CodingMPEG2:
		{
			VO_VIDEO_HEADDATAINFO videoinfor;
			memset( &videoinfor, 0, sizeof(VO_VIDEO_HEADDATAINFO) );

			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes &&  codecbuf.Length) {
				if (Mpeg2VideoInfo(&codecbuf, &videoinfor) == 0)
				{
					*bInterlace = videoinfor.Reserved[0] ? VO_TRUE : VO_FALSE;
					return VO_ERR_SOURCE_OK;
				}
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			memset( &videoinfor, 0, sizeof(VO_VIDEO_HEADDATAINFO) );
			nRes = Mpeg2VideoInfo(&codecbuf, &videoinfor);
			if (0 == nRes)
			{
				*bInterlace = videoinfor.Reserved[0] ? VO_TRUE : VO_FALSE;
			}

			return nRes;
		}
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}
}

VO_BOOL CBaseTrack::ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (nHeadSize < 12)
		return VO_TRUE;

	if (m_pSeqHeadData != NULL)
		return VO_TRUE;

	char* pData = (char *)pHeadData;
	VO_U32 numOfPictureParameterSets;
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

	VO_U32 nFrameSize = 0;
	GetMaxSampleSize(&nFrameSize);
	if (m_nNalLen < 3)
	{
		m_pVideoData = new VO_BYTE[512 + nFrameSize];
		nNalLen = 4;
	}

	m_pSeqHeadData = new VO_BYTE[512 + nFrameSize];
	m_nHeadSize = 0;

	VO_U32 i = 0;
	VO_U32 numOfSequenceParameterSets = pData[5]&0x1f;
	VO_U8 * pBuffer = (VO_U8*)pData+6;
	for (i=0; i< numOfSequenceParameterSets; i++)
	{
		VO_U32 sequenceParameterSetLength = (pBuffer[0]<<8)|pBuffer[1];
		pBuffer+=2;

		memcpy (m_pSeqHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
		m_nHeadSize += nNalLen;

		if(sequenceParameterSetLength > (nHeadSize - (VO_U32)(pBuffer-pHeadData))){
			delete []m_pSeqHeadData;
			m_pSeqHeadData = NULL;
			m_nHeadSize = 0;
			return VO_FALSE;
		}

		memcpy (m_pSeqHeadData + m_nHeadSize, pBuffer, sequenceParameterSetLength);
		m_nHeadSize += sequenceParameterSetLength;

		pBuffer += sequenceParameterSetLength;
	}

	numOfPictureParameterSets = *pBuffer++;
	for (i=0; i< numOfPictureParameterSets; i++)
	{
		VO_U32 pictureParameterSetLength = (pBuffer[0]<<8)|pBuffer[1];
		pBuffer+=2;

		memcpy (m_pSeqHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
		m_nHeadSize += nNalLen;
		
		if(pictureParameterSetLength > (nHeadSize - (VO_U32)(pBuffer - pHeadData))){
			delete []m_pSeqHeadData;
			m_pSeqHeadData = NULL;			
			m_nHeadSize = 0;
			return VO_FALSE;
		}

		memcpy (m_pSeqHeadData + m_nHeadSize, pBuffer, pictureParameterSetLength);
		m_nHeadSize += pictureParameterSetLength;

		pBuffer += pictureParameterSetLength;
	}

	return VO_TRUE;
}

VO_BOOL	CBaseTrack::Hevc_ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	 if (nHeadSize < 12)
	  return VO_TRUE;

	 if (m_pSeqHeadData != NULL)
	  return VO_TRUE;

	 VO_PBYTE pData = pHeadData;
	 m_nNalLen =  (pData[21]&0x03)+1;
	 VO_U32 nNalLen = m_nNalLen;

	VO_U32 nFrameSize = 0;
	GetMaxSampleSize(&nFrameSize);

	 if (m_nNalLen == 3)
	   m_nNalWord = 0X010000;
	 if (m_nNalLen < 3)
	 {
		  m_pVideoData = new VO_BYTE[512 + nFrameSize];
		  nNalLen = 4;
	 }

	 m_pSeqHeadData = new VO_BYTE[512 + nFrameSize];
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
						  memcpy (m_pSeqHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
						  m_nHeadSize += nNalLen;
						  VO_U16 sequenceParameterSetLength = pData[0];
						  sequenceParameterSetLength = (sequenceParameterSetLength << 8)|pData[1];
						  pData += 2;
						  memcpy (m_pSeqHeadData + m_nHeadSize, pData, sequenceParameterSetLength);
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
						  memcpy (m_pSeqHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
						  m_nHeadSize += nNalLen;
						  VO_U16 pictureParameterSetLength = pData[0];
						  pictureParameterSetLength = (pictureParameterSetLength << 8)|pData[1];
						  pData += 2;
						  memcpy (m_pSeqHeadData + m_nHeadSize, pData, pictureParameterSetLength);
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
						  memcpy (m_pSeqHeadData + m_nHeadSize, &m_nNalWord, nNalLen);
						  m_nHeadSize += nNalLen;
						  VO_U16 adaptationParameterSetLength = pData[0];
						  adaptationParameterSetLength = (adaptationParameterSetLength << 8)|pData[1];
						  pData += 2;
						  memcpy (m_pSeqHeadData + m_nHeadSize, pData, adaptationParameterSetLength);
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

VO_BOOL CBaseTrack::ConvertData (VO_PBYTE pData, VO_U32 nSize)
{
	if (m_pSeqHeadData == NULL)
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

VO_U32 CBaseTrack ::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo) 
{
	return VO_ERR_NOT_IMPLEMENT;
}

