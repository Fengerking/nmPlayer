/*-------------------------------------------------------------------*
 *                         cor_h_x.C							     *
 *-------------------------------------------------------------------*
 * Compute correlation between target "x[]" and "h[]".               *
 * Designed for codebook search (24 pulses, 4 tracks, 4 pulses per   *
 *    track, 16 positions in each track) to avoid saturation.        *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "count.h"

#define L_SUBFR   64
#define NB_TRACK  4
#define STEP      4

