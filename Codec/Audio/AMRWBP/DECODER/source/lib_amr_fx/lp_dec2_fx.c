/*-------------------------------------------------------------------*
 *                         LP_DEC2.C								 *
 *-------------------------------------------------------------------*
 * Decimate a vector by 2 with 2nd order fir filter.                 *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "cnst_wb_fx.h"

#define L_FIR  5
#define L_MEM  (L_FIR-2)

/* static float h_fir[L_FIR] = {0.13, 0.23, 0.28, 0.23, 0.13}; */
/* fixed-point: sum of coef = 32767 to avoid overflow on DC */
static Word16 h_fir[L_FIR] = {4260, 7536, 9175, 7536, 4260};

