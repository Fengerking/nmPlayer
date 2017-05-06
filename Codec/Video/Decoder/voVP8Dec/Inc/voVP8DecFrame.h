#ifndef __VP8_DEC_FRAME_H
#define __VP8_DEC_FRAME_H

#include "voVP8Common.h"
#include "voVP8DecMBlock.h"
#include "voVP8DecIdctDq.h"
#include "voVP8Thread.h"

#define NEWTHREAD 1

typedef struct
{
    int ithread;
    void *ptr1;
    void *ptr2;
} DECODETHREAD_DATA;

typedef struct
{
    MACROBLOCKD  mbd;
    int mb_row;
    int current_mb_col;
#ifdef MULTITHREAD_STABILITY
	int errorflag;
#endif
    short *coef_ptr;
} MB_ROW_DEC;


typedef struct
{
    signed short min_val;
    //signed short Length;
    unsigned char Probs[12];
} TOKENEXTRABITS;




typedef struct VP8Decompressor
{
	MACROBLOCKD  mb;

#ifdef NEWTHREAD
	MACROBLOCKD  mb2;
	MACROBLOCKD* mbrow;
	MACROBLOCKD* mbrow2;
#endif

	VP8_COMMON common;
    vp8_reader bc, bc2;
    VP8D_CONFIG oxcf;
    const unsigned char *Source;
    unsigned int   source_sz; 
#if CONFIG_MULTITHREAD
    pthread_t	h_decoding_thread;	
	sem_t        *h_event_startframe;
	sem_t        *h_event_main;
	DECODETHREAD_DATA   *threadParam;
	int  stop_decoder_thread;
	int thread_mb_row[2];
	int thread_mb_col[2];
	int thread_mbrow_parser_ready[2];
	int isloopfilter;
	int baseline_filter_level[MAX_MB_SEGMENTS];
#endif
    vp8_reader *mbc;
    unsigned long last_time_stamp;
    int   ready_for_new_data;

	long nCodecIdx;
	VO_CODEC_INIT_USERDATA * pUserData;
#ifdef STABILITY
	unsigned int KfFlag;
#endif

	unsigned char prob_intra;
	unsigned char prob_last;
	unsigned char prob_gf;
	unsigned char prob_skip_false;

	int disable_deblock;
    int max_threads;   

} VP8D_COMP;


#define CHECK_MEM_ERROR(lval,expr) do {\
        lval = (expr); \
        if(!lval) \
			return VPX_CODEC_MEM_ERROR;\
    } while(0)
vpx_codec_err_t vp8_decode_frame(VP8D_COMP *pbi);
void mb_init_dequantizer(VP8D_COMP *pbi, MACROBLOCKD *xd,MODE_INFO *pModeInfo);
void vp8_extend_mb_row(YV12_BUFFER_CONFIG *ybf, unsigned char *YPtr, unsigned char *UPtr, unsigned char *VPtr);

extern void vp8_init_mbmode_probs(VP8_COMMON *x);
extern void vp8_default_coef_probs(VP8_COMMON *pc);
extern void vp8_kf_default_bmode_probs(unsigned char p [VP8_BINTRAMODES] [VP8_BINTRAMODES] [VP8_BINTRAMODES-1]);
extern int vp8_decode_intra_macroblock(VP8D_COMP *pbi, MACROBLOCKD *xd,MODE_INFO *pModeInfo);
extern int vp8_decode_inter_macroblock(VP8D_COMP *pbi, MACROBLOCKD *xd,MODE_INFO *pModeInfo);

extern int vp8_decode_intra_macroblock_parser(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD* xdcol,BOOL_DECODER *bc,MODE_INFO *pModeInfo);
extern int vp8_decode_inter_macroblock_parser(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD* xdcol,BOOL_DECODER *bc,MODE_INFO *pModeInfo);
extern int vp8_decode_intra_macroblock_idctmc(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD *xdcol,MODE_INFO *pModeInfo);
extern int vp8_decode_inter_macroblock_idctmc(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD *xdcol,MODE_INFO *pModeInfo);

extern vpx_codec_err_t setup_token_decoder(VP8D_COMP *pbi, const unsigned char *cx_data);
extern void stop_token_decoder(VP8D_COMP *pbi);
extern void vp8_setup_block_dptrs(MACROBLOCKD *x);
extern void vp8_kfread_modes(VP8D_COMP *pbi);
extern void vp8_decode_mode_mvs_new(VP8D_COMP *pbi);
#endif