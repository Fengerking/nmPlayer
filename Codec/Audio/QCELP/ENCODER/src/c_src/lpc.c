/* lpc.c - Computes the Linear Preditive Coefficients using	*/
/*	Durbin's recursion.					*/
//#include	<malloc.h>
#include	"basic_op.h"
#include	"celp.h"

void durbin(
			int	    *R,
			short   *lpc,
			short   order
			)
{
    register short i, j, k, scale;
	long pc[LPCORDER];
	long pc_scratch[LPCORDER];
    long sum, Ltemp, E_long, A;
    long E_min;
	short shift;
	
    A = 0; /* eliminate compiler warning */

	if (*R <= 1)
	{
		/* if R(0)<=0, set LPC coeff. to zero */
		for (i = 0; i < order; i++)
		{
			lpc[i]=0;
		}
	}
	else
	{
		E_long = R[0]; /* maintain 32-bit normalized pred. error */
        E_min = E_long >> 10; /* 2**-10 = -30dB */

		for(i=0; i < order; i++)
        {
            if( E_long < E_min )
            {
                pc[i] = 0;   /* reflection coef = 0 */
            }
            else
            {
                sum = 0;
			    scale = 1;
                for(j = i - 1; j >= 0; j--)
                {
                    /* sum += pc[j] * R[i-j];  */
    				Ltemp = L_mpy_ll(R[i-j], pc[j]);
    				Ltemp >>= scale; /* Ltemp Q28 */
    				sum += Ltemp;
    				if ((sum > 0x40000000) || (sum < -0x40000000))
    				{
    					sum >>= 1;
    					scale++;
    				}
				}
			    sum <<= 3 + scale;  /* back to Q28 */

                /* pc[i] = (R[i+1] - sum) / E; */
			    A = R[i+1] - sum;
		        	
			    shift = norm_l(E_long);	
			    if (A < 0)
			    {
				    pc[i] = L_negate(((L_divide(L_abs(A), (E_long << shift))))) << shift;
				}
			    else
			    {
				    pc[i] = L_divide(A, (E_long << shift)) << shift;
				}
            } /* end else */

            /* pc[i] is now Q31 */
            for(j = 0; j < i; j++)
            {
                /* pc_scratch[j] = pc[j] - pc[i] * pc[i-j-1]; */
                Ltemp = L_mpy_ll(pc[i], pc[i-j-1]);  /* Ltemp Q27 */
                pc_scratch[j] = pc[j] - Ltemp;
            }

            if (i != order - 1)
            {
                for(k = j - 1; k >= 0; k--)
                {
                    pc[k] = pc_scratch[k];
                }

                /* E = (1 - pc[i]*pc[i]) * E; */
                Ltemp = L_mpy_ll(A, pc[i]); /* Ltemp Q28 */
                E_long -= Ltemp;
            }  /* end if */

            pc[i] += 0x4;   
            pc[i] >>= 3;
         } /*end for i */
    
		/* Round predictor coefficients to 16 bits (Q12) and
		   negate them for compatibility with other modules */
		lpc[i-1] = (short)((pc[i-1] + 0x8000) >> 16);
		for(k = order-2; k >= 0; k--)
		{
			lpc[k] = (short)((pc_scratch[k] + 0x8000) >> 16);
		}
    }  /* end else */
}

/* compute_autocorr - computes the "shift"th autocorrelation of	*/
/* 	"signal" of windowsize "length".			*/

int compute_autocorr(
					 short   *signal,
					 short   length,
					 short   shift
					 )
{
	int	R;
	short *indata1, *indata2;
	short	i;
	
	R=0;
	indata1 = signal;
	indata2 = signal + shift;
	for (i= length-shift - 1; i >= 0; i--) {
		R += (int)(*indata1)*(int)(*indata2);
		indata1++; indata2++;
	}

	return(R);
}

void compute_lpc(
				 short            *speech,
				 short            windowsize,
				 short            windowtype,
				 short            order,
				 short            *lpc, 
				 int			  *R
				 )
{
	short	i;
	short	shift; 
    short   wspeech[LPCSIZE];

	/* Window the speech signal		*/
	switch (windowtype) {
	case HAMMING:
	    HAMMINGwindow(speech, wspeech, windowsize);
	    break;
	default:
	    break;
	}

	for (i=FILTERORDER; i>=0; i--) {
		R[i]=compute_autocorr(wspeech, windowsize, i);
	}

	shift = norm_l(R[0]);
	for (i=FILTERORDER; i>=0; i--) {
		R[i] <<= shift;
	}

	durbin(R, lpc, order);	

	for (i=FILTERORDER; i>=0; i--) {
		R[i] >>= shift;
	}
}

void interp_lpcs(
				 short            mode,
				 short            *prev_lsp,
				 short            *curr_lsp,
				 short            lpc[2][MAXSF][2][LPCORDER],
				 short            type
				 )
{
    short i,j;
	short *indata1, *indata2;
	short invpitchsf;
    short wght_factor;
    short tmp_lsp[LPCORDER];
    short current_center;
    short interp_factor;
		
	invpitchsf = FSIZE_DIV_PITCHSF[mode];
    for(i=0; i< PITCHSF[mode]; i++) 
	{
		current_center = i * invpitchsf + (invpitchsf >> 1);
		interp_factor= current_center+FSIZE-LPC_CENTER;
		interp_factor = saturate((int)interp_factor*INVFSIZE);
	
		for (j=LPCORDER-1; j >= 0; j--) {
			tmp_lsp[j] = add(mult(sub(curr_lsp[j], prev_lsp[j]), interp_factor), prev_lsp[j]);
		}
		
		indata1 = lpc[PITCH][i][NOT_WGHTED];
		lsp2lpc(tmp_lsp, indata1, LPCORDER);
		
		wght_factor=MAX_16;
		for (j=0; j<LPCORDER; j++) {
			wght_factor = mult(wght_factor, BWE_FACTOR_16);
			 *indata1 = mult(*indata1, wght_factor);
			 indata1++;
		}
		
		if (type==BOTH) 
		{
			wght_factor = MAX_16;
			indata1 = lpc[PITCH][i][NOT_WGHTED];
			indata2 = lpc[PITCH][i][WGHTED];
			for (j=0; j<LPCORDER; j++) {
				wght_factor = mult(wght_factor, PERCEPT_WGHT_FACTOR_16);
				*indata2++ = mult(*indata1, wght_factor);
				indata1++;
			}
		}
	}
}


/* HAMMINGwindow - perform Hamming windowing    */

void HAMMINGwindow(
				   short *input,
				   short *output,
				   short  length
				   )
{
    short i;
	short *indata;
    extern short HAMMING_TAB[];

	indata = output + length - 1;
    for (i=0; i<length/2; i++) {
        output[i]		   = (short)(((int)input[i]*(int)HAMMING_TAB[i] + 0x2000) >> 14);
		output[length-1-i] = (short)(((int)input[length-1-i]*(int)HAMMING_TAB[i] + 0x2000) >> 14);
    }
}




