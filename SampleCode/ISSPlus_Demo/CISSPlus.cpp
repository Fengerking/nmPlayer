
#include "CISSPlus.h"



CISSPlus::CISSPlus(void)
{
	video_ff = NULL;
	audio_ff = NULL;
}

CISSPlus::~CISSPlus(void)
{
}


void CISSPlus::Open()
{

	strcpy(m_strDllFile, "voLiveSrcISS.dll");
	strcpy(m_strApiName, "voGetISSLiveSrcPlusAPI");

	Load();

	//GetParserPushAPI api = (GetParserPushAPI)m_api;

	GetISSLiveSrcPlusAPI api = (GetISSLiveSrcPlusAPI)m_api;

	api(&m_ReadHandle);
	m_callback.SendData = SendData;
	m_callback.SendEvent = SendEvent;
	m_callback.pUserData = this;
	m_ReadHandle.Init(&m_hFile, &m_callback);
}

VO_S32 CISSPlus::SendEvent (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	return 0;
}

static const VO_CHAR SYNC_HEADER[] = {0x00,0x00,0x00,0x01};
#define be32toh(x) 	((x &0xff000000)>> 24 | (x & 0xff0000) >>16 << 8 | (x &0xff00) >>8 <<16 | (x & 0xff)<<24)
VO_S32 CISSPlus::SendData (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	CISSPlus *iss = (CISSPlus*)pUserData;
	VO_LIVESRC_FRAMEBUFFER *pSample = (VO_LIVESRC_FRAMEBUFFER*)pData;
	if(VO_PARSER_OT_VIDEO == nOutputType)
  	{
		if( iss->m_videoCodec == VO_VIDEO_CodingH264 )
		{
			VO_BYTE *outputbuf = pSample->pData;
			VO_U32 slicesize = 0;
			VO_U32 headerlen = sizeof(SYNC_HEADER);

			for( VO_U32 i = 0 ;i < pSample->nSize; )
			{
				memcpy(&slicesize, outputbuf, headerlen);
				slicesize = (VO_U32)be32toh(slicesize);
				memcpy(outputbuf, SYNC_HEADER, headerlen);
				i += slicesize + headerlen;
				outputbuf +=  slicesize + headerlen;
			}
			//if( GetFrameType(ptr_buffer->pData, ptr_buffer->nSize) == 0 )
			//	ptr_buffer->nFrameType = 0;
		}
 		if(!iss->video_ff)iss->video_ff = fopen("video.264","wb");
  		fwrite(pSample->pData ,1, pSample->nSize, iss->video_ff);
  	}
	else if(VO_PARSER_OT_AUDIO== nOutputType)
	{
		VO_PBYTE pAudioData = NULL;
		if( iss->m_audioCodec == VO_AUDIO_CodingAAC )
		{
			//Wrap ADTS head for AAC raw data
			VO_U32 framelen = 0;
			int object, sampIdx, chanNum;

			framelen = pSample->nSize + 7; 
			pAudioData = new VO_BYTE[ framelen + 1 ];
			memset(pAudioData, 0, framelen + 1);

			object = 1;
			sampIdx = 4;//m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].samplerate_index;
			chanNum = 2;//int(m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].audio_info.Channels);

			pAudioData[0] = 0xFF; 
			pAudioData[1] = 0xF9;

			pAudioData[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

			if(framelen > 0x1FFF)
				return 0;

			pAudioData[3] = char(((chanNum << 6) | (framelen >> 11)));
			pAudioData[4] = char(((framelen & 0x7FF) >> 3));
			pAudioData[5] = char((((framelen & 7) << 5) | 0x1F));
			pAudioData[6] = 0xFC;	

			memcpy( pAudioData + 7, pSample->pData, pSample->nSize);

			pSample->pData = pAudioData;
			pSample->nSize = framelen;
		}

		if(!iss->audio_ff)iss->audio_ff = fopen("audio.aac","wb");

		fwrite(pAudioData, 1, pSample->nSize, iss->audio_ff);
		if(pAudioData) delete []pAudioData;
	}

	return 0;
}

void CISSPlus::Close()
{
	m_ReadHandle.Uninit(m_hFile);

	Unload();
}

int CISSPlus::GetFileTracks()
{
// 	VO_SOURCE_INFO info;
// 	m_ReadHandle.GetSourceInfo(m_hFile, &info);
// 	return info.Tracks;
	return 1;
}
int CISSPlus::HeaderParser(unsigned char *buf, int nSize)
{
	int ret = m_ReadHandle.HeaderParser(m_hFile, buf, nSize);
	return ret;
}

int CISSPlus::ChunkParser(VO_SOURCE_TRACKTYPE trackType, unsigned char *buf, int nSize)
{
	int ret = m_ReadHandle.ChunkParser(m_hFile, trackType,buf, nSize);
	return ret;
}

int CISSPlus::GetStreamInfo(int id, VO_LIVESRC_PLUS_STREAM_INFO **streamInfo)
{
	int ret = m_ReadHandle.GetStreamInfo(m_hFile, id, streamInfo);
	return ret;
}
int CISSPlus::SetCodecType(VO_SOURCE_TRACKTYPE typeType, VO_U32 nCodec)
{
	if( typeType == VO_SOURCE_TT_AUDIO )
	{
		m_audioCodec = nCodec;
	}
	else 	if( typeType == VO_SOURCE_TT_VIDEO )
	{
		m_videoCodec = nCodec;
	}
	return 0;
}
int CISSPlus::GetDrmInfo(VO_LIVESRC_PLUS_DRM_INFO **ppDRMInfo)
{
	int ret = m_ReadHandle.GetDRMInfo(m_hFile, ppDRMInfo);
	return ret;
}
int CISSPlus::GetStreamCount(VO_U32 *nCount)
{
	int ret = m_ReadHandle.GetStreamCounts(m_hFile, nCount);
	return 0;
}

void CISSPlus::SetFileName(char *name)
{
// 	VO_SOURCE_OPENPARAM param;
// 	VO_FILE_SOURCE source;
// 	source.nFlag = 	VO_FILE_TYPE_NAME;
// 	TCHAR szName[128];
// 	MultiByteToWideChar (CP_ACP, 0, name, -1, szName, sizeof (szName));
// 	source.pSource = (void*)szName;
// 	source.nOffset = 0;
// 	source.nLength = 0;
// 	source.nMode = VO_FILE_READ_ONLY;
// 	
// 	param.nFlag = VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE|VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR ;
// 	param.pDrmCB = NULL;
// 	param.pLibOP = NULL;
// 	param.pMemOP = NULL;
// 
// 	param.pSource = (void*)&source;
// 	param.pSourceOP = NULL;


//	m_ReadHandle.Open(&m_hFile, &param);
}

