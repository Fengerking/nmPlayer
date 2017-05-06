/*-------------------------------------------------------------------*
 *                         DECIM54.C								 *
 *-------------------------------------------------------------------*
 * Decim_12k8   : decimation of 16kHz signal to 12.8kHz.             *
 * Oversamp_16k : oversampling from 12.8kHz to 16kHz.                *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "acelp_fx.h"
#include "count.h"
#include "cnst_wb_fx.h"

#define FAC4   4
#define FAC5   5
#define INV_FAC5   6554                    /* 1/5 in Q15 */
#define DOWN_FAC  26215                    /* 4/5 in Q15 */
#define UP_FAC    20480                    /* 5/4 in Q14 */

#define NB_COEF_DOWN  15
#define NB_COEF_UP    12

/* Local functions */
//static void Down_samp(
//     Word16 * sig,                         /* input:  signal to downsampling  */
//     Word16 * sig_d,                       /* output: downsampled signal      */
//     Word16 L_frame_d                      /* input:  length of output        */
//);
static void Up_samp(
     Word16 * sig_d,                       /* input:  signal to oversampling  */
     Word16 * sig_u,                       /* output: oversampled signal      */
     Word16 L_frame                        /* input:  length of output        */
);
static Word16 Interpol(                    /* return result of interpolation */
     Word16 * x,                           /* input vector                   */
     Word16 * fir,                         /* filter coefficient             */
     Word16 frac,                          /* fraction (0..resol)            */
     Word16 resol,                         /* resolution                     */
     Word16 nb_coef                        /* number of coefficients         */
);


/* 1/5 resolution interpolation filter  (in Q14)  */
/* -1.5dB @ 6kHz, -6dB @ 6.4kHz, -10dB @ 6.6kHz, -20dB @ 6.9kHz, -25dB @ 7kHz, -55dB @ 8kHz */

static Word16 fir_up[120] =
{
    -1, -4, -7, -6, 0,
    12, 24, 30, 23, 0,
    -33, -62, -73, -52, 0,
    68, 124, 139, 96, 0,
    -119, -213, -235, -160, 0,
    191, 338, 368, 247, 0,
    -291, -510, -552, -369, 0,
    430, 752, 812, 542, 0,
    -634, -1111, -1204, -809, 0,
    963, 1708, 1881, 1288, 0,
    -1616, -2974, -3432, -2496, 0,
    3792, 8219, 12368, 15317, 16384,
    15317, 12368, 8219, 3792, 0,
    -2496, -3432, -2974, -1616, 0,
    1288, 1881, 1708, 963, 0,
    -809, -1204, -1111, -634, 0,
    542, 812, 752, 430, 0,
    -369, -552, -510, -291, 0,
    247, 368, 338, 191, 0,
    -160, -235, -213, -119, 0,
    96, 139, 124, 68, 0,
    -52, -73, -62, -33, 0,
    23, 30, 24, 12, 0,
    -6, -7, -4, -1, 0
};

//static Word16 fir_down[120] =
//{            /* table x4/5 */
//    -1, -3, -6, -5,
//    0, 9, 19, 24,
//    18, 0, -26, -50,
//    -58, -41, 0, 54,
//    99, 111, 77, 0,
//    -95, -170, -188, -128,
//    0, 153, 270, 294,
//    198, 0, -233, -408,
//    -441, -295, 0, 344,
//    601, 649, 434, 0,
//    -507, -888, -964, -647,
//    0, 770, 1366, 1505,
//    1030, 0, -1293, -2379,
//    -2746, -1997, 0, 3034,
//    6575, 9894, 12254, 13107,
//    12254, 9894, 6575, 3034,
//    0, -1997, -2746, -2379,
//    -1293, 0, 1030, 1505,
//    1366, 770, 0, -647,
//    -964, -888, -507, 0,
//    434, 649, 601, 344,
//    0, -295, -441, -408,
//    -233, 0, 198, 294,
//    270, 153, 0, -128,
//    -188, -170, -95, 0,
//    77, 111, 99, 54,
//    0, -41, -58, -50,
//    -26, 0, 18, 24,
//    19, 9, 0, -5,
//    -6, -3, -1, 0
//};

#if FILE_DECIM54_FX_OPT
//static Word16 signal[L_FRAME16k + (2 * NB_COEF_DOWN)];
#endif

void voAMRWBPDecInit_Oversamp_16k(
     Word16 mem[]                          /* output: memory (2*NB_COEF_UP) set to zeros  */
)
{
    Set_zero(mem, 2 * NB_COEF_UP);
    return;
}

void voAMRWBPDecOversamp_16k(
     Word16 sig12k8[],                     /* input:  signal to oversampling  */
     Word16 lg,                            /* input:  length of input         */
     Word16 sig16k[],                      /* output: oversampled signal      */
     Word16 mem[]                          /* in/out: memory (2*NB_COEF_UP)   */
)
{
#if FILE_DECIM54_FX_OPT
    Word16 lg_up;
    Word16 signal[L_SUBFR + (2 * NB_COEF_UP)];

    voAMRWBPDecCopy(mem, signal, 2 * NB_COEF_UP);
    voAMRWBPDecCopy(sig12k8, signal + (2 * NB_COEF_UP), lg);


//    lg_up = shl(mult(lg, UP_FAC), 1);
    //UP_FAC:20480
    lg_up = (((Word32)lg * UP_FAC) >> 15) << 1;
    
    Up_samp(signal + NB_COEF_UP, sig16k, lg_up);

    voAMRWBPDecCopy(signal + lg, mem, 2 * NB_COEF_UP);
    
    return;
    
#else

    Word16 lg_up;
    Word16 signal[L_SUBFR + (2 * NB_COEF_UP)];

    voAMRWBPDecCopy(mem, signal, 2 * NB_COEF_UP);

    voAMRWBPDecCopy(sig12k8, signal + (2 * NB_COEF_UP), lg);

    lg_up = shl(mult(lg, UP_FAC), 1);

    Up_samp(signal + NB_COEF_UP, sig16k, lg_up);

    voAMRWBPDecCopy(signal + lg, mem, 2 * NB_COEF_UP);

    return;

#endif
}


//static void Down_samp(
//     Word16 * sig,                         /* input:  signal to downsampling  */
//     Word16 * sig_d,                       /* output: downsampled signal      */
//     Word16 L_frame_d                      /* input:  length of output        */
//)
//{
//#if FILE_DECIM54_FX_OPT
//    Word16 i, j, frac, pos;
//
//    pos = 0;                                /* position is in Q2 -> 1/4 resolution  */
//    for (j = 0; j < L_frame_d; j++)
//    {
//        i = pos >> 2;                       /* integer part     */
//        frac = (Word16) (pos & 3);          /* fractional part  */
//
//        sig_d[j] = Interpol(&sig[i], fir_down, frac, FAC4, NB_COEF_DOWN);       
//
//        pos += FAC5;                        /* pos + 5/4 */
//    }
//
//    return;
//
//#else
//
//    Word16 i, j, frac, pos;
//
//    pos = 0;                                 /* position is in Q2 -> 1/4 resolution  */
//    for (j = 0; j < L_frame_d; j++)
//    {
//        i = shr(pos, 2);                   /* integer part     */
//        frac = (Word16) (pos & 3);         logic16();  /* fractional part */
//
//        sig_d[j] = Interpol(&sig[i], fir_down, frac, FAC4, NB_COEF_DOWN);       
//
//        pos = add(pos, FAC5);              /* pos + 5/4 */
//    }
//
//    return;
//#endif
//}


static void Up_samp(
     Word16 * sig_d,                       /* input:  signal to oversampling  */
     Word16 * sig_u,                       /* output: oversampled signal      */
     Word16 L_frame                        /* input:  length of output        */
)
{
#if FILE_DECIM54_FX_OPT

    Word16 i, j, pos, frac;

    pos = 0;                                 /* position with 1/5 resolution */

    for (j = 0; j < L_frame; j++)
    {
        i = (pos * INV_FAC5) >> 15;            /* integer part = pos * 1/5 */
        frac = pos - (i << 2) - i;          /* frac = pos - (pos/5)*5   */
        sig_u[j] = Interpol(&sig_d[i], fir_up, frac, FAC5, NB_COEF_UP); 
        pos += FAC4;                        /* position + 4/5 */
    }
    return;

#else

    Word16 i, j, pos, frac;

    pos = 0;                                 /* position with 1/5 resolution */

    for (j = 0; j < L_frame; j++)
    {
        i = mult(pos, INV_FAC5);           /* integer part = pos * 1/5 */
        frac = sub(pos, add(shl(i, 2), i));/* frac = pos - (pos/5)*5   */

        sig_u[j] = Interpol(&sig_d[i], fir_up, frac, FAC5, NB_COEF_UP); 

        pos = add(pos, FAC4);              /* position + 4/5 */
    }

    return;

#endif
}

/* Fractional interpolation of signal at position (frac/resol) */

static Word16 Interpol(                    /* return result of interpolation */
     Word16 * x,                           /* input vector                   */
     Word16 * fir,                         /* filter coefficient             */
     Word16 frac,                          /* fraction (0..resol)            */
     Word16 resol,                         /* resolution                     */
     Word16 nb_coef                        /* number of coefficients         */
)
{
#if (!FUNC_INTERPOL_WB_OPT)
    Word16 i, k;
    Word32 L_sum;

    x = x - nb_coef + 1;                   

    L_sum = 0L;                            move32();
    for (i = 0, k = sub(sub(resol, 1), frac); i < 2 * nb_coef; i++, k = (Word16) (k + resol))
    {
        L_sum = L_mac(L_sum, x[i], fir[k]);
    }
    L_sum = L_shl(L_sum, 1);               /* saturation can occur here */

    return (round16(L_sum));

#else

    Word16 i, k;
    Word32 L_sum;

    x -= nb_coef - 1;

    L_sum = 0L;
    k = resol - 1 - frac;
    for (i = 0; i < 2 * nb_coef; i += 2)
    {
        L_sum += x[i] * fir[k];
        k += resol;
        L_sum += x[i + 1] * fir[k];
        k += resol;
    }
    L_sum = L_shl(L_sum, 2);               /* saturation can occur here */

    return (round16(L_sum));              //can't be modified

#endif
}
