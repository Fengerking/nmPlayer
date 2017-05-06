#include"CAC3Reader.h"
#include"fCC.h"
#include"BitsFunctions.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define AC3_HEADER_SIZE 7
#define MALLOC_FAILED_ERROR -3
#define DEFAULT_PER_FRM_TIME  32    //default per-FRM time :ms


#define VOMAX(a,b) ((a) > (b) ? (a) : (b))
#define VOMAX3(a,b,c) FFMAX(FFMAX(a,b),c)
#define VOMIN(a,b) ((a) > (b) ? (b) : (a))
#define VOMIN3(a,b,c) FFMIN(FFMIN(a,b),c)

static const VO_U8 eac3_blocks[4] = {
    1, 2, 3, 6
};

CAC3Reader::CAC3Reader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
			: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
			,m_pAC3HeaderInfo(VO_NULL),m_FirstFrmPos(0),m_FrmSize(0)
			,m_CurSampleIndex(0),m_curReadPos(0),m_PerFrmTime(0)
{
	m_pAC3HeaderInfo = (AC3HeaderInfo *)malloc(sizeof(AC3HeaderInfo));
	memset(m_pAC3HeaderInfo,0,sizeof(AC3HeaderInfo));
	memset(m_AC3HeaderData,0,sizeof(m_AC3HeaderData));
}

CAC3Reader::~CAC3Reader()
{
	if(m_pAC3HeaderInfo)
	{
		free(m_pAC3HeaderInfo);
		m_pAC3HeaderInfo = VO_NULL;
	}

}

VO_U32	CAC3Reader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_pAC3HeaderInfo->channels;
	pAudioFormat->SampleRate = m_pAC3HeaderInfo->sample_rate;
	pAudioFormat->SampleBits = m_pAC3HeaderInfo->frame_size*8;
	return VO_ERR_SOURCE_OK;
}

VO_U32	CAC3Reader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
	m_FirstFrmPos = GetFirstFrmPos();
	if(m_FirstFrmPos < 0)
		return VO_ERR_SOURCE_FORMATUNSUPPORT;
	else//(m_FirstFrmPos >= 0)legality ac3 file
	{
		GetBitContext gbc;
		memset(&gbc,0,sizeof(GetBitContext));
		VO_BYTE *sContent = VO_NULL;
		sContent = (VO_BYTE *)malloc(m_FrmSize + 1);
		if(!sContent)
			return MALLOC_FAILED_ERROR;
		FileSeek(m_hFile,m_FirstFrmPos,VO_FILE_BEGIN);
		FileRead(m_hFile,sContent,m_FrmSize);

		memcpy(m_AC3HeaderData,sContent,AC3_HEADER_LEN);//get the header data
		m_AC3HeaderData[AC3_HEADER_LEN] = 0;
		gbc.buffer = sContent;
		gbc.index = 0;
		gbc.size_in_bits = m_FrmSize*8;
		vo_ac3_parse_header(&gbc,m_pAC3HeaderInfo);
		if(m_pAC3HeaderInfo->bit_rate)
		{
			m_PerFrmTime = ((m_pAC3HeaderInfo->frame_size*8*1000)/m_pAC3HeaderInfo->bit_rate);
			m_dwDuration = (VO_U32)((m_ullFileSize*8*1000) / m_pAC3HeaderInfo->bit_rate);
		}
		else
			m_PerFrmTime = DEFAULT_PER_FRM_TIME;
		if(sContent)
			free(sContent);
		m_pBuf = NEW_BUFFER(m_FrmSize +1);
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAC3Reader::GetFirstFrmPos()
{
	VO_U16 FirstFrmPos = 0;
	VO_U16 syn_word = 0;
	VO_U8 frmsizecode = 0;
	VO_U8 streadid = 0;
	VO_U16 framesize = 0;
	//while(VO_TRUE){
	while(m_ullFileSize - (FirstFrmPos+2)){
		read_word_l(syn_word);
		syn_word = ((syn_word&0x00FF)<<8)|((syn_word&0xFF00)>>8);
		if(0x0B77 == syn_word)
		{
			FirstFrmPos += 2;
			skip(2);
			VO_U8 strmid_frmsize = 0;
			read_byte(strmid_frmsize);
			frmsizecode = strmid_frmsize&0x3F;
			streadid = (strmid_frmsize&0xC0)>>6;
			framesize = vo_ac3_frame_size_tab[frmsizecode][streadid]*2;
			skip(framesize-5);
			read_word_l(syn_word)
			syn_word = ((syn_word&0x00FF)<<8)|((syn_word&0xFF00)>>8);
			if(0x0B77 == syn_word)//found
			{
				m_FrmSize = framesize;
				return (FirstFrmPos - 2);//back to the sync word
			}
			else
			{
				FileSeek(m_hFile,FirstFrmPos,VO_FILE_BEGIN);
				continue;
			}
		}
	}	

	return AAC_AC3_PARSE_ERROR_SYNC;
}

VO_S64  CAC3Reader::Getchannel_layout()
{
	return m_pAC3HeaderInfo->channel_layout;
}

VO_U32	CAC3Reader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_S32 nRes = FileRead(m_hFile,m_pBuf,m_FrmSize);
	if(nRes <= 0)
	return (-2 == nRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	pSample->Buffer = m_pBuf;
	pSample->Size = nRes;
	pSample->Time = m_CurSampleIndex*m_PerFrmTime;
	pSample->Duration = 1;
	m_CurSampleIndex++;
	return VO_ERR_SOURCE_OK;
}
VO_U32  CAC3Reader::GetChannelNum()
{
	return m_pAC3HeaderInfo->channels;
}
VO_U32	CAC3Reader::SetPos(VO_S64* pPos)
{
	if(!m_PerFrmTime)
		return AAC_AC3_PARSE_ERROR_FRAME_SIZE;
	m_CurSampleIndex = (VO_U32)(*pPos/m_PerFrmTime);
	VO_S32 nSetPos = m_CurSampleIndex*m_FrmSize;
	if(nSetPos < FileSize(m_hFile))
		FileSeek(m_hFile,nSetPos,VO_FILE_BEGIN);
	else
		return VO_ERR_SOURCE_END;
	return VO_ERR_SOURCE_OK;
}
VO_U32	CAC3Reader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = AC3_HEADER_LEN;
	pHeadData->Buffer = m_AC3HeaderData;

	return VO_ERR_SOURCE_OK;
}


VO_U32 CAC3Reader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = m_pAC3HeaderInfo->bit_rate;
	return VO_ERR_SOURCE_OK;
}

VO_S32 CAC3Reader::vo_ac3_parse_header(GetBitContext *pBc, AC3HeaderInfo *pAC3Headerinfo)
{
    VO_U32 frame_size_code;

    memset(pAC3Headerinfo, 0, sizeof(*pAC3Headerinfo));

    pAC3Headerinfo->sync_word = (VO_U16)(vo_get_ac3_bits(pBc, 16));
    if(pAC3Headerinfo->sync_word != 0x0B77)
        return AAC_AC3_PARSE_ERROR_SYNC;

    /* read ahead to bsid to distinguish between AC-3 and E-AC-3 */
    pAC3Headerinfo->bitstream_id = vo_show_ac3_bits_long(pBc, 29) & 0x1F;
    if(pAC3Headerinfo->bitstream_id > 16)
        return AAC_AC3_PARSE_ERROR_BSID;

    pAC3Headerinfo->num_blocks = 6;

    /* set default mix levels */
    pAC3Headerinfo->center_mix_level   = 1;  // -4.5dB
    pAC3Headerinfo->surround_mix_level = 1;  // -6.0dB

    if(pAC3Headerinfo->bitstream_id <= 10) {
        /* Normal AC-3 */
        pAC3Headerinfo->crc1 = (VO_U16)(vo_get_ac3_bits(pBc, 16));
        pAC3Headerinfo->sr_code = (VO_BYTE)(vo_get_ac3_bits(pBc, 2));
        if(pAC3Headerinfo->sr_code == 3)
            return AAC_AC3_PARSE_ERROR_SAMPLE_RATE;

        frame_size_code = vo_get_ac3_bits(pBc, 6);
        if(frame_size_code > 37)
            return AAC_AC3_PARSE_ERROR_FRAME_SIZE;

        vo_ac3_skip_bits(pBc, 5); // skip bsid, already got it

        vo_ac3_skip_bits(pBc, 3); // skip bitstream mode
        pAC3Headerinfo->channel_mode = (VO_BYTE)(vo_get_ac3_bits(pBc, 3));

        if(pAC3Headerinfo->channel_mode == AC3_CHMODE_STEREO) {
            vo_ac3_skip_bits(pBc, 2); // skip dsurmod
        } else {
            if((pAC3Headerinfo->channel_mode & 1) && pAC3Headerinfo->channel_mode != AC3_CHMODE_MONO)
                pAC3Headerinfo->center_mix_level = vo_get_ac3_bits(pBc, 2);
            if(pAC3Headerinfo->channel_mode & 4)
                pAC3Headerinfo->surround_mix_level = vo_get_ac3_bits(pBc, 2);
        }
        pAC3Headerinfo->lfe_on = (VO_BYTE)(vo_get_ac3_bits1(pBc));

        pAC3Headerinfo->sr_shift = VOMAX(pAC3Headerinfo->bitstream_id, 8) - 8;
        pAC3Headerinfo->sample_rate = vo_ac3_sample_rate_tab[pAC3Headerinfo->sr_code] >> pAC3Headerinfo->sr_shift;
        pAC3Headerinfo->bit_rate = (vo_ac3_bitrate_tab[frame_size_code>>1] * 1000) >> pAC3Headerinfo->sr_shift;
        pAC3Headerinfo->channels = vo_ac3_channels_tab[pAC3Headerinfo->channel_mode] + pAC3Headerinfo->lfe_on;
        pAC3Headerinfo->frame_size = vo_ac3_frame_size_tab[frame_size_code][pAC3Headerinfo->sr_code] * 2;
        pAC3Headerinfo->frame_type = EAC3_FRAME_TYPE_AC3_CONVERT; //EAC3_FRAME_TYPE_INDEPENDENT;
        pAC3Headerinfo->substreamid = 0;
    } else {
        /* Enhanced AC-3 */
        pAC3Headerinfo->crc1 = 0;
        pAC3Headerinfo->frame_type = (VO_BYTE)(vo_get_ac3_bits(pBc, 2));
        if(pAC3Headerinfo->frame_type == EAC3_FRAME_TYPE_RESERVED)
            return AAC_AC3_PARSE_ERROR_FRAME_TYPE;

        pAC3Headerinfo->substreamid = vo_get_ac3_bits(pBc, 3);

        pAC3Headerinfo->frame_size = (VO_U16)((vo_get_ac3_bits(pBc, 11) + 1) << 1);
        if(pAC3Headerinfo->frame_size < AC3_HEADER_SIZE)
            return AAC_AC3_PARSE_ERROR_FRAME_SIZE;

        pAC3Headerinfo->sr_code = (VO_BYTE)(vo_get_ac3_bits(pBc, 2));
        if (pAC3Headerinfo->sr_code == 3) {
            int sr_code2 = vo_get_ac3_bits(pBc, 2);
            if(sr_code2 == 3)
                return AAC_AC3_PARSE_ERROR_SAMPLE_RATE;
            pAC3Headerinfo->sample_rate = vo_ac3_sample_rate_tab[sr_code2] / 2;
            pAC3Headerinfo->sr_shift = 1;
        } else {
            pAC3Headerinfo->num_blocks = eac3_blocks[vo_get_ac3_bits(pBc, 2)];
            pAC3Headerinfo->sample_rate = vo_ac3_sample_rate_tab[pAC3Headerinfo->sr_code];
            pAC3Headerinfo->sr_shift = 0;
        }

        pAC3Headerinfo->channel_mode = (VO_BYTE)(vo_get_ac3_bits(pBc, 3));
        pAC3Headerinfo->lfe_on = (VO_BYTE)(vo_get_ac3_bits1(pBc));

        pAC3Headerinfo->bit_rate = (VO_U32)(8.0 * pAC3Headerinfo->frame_size * pAC3Headerinfo->sample_rate /
                        (pAC3Headerinfo->num_blocks * 256.0));
        pAC3Headerinfo->channels = vo_ac3_channels_tab[pAC3Headerinfo->channel_mode] + pAC3Headerinfo->lfe_on;
    }
    pAC3Headerinfo->channel_layout = vo_ac3_channel_layout_tab[pAC3Headerinfo->channel_mode];
    if (pAC3Headerinfo->lfe_on)
        pAC3Headerinfo->channel_layout |= CH_LOW_FREQUENCY;

    return 0;

}


VO_U32	CAC3Reader::GetCodecCC(VO_U32* pCC)
{
	*pCC = AudioFlag_AC3;

	return VO_ERR_SOURCE_OK;
}
