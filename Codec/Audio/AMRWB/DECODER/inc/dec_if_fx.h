/*
 *===================================================================
 *  3GPP AMR Wideband Floating-point Speech Codec
 *===================================================================
 */
#ifndef DEC_IF_FX_H
#define DEC_IF_FX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "stream.h"

#define NB_SERIAL_MAX   61    /* max serial size      */
#define L_FRAME16k      320   /* Frame size at 16kHz  */

#define _good_frame  0
#define _bad_frame   1
#define _lost_frame  2
#define _no_frame    3

extern void *g_hAMRWBDecInst;
typedef struct
{
	Word16           reset_flag_old;     /* previous was homing frame  */
	Word16           prev_ft;            /* previous frame type        */
	Word16           prev_mode;          /* previous mode              */
	void             *decoder_state;       /* Points decoder state       */
	VO_MEM_OPERATOR  *pvoMemop;
	VO_MEM_OPERATOR  voMemoprator;
	FrameStream      *stream;       /* Input Buffer Ptr*/
	VO_PTR           hCheck;
} WB_dec_if_state_fx;

void voAMRWBDec_D_IF_reset_fx(WB_dec_if_state_fx *st);

void Init_decoder(void **spd_state, VO_MEM_OPERATOR *pMemOP);
void Close_decoder(void *spd_state, VO_MEM_OPERATOR  *pMemOP);

void voAMRWBDec_D_IF_decode_fx(void *st, UWord8 *bits, Word16 *synth, Word16 bfi);

#ifdef __cplusplus
}
#endif

#endif
