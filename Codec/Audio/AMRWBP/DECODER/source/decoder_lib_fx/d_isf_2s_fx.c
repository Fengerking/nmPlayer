#include "typedef.h"
#include "basic_op.h"
#include "count.h"


#include "amr_plus_fx.h"

/*-------------------------------------------------------------------*
 * routine:   dpisf_2s()                                              *
 *            ~~~~~~~~~                                              *
 * Decoding of ISF parameters                                        *
 *-------------------------------------------------------------------*
 *  Arguments:                                                       *
 *    indice[] : indices of the two selected codebook entries        *
 *    isf[]    : quantized ISFs (in frequency domain)                *
 *-------------------------------------------------------------------*/

#define ALPHA        29491          /* 0. 9 in Q15                         */
#define ISF_GAP      128            /* 50 Hz */
#define L_MEANBUF    3
#define ONE_ALPHA    (32768-ALPHA)  /* (1.0 - ALPHA) in Q15                */
#define ORDER        16             /* order of linear prediction filter   */
#define MU           10923          /* Prediction factor (1.0/3.0) in Q15  */

void Dpisf_2s_46b_2(
  Word16 *indice,    /* input:  quantization indices                  */
  Word16 *isf_q,     /* output: quantized ISFs in the cosine domain   */
  Word16 *past_isfq, /* i/0   : past ISF quantizer                    */
  Word16 *isfold,    /* input : past quantized ISF                    */
  Word16 *isf_buf,   /* input : isf buffer                            */
  Word16 bfi,        /* input : Bad frame indicator                   */
  Word16 bfi_2nd_st, /* input : 2nd stage bfi mask (bin: 011111)      */
  Word16 enc_dec
)
{
  Word16 ref_isf[ORDER];
  Word32 L_tmp;
  Word16 i, j, tmp;

  
  if(bfi == 0) /* Good frame */
  {
    for(i = 0; i < 9; i++)
    {
      isf_q[i] = Dico1_isf[indice[0] * 9 + i]; 
    }

    for(i = 0; i < 7; i++)
    {
      isf_q[i + 9] = Dico2_isf[indice[1] * 7 + i]; 
    }
    logic16();
    if ((bfi_2nd_st & 1) == 0)
    for(i = 0; i < 3; i++)
    {
      isf_q[i] = add(isf_q[i] , Dico21_isf[indice[2] * 3 + i]);     
    }
    logic16(); 
    if ((bfi_2nd_st & 2) == 0)
    for(i = 0; i < 3; i++)
    {
      isf_q[i + 3] = add(isf_q[i + 3] , Dico22_isf[indice[3] * 3 + i]);  
    }
    logic16();
    if ((bfi_2nd_st & 4) == 0)
    for(i = 0; i < 3; i++)
    {
      isf_q[i + 6] =
      add(isf_q[i + 6] , Dico23_isf[indice[4] * 3 + i]);   
    }
    logic16(); 
    if ((bfi_2nd_st & 8) == 0)
    for(i = 0; i < 3; i++)
    {
      isf_q[i + 9] =
      add(isf_q[i + 9] , Dico24_isf[indice[5] * 3 + i]);   
    }
    logic16();
    if ((bfi_2nd_st & 16) == 0)
    for(i = 0; i < 4; i++)
    {
      isf_q[i + 12] =
      add(isf_q[i + 12] , Dico25_isf[indice[6] * 4 + i]);   
    }

    for(i = 0; i < ORDER; i++)
    {
      tmp = isf_q[i];                                            
      isf_q[i] =
      add(add(tmp , Mean_isf[i]) , mult(MU , past_isfq[i]) );    
      past_isfq[i] = tmp;                                          
    }

    
    if(enc_dec > 0)
    {
      for(i = 0; i < ORDER; i++)
      {
        for(j = (L_MEANBUF - 1); j > 0; j--)
        {
          isf_buf[j * ORDER + i] = isf_buf[(j - 1) * ORDER + i];  
        }
        isf_buf[i] = isf_q[i];   
      }
    }

  }
  else
  {
    /* bad frame */

    for(i = 0; i < ORDER; i++)
    {
      L_tmp = L_mult(Mean_isf[i], 8192);

      for(j = 0; j < L_MEANBUF; j++)
      {
        L_tmp = L_mac(L_tmp, isf_buf[j * ORDER + i], 8192);

      }

      ref_isf[i] = round16(L_tmp);     
    }

    /* use the past ISFs slightly shifted towards their mean */
    for(i = 0; i < ORDER; i++)
    {
      isf_q[i] = add(mult(ALPHA, isfold[i]), mult(ONE_ALPHA, ref_isf[i]));        

    }

    /* estimate past quantized residual to be used in next frame */
    for(i = 0; i < ORDER; i++)
    {
      /* predicted ISF */
      tmp = add(ref_isf[i], mult(past_isfq[i], MU));      /* predicted ISF */
      /* past_isfq[i] *= 0.5 */
      past_isfq[i] = sub(isf_q[i], tmp);  
      past_isfq[i] = shr(past_isfq[i], 1);          /* past_isfq[i] *= 0.5 */
    }
  }

  voAMRWBPDecReorder_isf(isf_q, ISF_GAP, ORDER);
   
  return;
  
}

