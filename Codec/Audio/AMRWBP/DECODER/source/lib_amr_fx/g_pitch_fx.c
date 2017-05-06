/*-------------------------------------------------------------------*
 *                         G_PITCH.C								 *
 *-------------------------------------------------------------------*
 * Compute the gain of pitch. Result in Q12                          *
 *  if (gain < 0)  gain =0                                           *
 *  if (gain > 1.2) gain =1.2                                        *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "count.h"
