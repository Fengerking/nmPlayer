/*-------------------------------------------------------------------*
* functions: q_isf_hf()                                             *
*                                                                   *
* Coding/Decoding of HF ISF parameters with AR prediction.          *
*                                                                   *
* The ISF vector is quantized using one-stage VQ with 8 elements.   *
*-------------------------------------------------------------------*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "amr_plus_fx.h"

#define ISF_GAP_HF      180.0f
#define ISF_GAP_HF_FX   461
#define MU_FX           16384
#define MU              0.5f

#define ALPHA_FX     29491

void D_isf_hf(
			  Word16 *indice,      /* (i) : quantization indices                    */
			  Word16 *isf_q,       /* (o) : quantized ISFs in the cosine domain     */
			  Word16 *past_q,      /* i/o : past quantized isf (for AR prediction)  */
			  Word16 bfi,          /* (i) : Bad frame indicator                     */
			  const Word16 *Mean_isf_hf, /* (i) : Mean isf Dico                           */
			  const Word16 *Dico1_isf_hf /* (i) : sif Dico                                */
			  )
{
	Word16 i, iQ0, iQ1, tmp;


	iQ0 = shl(indice[0],3);
	iQ1 = shl(indice[1],3); 

	if(bfi == 0) /* Good frame */
	{
		for (i = 0; i < Q_ISF_ORDER; i++) 
		{
			tmp = add(Dico1_isf_hf[iQ0+i],Dico2_isf_hf[iQ1+i]); 
			tmp = add(tmp, Mean_isf_hf[i]);
			isf_q[i] = add(tmp, mult_r(MU_FX, past_q[i]));
		}
	}
	else /* bad frame */
	{
		/* use the past ISFs slightly shifted towards the means */
		for (i=0; i<Q_ISF_ORDER; i++) 
		{
			isf_q[i] = add(mult_r(ALPHA_FX,past_q[i]), Mean_isf_hf[i]);
		}
	}

	voAMRWBPDecReorder_isf(isf_q, ISF_GAP_HF_FX, Q_ISF_ORDER);

	for (i=0; i<Q_ISF_ORDER; i++) 
	{
		past_q[i] = sub(isf_q[i] , Mean_isf_hf[i]);
	}

	return;
}


