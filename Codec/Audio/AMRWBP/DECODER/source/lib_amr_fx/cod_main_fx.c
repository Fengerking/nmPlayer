/*------------------------------------------------------------------------*
 *                         COD_MAIN.C                                     *
 *------------------------------------------------------------------------*
 * Performs the main encoder routine                                      *
 *------------------------------------------------------------------------*/

/*___________________________________________________________________________
 |                                                                           |
 | Fixed-point C simulation of AMR WB ACELP coding algorithm with 20 ms      |
 | speech frames for wideband speech signals.                                |
 |___________________________________________________________________________|
*/

//#include <stdio.h>
//#include <stdlib.h>

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "cnst_wb_fx.h"
#include "acelp_fx.h"
#include "cod_main_fx.h"
#include "bits_fx.h"
#include "count.h"
#include "main_fx.h"
#include "ham_wind.tab"

#define L_WINDOW     384                   /* window size in LP analysis                 */
#define L_NEXT       64                    /* Overhead in LP analysis                    */
#define NB_SUBFR     4                     /* Number of subframe per frame               */

/* isp tables for initialization */

static Word16 isp_init[M] =
{
   32138, 30274, 27246, 23170, 18205, 12540, 6393, 0,
   -6393, -12540, -18205, -23170, -27246, -30274, -32138, 1475
};

static Word16 isf_init[M] =
{
   1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192,
   9216, 10240, 11264, 12288, 13312, 14336, 15360, 3840
};

/* High Band encoding */
static const Word16 HP_gain[16] =
{
   3624, 4673, 5597, 6479, 7425, 8378, 9324, 10264,
   11210, 12206, 13391, 14844, 16770, 19655, 24289, 32728
};

static Word16 synthesis(
     Word16 Aq[],                          /* A(z)  : quantized Az               */
     Word16 exc[],                         /* (i)   : excitation at 12kHz        */
     Word16 Q_new,                         /* (i)   : scaling performed on exc   */
     Word16 synth16k[],                    /* (o)   : 16kHz synthesis signal     */
     Coder_State * st                      /* (i/o) : State structure            */
);

/*-----------------------------------------------------*
 * Function synthesis()                                *
 *                                                     *
 * Synthesis of signal at 16kHz with HF extension.     *
 *                                                     *
 *-----------------------------------------------------*/

static Word16 synthesis(
     Word16 Aq[],                          /* A(z)  : quantized Az               */
     Word16 exc[],                         /* (i)   : excitation at 12kHz        */
     Word16 Q_new,                         /* (i)   : scaling performed on exc   */
     Word16 synth16k[],                    /* (o)   : 16kHz synthesis signal     */
     Coder_State * st                      /* (i/o) : State structure            */
)
{
    Word16 i, fac, tmp, exp;
    Word16 ener, exp_ener;
    Word32 L_tmp;

    Word16 synth_hi[M + L_SUBFR], synth_lo[M + L_SUBFR];
    Word16 synth[L_SUBFR];
    Word16 HF[L_SUBFR16k];                 /* High Frequency vector      */
    Word16 Ap[M + 1];

    Word16 HF_SP[L_SUBFR16k];              /* High Frequency vector (from original signal) */

    Word16 HP_est_gain, HP_calc_gain, HP_corr_gain;
    Word16 dist_min, dist;
    Word16 HP_gain_ind = 0;
    Word16 gain1, gain2;
    Word16 weight1, weight2;
    /*------------------------------------------------------------*
     * speech synthesis                                           *
     * ~~~~~~~~~~~~~~~~                                           *
     * - Find synthesis speech corresponding to exc2[].           *
     * - Perform fixed deemphasis and hp 50hz filtering.          *
     * - Oversampling from 12.8kHz to 16kHz.                      *
     *------------------------------------------------------------*/


    voAMRWBPDecCopy(st->mem_syn_hi, synth_hi, M);
    voAMRWBPDecCopy(st->mem_syn_lo, synth_lo, M);

    voAMRWBPDecSyn_filt_32(Aq, M, exc, Q_new, synth_hi + M, synth_lo + M, L_SUBFR);

    voAMRWBPDecCopy(synth_hi + L_SUBFR, st->mem_syn_hi, M);
    voAMRWBPDecCopy(synth_lo + L_SUBFR, st->mem_syn_lo, M);

    voAMRWBPDecDeemph_32(synth_hi + M, synth_lo + M, synth, PREEMPH_FAC, L_SUBFR, &(st->mem_deemph));

    voAMRWBPDecHP50_12k8(synth, L_SUBFR, st->mem_sig_out);

    /* Original speech signal as reference for high band gain quantisation */
    for (i = 0; i < L_SUBFR16k; i++)
    {
        HF_SP[i] = synth16k[i];            move16();
    }
    /*------------------------------------------------------*
     * HF noise synthesis                                   *
     * ~~~~~~~~~~~~~~~~~~                                   *
     * - Generate HF noise between 5.5 and 7.5 kHz.         *
     * - Set energy of noise according to synthesis tilt.   *
     *     tilt > 0.8 ==> - 14 dB (voiced)                  *
     *     tilt   0.5 ==> - 6 dB  (voiced or noise)         *
     *     tilt < 0.0 ==>   0 dB  (noise)                   *
     *------------------------------------------------------*/

    /* generate white noise vector */
    for (i = 0; i < L_SUBFR16k; i++)
    {
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
        HF[i] = shr(Random(&(st->seed2)), 3);   move16();
#elif EVC_AMRWBPLUS
        HF[i] = shr(Random_evc(&(st->seed2)), 3);   move16();
#endif
// end
    }

    /* energy of excitation */

    voAMRWBPDecScale_sig(exc, L_SUBFR, -3);
    Q_new = sub(Q_new, 3);

    ener = extract_h(Dot_product12(exc, exc, L_SUBFR, &exp_ener));
    exp_ener = sub(exp_ener, add(Q_new, Q_new));

    /* set energy of white noise to energy of excitation */

    tmp = extract_h(Dot_product12(HF, HF, L_SUBFR16k, &exp));

    test();
    if (sub(tmp, ener) > 0)
    {
        tmp = shr(tmp, 1);                 /* Be sure tmp < ener */
        exp = add(exp, 1);
    }
    L_tmp = L_deposit_h(div_s(tmp, ener)); /* result is normalized */
    exp = sub(exp, exp_ener);
    Isqrt_n(&L_tmp, &exp);
    L_tmp = L_shl(L_tmp, add(exp, 1));     /* L_tmp x 2, L_tmp in Q31 */
    tmp = extract_h(L_tmp);                /* tmp = 2 x sqrt(ener_exc/ener_hf) */

    for (i = 0; i < L_SUBFR16k; i++)
    {
        HF[i] = mult(HF[i], tmp);          move16();
    }

    /* find tilt of synthesis speech (tilt: 1=voiced, -1=unvoiced) */

    voAMRWBPDecHP400_12k8(synth, L_SUBFR, st->mem_hp400);

    L_tmp = 1L;                            move32();
    for (i = 0; i < L_SUBFR; i++)
        L_tmp = L_mac(L_tmp, synth[i], synth[i]);

    exp = norm_l(L_tmp);
    ener = extract_h(L_shl(L_tmp, exp));   /* ener = r[0] */

    L_tmp = 1L;                            move32();
    for (i = 1; i < L_SUBFR; i++)
        L_tmp = L_mac(L_tmp, synth[i], synth[i - 1]);

    tmp = extract_h(L_shl(L_tmp, exp));    /* tmp = r[1] */

    test();
    if (tmp > 0)
    {
        fac = div_s(tmp, ener);
    } else
    {
        fac = 0;                           move16();
    }


    /* modify energy of white noise according to synthesis tilt */
    gain1 = sub(32767, fac);
    gain2 = mult(sub(32767, fac), 20480);
    gain2 = shl(gain2, 1);

    test();
    if (st->vad_hist > 0)
    {
        weight1 = 0;
        weight2 = 32767;
    } else
    {
        weight1 = 32767;
        weight2 = 0;
    }
    tmp = mult(weight1, gain1);
    tmp = add(tmp, mult(weight2, gain2));

    test();
    if (tmp != 0)
    {
        tmp = add(tmp, 1);
    }
    HP_est_gain = tmp;

    test();
    if (sub(HP_est_gain, 3277) < 0)
    {
        HP_est_gain = 3277;                /* 0.1 in Q15 */
        move16();
    }
    /* synthesis of noise: 4.8kHz..5.6kHz --> 6kHz..7kHz */
    voAMRWBPDecWeight_a(Aq, Ap, 19661, M);            /* fac=0.6 */
    voAMRWBPDecSynFilt(Ap, M, HF, HF, L_SUBFR16k, st->mem_syn_hf, 1);

    /* noise High Pass filtering (1ms of delay) */
    voAMRWBPDecFilt_6k_7k(HF, L_SUBFR16k, st->mem_hf);

    /* filtering of the original signal */
    voAMRWBPDecFilt_6k_7k(HF_SP, L_SUBFR16k, st->mem_hf2);

    /* check the gain difference */
    voAMRWBPDecScale_sig(HF_SP, L_SUBFR16k, -1);

    ener = extract_h(Dot_product12(HF_SP, HF_SP, L_SUBFR16k, &exp_ener));

    /* set energy of white noise to energy of excitation */
    tmp = extract_h(Dot_product12(HF, HF, L_SUBFR16k, &exp));

    test();
    if (sub(tmp, ener) > 0)
    {
        tmp = shr(tmp, 1);                 /* Be sure tmp < ener */
        exp = add(exp, 1);
    }
    L_tmp = L_deposit_h(div_s(tmp, ener)); /* result is normalized */
    exp = sub(exp, exp_ener);
    Isqrt_n(&L_tmp, &exp);
    L_tmp = L_shl(L_tmp, exp);             /* L_tmp, L_tmp in Q31 */
    HP_calc_gain = extract_h(L_tmp);       /* tmp = sqrt(ener_input/ener_hf) */

    /* st->gain_alpha *= st->dtx_encSt->dtxHangoverCount/7 */
    L_tmp = L_shl(L_mult(st->dtx_encSt->dtxHangoverCount, 4681), 15);
    st->gain_alpha = mult(st->gain_alpha, extract_h(L_tmp));

    test();
    if (sub(st->dtx_encSt->dtxHangoverCount, 6) > 0)
        st->gain_alpha = 32767;
    HP_est_gain = shr(HP_est_gain, 1);     /* From Q15 to Q14 */
    HP_corr_gain = add(mult(HP_calc_gain, st->gain_alpha), mult(sub(32767, st->gain_alpha), HP_est_gain));

    /* Quantise the correction gain */
    dist_min = 32767;
    for (i = 0; i < 16; i++)
    {
        dist = mult(sub(HP_corr_gain, HP_gain[i]), sub(HP_corr_gain, HP_gain[i]));
        test();
        if (dist_min > dist)
        {
            dist_min = dist;
            HP_gain_ind = i;
        }
    }

    HP_corr_gain = HP_gain[HP_gain_ind];

    /* return the quantised gain index when using the highest mode, otherwise zero */
    return (HP_gain_ind);

}
