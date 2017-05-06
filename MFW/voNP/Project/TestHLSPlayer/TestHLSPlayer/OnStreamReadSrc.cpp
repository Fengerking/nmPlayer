#include "OnStreamReadSrc.h"

#define VIDEO_BUFFER_LEN	1024000*8
#define AUDIO_BUFFER_LEN	1024000	

const int sampRateTab[12] = {
    96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

OnStreamReadSrc::OnStreamReadSrc()
	: m_fH264(NULL)
	, m_pVideoBuffer (NULL)
	, m_nVideoUseLength(0)
	, m_nVideoFilledlen(0)
	, m_nVideoFileLeft(0)
	, m_nVideoFileLength(0)
	, m_fAAC (NULL)
	, m_pAudioBuffer(NULL)
	, m_nAudioUseLength(0)
	, m_nAudioFilledlen(0)
	, m_nAudioFileLeft(0)
	, m_nAudioFileLength(0)	
	, m_nAudioRun(0)
	, m_nVideoRun(0)
	, m_nAudioCodec(VOOSMP_AUDIO_CodingAAC)
	, m_nVideoCodec(VOOSMP_VIDEO_CodingH264)
	, m_nOnlyAudio(0)
	, m_nOnlyVideo(0)
	, m_nAudioEOS(0)
	, m_nVideoEOS(0)
	, m_nDuration(0)
    , m_bLoop(false)
{
	m_fReadBuf.pUserData = this;
	m_fReadBuf.ReadAudio = voSrcReadAudio;
	m_fReadBuf.ReadVideo = voSrcReadVideo;

	m_nVideoFrameRate = 29970;
	m_nVideoFameTime = 1000000000 / m_nVideoFrameRate;
	m_nVideoFameNum = 0;

	m_nSampleIndex = 0;
	m_nAudioSampleRate = 44100;
	m_nAudioFameNum = 0;

	m_nStauts = -1;

	memset(m_sFilePath, 0, 1024*sizeof(char));
}

OnStreamReadSrc::~OnStreamReadSrc ()
{
	Uninit();
}

int	OnStreamReadSrc::Init(void * pSource, int nFlag)
{
	Uninit();

	char AACFilePath[1024];
	char H264FilePath[1024];

	memset(AACFilePath, 0, sizeof(1024));
	memset(H264FilePath, 0, sizeof(1024));

	if(strlen(m_sFilePath) > 0)
	{
//		strcpy(AACFilePath, m_sFilePath);
//		strcat(AACFilePath, "dump.aac");
//		strcpy(H264FilePath, m_sFilePath);
//		strcat(H264FilePath, "dump.H264");
        
        strcpy(AACFilePath, m_sFilePath);
		strcat(AACFilePath, (char*)pSource);
        strcat(AACFilePath, ".aac");
		strcpy(H264FilePath, m_sFilePath);
		strcat(H264FilePath, (char*)pSource);
        strcat(H264FilePath, ".H264");
	}
	else
	{
#ifdef _WIN32
		strcpy(AACFilePath, "D:/dump.aac");
		strcpy(H264FilePath, "D:/dump.H264");
#elif defined LINUX
		strcpy(AACFilePath, "/sdcard/dump.aac");
		strcpy(H264FilePath, "/sdcard/dump.H264");
#endif
	}


	m_fH264 = fopen(H264FilePath, "rb");
	if(m_fH264 == NULL)
	{
		return -1;
	}

	fseek(m_fH264,   0,   SEEK_END); 
	m_nVideoFileLength   =   ftell(m_fH264);
	fseek(m_fH264,   0,   SEEK_SET);
	m_nVideoFileLeft = m_nVideoFileLength;


	m_fAAC = fopen(AACFilePath, "rb");
	if(m_fAAC == NULL)
	{
		return -1;
	}

	fseek(m_fAAC,   0,   SEEK_END); 
	m_nAudioFileLength   =   ftell(m_fAAC);
	fseek(m_fAAC,   0,   SEEK_SET);

	m_nAudioFileLeft = m_nAudioFileLength;

	m_pVideoBuffer = new unsigned char[VIDEO_BUFFER_LEN];
	FillVideoBuffer();
	
	m_pAudioBuffer =  new unsigned char[AUDIO_BUFFER_LEN];
	FillAudioBuffer();

	if(m_nDuration == 0)
		m_nDuration = GetAVDuration();

	m_nStauts = 0;

	return VOOSMP_ERR_None;
}

int	OnStreamReadSrc::Uninit()
{
	if(m_fH264)
	{
		fclose(m_fH264);
		m_fH264 = NULL;
	}
	
	if(m_fAAC)
	{
		fclose(m_fAAC);
		m_fAAC =NULL;
	}

	if(m_pVideoBuffer)
	{
		delete m_pVideoBuffer;
		m_pVideoBuffer = NULL;
	}
	
	if(m_pAudioBuffer)
	{
		delete m_pAudioBuffer;
		m_pAudioBuffer = NULL;
	}

	m_nAudioRun = 0;
	m_nVideoRun = 0;
	m_nVideoUseLength = 0;
	m_nVideoFilledlen = 0;
	m_nVideoFileLeft = 0;
	m_nVideoFileLength = 0;
	m_nVideoFameNum = 0;
	m_nAudioUseLength = 0;
	m_nAudioFilledlen = 0;
	m_nAudioFileLength = 0;
	m_nAudioFileLeft = 0;
	m_nAudioFameNum = 0;
	
	return VOOSMP_ERR_None;
}

void* OnStreamReadSrc::GetReadBufPtr()
{
	return &m_fReadBuf;
}

int OnStreamReadSrc::Open (void)
{
	if(m_nStauts == 4 || m_nStauts == -1)
		Init(NULL, 0);
	m_nStauts = 1;
	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::Run (void)
{
	if(m_nStauts == 4 || m_nStauts == -1)
		Init(NULL, 0);
	m_nStauts = 2;
	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::Pause (void)
{
	if(m_nStauts == 4 || m_nStauts == -1)
		Init(NULL, 0);	
	m_nStauts = 3;
	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::Stop (void)
{
	m_nStauts = 4;
	m_nAudioRun = 0;
	m_nVideoRun = 0;
	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::Close (void)
{
	if(m_fH264)
	{
		fclose(m_fH264);
		m_fH264 = NULL;
	}

	if(m_fAAC)
	{
		fclose(m_fAAC);
		m_fAAC =NULL;
	}

	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::Flush (void)
{
	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::GetDuration (int * pDuration)
{
	*pDuration = m_nDuration;

	return 0;
}

int OnStreamReadSrc::GetAVDuration ()
{
	int AudioDuration;
	int VideoDuration;
	
	osCAutoLock lockVideoSrc (&m_MutexVideoRead);
	osCAutoLock lockAudioSrc (&m_MutexAudioRead);

	if(m_fH264)
	{
		int Length = 0;

		m_nVideolength  = GetH264Frame();
		m_pVideoHeader = m_pVideoBuffer;

		do
		{
			Length = GetH264Frame();
			if(Length > 0)
			{
				m_nVideoFameNum++;
				m_nVideoUseLength += Length;
			}
			else
			{
				break;
			}
		}while(1);

		VideoDuration = m_nVideoFameNum*(int)m_nVideoFameTime/1000;

		fseek(m_fH264, 0, SEEK_SET);

		m_nVideoFameNum = 0;
		m_nVideoUseLength = 0;
		m_nVideoFilledlen = 0;
		m_nVideoFileLeft = m_nVideoFileLength;

		FillVideoBuffer();		
	}
	
	if(m_fAAC)
	{
		int Length = 0;
		do
		{
			Length = GetAACFrame();
			if(Length > 0)
			{
				m_nAudioFameNum++;

				m_nAudioUseLength += Length;
			}
			else
			{
				break;
			}
		}while(1);
		
		AudioDuration = m_nAudioFameNum*1024000/m_nAudioSampleRate;

		fseek(m_fAAC, 0, SEEK_SET);

		m_nAudioFameNum = 0;
		m_nAudioUseLength = 0;
		m_nAudioFilledlen = 0;
		m_nAudioFileLeft = m_nAudioFileLength;

		FillAudioBuffer();
	}

	return AudioDuration > VideoDuration ? AudioDuration : VideoDuration;
}

int OnStreamReadSrc::SetCurPos (int nCurPos)
{
	osCAutoLock lockVideoSrc (&m_MutexVideoRead);
	osCAutoLock lockAudioSrc (&m_MutexAudioRead);

	long long VideoTime = 0;

	m_nAudioRun = 0;
	m_nVideoRun = 0;

	if(m_fH264)
	{
		
		fseek(m_fH264, 0, SEEK_SET);

		m_nVideoFameNum = 0;
		m_nVideoUseLength = 0;
		m_nVideoFilledlen = 0;
		m_nVideoFileLeft = m_nVideoFileLength;

		FillVideoBuffer();

		int Length = 0;
		long long time = 0;

		do
		{
			Length = GetH264Frame();
			if(Length > 0)
			{
				m_nVideoFameNum++;

				time = (int)m_nVideoFameNum*(int)m_nVideoFameTime/1000;
				
				if(time > nCurPos - 1000 && m_nKeyFrame)
					break;

				m_nVideoUseLength += Length;
			}
			else
			{
				break;
			}
		}while(1);

		VideoTime = time;
        printf("Video seek pos %lld\n", time);
	}	 
	
	if(m_fAAC)
	{
		fseek(m_fAAC, 0, SEEK_SET);

		m_nAudioFameNum = 0;
		m_nAudioUseLength = 0;
		m_nAudioFilledlen = 0;
		m_nAudioFileLeft = m_nAudioFileLength;

		FillAudioBuffer();

		int Length = 0;
		long long time = 0;

		do
		{
			Length = GetAACFrame();
			if(Length > 0)
			{
				m_nAudioFameNum++;

				//time = m_nAudioFameNum*1024000/m_nAudioSampleRate;
                time = ((long long)m_nAudioFameNum)*1024000/m_nAudioSampleRate;

				
				if(time > VideoTime - 23)
                {
                    printf("Aduio seek pos %lld\n", time);
                    break;
                }
                //printf("audio time %lld, %d, %d\n", time, m_nAudioSampleRate, m_nAudioFameNum);
					
				m_nAudioUseLength += Length;
			}
			else
			{
				break;
			}
		}while(1);
        
        printf("Audio seek pos %lld\n", time);
    }

	return nCurPos;
}

int OnStreamReadSrc::GetParam (int nID, void * pValue)
{
	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::SetParam (int nID, void * pValue)
{
	switch(nID)
	{
	case PLAY_VIDEO_ONLY:
		m_nOnlyVideo = *((int *)pValue);
		break;
	case PLAY_AUDIO_ONLY:
		m_nOnlyAudio = *((int *)pValue);
		break;
	case AV_FILE_PATH:
		strcpy(m_sFilePath, (const char *)pValue);
		break;
	case VIDEO_FRAME_RATE:
		m_nVideoFrameRate = *((int *)pValue);
		m_nVideoFameTime = 1000000000 / m_nVideoFrameRate;
	default:
		break;
	}

	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::onReadAudio(VOOSMP_BUFFERTYPE * pBuffer)
{
	if(m_nAudioRun == 0)
	{
		m_nAudioRun = 1;
		if(m_nOnlyVideo)
		{
			return VOOSMP_ERR_Audio_No_Now;
		}

		memset(&m_sAudioFormat, 0, sizeof(m_sAudioFormat));
		
		m_sAudioFormat.nStreamtype = VOOSMP_SS_AUDIO;
		m_sAudioFormat.nCodec = m_nAudioCodec;
		m_sAudioFormat.nFourCC = 0x7f;

		m_sAudioFormat.pHeadData = NULL;
		m_sAudioFormat.nHeadDataLen	 = 0;

		pBuffer->pData = &m_sAudioFormat;

		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_FORMAT;
        
        if(m_nAudioSampleRate)
        {
            pBuffer->llTime = ((long long)m_nAudioFameNum)*1024000/m_nAudioSampleRate;
        }

		return VOOSMP_ERR_None;
	}

	if(m_nOnlyVideo)
	{
		if(m_nVideoEOS)
		{
			return VOOSMP_ERR_EOS;
		}
		else
		{
			return VOOSMP_ERR_Retry;
		}
	}

	osCAutoLock lockAudioSrc (&m_MutexAudioRead);


	int Length = GetAACFrame();

	if(Length < 0)
	{
		if(m_nAudioFileLeft == 0)
		{
			m_nAudioEOS = 1;
			return VOOSMP_ERR_EOS;
		}
		else
		{
			return VOOSMP_ERR_Unknown;
		}
	}
	
	if(m_nAudioSampleRate)
	{
		pBuffer->llTime = ((long long)m_nAudioFameNum)*1024000/m_nAudioSampleRate;
	}
	pBuffer->pBuffer = m_pAudioBuffer + m_nAudioUseLength;
	pBuffer->nSize = Length;

	m_nAudioUseLength += Length;

	m_nAudioFameNum++;

	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::onReadVideo(VOOSMP_BUFFERTYPE * pBuffer)
{
	if(m_nVideoRun == 0)
	{
		m_nVideoRun = 1;
		if(m_nOnlyAudio)
		{
			return VOOSMP_ERR_Video_No_Now;
		}

		memset(&m_sVideoFormat, 0, sizeof(m_sVideoFormat));

		m_sVideoFormat.nStreamtype = VOOSMP_SS_VIDEO;
		m_sVideoFormat.nCodec = m_nVideoCodec;
		m_sVideoFormat.nFourCC = 0;

		m_sVideoFormat.pHeadData = (char *)m_pVideoHeader;
		m_sVideoFormat.nHeadDataLen	 = m_nVideolength;

		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_FORMAT;

		pBuffer->pData = &m_sVideoFormat;

        if(m_nVideoFameNum == 0)
            pBuffer->llTime = 0;
        else
            pBuffer->llTime = m_nVideoFameNum*m_nVideoFameTime/1000;

		return VOOSMP_ERR_None;
	}

	if(m_nOnlyAudio)
	{
		if(m_nAudioEOS)
		{
			return VOOSMP_ERR_EOS;
		}
		else
		{
			return VOOSMP_ERR_Retry;
		}
	}

	osCAutoLock lockVideoSrc (&m_MutexVideoRead);

	int Length = GetH264Frame();

	if(Length < 0)
	{
		if(m_nVideoFileLeft == 0)
		{
			m_nVideoEOS = 1;
			return VOOSMP_ERR_EOS;
		}
		else
		{
			return VOOSMP_ERR_Unknown;
		}
	}

	if(m_nKeyFrame)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_KEYFRAME;
	}

    if(m_nVideoFameNum == 0)
        pBuffer->llTime = 0;
    else
        pBuffer->llTime = m_nVideoFameNum*m_nVideoFameTime/1000;
    
	pBuffer->pBuffer = m_pVideoBuffer + m_nVideoUseLength;
	pBuffer->nSize = Length;

	m_nVideoUseLength += Length;

	m_nVideoFameNum++;

	return VOOSMP_ERR_None;
}

int OnStreamReadSrc::voSrcReadAudio(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer)
{
	OnStreamReadSrc* pSrc = (OnStreamReadSrc*)pUserData;

	int nRC = pSrc->onReadAudio(pBuffer);
	
	return nRC;
}

int OnStreamReadSrc::voSrcReadVideo(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer)
{
	OnStreamReadSrc* pSrc = (OnStreamReadSrc*)pUserData;

	int nRC = pSrc->onReadVideo(pBuffer);

	return nRC;
}

int OnStreamReadSrc::FillAudioBuffer()
{
	if(m_nAudioFileLeft == 0)
		return 1;

	m_nAudioFilledlen = m_nAudioFilledlen - m_nAudioUseLength;
	if(m_nVideoFilledlen < 0)
		return -1;

	memcpy(m_pAudioBuffer, m_pAudioBuffer + m_nAudioUseLength, m_nAudioFilledlen);

	int nRead = AUDIO_BUFFER_LEN - m_nAudioFilledlen;

	if(m_nAudioFileLeft < nRead)
	{
		nRead = m_nAudioFileLeft;
	}
	
	osCAutoLock lockAudioSrc (&m_MutexAudioRead);
	if(m_fAAC == 0)
		return -1;
	nRead = fread(m_pAudioBuffer + m_nAudioFilledlen, 1, nRead, m_fAAC);
	m_nAudioFileLeft -= nRead;
	m_nAudioFilledlen += nRead;

	m_nAudioUseLength = 0;

	return 0;	
}

int OnStreamReadSrc::FillVideoBuffer()
{
	if(m_nVideoFileLeft == 0)
		return 1;

	m_nVideoFilledlen = m_nVideoFilledlen - m_nVideoUseLength;
	if(m_nVideoFilledlen < 0)
		return -1;

	memcpy(m_pVideoBuffer, m_pVideoBuffer + m_nVideoUseLength, m_nVideoFilledlen);

	int nRead = VIDEO_BUFFER_LEN - m_nVideoFilledlen;

	if(m_nVideoFileLeft < nRead)
	{
		nRead = m_nVideoFileLeft;
	}
	
	osCAutoLock lockVideoSrc (&m_MutexVideoRead);
	if(m_fH264 == 0)
		return -1;
	nRead = fread(m_pVideoBuffer + m_nVideoFilledlen, 1, nRead, m_fH264);
	m_nVideoFileLeft -= nRead;
	m_nVideoFilledlen += nRead;

	m_nVideoUseLength = 0;

	return 0;	
}

int OnStreamReadSrc::GetAACFrame()
{
	int Framelen = 0;
	int SampleIndex, Profile, Channel;
	int MaxFrameLen, i;
	int Sync = 1;
	
	MaxFrameLen = 2048;

	do {
		unsigned char *pBuf = m_pAudioBuffer + m_nAudioUseLength;
		int  inLen = m_nAudioFilledlen - m_nAudioUseLength;

		if(inLen < 7)
		{
			if(FillAudioBuffer())
				return -1;

			 inLen = m_nAudioFilledlen - m_nAudioUseLength;
		}

		for (i = 0; i < inLen - 1; i++) {			
			if ( (pBuf[0] & 0xFF) == 0xFF && (pBuf[1] & 0xF0) == 0xF0 )
				break;

			pBuf++;
			m_nAudioUseLength++;
			inLen--;
			if (inLen <= 7)
			{
				if(FillAudioBuffer())
					return -1;
			}
		}

		Framelen = ((pBuf[3] & 0x3) << 11) + (pBuf[4] << 3) + (pBuf[5] >> 5);
		SampleIndex = (pBuf[2] >> 2) &0xF;
		Profile = (pBuf[2] >> 6) + 1;
		Channel = ((pBuf[2]&0x01) << 2) | (pBuf[3] >> 6);

		if(Framelen > MaxFrameLen || Profile > 2 || Channel > 6 || SampleIndex > 12)
		{
			pBuf++;
			m_nAudioUseLength++;
			inLen--;
			continue;
		}

		if(Framelen > inLen || inLen == 0)
		{
			if(FillAudioBuffer())
				return -1;

			 inLen = m_nAudioFilledlen - m_nAudioUseLength;
		}

		if(Framelen > inLen)
		{
			pBuf++;
			m_nAudioUseLength++;
			inLen--;
			continue;		
		}

		if(Framelen + 2 < inLen)
		{
			if(pBuf[Framelen] == 0xFF && (pBuf[Framelen + 1] & 0xF0) == 0xF0)
			{
				m_nSampleIndex = SampleIndex;
				Sync = 0;
			}				
		}

		if(Framelen == inLen)
		{
			Sync = 0;
		}

	}while(Sync);

	m_nAudioSampleRate = sampRateTab[m_nSampleIndex];

	return Framelen;
}

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

int OnStreamReadSrc::GetH264Frame()
{
	int Framelen = 0;
	int Sync = 1;
	int naluType = 0;
	unsigned char *pBuf;
	unsigned char* p;
	unsigned char* endPos;
	int  inLen;

	pBuf = m_pVideoBuffer + m_nVideoUseLength;
	inLen = m_nVideoFilledlen - m_nVideoUseLength;

	do
	{
		p = pBuf;  
		endPos = pBuf+inLen;
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				Sync = 0;
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				Sync = 0;
				break;
			}
			m_nVideoUseLength++;
		}

		if(p >= endPos)
		{
			if(FillVideoBuffer())
				return -1;			

			pBuf = m_pVideoBuffer + m_nVideoUseLength;
			inLen = m_nVideoFilledlen - m_nVideoUseLength;
		}
	}while(Sync);

	Sync = 1;

	do
	{
		p += 3;
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				naluType = p[3]&0x0f;
				if(naluType>0 && naluType < 6 && (p[4] & 0x80))
				{
					Sync = 0;
				}
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				naluType = p[4]&0x0f;
				if(naluType>0 && naluType < 6 && (p[5] & 0x80))
				{
					Sync = 0;
				}
				break;
			}
		}

		if(p >= endPos)
		{
			if(FillVideoBuffer())
				return -1;

			p = m_pVideoBuffer;  
			endPos = p + m_nVideoFilledlen;
		}
	}while(Sync);

	Framelen = p - (m_pVideoBuffer + m_nVideoUseLength);

	int FrameType = voGetH264NalFrameType(m_pVideoBuffer + m_nVideoUseLength, Framelen);

	if(FrameType == 2)
		m_nKeyFrame = 1;
	else
		m_nKeyFrame = 0;

	return Framelen;
}

int OnStreamReadSrc::voGetH264NalFrameType(unsigned char* inbuf, int inlength)
{
	int buf_index = 0;
	int nal_unit_type;
	int nal_ref_idc;
	int frameType;

	while(buf_index < inlength)
	{
		for(; buf_index + 3 < inlength; buf_index++){
            if(inbuf[buf_index] == 0 && inbuf[buf_index+1] == 0 && inbuf[buf_index+2] == 1)
                break;
        }
		
        if(buf_index+3 >= inlength) break;
		
        buf_index += 3;
		
		nal_unit_type = inbuf[buf_index] & 0x1F;
		nal_ref_idc = (inbuf[buf_index] & 0x60) >> 5;

		if(nal_unit_type == 5)
		{
			return 2;
		}
		else if(nal_unit_type == 1)
		{
			if(inlength - buf_index > 128)
			{
				frameType = GetH264FrameType(inbuf + buf_index, inlength - buf_index);				
				return frameType;
			}
		}		
	}

	return -1;
}

int OnStreamReadSrc::GetH264FrameType(unsigned char *buffer, int inlength)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len;  //ACW_UNUSED <value>: int len,value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;
	if(naluType==1)//need continuous check
		buffer++;
	else//the nalu type is params info
		return -1;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		
		inf=0;                          
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return -1;
	}
	
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if (inf>=5)
	{
		inf-=5;
	}

	return inf;
}

int OnStreamReadSrc::SetLoop(bool bLoop)
{
    m_bLoop = bLoop;
    return 0;
}