#include "fVideoHeadDataInfo.h"
#include "StreamFormat.h"
#include "mp4cfg.h"
#include "adtshead.h"
#include "LatmParser.h"
#include "CDumper.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


const int MIN_H264_FRAME_SIZE = 16 << 10; 
const int MIN_AAC_FRAME_SIZE = 1 << 10;
const int MAX_AAC_FRAME_SIZE = 16 << 10;

#define MAX_ADTS_HEADER_SIZE 9
#define MAX_LATM_HEADER_SIZE 32

bool CStreamFormat::ParseMP3Head(const void *pbData, int nSize, int& samplerate, int& channels, int& bitrate, int& framelen)
{
	// 1. index = MPEG Version ID; 2. index = sampling rate index
	const int TABLE_SAMPLE_RATE[4][3] = 
	{
		{11025, 12000, 8000,  },	// MPEG 2.5
		{0,     0,     0,     },	// reserved
		{22050, 24000, 16000, },	// MPEG 2
		{44100, 48000, 32000  }		// MPEG 1
	};

	// bitrates: 1. index = LSF, 2. index = Layer, 3. index = bitrate index
	const int TABLE_BITRATE[2][3][15] = 
	{
		{	// MPEG 1
			{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},	// Layer1
			{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},	// Layer2
			{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}	// Layer3
		},
		{	// MPEG 2, 2.5		
			{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},		// Layer1
			{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},			// Layer2
			{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}			// Layer3
		}
	};

	// 1. index = LSF, 2. index = Layer
	const int TABLE_COEFFICIENTS[2][3] = 
	{
		{	// MPEG 1
			48,		// Layer1
			144,	// Layer2
			144		// Layer3
		},
		{	// MPEG 2, 2.5
			48,		// Layer1
			144,	// Layer2
			72		// Layer3
		}	
	};

	// padding sizes in bytes for different layers: 1. index = layer
	const int TABLE_PADDING_RATIO[3] = 
	{
		4,	// Layer1
		1,	// Layer2
		1	// Layer3
	};

	const int TABLE_CHANNEL[] = {2, 2, 2, 1};

	const uint8* p = (const uint8*) pbData;
	uint8 b1 = *p++;
	uint8 b2 = *p++;
	uint8 b3 = *p++;
	uint8 b4 = *p++;

	int sync = b1;
	sync <<= 3;
	sync |= b2 >> 5;
	if (sync != 0x7ff)
		return false;

	int version = (b2 >> 3) & 0x03;
	int layer = ((b2 >> 1) & 0x03) - 1; //0 based
	int bitrateindex = b3 >> 4;
	int samplerateindex = (b3 >> 2) & 0x03;
	int padding = (b3 >> 1) & 1;
	int channelmode = b4 >> 6;
	int LSF = version == 3 ? 0 : 1;

#if 0 //now we support all
	if ((version != 3) || (layer != 1)) //MPEG1, layer2
		return false;
#endif

	samplerate = TABLE_SAMPLE_RATE[version][samplerateindex];
	bitrate = TABLE_BITRATE[LSF][layer][bitrateindex] * 1000;
	channels = TABLE_CHANNEL[channelmode];
	framelen = TABLE_COEFFICIENTS[LSF][layer] * bitrate / samplerate + padding * TABLE_PADDING_RATIO[layer];
	return true;
}

bool ParseMPEG2Head(const void *pbData, int nSize, int& width, int& height, int& progressive)
{
	// 20100123
    if(NULL == pbData)
	{
		return false;
	}

	uint8* dsidata	= (uint8*)pbData;
	width			= ((*(dsidata + 4)) << 4) | ((*(dsidata + 5)) >> 4);
	height			= (((*(dsidata + 5)) << 8) | ((*(dsidata + 6)))) & 0x0FFF;
	// end

	const uint32 EXT_START_CODE = 0xB5010000;
	uint8* p = (uint8*)pbData;
	for (int i=0; i<nSize; i++, p++)
	{
		if (!memcmp(p, &EXT_START_CODE, 4))
		{
			uint8 b1 = (*(p+5) >> 3) & 0x01;
			progressive = b1;
			return true;
		}
	}
	return false;
}

const uint8* FindH264Header(const uint8* p1, const uint8* p2)
{
	const uint8* p = p1;
	while (p < p2 - 4)
	{
		if ( (*p == 0) && (*(p+1) == 0) && (*(p+2) == 0) && (*(p+3) == 1) )
			return p;
		++p;
	}
	return NULL;
}

int GetH264SequenceHeaderSize(const void* data, int size)
{
	const uint8* p1 = (const uint8*) data;
	const uint8* p2 = p1 + size;
	const uint8* p = p1 + 4;
	while ((p = FindH264Header(p, p2)))
	{
		uint8 flag = *(p + 4) & 0x0f;
		if ((flag != 7) && (flag != 8))
			return p - p1;
		p += 4;
	}
	return size;
}

static inline int _log2(unsigned int data)
{
	int i;
	if (!data) ++data;
	for (i=0;data;++i)
		data >>= 1;
	return i;
}

int ParseMPEG4Head(const void* data, int size, int* width, int* height)
{
	if (!data)
	{
		return 0;
	}
	uint8 * pHead = (uint8 *)data;
	uint8 * pEnd = pHead + size;
	uint32 MpegSyncWord = 0xFFFFFFFF;
	uint32 Mask = 0xFFFFFFF0;
	while (pHead < pEnd)
	{
		MpegSyncWord <<= 8;
		MpegSyncWord |= *pHead++;
		if ((MpegSyncWord & Mask) == 0x00000120)
		{
			break;
		}
	}
	if (pHead >= pEnd)
	{
		return 0;
	}
	BitStream bs(pHead);
	bs.SkipBits(9);
	bit1 is_object_layer_identifier = 0;
	bs.ReadBits(1,is_object_layer_identifier);
	bit4 video_object_layer_verid = 0;
	if (is_object_layer_identifier)
	{
		bs.ReadBits(4,video_object_layer_verid);
		bs.SkipBits(3);
	}
	bit4 aspect_ratio_info  = 0;
	bs.ReadBits(4,aspect_ratio_info);
	if (aspect_ratio_info == 0xF)
	{
		bs.SkipBits(16);
	}
	bit1 vol_control_parameters = 0;
	bs.ReadBits(1,vol_control_parameters);
	if (vol_control_parameters)
	{
		bs.SkipBits(3);
		bit1 vbv_parameters = 0;
		bs.ReadBits(1,vbv_parameters);
		if (vbv_parameters)
		{
			bs.SkipBytes(9);
			bs.SkipBits(7);
		}
	}
	bit2 video_object_layer_shape = 0;
	bs.ReadBits(2,video_object_layer_shape);
	if (video_object_layer_shape == 0x3 && video_object_layer_verid != 1)
	{
		bs.SkipBits(4);
	}
	bs.SkipBits(1);
	bit16 vop_time_increment_resolution = 0;
	bs.ReadBits(16,vop_time_increment_resolution);
	bs.SkipBits(1);
	bit1 fixed_vop_rate = 0;
	bs.ReadBits(1,fixed_vop_rate);
	if (fixed_vop_rate)
	{
		int time_increment_bits = _log2(vop_time_increment_resolution - 1);
		bs.SkipBits(time_increment_bits);
	}
	if (video_object_layer_shape == 00)
	{
		bit13 nWidth = 0;
		bit13 nHeight = 0;
		bs.SkipBits(1);
		bs.ReadBits(13,nWidth);
		bs.SkipBits(1);
		bs.ReadBits(13,nHeight);
		if (width)
		{
			*width = nWidth;
		}
		if (height)
		{
			*height = nHeight;
		}
	}
	return 0;
}

CStreamFormat::CStreamFormat(VOSTREAMPARSESTREAMINFO* pStreamInfo)
: m_codec(pStreamInfo->codec)
, m_pHeadData(VO_NULL)
, m_nHeadSize(0)
, m_pFrameChecker(VO_NULL)
{
	// default values

	m_bVideo = pStreamInfo->is_video;
    m_eStreamMediaType = pStreamInfo->eStreamMediaType;

	m_videoProgressive = true;

	m_nMaxFrameSize = pStreamInfo->max_frame_len;

	if (pStreamInfo->is_video)
	{
		memcpy(&m_video, &pStreamInfo->video, sizeof(m_video));
		switch (m_codec)
		{
		case VO_VIDEO_CodingH264:
			InitH264(pStreamInfo);
			break;
		case VO_VIDEO_CodingMPEG2:
			InitMPEG2(pStreamInfo);
			break;
		case VO_VIDEO_CodingMPEG4:
			InitMPEG4(pStreamInfo);
			break;
		case VO_VIDEO_CodingH265:
            InitHEVC(pStreamInfo);
			break;
	// 20100308 : to do
	// 	case VC_RICHMEDIAXML:
	// 		InitRMX(pStreamInfo);
	// 		break;
	// 	case VC_AVC:
	// 		InitAVC(pStreamInfo);
	// 		break;
		default:
			break;
		}
		
	}
	if (!pStreamInfo->is_video)
	{
		memcpy(&m_audio, &pStreamInfo->audio, sizeof(m_audio));
		switch (m_codec)
		{
		case VO_AUDIO_CodingAAC:
			if (pStreamInfo->extra_size == MAX_ADTS_HEADER_SIZE)
				InitADTSAAC(pStreamInfo);
			else if (pStreamInfo->extra_size == MAX_LATM_HEADER_SIZE)
				InitLATMAAC(pStreamInfo);
			else
				InitAAC(pStreamInfo);
			break;
		case VO_AUDIO_CodingMP1:
		case VO_AUDIO_CodingMP3:
			InitMP3(pStreamInfo);
			break;
		case VO_AUDIO_CodingAC3:
			InitAC3(pStreamInfo);
			break;
		case VO_AUDIO_CodingPCM:
			InitM2TSLPCM(pStreamInfo);
			break;
		default:
			break;
		}
	}

	if (m_bVideo)
		CDumper::WriteLog((char *)"Video resolution : %d x %d", m_video.width, m_video.height);
	else
		CDumper::WriteLog((char *)"Audio prop : %d channels, %d sample bits, %d sample rates", m_audio.channels, m_audio.sample_bits, m_audio.sample_rate);
}

CStreamFormat::~CStreamFormat(void)
{
	if (m_pHeadData)
		delete m_pHeadData;

	if (m_pFrameChecker)
	{
		delete m_pFrameChecker;
		m_pFrameChecker = VO_NULL;
	}
}

void CStreamFormat::InitH264(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
    int  iRet = 0;
	VO_CODECBUFFER   varCodecBuffer = {0};
	VO_VIDEO_HEADDATAINFO    varVideoHeaderDataInfo = {0};

	if (pStreamInfo->extra_size <= 0)
		return;
	m_nHeadSize = GetH264SequenceHeaderSize(pStreamInfo->extra_data, pStreamInfo->extra_size);
	m_pHeadData = new uint8[m_nHeadSize];
	memcpy(m_pHeadData, pStreamInfo->extra_data, m_nHeadSize);
    
	varCodecBuffer.Buffer = (VO_PBYTE)m_pHeadData;
	varCodecBuffer.Length = m_nHeadSize;

	iRet = getResolution_H264(&varCodecBuffer, &varVideoHeaderDataInfo);

	int width = pStreamInfo->video.width;
	int height = pStreamInfo->video.height;
	if (width == 0 || height == 0)
	{
		if (0 == iRet)
		{
			m_video.height = varVideoHeaderDataInfo.Height;
			m_video.width = varVideoHeaderDataInfo.Width;

// #ifdef _DEBUG
// 			TCHAR tmp[128];
// 			swprintf(tmp, _T("Video resolution : %d x %d\r\n"), width, height);
// 			OutputDebugString(tmp);
// #endif //_DEBUG
		}
	}
	pStreamInfo->video.width = varVideoHeaderDataInfo.Width;
	pStreamInfo->video.height = varVideoHeaderDataInfo.Height;
	m_nMaxFrameSize = varVideoHeaderDataInfo.Height * varVideoHeaderDataInfo.Width;//width * height / 2;
#ifdef _DEBUG
	if (m_nMaxFrameSize == 0)
		OutputDebugString(_T("Cannot get H264 width & height\n"));
#endif //_DEBUG

	if(m_nMaxFrameSize == 0)
	{
		m_nMaxFrameSize = 1080*1920;
	}
	else
	{
		if (m_nMaxFrameSize < MIN_H264_FRAME_SIZE)
			m_nMaxFrameSize = MIN_H264_FRAME_SIZE;
	}
	m_avc.stream_format = VO_H264_ANNEXB;

	m_pFrameChecker = FrameChecker::CreateCheckerH264();
}


void CStreamFormat::InitHEVC(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
    if(pStreamInfo != NULL)
    {
        m_nHeadSize = pStreamInfo->extra_size;
		m_pHeadData = new uint8[m_nHeadSize];
        if(m_pHeadData == NULL)
        {
            return;
        }
		memcpy(m_pHeadData, pStreamInfo->extra_data, m_nHeadSize);
    }

	if(m_nMaxFrameSize == 0)
	{
		m_nMaxFrameSize = 1080*1920;
	}

	m_pFrameChecker = FrameChecker::CreateCheckerH265();
}


void CStreamFormat::InitAVC(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	int  iRet = 0;
	VO_CODECBUFFER   varCodecBuffer = {0};
	VO_VIDEO_HEADDATAINFO    varVideoHeaderDataInfo = {0};

	CheckSaveHeadData(pStreamInfo);

	MP4::AVCDecoderConfigurationRecord cfg;
	if (!cfg.Load(pStreamInfo->extra_data, pStreamInfo->extra_size))
		return;
    
    varCodecBuffer.Buffer = (VO_PBYTE)pStreamInfo->extra_data;
	varCodecBuffer.Length = pStreamInfo->extra_size;

	iRet = getResolution_H264(&varCodecBuffer, &varVideoHeaderDataInfo);

	if (0 == iRet)
	{
		m_video.height = varVideoHeaderDataInfo.Height;
		m_video.width = varVideoHeaderDataInfo.Width;
	}

	m_nMaxFrameSize = varVideoHeaderDataInfo.Height * varVideoHeaderDataInfo.Width / 2;

	if(m_nMaxFrameSize == 0)
	{
		m_nMaxFrameSize = 1080*1920;
	}
	else
	{
		if (m_nMaxFrameSize < MIN_H264_FRAME_SIZE)
			m_nMaxFrameSize = MIN_H264_FRAME_SIZE;
	}

#if 0
	m_nHeadSize = cfg.ToH264Header(NULL, 0);
	m_pHeadData = new uint8[m_nHeadSize];
	cfg.ToH264Header(m_pHeadData, m_nHeadSize);
#else
	m_avc.nalu_length_size = cfg.lengthSizeMinusOne + 1;
	m_avc.stream_format = VO_H264_AVC;
#endif

	m_pFrameChecker = FrameChecker::CreateCheckerAVC(m_avc.nalu_length_size);
}

void CStreamFormat::InitAAC(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CheckSaveHeadData(pStreamInfo);

	MP4::AudioSpecificConfig cfg;
	if (!cfg.Load(pStreamInfo->extra_data, pStreamInfo->extra_size))
		return;
	m_audio.channels = cfg.channelConfiguration;
	m_audio.sample_bits = 16;
	m_audio.sample_rate = cfg.samplingFrequency;
	m_aac.object_type = cfg.audioObjectType;
	m_aac.frame_type = VOAAC_RAWDATA;
	if (m_nMaxFrameSize < MIN_AAC_FRAME_SIZE)
		m_nMaxFrameSize = MIN_AAC_FRAME_SIZE;
	if (m_nMaxFrameSize > MAX_AAC_FRAME_SIZE)
		m_nMaxFrameSize = MAX_AAC_FRAME_SIZE;
}

VO_U32 CStreamFormat::GetHeadData(VO_PTR pVal)
{
	VO_CODECBUFFER* pParams = (VO_CODECBUFFER*)pVal;

	pParams->Buffer	= m_pHeadData;
	pParams->Length	= m_nHeadSize;
	return VO_ERR_NONE;

// 	if ((pParams->Buffer == NULL) || (pParams->Length< m_nHeadSize))
// 	{
// 		pParams->Length = m_nHeadSize;
// 		return VO_ERR_WRONG_PARAM_ID;
// 	}
// 	if (m_pHeadData)
// 		memcpy(pParams->Buffer, m_pHeadData, m_nHeadSize);
// 	pParams->Length = m_nHeadSize;
// 	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetAudioFormat(VO_PTR pVal) 
{ 
	if (IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	memcpy(pVal, &m_audio, sizeof(m_audio));
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetVideoFormat(VO_PTR pVal) 
{ 
	if (!IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	memcpy(pVal, &m_video, sizeof(m_video));
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetAudioChannels(VO_PTR pVal) 
{ 
	if (IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	*(int*)pVal = m_audio.channels; 
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetAudioSampleRate(VO_PTR pVal) 
{ 
	if (IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	*(int*)pVal = m_audio.sample_rate; 
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetVideoWidth(VO_PTR pVal) 
{ 
	if (!IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	*(int*)pVal = m_video.width; 
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetVideoHeight(VO_PTR pVal) 
{ 
	if (!IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	*(int*)pVal = m_video.height; 
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetVideoProgressive(VO_PTR pVal)
{
	if (!IsVideo()) 
		return VO_ERR_WRONG_PARAM_ID;
	*((bool*)pVal) = m_videoProgressive; 
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetH264StreamFormat(VO_PTR pVal)
{
	if ((VO_VIDEO_CodingH264 != GetCodec()) ) //&& (VC_AVC != GetCodec())) 
		return VO_ERR_WRONG_PARAM_ID;
	*(VO_H264FILEFORMAT*)pVal = m_avc.stream_format;
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetAACObjectType(VO_PTR pVal)
{
	if (VO_AUDIO_CodingAAC != GetCodec()) 
		return VO_ERR_WRONG_PARAM_ID;
	*(int*)pVal = m_aac.object_type;
	return VO_ERR_NONE;
}

VO_U32 CStreamFormat::GetAACFrameType(VO_PTR pVal)
{
	if (VO_AUDIO_CodingAAC != GetCodec()) 
		return VO_ERR_WRONG_PARAM_ID;
	*(int*)pVal = m_aac.frame_type;
	return VO_ERR_NONE;
}

void CStreamFormat::CheckSaveHeadData(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	m_nHeadSize = pStreamInfo->extra_size;
	if (m_nHeadSize == 0)
		return;
	m_pHeadData = new uint8[m_nHeadSize];
	memcpy(m_pHeadData, pStreamInfo->extra_data, m_nHeadSize);
}

void CStreamFormat::InitMP3(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CheckSaveHeadData(pStreamInfo);
	//m_nMaxFrameSize = 1024;
	int channels, samplerate, bitrate, framelen;
	bool b = ParseMP3Head(m_pHeadData, m_nHeadSize, samplerate, channels, bitrate, framelen);
	if (b)
	{
		pStreamInfo->audio.channels = m_audio.channels = channels;
		pStreamInfo->audio.sample_bits = m_audio.sample_bits = 16;
		pStreamInfo->audio.sample_rate = m_audio.sample_rate = samplerate;
	}

	// 20100505
	m_pFrameChecker = FrameChecker::CreateCheckerDefault();
	// end
}

void CStreamFormat::InitMPEG2(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CheckSaveHeadData(pStreamInfo);
	
	int width, height, progressive;
	bool b = ParseMPEG2Head(m_pHeadData, m_nHeadSize, width, height, progressive);
	if (b)
		m_videoProgressive = (progressive > 0);

	// 20100123
	m_nMaxFrameSize = width*height;

	m_pFrameChecker = FrameChecker::CreateCheckerMPEG2();
	// end
}

void CStreamFormat::InitMPEG4(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	ParseMPEG4Head(pStreamInfo->extra_data,pStreamInfo->extra_size,&(pStreamInfo->video.width),&(pStreamInfo->video.height));
	m_video.width = pStreamInfo->video.width;
	m_video.height = pStreamInfo->video.height;

	CheckSaveHeadData(pStreamInfo);

// 	int width, height, progressive;
// 	bool b = ParseMPEG2Head(m_pHeadData, m_nHeadSize, width, height, progressive);
// 	if (b)
// 		m_videoProgressive = (progressive > 0);
// 
// 	m_nMaxFrameSize = width*height / 2;

	m_nMaxFrameSize = pStreamInfo->max_frame_len;
	m_pFrameChecker = FrameChecker::CreateCheckerMPEG4();
}

void CStreamFormat::InitADTSAAC(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	ADTSHeader adtsh;
	if (!adtsh.Parse((uint8*)pStreamInfo->extra_data))
		return;

	CDumper::WriteLog((char *)"CStreamFormat::InitADTSAAC...");

	pStreamInfo->audio.channels = m_audio.channels = adtsh.GetChannelCount();
	pStreamInfo->audio.sample_bits = m_audio.sample_bits = 16;
	pStreamInfo->audio.sample_rate = m_audio.sample_rate = adtsh.GetSampleRate();
	m_aac.object_type = adtsh.GetAACObjectType();
	//m_audio.sample_rate  *= 2; //??? 5/12/2009
	m_aac.frame_type = VOAAC_ADTS;
	if (m_nMaxFrameSize < MIN_AAC_FRAME_SIZE)
		m_nMaxFrameSize = MIN_AAC_FRAME_SIZE;
	if (m_nMaxFrameSize > MAX_AAC_FRAME_SIZE)
		m_nMaxFrameSize = MAX_AAC_FRAME_SIZE;

	//int temp = adtsh.GetHeadLen();

	// make header data
	m_nHeadSize = 2;
	m_pHeadData = new uint8[m_nHeadSize];
	adtsh.ToDSI(m_pHeadData);

	if (m_aac.object_type == 22)
		m_pFrameChecker = FrameChecker::CreateCheckerBSAC();
	else // tag: 20100413
		m_pFrameChecker = FrameChecker::CreateCheckerAdtsAAC();
	// end
}

void CStreamFormat::InitLATMAAC(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CLATMParser* parser = new CLATMParser();
	int ret = parser->Parse((uint8*)pStreamInfo->extra_data, pStreamInfo->extra_size);
	if (ret)
	{
// 		TCHAR tmp[256];
// 		memset(tmp, 0 , 256);
// 		swprintf(tmp, _T("CLATMParser parser return %d\r\n"), ret);
// 		OutputDebugString(tmp);
		delete parser;
		return;
	}

	CDumper::WriteLog((char *)"CStreamFormat::InitLATMAAC...");

	pStreamInfo->audio.channels = m_audio.channels = parser->GetChannelCount();
	pStreamInfo->audio.sample_bits = m_audio.sample_bits = 16;
	pStreamInfo->audio.sample_rate = m_audio.sample_rate = parser->GetSampleRate();
	m_aac.object_type = parser->GetAACObjectType();
	m_aac.frame_type = VOAAC_RAWDATA;
	if (m_nMaxFrameSize < MIN_AAC_FRAME_SIZE)
		m_nMaxFrameSize = MIN_AAC_FRAME_SIZE;
	if (m_nMaxFrameSize > MAX_AAC_FRAME_SIZE)
		m_nMaxFrameSize = MAX_AAC_FRAME_SIZE;

	// make header data
	m_nHeadSize = 2;
	m_pHeadData = new uint8[m_nHeadSize];
	memcpy(m_pHeadData, parser->GetAudioSpecificConfig(), m_nHeadSize);

	if (m_pFrameChecker)
	{
		delete m_pFrameChecker;
	}

	m_pFrameChecker = FrameChecker::CreateCheckerLATM(parser);
	//m_pFrameChecker = FrameChecker::CreateCheckerDefault(); //test only
}


void CStreamFormat::InitRMX(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CheckSaveHeadData(pStreamInfo);
	//m_nMaxFrameSize = 1024;

	// 20100505, need create frame checker?
	//m_pFrameChecker = FrameChecker::CreateCheckerDefault();
	// end
}

void CStreamFormat::InitM2TSLPCM(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CheckSaveHeadData(pStreamInfo);
	m_pFrameChecker = FrameChecker::CreateCheckerDefault();
}
void CStreamFormat::InitAC3(VOSTREAMPARSESTREAMINFO* pStreamInfo)
{
	CheckSaveHeadData(pStreamInfo);
	m_pFrameChecker = FrameChecker::CreateCheckerAC3();
}
