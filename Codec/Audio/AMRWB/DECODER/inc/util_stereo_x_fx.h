#ifndef _UTIL_STEREO_X_FX_H
#define _UTIL_STEREO_X_FX_H

#include "typedef.h"

typedef struct {
	const Word16 vdim;
	const Word16 nstages;
	const Word16 intens;
	const Word16 *cbsizes;
	const Word16 **cbs;
} MSVQ_fx;

typedef struct {
	const Word16 a;							/* Prediction coefficient */		
	const Word16 a_fe;						/* Prediction coefficient to be applied in decoder in case of fe */		
	const Word16 *mean;						/* Mean vector */
	const MSVQ_fx msvq;
} PMSVQ_fx;

#endif
