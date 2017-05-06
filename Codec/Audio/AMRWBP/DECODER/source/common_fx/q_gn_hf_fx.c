/*-------------------------------------------------------------------*
 * functions: q_gain_hf()                                            *
 *                                                                   *
 * Coding/Decoding of HF gain parameters with AR prediction.         *
 *                                                                   *
 * The gain vector is quantized using one-stage VQ with 4 elements.  *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "amr_plus_fx.h"

/*-------------------------------------------------------------------*
 * routine:   d_gain_hf()                                            *
 *            ~~~~~~~~~                                              *
 * Decoding of gain parameters                                       *
 *-------------------------------------------------------------------*
 *  Arguments:                                                       *
 *    indice[] : indices of the two selected codebook entries        *
 *    gain[]   : quantized gains                                     *
 *-------------------------------------------------------------------*/

#define ALPHA_FX    29491   /*Q15*/

void D_gain_hf(
  Word16 indice,     /* input:  quantization indices */
  Word16 *gain_q,    /* output: quantized gains      */
  Word16 *past_q,    /* i/o   : past quantized gain (1 word) */
  Word16 bfi         /* input : Bad frame indicator  */
)
{
  Word16 i, iQ;
  Word32 Ltmp;
  
  iQ = shl(indice, 2);
  
  if(bfi == 0) /* Good frame */
  {
    for (i=0; i<Q_GN_ORDER; i++)
    {
      gain_q[i] = add(Dico_gain_hf[iQ+i], MEAN_GAIN_HF_FX);  
    }
  }
  else /* bad frame */
  {
    /* use the past gains slightly shifted towards the means */

    *past_q = sub(mult_r(ALPHA_FX, add(*past_q ,5120)), 5120);  

    for (i=0; i<Q_GN_ORDER; i++)
    {
      gain_q[i] = add( *past_q ,  MEAN_GAIN_HF_FX);   
    }
  }

  Ltmp = 0;   move32();

  for (i=0; i<Q_GN_ORDER; i++) 
  {
    Ltmp = L_add(Ltmp, gain_q[i]);
  }

  *past_q = extract_l(L_shr(L_sub(Ltmp, 2875),2));

  return;
}

