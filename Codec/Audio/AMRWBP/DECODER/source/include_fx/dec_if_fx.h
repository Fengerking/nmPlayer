/*
 *===================================================================
 *  3GPP AMR Wideband Floating-point Speech Codec
 *===================================================================
 */
#ifndef DEC_IF_FX_H
#define DEC_IF_FX_H

#include "typedef.h"

#define NB_SERIAL_MAX   61    /* max serial size      */
#define L_FRAME16k      320   /* Frame size at 16kHz  */

#define _good_frame  0
#define _bad_frame   1
#define _lost_frame  2
#define _no_frame    3

void D_IF_decode_fx(void *st, UWord8 *bits, Word16 *synth, Word16 bfi);
void * D_IF_init_fx(void);
void D_IF_exit_fx(void *state);

#endif
