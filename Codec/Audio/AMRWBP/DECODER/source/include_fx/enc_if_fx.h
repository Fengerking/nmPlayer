/*
 *===================================================================
 *  3GPP AMR Wideband Floating-point Speech Codec
 *===================================================================
 */
#ifndef WB_E_IF_FX_H
#define WB_E_IF_FX_H

#include "typedef.h"


#define L_FRAME16k   320   /* Frame size at 16kHz  */
#define NB_SERIAL_MAX 61   /* max serial size      */

int E_IF_encode_fx(void *st, Word16 mode, Word16 *speech,
                UWord8 *serial, Word16 dtx);
void *E_IF_init_fx(void);
void E_IF_exit_fx(void *state);

void E_IF_encode_first_fx(void *st, Word16 *speech);

#endif
