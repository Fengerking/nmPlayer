/*-----------------------------------------------------------------*
 *                         GPCLIP.C								   *
 *-----------------------------------------------------------------*
 * To avoid unstable synthesis on frame erasure, the gain need to  *
 * be limited (gain pitch < 1.0) when the following case occurs:   *
 *   - a resonance on LPC filter (lp_disp < 60 Hz)                 *
 *   - a good pitch prediction (lp_gp > 0.95)                      *
 *-----------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "count.h"



