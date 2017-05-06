/*-------------------------------------------------------------------*
 *                         QPISF_2S.C								 *
 *-------------------------------------------------------------------*
 * Coding/Decoding of ISF parameters  with prediction.               *
 *                                                                   *
 * The ISF vector is quantized using two-stage VQ with split-by-2    *
 * in 1st stage and split-by-5 (or 3)in the second stage.            *
 *-------------------------------------------------------------------*/


#include "typedef.h"
#include "basic_op.h"
#include "cnst_wb_fx.h"
#include "acelp_fx.h"
#include "count.h"

#define ORDER   16            /* order of linear prediction filter */
#define ISF_GAP 128           /* 50 Hz */
#define N_SURV  4

#define SIZE_BK1  256
#define SIZE_BK2  256
#define SIZE_BK21 64
#define SIZE_BK22 128
#define SIZE_BK23 128
#define SIZE_BK24 32
#define SIZE_BK25 32

#define SIZE_BK21_36b 128
#define SIZE_BK22_36b 128
#define SIZE_BK23_36b 64

/* LF ISF quantiser codebooks */
extern const Word16 Mean_isf[16];
extern const Word16 Dico1_isf[SIZE_BK1 * 9];
extern const Word16 Dico2_isf[SIZE_BK2 * 7];
extern const Word16 Dico21_isf[SIZE_BK21 * 3];
extern const Word16 Dico22_isf[SIZE_BK22 * 3];
extern const Word16 Dico23_isf[SIZE_BK23 * 3];
extern const Word16 Dico24_isf[SIZE_BK24 * 3];
extern const Word16 Dico25_isf[SIZE_BK25 * 4];
extern const Word16 Dico21_isf_36b[SIZE_BK21_36b * 5];
extern const Word16 Dico22_isf_36b[SIZE_BK22_36b * 4];
extern const Word16 Dico23_isf_36b[SIZE_BK23_36b * 7];


#define MU         10923                   /* Prediction factor   (1.0/3.0) in Q15 */
#define N_SURV_MAX 4                       /* 4 survivors max */
#define ALPHA      29491                   /* 0. 9 in Q15     */
#define ONE_ALPHA (32768-ALPHA)            /* (1.0 - ALPHA) in Q15 */

/*-------------------------------------------------------------------*
 * routine:   Disf_2s_46b()                                          *
 *            ~~~~~~~~~                                              *
 * Decoding of ISF parameters                                        *
 *-------------------------------------------------------------------*/

void voAMRWBPDecDpisf_2s_46b(
     Word16 * indice,                      /* input:  quantization indices                       */
     Word16 * isf_q,                       /* output: quantized ISF in frequency domain (0..0.5) */
     Word16 * past_isfq,                   /* i/0   : past ISF quantizer                    */
     Word16 * isfold,                      /* input : past quantized ISF                    */
     Word16 * isf_buf,                     /* input : isf buffer                                                        */
     Word16 bfi,                           /* input : Bad frame indicator                   */
     Word16 enc_dec
)
{
    Word16 ref_isf[M];
    Word16 i, j, tmp;
    Word32 L_tmp;

    if (bfi == 0)                          /* Good frame */
    {
        for (i = 0; i < 9; i++)
        {
            isf_q[i] = Dico1_isf[indice[0] * 9 + i]; 
        }
        for (i = 0; i < 7; i++)
        {
            isf_q[i + 9] = Dico2_isf[indice[1] * 7 + i]; 
        }

        for (i = 0; i < 3; i++)
        {
            isf_q[i] = add(isf_q[i], Dico21_isf[indice[2] * 3 + i]); 
        }
        for (i = 0; i < 3; i++)
        {
            isf_q[i + 3] = add(isf_q[i + 3], Dico22_isf[indice[3] * 3 + i]);    
        }
        for (i = 0; i < 3; i++)
        {
            isf_q[i + 6] = add(isf_q[i + 6], Dico23_isf[indice[4] * 3 + i]);    
        }
        for (i = 0; i < 3; i++)
        {
            isf_q[i + 9] = add(isf_q[i + 9], Dico24_isf[indice[5] * 3 + i]);    
        }
        for (i = 0; i < 4; i++)
        {
            isf_q[i + 12] = add(isf_q[i + 12], Dico25_isf[indice[6] * 4 + i]);  
        }

        for (i = 0; i < ORDER; i++)
        {
            tmp = isf_q[i]; 
            isf_q[i] = add(tmp, Mean_isf[i]);
            isf_q[i] = add(isf_q[i], mult(MU, past_isfq[i]));  
            past_isfq[i] = tmp;
        }

        if (enc_dec)
        {
            for (i = 0; i < M; i++)
            {
                for (j = (L_MEANBUF - 1); j > 0; j--)
                {
                    isf_buf[j * M + i] = isf_buf[(j - 1) * M + i]; 
                }
                isf_buf[i] = isf_q[i]; 
            }
        }
    } else
    {                                      /* bad frame */
        for (i = 0; i < M; i++)
        {
            L_tmp = L_mult(Mean_isf[i], 8192);
            for (j = 0; j < L_MEANBUF; j++)
            {
                L_tmp = L_mac(L_tmp, isf_buf[j * M + i], 8192);
            }
            ref_isf[i] = round16(L_tmp);
        }

        /* use the past ISFs slightly shifted towards their mean */
        for (i = 0; i < ORDER; i++)
        {
            isf_q[i] = add(mult(ALPHA, isfold[i]), mult(ONE_ALPHA, ref_isf[i])); 
        }

        /* estimate past quantized residual to be used in next frame */

        for (i = 0; i < ORDER; i++)
        {
            tmp = add(ref_isf[i], mult(past_isfq[i], MU));      /* predicted ISF */
            past_isfq[i] = isf_q[i] - tmp; 
            past_isfq[i] = past_isfq[i] >> 1;              /* past_isfq[i] *= 0.5 */
        }
    }

    voAMRWBPDecReorder_isf(isf_q, ISF_GAP, ORDER);

    return;
}

/*-------------------------------------------------------------------*
 * routine:   Disf_2s_36b()                                          *
 *            ~~~~~~~~~                                              *
 * Decoding of ISF parameters                                        *
 *-------------------------------------------------------------------*/
void voAMRWBPDecDpisf_2s_36b(
     Word16 * indice,                      /* input:  quantization indices                       */
     Word16 * isf_q,                       /* output: quantized ISF in frequency domain (0..0.5) */
     Word16 * past_isfq,                   /* i/0   : past ISF quantizer                    */
     Word16 * isfold,                      /* input : past quantized ISF                    */
     Word16 * isf_buf,                     /* input : isf buffer                                                        */
     Word16 bfi,                           /* input : Bad frame indicator                   */
     Word16 enc_dec
)
{
    Word16 ref_isf[M];
    Word16 i, j, tmp;
    Word32 L_tmp;

    if (bfi == 0)                          /* Good frame */
    {
        for (i = 0; i < 9; i++)
        {
            isf_q[i] = Dico1_isf[indice[0] * 9 + i]; 
        }
        for (i = 0; i < 7; i++)
        {
            isf_q[i + 9] = Dico2_isf[indice[1] * 7 + i]; 
        }

        for (i = 0; i < 5; i++)
        {
            isf_q[i] = add(isf_q[i], Dico21_isf_36b[indice[2] * 5 + i]);   
        }
        for (i = 0; i < 4; i++)
        {
            isf_q[i + 5] = add(isf_q[i + 5], Dico22_isf_36b[indice[3] * 4 + i]);  
        }
        for (i = 0; i < 7; i++)
        {
            isf_q[i + 9] = add(isf_q[i + 9], Dico23_isf_36b[indice[4] * 7 + i]); 
        }

        for (i = 0; i < ORDER; i++)
        {
            tmp = isf_q[i];
            isf_q[i] = add(tmp, Mean_isf[i]); 
            isf_q[i] = add(isf_q[i], mult(MU, past_isfq[i])); 
            past_isfq[i] = tmp; 
        }

        if (enc_dec)
        {
            for (i = 0; i < M; i++)
            {
                for (j = (L_MEANBUF - 1); j > 0; j--)
                {
                    isf_buf[j * M + i] = isf_buf[(j - 1) * M + i];
                }
                isf_buf[i] = isf_q[i]; 
            }
        }
    } else
    {                                      /* bad frame */
        for (i = 0; i < M; i++)
        {
            L_tmp = L_mult(Mean_isf[i], 8192);
            for (j = 0; j < L_MEANBUF; j++)
            {
                L_tmp = L_mac(L_tmp, isf_buf[j * M + i], 8192);
            }

            ref_isf[i] = round16(L_tmp); 
        }

        /* use the past ISFs slightly shifted towards their mean */
        for (i = 0; i < ORDER; i++)
        {
            isf_q[i] = add(mult(ALPHA, isfold[i]), mult(ONE_ALPHA, ref_isf[i]));  
        }

        /* estimate past quantized residual to be used in next frame */

        for (i = 0; i < ORDER; i++)
        {
            tmp = add(ref_isf[i], mult(past_isfq[i], MU));      /* predicted ISF */
            past_isfq[i] = sub(isf_q[i], tmp); 
            past_isfq[i] = shr(past_isfq[i], 1);                /* past_isfq[i] *= 0.5 */
        }
    }

    voAMRWBPDecReorder_isf(isf_q, ISF_GAP, ORDER);
    return;
}


/*--------------------------------------------------------------------------*
 * procedure  Reorder_isf()                                                 *
 *            ~~~~~~~~~~~~~                                                 *
 * To make sure that the  isfs are properly order and to keep a certain     *
 * minimum distance between consecutive isfs.                               *
 *--------------------------------------------------------------------------*
 *    Argument         description                     in/out               *
 *    ~~~~~~~~         ~~~~~~~~~~~                     ~~~~~~               *
 *     isf[]           vector of isfs                    i/o                *
 *     min_dist        minimum required distance         i                  *
 *     n               LPC order                         i                  *
 *--------------------------------------------------------------------------*/

void voAMRWBPDecReorder_isf(
     Word16 * isf,                         /* (i/o) Q15: ISF in the frequency domain (0..0.5) */
     Word16 min_dist,                      /* (i) Q15  : minimum distance to keep             */
     Word16 n                              /* (i)      : number of ISF                        */
)
{
    Word16 i, isf_min;
    isf_min = min_dist;                    

    for (i = 0; i < n - 1; i++)
    {
        if(isf[i] < isf_min)
        {
            isf[i] = isf_min;              
        }
        isf_min = isf[i] + min_dist;
    }

    return;
}


Word16 voAMRWBPDecSubVQ(                             /* output: return quantization index     */
     Word16 * x,                           /* input : ISF residual vector           */
     const Word16 * dico,                        /* input : quantization codebook         */
     Word16 dim,                           /* input : dimention of vector           */
     Word16 dico_size,                     /* input : size of quantization codebook */
     Word32 * distance                     /* output: error of quantization         */
)
{
    Word16 i, j, index, temp;
    const Word16 *p_dico;
    Word32 dist_min, dist;

    dist_min = MAX_32;                     
    p_dico = dico;                         

    index = 0;                             
    for (i = 0; i < dico_size; i++)
    {
        dist = 0;                          
        for (j = 0; j < dim; j++)
        {
            temp = sub(x[j], *p_dico++);
            dist = L_mac(dist, temp, temp);
        }

        if (dist < dist_min)
        {
            dist_min = dist;               
            index = i;                     
        }
    }

    *distance = dist_min;                  

    /* Reading the selected vector */

    p_dico = &dico[index * dim];           
    for (j = 0; j < dim; j++)
    {
        x[j] = *p_dico++;                  
    }

    return index;
}
