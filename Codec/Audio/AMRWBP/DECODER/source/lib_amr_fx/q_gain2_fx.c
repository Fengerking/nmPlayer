/*--------------------------------------------------------------------------*
 *                         Q_GAIN2.C										*
 *--------------------------------------------------------------------------*
 * Quantization of pitch and codebook gains.                                *
 * MA prediction is performed on the innovation energy (in dB with mean     *
 * removed).                                                                *
 * An initial predicted gain, g_0, is first determined and the correction   *
 * factor     alpha = gain / g_0    is quantized.                           *
 * The pitch gain and the correction factor are vector quantized and the    *
 * mean-squared weighted error criterion is used in the quantizer search.   *
 *--------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"
#include "log2.h"
#include "acelp_fx.h"





