#include "decoder.h"
#ifdef	BSAC_DEC
#include "decode_bsac.h"
#include "struct.h"
#include "sam_dec.h"

#define SF_OFFSET   100
#define TEXP    128
#define MAX_IQ_TBL  128

const short sfb_96_1024[] =
{
	  4,   8,  12,  16,  20,   24,  28, 
	 32,  36,  40,  44,  48,   52,  56, 
	 64,  72,  80,  88,  96,  108, 120, 
	132, 144, 156, 172, 188,  212, 240, 
	276, 320, 384, 448, 512,  576, 640, 
	704, 768, 832, 896, 960, 1024
};   /* 41 scfbands */

const short sfb_96_128[] =
{
	4,   8, 12, 16, 20, 24, 32, 
	40, 48, 64, 92, 128
};   /* 12 scfbands */

const short sfb_96_960[] =
{
	  4,   8,  12,  16,  20,  24,  28, 
	 32,  36,  40,  44,  48,  52,  56, 
	 64,  72,  80,  88,  96, 108, 120, 
	132, 144, 156, 172, 188, 212, 240, 
	276, 320, 384, 448, 512, 576, 640, 
	704, 768, 832, 896, 960
};   /* 41 scfbands */

const short sfb_96_120[] =
{
	4, 8, 12, 16, 20, 24, 32, 
    40, 48, 64, 92, 120
};   /* 12 scfbands */

const short sfb_64_1024[] =
{
	  4,   8,  12,  16,  20,  24,  28, 
	 32,  36,  40,  44,  48,  52,  56, 
	 64,  72,  80,  88, 100, 112, 124, 
	140, 156, 172, 192, 216, 240, 268, 
	304, 344, 384, 424, 464, 504, 544, 
	584, 624, 664, 704, 744, 784, 824, 
	864, 904, 944, 984, 1024
};   /* 41 scfbands 47 */ 

const short sfb_64_128[] =
{
	4, 8, 12, 16, 20, 24, 32, 
	40, 48, 64, 92, 128
};   /* 12 scfbands */

const short sfb_64_960[] =
{
	  4,   8,  12,  16,  20,  24,  28, 
	 32,  36,  40,  44,  48,  52,  56, 
	 64,  72,  80,  88, 100, 112, 124, 
	140, 156, 172, 192, 216, 240, 268, 
	304, 344, 384, 424, 464, 504, 544, 
	584, 624, 664, 704, 744, 784, 824, 
	864, 904, 944, 960
};   /* 41 scfbands 47 */ 

const short sfb_64_120[] =
{
	 4,  8, 12, 16, 20, 24, 32, 
	40, 48, 64, 92, 120
};   /* 12 scfbands */

#ifdef SRS
static const int     ssr_decoder_band = 4;
#endif


const short sfb_48_1024[] = {
  4,	8,		12,		16,		20,		24,		28,	
  32,	36,		40,		48,		56,		64,		72,	
  80,	88,		96,		108,	120,	132,	144,	
  160,	176,	196,	216,	240,	264,	292,	
  320,	352,	384,	416,	448,	480,	512,	
  544,	576,	608,	640,	672,	704,	736,	
  768,	800,	832,	864,	896,	928,	1024
};

const short sfb_48_960[] = { 
  4,      8,      12,     16,     20,     24,     28,
  32,     36,     40,     48,     56,     64,     72,
  80,     88,     96,     108,    120,    132,    144,
  160,    176,    196,    216,    240,    264,    292,
  320,    352,    384,    416,    448,    480,    512,
  544,    576,    608,    640,    672,    704,    736,
  768,    800,    832,    864,    896,    928,    960, 0 };

const short sfb_48_512[] = {
  4,      8,      12,     16,     20,     24,     28, 
  32,     36,     40,     44,     48,     52,     56, 
  60,     68,     76,     84,     92,     100,    112,
  124,    136,    148,    164,    184,    208,    236,
  268,    300,    332,    364,    396,    428,    460,
  512
};   /* 36 scfbands */

const short sfb_48_480[] = { 
  4,      8,      12,     16,     20,     24,     28, 
  32,     36,     40,     44,     48,     52,     56, 
  64,     72,     80,     88,     96,     108,    120,
  132,    144,    156,    172,    188,    212,    240,
  272,    304,    336,    368,    400,    432,    480
};   /* 35 scfbands */

const short sfb_48_128[] =
{
	4,	8,	12,	16,	20,	 28, 36,	
	44,	56,	68,	80,	96,	112, 128
};

const short sfb_48_120[] =
{
	4,   8,   12,  16,  20,  28,  36,
    44,  56,  68,  80,  96, 112, 120
};

const short sfb_32_1024[] =
{
	4,		8,		12,		16,		20,		24,		28,	
	32,		36,		40,		48,		56,		64,		72,	
	80,		88,		96,		108,	120,	132,	144,	
	160,	176,	196,	216,	240,	264,	292,	
	320,	352,	384,	416,	448,	480,	512,	
	544,	576,	608,	640,	672,	704,	736,	
	768,	800,	832,	864,	896,	928,	960,
	992,	1024
};

const short sfb_32_512[] =
{
    4,     8,       12,     16,     20,     24,     28, 
	32,    36,      40,     44,     48,     52,     56, 
	64,    72,      80,     88,     96,     108,    120, 
	132,   144,     160,    176,    192,    212,    236, 
	260,   288,     320,    352,    384,    416,    448, 
	480,   512
};   /* 37 scfbands */

const short sfb_32_480[] =
{ 
	4,     8,       12,     16,     20,      24,     28, 
	32,    36,      40,     44,     48,      52,     56, 
	60,    64,      72,     80,     88,      96,     104, 
	112,   124,     136,    148,    164,     180,    200, 
	224,   256,     288,    320,    352,     384,    416, 
	448,   480
};   /* 37 scfbands */

const short sfb_24_1024[] =
{
	4,   8,   12,  16,  20,  24,  28, 
	32,  36,  40,  44,  52,  60,  68, 
	76,  84,  92,  100, 108, 116, 124, 
	136, 148, 160, 172, 188, 204, 220, 
	240, 260, 284, 308, 336, 364, 396, 
	432, 468, 508, 552, 600, 652, 704, 
	768, 832, 896, 960, 1024
};   /* 47 scfbands */

const short sfb_24_960[] =
{
	4,   8,   12,  16,  20,  24,  28,
	32,  36,  40,  44,  52,  60,  68,
	76,  84,  92,  100, 108, 116, 124,
	136, 148, 160, 172, 188, 204, 220,
	240, 260, 284, 308, 336, 364, 396,
	432, 468, 508, 552, 600, 652, 704,
	768, 832, 896, 960,0
};   /* 47 scfbands */

const short sfb_24_128[] =
{
	4, 8, 12, 16, 20, 24, 28, 36, 
	44, 52, 64, 76, 92, 108, 128
};   /* 15 scfbands */

const short sfb_24_120[] =
{
	4, 8, 12, 16, 20, 24, 28, 36, 
	44, 52, 64, 76, 92, 108, 120, 0
};   /* 15 scfbands */

const short sfb_24_512[] =
{
	4,   8,   12,  16,  20,  24,  28,  
	32,  36,  40,  44,  52,  60,  68, 
	80,  92,  104, 120, 140, 164, 192, 
	224, 256, 288, 320, 352, 384, 416, 
	448, 480, 512
};   /* 31 scfbands */

const short sfb_24_480[] =
{
	4,   8,   12,  16,  20,  24,  28,  
	32,  36,  40,  44,  52,  60,  68, 
	80,  92,  104, 120, 140, 164, 192, 
	224, 256, 288, 320, 352, 384, 416, 
	448, 480
};   /* 30 scfbands */

const short sfb_16_1024[] =
{
 	 8,  16,  24,  32,  40,  48,  56, 
  	64,  72,  80,  88,  100, 112, 124, 
	136, 148, 160, 172, 184, 196, 212, 
	228, 244, 260, 280, 300, 320, 344, 
	368, 396, 424, 456, 492, 532, 572, 
	616, 664, 716, 772, 832, 896, 960, 
	1024
};   /* 43 scfbands */

const short sfb_16_960[] =
{
	 8,   16,  24,  32,  40,  48,  56, 
	 64,  72,  80,  88, 100, 112, 124, 
	136, 148, 160, 172, 184, 196, 212, 
	228, 244, 260, 280, 300, 320, 344, 
	368, 396, 424, 456, 492, 532, 572, 
	616, 664, 716, 772, 832, 896, 960
};   /* 42 scfbands */

const short sfb_16_128[] =
{
	4, 8, 12, 16, 20, 24, 28, 32, 
	40, 48, 60, 72, 88, 108, 128
};   /* 15 scfbands */

const short sfb_16_120[] =
{
	4, 8, 12, 16, 20, 24, 28, 32, 
	40, 48, 60, 72, 88, 108, 120
};   /* 15 scfbands */


const short sfb_8_1024[] =
{
	 12, 24, 36, 48, 60, 72, 84, 
	 96, 108, 120, 132, 144, 156, 172, 
	188, 204, 220, 236, 252, 268, 288, 
	308, 328, 348, 372, 396, 420, 448, 
	476, 508, 544, 580, 620, 664, 712, 
	764, 820, 880, 944, 1024
};   /* 40 scfbands */

const short sfb_8_128[] =
{
	 4, 8, 12, 16, 20, 24, 28, 
	36, 44, 52, 60, 72, 88, 108, 
	128
};   /* 15 scfbands */

const short sfb_8_960[] =
{
	 12, 24, 36, 48, 60, 72, 84, 
	 96, 108, 120, 132, 144, 156, 172, 
	188, 204, 220, 236, 252, 268, 288, 
	308, 328, 348, 372, 396, 420, 448, 
	476, 508, 544, 580, 620, 664, 712, 
	764, 820, 880, 944, 960
};   /* 40 scfbands */

const short sfb_8_120[] =
{
	4, 8, 12, 16, 20, 24, 28, 
	36, 44, 52, 60, 72, 88, 108, 
	120
};   /* 15 scfbands */

const short sfb_32_960[] =
{ 
	  4, 8, 12, 16,  20,  24,  28, 32, 
	 36,  40,  48,  56,  64,  72,  80, 
	 88,  96, 108, 120, 132, 144, 160, 
	176, 196, 216, 240, 264, 292, 320, 
    352, 384, 416, 448, 480, 512, 544, 
	576, 608, 640, 672, 704, 736, 768, 
	800, 832, 864, 896, 928, 960, 0 
};

const short sfb_32_120[] =
{ 
	4, 8, 12, 16, 20, 28, 36,
	44, 56, 68, 80, 96, 112, 120, 0 
};

typedef struct {
	int     samp_rate;
	int     nsfb1024;
	const short*  SFbands1024;
	int     nsfb128;
	const short*  SFbands128;
	int     nsfb960;
	const short*  SFbands960;
	int     nsfb120;
	const short*  SFbands120;
	int    shortFssWidth;
	int    longFssGroups;
	int     nsfb480;
	const short*  SFbands480;
	int     nsfb512;
	const short*  SFbands512;
} SR_Info;

SR_Info samp_rate_info[16] =
{
	/* sampling_frequency, #long sfb, long sfb, #short sfb, short sfb */
	/* samp_rate, nsfb1024, SFbands1024, nsfb128, SFbands128 */
	{96000, 41, sfb_96_1024, 12, sfb_96_128, 40, sfb_96_960, 12, sfb_96_120,   8, 4
		,  0,          0,  0,         0
	},	    /* 96000 */
	{88200, 41, sfb_96_1024, 12, sfb_96_128, 40, sfb_96_960, 12, sfb_96_120,   8, 4
	,  0,          0,  0,         0
	},	    /* 88200 */
	{64000, 47, sfb_64_1024, 12, sfb_64_128, 46, sfb_64_960, 12, sfb_64_120,  13, 5
	,  0,          0,  0,         0
	},	    /* 64000 */
	{48000, 49, sfb_48_1024, 14, sfb_48_128, 49, sfb_48_960, 14, sfb_48_120,  18, 5
	, 35, sfb_48_480, 36, sfb_48_512 
	},	    /* 48000 */
	{44100, 49, sfb_48_1024, 14, sfb_48_128, 49, sfb_48_960, 14, sfb_48_120,  18, 5
	, 35, sfb_48_480, 36, sfb_48_512 
	},	    /* 44100 */
	{32000, 51, sfb_32_1024, 14, sfb_48_128, 49, sfb_32_960, 14, sfb_48_120,  26, 6
	, 37, sfb_32_480, 37, sfb_32_512
	},	    /* 32000 */
	{24000, 47, sfb_24_1024, 15, sfb_24_128, 46, sfb_24_960, 15, sfb_24_120,  36, 8
	, 30, sfb_24_480, 31, sfb_24_512
	},	    /* 24000 */
	{22050, 47, sfb_24_1024, 15, sfb_24_128, 46, sfb_24_960, 15, sfb_24_120,  36, 8
	, 30, sfb_24_480, 31, sfb_24_512
	},	    /* 22050 */
	{16000, 43, sfb_16_1024, 15, sfb_16_128, 42, sfb_16_960, 15, sfb_16_120,  54,  8
	,  0,          0,  0,         0
	},	    /* 16000 */
	{12000, 43, sfb_16_1024, 15, sfb_16_128, 42, sfb_16_960, 15, sfb_16_120, 104, 10
	,  0,          0,  0,         0
	},	    /* 12000 */
	{11025, 43, sfb_16_1024, 15, sfb_16_128, 42, sfb_16_960, 15, sfb_16_120, 104, 10
	,  0,          0,  0,         0
	},	    /* 11025 */
	{ 8000, 40,  sfb_8_1024, 15,  sfb_8_128, 40,  sfb_8_960, 15,  sfb_8_120, 104, 9
	,  0,          0,  0,         0
	},	    /*  8000 */
	{    0,  0,           0,  0,          0,  0,          0,  0,          0, 0,   0
	,  0,          0,  0,         0
	}
};

void sam_scale_bits_init(AACDecoder* decoder,int sampling_rate_decoded, int block_size_samples)
{
	int layer, layer_bit_rate;
	int average_layer_bits;
	int init_pns_noise_nrg_model = 1; 
	int nch = decoder->channelNum;
	BSAC_AAC* bsac = decoder->bsac;
	int* sam_scale_bits_dec = bsac->sam_scale_bits_dec;
	short* AModelNoiseNrg = bsac->AModelNoiseNrg;
	/* calculate the avaerage amount of bits available
	for the scalability layer of one block per channel */
	for (layer=0; layer<MAX_LAYER; layer++) {
		layer_bit_rate = (layer+16) * 1000;//hbfTODO:
#if !USE_DIVIDE_FUNC
		average_layer_bits = (layer_bit_rate*block_size_samples/sampling_rate_decoded);
#else
		average_layer_bits = voUnsignedDivide(decoder,layer_bit_rate*block_size_samples,sampling_rate_decoded);
		//(int)((double)layer_bit_rate*(double)block_size_samples/(double)sampling_rate_decoded);
#endif
		if(nch==2)
			*sam_scale_bits_dec++ = (average_layer_bits >> 3) << 4;
		else
			*sam_scale_bits_dec++ = (average_layer_bits >> 3) << 3;
	}
	
	decoder->sampleRate = sampling_rate_decoded;
	if (init_pns_noise_nrg_model) {
		int i;
		
		AModelNoiseNrg[0] = 16384 - 32;
		for (i=1; i < 512; i++) 
			AModelNoiseNrg[i] = AModelNoiseNrg[i-1] - 32;
		init_pns_noise_nrg_model = 0;
	}
}


int sam_decode_init(AACDecoder* decoder,int sampling_rate_decoded, int block_size_samples)
{
	int  i,fs_idx;
	int *outdata;
	const short *indata;
	BSAC_AAC* bsac;

	bsac = (BSAC_AAC*)voAACDecAlignedMalloc(decoder->voMemop, sizeof(BSAC_AAC));
	if(bsac == NULL)
		return VO_ERR_OUTOF_MEMORY;

	decoder->bsac = bsac;
	bsac->preWinSequence = -1;	
	
	sam_scale_bits_init(decoder,sampling_rate_decoded, block_size_samples);
	
	fs_idx = 3;//Fs_48;
	for(i = 0; i < 16/*(1<<LEN_SAMP_IDX)*/; i++) {
		if(sampling_rate_decoded == samp_rate_info[i].samp_rate) {
			fs_idx = i;
			break;
		}
	}
	
	if(block_size_samples == 1024) 
	{
		bsac->long_sfb_top = samp_rate_info[fs_idx].nsfb1024;
		bsac->short_sfb_top = samp_rate_info[fs_idx].nsfb128;
		outdata = bsac->swb_offset_long;
		*outdata++ = 0;
		indata = samp_rate_info[fs_idx].SFbands1024;
		
		for(i = bsac->long_sfb_top; i; i--)
			*outdata++ = *indata++;

		outdata = bsac->swb_offset_short;
		*outdata++ = 0;
		indata = samp_rate_info[fs_idx].SFbands128;
		for(i = bsac->short_sfb_top; i; i--)
			*outdata++ = *indata++;
	}
	else
	{
		return VO_ERR_AUDIO_UNSFEATURE;
	}
	
	return 0;	
}


int bsac_raw_data_block(AACDecoder *decoder,unsigned char *inptr,int sizeInBytes)
{
	int           i, ch, b;
	int			  w;
	int			  sfb;
	int			  top_band;
	int           max_sfb;
	int           groupInfo[7];
	int           stereo_mode;
	BitStream	  *bs;
	int			  used_bits;


	int	      nch = decoder->channelNum;
	int       *samples,*samples1;
	int       *outdata,*indata;
	BSAC_AAC* bsac = decoder->bsac;
	VO_MEM_OPERATOR *voMemop;
	

	//int bitsUsed;
	ICS_Data* ICS_Data[2];

	bs = &decoder->bs;
	voMemop = decoder->voMemop;

	if(sizeInBytes <= 2)
		return VO_ERR_INPUT_BUFFER_SMALL;

	decoder->frame_length = (inptr[0] << 3) | (inptr[1] >> 5);
	if(decoder->frame_length > sizeInBytes)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	BitStreamInit(bs, decoder->frame_length, inptr);

	ICS_Data[0] = &(decoder->ICS_Data[0]);
	ICS_Data[1] = &(decoder->ICS_Data[1]);
	decoder->common_window = 1;
	/********* READ BITSTREAM **********/
	/* ***************************************************** */
	/* # the side information part for Mulit-channel       # */ 
	/* ***************************************************** */	

	/* ***************************************************** */
	/* #                  frame_length                     # */
	/* ***************************************************** */
	b = BitStreamGetBits(bs, 11);//bytes
	
	decoder->frame_length = b;

	/* ***************************************************** */
	/* #                 bsac_header()                     # */
	/* ***************************************************** */	
	bsac->header_length  = BitStreamGetBits(bs, 4);
	bsac->sba_mode       = BitStreamGetBits(bs, 1);
	bsac->top_layer      = BitStreamGetBits(bs, 6);
	bsac->base_snf_thr   = BitStreamGetBits(bs, 2) + 1;

#if !SUPPORT_SBA_MODE	
	if(bsac->sba_mode)
	{
		error(decoder, "this decoder does not support sba\n", 7);
		return -1;
	}
#endif

	for(ch = 0; ch < nch; ch++)
		bsac->max_scalefactor[ch]  = BitStreamGetBits(bs, 8); 
	
	bsac->base_band     = BitStreamGetBits(bs, 5);
	
	for(ch = 0; ch < nch; ch++) {
		bsac->cband_si_type[ch]  = BitStreamGetBits(bs, 5); 
		bsac->scf_model0[ch]     = BitStreamGetBits(bs, 3); 
		bsac->scf_model1[ch]    =  BitStreamGetBits(bs, 3); 
		bsac->max_sfb_si_len[ch]  = BitStreamGetBits(bs, 4);
	}

	/* ***************************************************** */
	/* #                general_header()                   # */
	/* ***************************************************** */
	/* ics_reserved_bit  */
	ICS_Data[0]->ics_reserved_bit  = (VO_U8)BitStreamGetBits(bs, 1); 
	
	/*  window sequence */
	ICS_Data[0]->window_sequence  = (WINDOW_SEQUENCE)BitStreamGetBits(bs, 2); 
	
	/*  window shape */
	ICS_Data[0]->window_shape  =  (VO_U8)BitStreamGetBits(bs, 1); 
	
	if (ICS_Data[0]->window_sequence == EIGHT_SHORT_SEQUENCE)
	{
		max_sfb = BitStreamGetBits(bs, 4);
		
		for (i = 0; i < 7; i++)
			groupInfo[i] =  BitStreamGetBits(bs, 1); 
	}
	else 
	{
		max_sfb = BitStreamGetBits(bs, 6);
	}

	ICS_Data[0]->max_sfb = max_sfb;
	ICS_Data[0]->num_window_groups  = 1;
	ICS_Data[0]->window_group_length[0]  = 1;
	
	if (ICS_Data[0]->window_sequence == 2) {
		b = ICS_Data[0]->num_window_groups;
		for (i=0; i<7; i++) {
			if (groupInfo[i]==0) {
				ICS_Data[0]->window_group_length[b]  = 1;
				b++;
			}
			else {
				ICS_Data[0]->window_group_length[b-1]++;
			}
		}

		ICS_Data[0]->num_window_groups = b;
	}

	if(nch == 2) {
		ICS_Data[1]->window_sequence =ICS_Data[0]->window_sequence;
		ICS_Data[1]->window_shape = ICS_Data[0]->window_shape;
	}


	/* ***************************************************** */
	/* #                  PNS data                         # */
	/* ***************************************************** */	 
	decoder->pns_data_present[0]  = BitStreamGetBits(bs, 1);
	if(decoder->pns_data_present[0])
		decoder->pns_start_sfb  = BitStreamGetBits(bs, 6);

	voMemop->Set(VO_INDEX_DEC_AAC, decoder->pns_sfb_flag,0,sizeof(decoder->pns_sfb_flag));
	voMemop->Set(VO_INDEX_DEC_AAC, decoder->pns_sfb_mode,0,sizeof(decoder->pns_sfb_mode));
	
	/* ***************************************************** */
	/* #                  stereo mode                      # */
	/* ***************************************************** */
	decoder->stereo_mode = 0;
	if(nch == 2) {
		decoder->stereo_mode = stereo_mode = BitStreamGetBits(bs, 2);
		decoder->ms_mask_present = stereo_mode;
		if(stereo_mode == 1) {
			voMemop->Set(VO_INDEX_DEC_AAC, decoder->ms_used,0,sizeof(decoder->ms_used));
		} else if(stereo_mode == 3) {
			voMemop->Set(VO_INDEX_DEC_AAC, decoder->ms_used,0,sizeof(decoder->ms_used));
			voMemop->Set(VO_INDEX_DEC_AAC, decoder->is_used,0,sizeof(decoder->is_used));
			decoder->intensity_used  = 1;
		}
	}
	
	/* ***************************************************** */
	/* #                  TNS & LTP info                   # */
	/* ***************************************************** */
	for(ch = 0; ch < nch; ch++) {
		decoder->tns_data_present[ch]  = BitStreamGetBits(bs, 1);
		if(decoder->tns_data_present[ch]) {
			tns_data(decoder, bs, ICS_Data[0]->window_sequence, ch);
		}
		
		ICS_Data[ch]->predictor_data_present = (VO_U8)BitStreamGetBits(bs, 1);
		if(ICS_Data[ch]->predictor_data_present) 
		{
			return VO_ERR_AUDIO_UNSFEATURE;
		}
	}
	
	BitStreamByteAlign(bs);

	used_bits = CalcBitsUsed(bs, inptr, 0);

	/* ***************************************************** */
	/* #                  BSAC  D E C O D I N G            # */
	/* ***************************************************** */
	top_band = bsac->long_sfb_top;
	
	if(ICS_Data[0]->window_sequence == EIGHT_SHORT_SEQUENCE) {
		b = 1;
		for(w = 0; w < ICS_Data[0]->num_window_groups; w++, b++) {
			outdata = bsac->swb_offset[w];
			*outdata++ = 0;
			b = ICS_Data[0]->window_group_length[w];
			indata = bsac->swb_offset_short + 1;
			
			for(sfb = bsac->short_sfb_top; sfb; sfb--) {
				*outdata++ = (*indata++) * b;
			}
		}
		top_band = bsac->short_sfb_top;
	} else {
		outdata = bsac->swb_offset[0];
		*outdata++ = 0;
		indata = bsac->swb_offset_long + 1;

		for(sfb = bsac->long_sfb_top+1; sfb; sfb--)
			*outdata++ = *indata++;
		top_band = bsac->long_sfb_top;
	}
	
	if(ICS_Data[0]->window_sequence == EIGHT_SHORT_SEQUENCE) {
		int offset, windowsl, s, k, *tmpBuffer;
		tmpBuffer = (int *)decoder->tmpBuffer;
		sam_decode_bsac_stream(decoder,ICS_Data[0],bsac->long_sfb_top, used_bits,tmpBuffer);
		samples  = decoder->coef[0];
		samples1 = decoder->coef[1];
		s = 0;
		for(w = 0; w < ICS_Data[0]->num_window_groups; w++) {
			windowsl = ICS_Data[0]->window_group_length[w];
			for(i = 0; i < 128; i+=4) {
				offset = (128 * s) + (i * windowsl);
				for(k = 0; k < 4; k++) 
				{
					if(decoder->channelNum==2)
					{
						for(b = 0; b < windowsl; b++) {
							samples[128*(b+s)+i+k] = tmpBuffer[0*1024+offset+4*b+k];
							samples1[128*(b+s)+i+k] = tmpBuffer[1*1024+offset+4*b+k];
						}
					}
					else
					{
						for(b = 0; b < windowsl; b++) {
							samples[128*(b+s)+i+k] = tmpBuffer[offset+4*b+k];
						}
					}
				}
			}
			s += windowsl;
		}
	} else {
		samples  = decoder->coef[0];
		samples1 = decoder->coef[1];
		sam_decode_bsac_stream(decoder,ICS_Data[0], bsac->long_sfb_top, used_bits, samples);
	}

	return 0;
}



#endif//BSAC_DEC
