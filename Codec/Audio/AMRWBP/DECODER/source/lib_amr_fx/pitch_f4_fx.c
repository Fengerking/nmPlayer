/*-----------------------------------------------------------------------*
 *                         PITCH_F4.C									 *
 *-----------------------------------------------------------------------*
 * Find the closed loop pitch period with 1/4 subsample resolution.		 *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "acelp_fx.h"
#include "cnst_wb_fx.h"
#include "count.h"

#define UP_SAMP      4
#define L_INTERPOL1  4

/* Local functions */

static Word16 Interpol_4(                  /* (o)  : interpolated value  */
     Word16 * x,                           /* (i)  : input vector        */
     Word16 frac                           /* (i)  : fraction (-4..+3)   */
);

/*--------------------------------------------------------------------------*
 * Procedure Interpol_4()                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~                                                   *
 * For interpolating the normalized correlation with 1/4 resolution.        *
 *--------------------------------------------------------------------------*/

/* 1/4 resolution interpolation filter (-3 dB at 0.791*fs/2) in Q14 */

static Word16 inter4_1[UP_SAMP * 2 * L_INTERPOL1] =
{
    -12, -26, 32, 206,
    420, 455, 73, -766,
    -1732, -2142, -1242, 1376,
    5429, 9910, 13418, 14746,
    13418, 9910, 5429, 1376,
    -1242, -2142, -1732, -766,
    73, 455, 420, 206,
    32, -26, -12, 0
};

/*** Coefficients in floating point
static float inter4_1[UP_SAMP*L_INTERPOL1+1] = {
   0.900000,
   0.818959,  0.604850,  0.331379,  0.083958,
  -0.075795, -0.130717, -0.105685, -0.046774,
   0.004467,  0.027789,  0.025642,  0.012571,
   0.001927, -0.001571, -0.000753,  0.000000};
***/

static Word16 Interpol_4(                  /* (o)  : interpolated value  */
     Word16 * x,                           /* (i)  : input vector        */
     Word16 frac                           /* (i)  : fraction (-4..+3)   */
)
{
    Word16 i, k, sum;
    Word32 L_sum;

    test();
    if (frac < 0)
    {
        frac = add(frac, UP_SAMP);
        x--;
        move16();
    }
    x = x - L_INTERPOL1 + 1;
    move16();

    L_sum = 0L;                            move32();
    for (i = 0, k = sub(sub(UP_SAMP, 1), frac); i < 2 * L_INTERPOL1; i++, k += UP_SAMP)
    {
        L_sum = L_mac(L_sum, x[i], inter4_1[k]);
    }

    sum = round16(L_shl(L_sum, 1));

    return (sum);
}
