/*------------------------------------------------------------------------*
 *                         C2T64FX.C                                      *
 *------------------------------------------------------------------------*
 * Performs algebraic codebook search for 6.60 kbit/s mode                *
 *------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*
 * Function  ACELP_2t64_fx()                                             *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~                                             *
 * 12 bits algebraic codebook.                                           *
 * 2 tracks x 32 positions per track = 64 samples.                       *
 *                                                                       *
 * 12 bits --> 2 pulses in a frame of 64 samples.                        *
 *                                                                       *
 * All pulses can have two (2) possible amplitudes: +1 or -1.            *
 * Each pulse can have 32 possible positions.                            *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "acelp_fx.h"
#include "count.h"
#include "cnst_wb_fx.h"

#define NB_TRACK  2
#define STEP      2
#define NB_POS    32
#define MSIZE     1024
