        /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#ifndef _STRUCT_H
#define _STRUCT_H

#include "voAAC.h"
#include "global.h"
#include "bitstream.h"

//#define MAX_CHANNELS		8		
#define MAX_SAMPLES		1024

#define BUFFER_GUARD	8
#define BUFFER_DATA		2048*4 + BUFFER_GUARD

#define SQRT1_2 0x5a82799a	/* sqrt(1/2) in Q31 */

#define NUM_LONG_WIN           1
#define NUM_SHORT_WIN          8

#define FILL_BUF_SIZE           269             /* max count = 15 + 255 - 1*/


#define RND_VAL		(1 << (SCLAE_IMDCT-1))
#define NUM_FFT_SIZES	2

#define MAX_HUFF_BITS                   20
#define HUFFTAB_SPEC_OFFSET             1



#define SF_DQ_OFFSET            6
#define FBITS_OUT_DQ            11
#define FBITS_OUT_DQ_OFF        5   /* number of fraction bits out of dequant, including 2^15 bias */

#define FBITS_IN_IMDCT          FBITS_OUT_DQ_OFF        /* number of fraction bits into IMDCT */
#define GBITS_IN_DCT4           4                                       /* min guard bits in for DCT4 */

#define FBITS_LOST_DCT4         1               /* number of fraction bits lost (>> out) in DCT-IV */
#define FBITS_LOST_WND          1               /* number of fraction bits lost (>> out) in synthesis window (neg = gain frac bits) */
#define FBITS_LOST_IMDCT        2
#define SCLAE_IMDCT         3

#define NUM_IMDCT_SIZES         2


typedef union _U64 {
	VO_S64 w64;
	struct {
		VO_U32  lo32; 
		VO_S32  hi32;
	} r;
} U64;

typedef struct _HuffInfo {
    int  maxBits;                                                /* number of bits in longest codeword */
    VO_U8 count[MAX_HUFF_BITS];		/* count[i] = number of codes with length i+1 bits */
    int  offset;                                                       /* offset into symbol table */
} HuffInfo;

typedef enum  { 
	ONLY_LONG_SEQUENCE, 
	LONG_START_SEQUENCE, 
	EIGHT_SHORT_SEQUENCE,
	LONG_STOP_SEQUENCE,
	NUM_WIN_SEQ        
} WINDOW_SEQUENCE; 

typedef struct{
    VO_U8  number_pulse;
    VO_U8  pulse_start_sfb;
    VO_U8  pulse_offset[4];
    VO_U8  pulse_amp[4];
} Pulse_Data;

typedef struct{
    VO_U8  n_filt;     
    VO_U8  coef_res;
    VO_U8  length[4];
    VO_U8  order[4];
    VO_U8  direction[4];
	VO_U8  coef_compress[4];
    VO_U8  coef[4][32];              
} TNS_Data;

typedef struct
{
	VO_U8  last_band;
	VO_U8  data_present;
	VO_U16 lag;
	VO_U8  lag_update;
	VO_U8  coef;
	VO_U8  long_used[MAX_SFB];
	VO_U8  short_used[8];
	VO_U8  short_lag_present[8];
	VO_U8  short_lag[8];
}LTP_Data;

typedef struct{
    VO_U8  ics_reserved_bit;
    VO_U8  window_sequence;
    VO_U8  window_shape;
    VO_U8  max_sfb;
    VO_U8  scale_factor_grouping;
    VO_U8  predictor_data_present;

    VO_U8  predictor_reset;
    VO_U8  predictor_reset_group_number;
    VO_U8  prediction_used[MAX_SFB];
	VO_U8  num_window_groups;
    VO_U8  window_group_length[MAX_WINDOW_GROUPS];
} ICS_Data;

typedef struct {
    VO_S32 r[2];
    VO_S64 COR[2];
    VO_S64 VAR[2];
} PRED_State;

typedef struct
{
    VO_U8  element_instance_tag;
    VO_U8  object_type;
    VO_U8  sampling_frequency_index;
    VO_U8  num_front_channel_elements;
    VO_U8  num_side_channel_elements;
    VO_U8  num_back_channel_elements;
    VO_U8  num_lfe_channel_elements;
    VO_U8  num_assoc_data_elements;
    VO_U8  num_valid_cc_elements;
    VO_U8  mono_mixdown_present;
    VO_U8  mono_mixdown_element_number;
    VO_U8  stereo_mixdown_present;
    VO_U8  stereo_mixdown_element_number;
    VO_U8  matrix_mixdown_idx_present;
    VO_U8  matrix_mixdown_idx;
	VO_U8  pseudo_surround_enable;
    VO_U8  front_element_is_cpe[16];
    VO_U8  front_element_tag_select[16];
    VO_U8  side_element_is_cpe[16];
    VO_U8  side_element_tag_select[16];
    VO_U8  back_element_is_cpe[16];
    VO_U8  back_element_tag_select[16];
    VO_U8  lfe_element_tag_select[16];
    VO_U8  assoc_data_element_tag_select[16];
    VO_U8  cc_element_is_ind_sw[16];
    VO_U8  valid_cc_element_tag_select[16];
	
    VO_U8  comment_field_bytes;

	//output value
	VO_U8  num_front_channels;
    VO_U8  num_side_channels;
    VO_U8  num_back_channels;
    VO_U8  num_lfe_channels;
	VO_U8  channels;
	
} program_config;

typedef struct
{
    VO_U16 syncword;
    VO_U8  ID;
    VO_U8  layer;
    VO_U8  protection_absent;
    VO_U8  profile;
    VO_U8  sampling_frequency_index;
    VO_U8  private_bit;
    VO_U8  channel_configuration;
    VO_U8  original;
    VO_U8  home;
    VO_U8  copyright_identification_bit;
    VO_U8  copyright_identification_start;
    VO_U16 frame_length;
    VO_U16 adts_buffer_fullness;
    VO_U8  number_of_raw_data_blocks_in_frame;
    VO_U16 crc_check;	
} adts_header;

#define IS_ADIFHEADER(h)				((h)[0] == 'A' && (h)[1] == 'D' && (h)[2] == 'I' && (h)[3] == 'F')

typedef struct
{
	VO_U32 adif_id;
    VO_U8  copyright_id_present;
    VO_S8  copyright_id[9];//72 bits
    VO_U8  original_copy;
    VO_U8  home;
    VO_U8  bitstream_type;
    VO_U32 bitrate;
    VO_U8  num_program_config_elements;
    VO_U32 adif_buffer_fullness;
    program_config pce[16];
} adif_header;

#define MAXLATMLAYER 4

typedef struct {

	int			  streamId[MAXLATMLAYER];	

	VO_U8		  progSIndex[MAXLATMLAYER];
	VO_U8   	  laySIndex[MAXLATMLAYER];

	VO_U8		  frameLengthTypes[MAXLATMLAYER];
	VO_U8		  latmBufferFullness[MAXLATMLAYER];
	VO_U16		  frameLength[MAXLATMLAYER];
	VO_U16		  muxSlotLengthBytes[MAXLATMLAYER];
	VO_U8		  muxSlotLengthCoded[MAXLATMLAYER];
	VO_U8		  progCIndx[MAXLATMLAYER];
	VO_U8		  layCIndx[MAXLATMLAYER];
	VO_U8		  AuEndFlag[MAXLATMLAYER];
	VO_U8		  numLayer;
	VO_U8		  objTypes;
	VO_U8		  audio_mux_version;
	VO_U8		  audio_mux_version_A;
	VO_U8		  all_same_framing;
	VO_U8		  config_crc;
	int			  taraFullness;
	int			  other_data_bits;
	int			  numProgram;
	int			  numSubFrames;
	int			  numChunk;
	int			  samplerate;
	int			  channel;
	int			  muxlength;
} latm_header;

typedef struct{
	unsigned char *start;
	unsigned char *input;
	unsigned char *this_frame;
	unsigned int  uselength;
	unsigned int  inlen;
	unsigned int  length;
	unsigned int  storelength;
	unsigned int  maxLength;
} FrameStream;

typedef struct {

	/*channel configure */
	program_config pce;
	int			pce_set;
	int			channel_position[MAX_CHANNELS];
	int			channel_offsize[MAX_CHANNELS];

    ICS_Data    ICS_Data[MAX_SYNTAX_ELEMENTS];
	int			global_gain;
    int         common_window;
    VO_S16      scaleFactors[MAX_SYNTAX_ELEMENTS][MAX_SFB];
    VO_U8		sfb_cb[MAX_SYNTAX_ELEMENTS][MAX_SFB];
	
	VO_U32		pns_seed;
	int			pns_start_sfb;
	int         pns_data_present[MAX_SYNTAX_ELEMENTS];
	VO_U8		pns_sfb_flag[MAX_SYNTAX_ELEMENTS][MAX_SFB];
	VO_U8		pns_sfb_mode[MAX_SFB];

    int         ms_mask_present;
	VO_U8		ms_used[8][MAX_SFB/2];    
    int         intensity_used;
	VO_U8		is_used[8][MAX_SFB/2];

	int         gbCurrent[MAX_SYNTAX_ELEMENTS];

    int			pulse_data_present[MAX_SYNTAX_ELEMENTS];
	Pulse_Data  pulse_data[MAX_SYNTAX_ELEMENTS];
	
    int			tns_data_present[MAX_SYNTAX_ELEMENTS];
	TNS_Data    tns_data[MAX_SYNTAX_ELEMENTS][MAX_WINDOW_GROUPS];
	int			tns_lpc[48];

#ifdef LTP_DEC	
	LTP_Data	Ltp_Data[MAX_SYNTAX_ELEMENTS];//make it compatible to the LC,keep the data structure for syntax parsing	
	VO_U16		ltp_lag[MAX_CHANNELS];
	int			*ltp_coef[MAX_CHANNELS]; //[MAX_SAMPLES*4];
	int			*t_est_buf;
	int			*f_est_buf;
#endif//LTP_DEC

	/* used to save the prediction state */
#ifdef MAIN_DEC
	PRED_State  *pred_stat[MAX_CHANNELS];
#endif//LTP_DEC

	int         prevWinShape[MAX_CHANNELS];	
	int			*coef[MAX_SYNTAX_ELEMENTS];
	VO_U32		*tmpBuffer;//16KB this is a scratch buffer
	int			*overlap[MAX_CHANNELS];

	/* raw decoded data, before rounding to 16-bit PCM (for postprocessing such as SBR) */
	void		*rawSampleBuf[MAX_CHANNELS];
	int			rawSampleBytes;
	int			rawSampleFBits;

	/* MPEG-4 BSAC decoding */
	void		*bsac; 
	/* MPEG-4 SBR decoding */
	void		*sbr;	

	/* fill data (can be used for processing SBR or other extensions) */
	int			fillExtType;
    int         fillCount;
	VO_U8		fillBuf[FILL_BUF_SIZE];

	BitStream	bs;	
	
	/* frame Header */
	adts_header adts;
	adts_header adts_new;
	adif_header adif;
	latm_header *latm;

	/* block information */
	int			old_id_syn_ele;
	int			id_syn_ele;
	int			first_id_syn_ele;
	
	/*channels info*/
	int			decodedChans;
	int			ChansMode;
	int			elementChans;
	int			decodedSBRChans;
	int			elementSBRChans;
	
	/* frame info */
	int			frame_length;
	int			stereo_mode;
	int			channelNum;
	int         sampRateIdx;
	int			sampleRate;
	int			sampleBits;
	int			profile;
	int			frametype;

	/* decoder parameter */
	int			framenumber;
	int			profilenumber;
	int			Checknumber;
	int			errNumber;
	int			sbrEnabled;
	int			chSpec;
	int			seletedChs;
	int			seletedChDecoded;
	int			seletedSBRChDecoded;
	int			lp_sbr;//low power sbr
	int			forceUpSample;
	int			errorConcealment;
	int			disableSBR;
	int			disablePS;
	int			decoderNum;

	int         nFlushFlag;    //fix #8710

	/* frame buffer  */
	FrameStream	Fstream;

	/* memory operator */
	VO_MEM_OPERATOR *voMemop;
	VO_MEM_OPERATOR voMemoprator;

	/* outFormat */
	VO_AUDIO_FORMAT outAudioFormat;

	/* checklicense */
	VO_PTR hCheck;

} AACDecoder;

extern const int cos4sin4tab[128 + 1024];
extern const int sinWindow[128 + 1024];
extern const int kbdWindow[128 + 1024];
extern const int twidTabEven[4*6 + 16*6 + 64*6];
extern const int twidTabOdd[8*6 + 32*6 + 128*6];
extern const int cos1sin1tab[514];
extern const int nfftTab[NUM_FFT_SIZES];
extern const int nfftlog2Tab[NUM_FFT_SIZES];
extern const int nmdctTab[NUM_IMDCT_SIZES];
extern const int postSkip[NUM_IMDCT_SIZES];

/* hufftabs.c */
//extern const HuffInfo huffTabSpecInfo[11];
extern const signed short huffTabSpec[1241];
extern const HuffInfo huffTabScaleFactInfo; 
extern const signed short huffTabScaleFact[121];

/* tabs.c */
extern const int cos4sin4tabOffset[NUM_IMDCT_SIZES];
extern const int sinWindowOffset[NUM_IMDCT_SIZES];
extern const int kbdWindowOffset[NUM_IMDCT_SIZES];
extern const VO_U8 BitRevTab[17 + 129];
extern const int bitrevtabOffset[NUM_IMDCT_SIZES];

/* aactabs.c - global ROM tables */
extern const int sampRateTab[NUM_SAMPLE_RATES];
extern const int predSFBMax[NUM_SAMPLE_RATES];
extern const int elementNumChans[ELM_ID_SIZE];
extern const int sfBandTabShortOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabShort[76];
extern const int sfBandTabLongOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabLong[325];

#endif// _STRUCT_H

