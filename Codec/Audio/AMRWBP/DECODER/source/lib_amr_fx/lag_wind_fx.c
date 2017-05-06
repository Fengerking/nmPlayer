/*---------------------------------------------------------*
 *                         LAG_WIND.C					   *
 *---------------------------------------------------------*
 * Lag_window on autocorrelations.                         *
 *                                                         *
 * r[i] *= lag_wind[i]                                     *
 *                                                         *
 *  r[i] and lag_wind[i] are in special double precision.  *
 *  See "oper_32b.c" for the format                        *
 *---------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "lag_wind.tab"

