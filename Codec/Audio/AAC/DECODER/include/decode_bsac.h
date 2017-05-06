#ifndef VOI_DECODE_BSAC_H
#define VOI_DECODE_BSAC_H

#include "global.h"

#define MAX_SCFAC_BANDS 120
#define MAX_LAYER 100

#define SUPPORT_SBA_MODE	0
#if SUPPORT_SBA_MODE
#define USE_EXTRA_BITBUFFER 1
#else
#define USE_EXTRA_BITBUFFER 0
#endif

typedef struct {
	char		enc_sign_vec_buf[2][256];
	char		enc_vec_buf[2][256];
	char		cur_snf_buf[2][1024];
	char		sign_is_coded_buf[2][1024];
	char		coded_samp_bit_buf[2][1024];
	int			ModelIndex[2][8][32];
	int			band_snf[2][8][32];
	short		AModelNoiseNrg[512];
	
	int			swb_offset_long[52];
	int			swb_offset_short[16];
	int			swb_offset[8][52];

	int			sam_scale_bits_dec[MAX_LAYER];
	int			layer_max_freq[MAX_LAYER];
	int			layer_max_cband[MAX_LAYER];
	int			layer_max_qband[MAX_LAYER];
	int			layer_buf_offset[MAX_LAYER];
	int			layer_si_maxlen[MAX_LAYER];
	int			layer_extra_len[MAX_LAYER];
	int			layer_cw_offset[MAX_LAYER];
	char		layer_reg[MAX_LAYER];
	char		layer_bit_flush[MAX_LAYER];
	char		stereo_si_coded[MAX_SCFAC_BANDS];

	int			start_freq[2][8];
	int			start_qband[2][8];
	int			start_cband[2][8];
	int			end_freq[2][8];
	int			end_cband[2][8];
	int			end_qband[2][8];
	int			s_freq[2][8];
	int			e_freq[2][8];

	int			max_scalefactor[MAX_SYNTAX_ELEMENTS];
    int			cband_si_type[MAX_SYNTAX_ELEMENTS];
    int			scf_model0[MAX_SYNTAX_ELEMENTS];//base_scf_mode[MAX_SYNTAX_ELEMENTS];
    int			scf_model1[MAX_SYNTAX_ELEMENTS];//enh_scf_mode[MAX_SYNTAX_ELEMENTS];
    int			max_sfb_si_len[MAX_SYNTAX_ELEMENTS];

	int			header_length;
    int			sba_mode;
    int			top_layer;
    int			base_snf_thr;
    int			base_band;
	int			preBaseBand;
	int			preMaxSfb;
	int			SameAsPrev;
	int			preWinSequence;
	int			preSlayerSize;
	int			preTopLayer;
	int			long_sfb_top;
	int			short_sfb_top;
}BSAC_AAC;

int sam_decode_init(AACDecoder* decoder,int sampling_rate_decoded, int block_size_samples);
int bsac_raw_data_block(AACDecoder *decoder,unsigned char *inptr,int sizeInBytes);

int sam_decode_symbol2(BitStream *bs,int, int *);

int sam_decode_symbol(BitStream *bs, const short *, int *);

int select_freq0 (
				  int model_index,    /* model index for coding quantized spectral data */
				  int bpl,            /* bit position */
				  int enc_msb_vec,    /* vector of the encoded bits of 4-samples until the 'bpl+1'-th bit position */ 									 
				  int samp_pos,       /* current sample position among 4-samples */
				  int enc_csb_vec,    /* vector of the previously encoded bits of 4-samples at the current bit plane  */														 
				  int available_len);
int select_freq1(
				 int model_index,     /* model index for coding quantized spectral data */
				 int  bpl,            /* bit position */
				 int coded_samp_bit,  /* vector of previously encoded MSB of sample */
				 int available_len);
#endif