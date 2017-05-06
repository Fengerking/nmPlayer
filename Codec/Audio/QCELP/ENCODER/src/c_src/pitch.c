/* pitch.c - perform pitch functions, including open loop pitch */
/*    estimation, and closed loop pitch search                  */

#include "basic_op.h"
#include "celp.h"

#define      FR_LAG_SEGMENTS             3
//static  int  FRL_RANGE[FR_LAG_SEGMENTS+1]= {MINLAG, 45, 85, MAXLAG+1};
//static  int  FRL_RES[FR_LAG_SEGMENTS]    = {2, 2, 2};

/* For CELP+ Fractional Pitch */
//static int FRAC[NUMMODES] = {NO,NO,NO,YES,YES};  /* make fractional pitch */

void set_lag_range(
				   struct PITCHPARAMS  *pitch_params,
				   short               *qcode_min_lag)
{
    pitch_params->min_lag = MINLAG;
    pitch_params->max_lag = MAXLAG;
    quantize_min_lag(pitch_params->min_lag, qcode_min_lag);
}

#ifndef ASM_OPT   
void comp_corr( 
			   short *scal_sig,   /* i   : scaled signal.                          */
			   short *target,
			   short L_frame,     /* i   : length of frame to compute pitch        */
			   short lag_max,     /* i   : maximum lag                             */
			   short lag_min,     /* i   : minimum lag                             */
			   int	*corr)       /* o   : correlation of selected lag             */
{
    int  i, j;
    short *p, *p1;
    int t0;
    //TO_DO_ASM
    for (i = lag_max; i >= lag_min; i--)
    {
		p = target;           
		p1 = &scal_sig[-i];     
		t0 = 0;                
		for (j = L_frame; j !=0;)
		{	

			/*t0  = L_mac(t0, *p, *p1); p++; p1++;
            t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;
			t0  = L_mac(t0, *p, *p1); p++; p1++;*/

			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			t0 = (t0 + (*p)*(*p1));	p++; p1++;
			
			j-=10;       
		}
		
		corr[-i] = t0;	
		
//		i--; 
//		corr[-i] = t0; 

    }
	
    return;
} 
#endif

int Lag_max(   
		int   *corr,      
		short scal_sig[],   
		int   L_frame,      
		int   lag_max,      
		int   lag_min,    
		int   *cor_max)
{
	int  i;
	short *p;
	int max, t0;
	int p_max = 0; 
	max = MIN_32;               
	p_max = lag_max;           

	for (i = lag_max; i >= lag_min; i--)  
	{
		if ((corr[-i] >= max)) 
		{ 
			max = corr[-i];       
			p_max = i;           
		} 
	}
	/* compute energy */
	t0 = 0;                     
	p = &scal_sig[-p_max];     
	for (i = 0; i < L_frame; i+= 10)
	{
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
		t0 += ((*p)*(*p)); p++; 
	}

	if(t0 == 0)
	{
		*cor_max = MAX_16;
		return (p_max);
	}
	i = norm_l(t0);
	if(i & 1) i--;
	t0 <<= i;
	t0 = sqroot(t0);
	t0 = mult(t0, 23170);
	i >>= 1;
	t0 = shr(t0, (short)(i - 1));

	*cor_max = saturate(max/t0);	

	return (p_max);
}

void compute_pitch(
		short                 mode,
		short                 *target,
		struct ENCODER_MEM    *e_mem,
		short				 *lpc_ir,
		struct PITCHPARAMS    *pitch_params)
{
	int	   corr[MAXLAG], *corr_ptr;	
	short  y[FSIZE];
	short  pitch_out[MAXLAG+FSIZE+FR_INTERP_FILTER_LENGTH];	
	int    min_err,err_t, err_t1;
	int	   minExy = 0, minEyy = 0;	
	int	   tmpExy, tmpEyy;
	int    i,j, fsizepitch;
	//int	   *intExy;	
	short  *indata1, *indata2, *outdata;
	short  *pitch_out_shifted;
	int    p_max1, p_max2, p_max3;
	int    n_shiftcnt, d_shiftcnt;
	int    max1, max2, max3;
	short  s_tmpExy, s_tmpEyy;	 
	short  unquantb, qcode_b;	

	fsizepitch = FSIZE_DIV_PITCHSF[mode];

	/* do pitch search unless in 1/4 or 1/8 rate mode */		
	min_err =  MAX_32;
	pitch_out_shifted= &pitch_out[MAXLAG+2];

	/* construct p(n) */
	outdata = pitch_out_shifted - pitch_params->min_lag;
	indata1 = e_mem->dec.pitch_filt.memory + pitch_params->min_lag - 1;
	*outdata-- = *indata1++;
	for (i = pitch_params->max_lag + 2 - pitch_params->min_lag; i; i-=8) 
	{
		*outdata-- = *indata1++; *outdata-- = *indata1++;
		*outdata-- = *indata1++; *outdata-- = *indata1++;
		*outdata-- = *indata1++; *outdata-- = *indata1++;
		*outdata-- = *indata1++; *outdata-- = *indata1++;
	}

	tmpExy = pitch_params->min_lag- fsizepitch + 1;
	if(tmpExy > 0)
	{
		outdata = pitch_out_shifted - pitch_params->min_lag + 1;
		indata1 = e_mem->dec.pitch_filt.memory + pitch_params->min_lag - 2;
		for (i = pitch_params->min_lag-1; i>=tmpExy; i--) {
			*outdata++ = *indata1--;
		}
	}
	else
	{
		outdata = pitch_out_shifted - pitch_params->min_lag + 1;
		indata1 = e_mem->dec.pitch_filt.memory + pitch_params->min_lag - 2;
		for (i = pitch_params->min_lag-1; i>0; i-=4) {
			*outdata++ = *indata1--; *outdata++ = *indata1--;
			*outdata++ = *indata1--; *outdata++ = *indata1--;
		}			
	}
	outdata = pitch_out_shifted;
	for (i =fsizepitch; i; i-= 10) {
		*outdata++ = 0; *outdata++ = 0;
		*outdata++ = 0; *outdata++ = 0;
		*outdata++ = 0; *outdata++ = 0;
		*outdata++ = 0; *outdata++ = 0;
		*outdata++ = 0; *outdata++ = 0;
	}   
	pitch_params->lag=0;
	/* open loop pitch  search */
	corr_ptr = &corr[MAXLAG];
	indata1 = target;
	#ifdef ARMv7_OPT
	comp_corr40_pitch(pitch_out_shifted, indata1, fsizepitch, 
			pitch_params->max_lag, pitch_params->min_lag, corr_ptr);
	#else
	comp_corr(pitch_out_shifted, indata1, fsizepitch, 
			pitch_params->max_lag, pitch_params->min_lag, corr_ptr);
	#endif

	j = 71;
	p_max1 = Lag_max(corr_ptr, pitch_out_shifted, fsizepitch,
			pitch_params->max_lag, j, &max1);  

	i = (j- 1);
	j = 35;
	p_max2 = Lag_max(corr_ptr, pitch_out_shifted, fsizepitch,
			i, j, &max2);        

	i = (j- 1);
	p_max3 = Lag_max(corr_ptr, pitch_out_shifted, fsizepitch,
			i, pitch_params->min_lag, &max3);

	if (((max1* THRESHOLD>>15) - max2) < 0)
	{
		max1 = max2;                       
		p_max1 = p_max2;   
	}

	if (( (max1 * THRESHOLD>>15) - max3) < 0)
	{
		p_max1 = p_max3;                   
	}

	p_max2 = p_max1 - 4;
	if ((p_max2- MINLAG) < 0) {
		p_max2 = MINLAG;                                
	}

	p_max3 = p_max2 + 8;		
	if ((p_max3 - MAXLAG) > 0) {
		p_max3 = MAXLAG;                                
		p_max2 = p_max3 - 8;
	}
	/*  close loop pitch  search  */
#ifdef ASM_OPT 
	initial_recursive_conv_asm(&pitch_out_shifted[1-p_max2],
			fsizepitch - 1, lpc_ir);
#else
	initial_recursive_conv(&pitch_out_shifted[1-p_max2],
			fsizepitch - 1, lpc_ir);
#endif
	for (i=p_max2; i <= p_max3; i++) {
		#ifdef ASM_OPT
		recursive_conv_40_Opt(&pitch_out_shifted[-i], lpc_ir, fsizepitch);
		#else
		recursive_conv_10(&pitch_out_shifted[-i], lpc_ir, fsizepitch);
		#endif
		
		tmpExy=0; 	tmpEyy=0;
		if (i>=fsizepitch) {
			indata1 = target;
			indata2 = pitch_out_shifted - i;
			for (j=fsizepitch; j; j-=5) {
				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy +=((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				// delete L_add operate, have some error.for the test case ,the output is not bit match .lhp
				//tmpEyy = L_add(tmpEyy, (int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;
			}
		}
		else {
			outdata = y;
			indata1 = pitch_out_shifted - i;
			for (j = fsizepitch; j; j-=10) {
				*outdata++ = *indata1++; *outdata++ = *indata1++;
				*outdata++ = *indata1++; *outdata++ = *indata1++;
				*outdata++ = *indata1++; *outdata++ = *indata1++;
				*outdata++ = *indata1++; *outdata++ = *indata1++;
				*outdata++ = *indata1++; *outdata++ = *indata1++;
			}

			outdata = y+i;
			indata1 = pitch_out_shifted - i;
			for (j = fsizepitch - i; j; j--) {
				*outdata++ += *indata1++;
			}

			indata1 = target;
			indata2 = y;
			for (j = fsizepitch; j; j-= 5) {
				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;

				tmpExy += (int)(*indata1)*(int)(*indata2);
				tmpEyy += ((int)(*indata2)*(int)(*indata2));
				// delete L_add operate, have some error for the test case ,the output is not bit match. lhp
				//tmpEyy = L_add(tmpEyy, (int)(*indata2)*(int)(*indata2));
				indata1++;	indata2++;
			}
		}
		if(tmpExy > 0)
		{
			if (tmpEyy>1) {
				n_shiftcnt = (norm_l(tmpExy) - 1);
				d_shiftcnt = norm_l(tmpEyy);

				s_tmpExy = extract_h(tmpExy << n_shiftcnt);
				s_tmpEyy = extract_h(tmpEyy << d_shiftcnt);
				err_t1 = divide_s(s_tmpExy, s_tmpEyy);
				err_t1 = err_t1 * s_tmpExy;
				err_t = d_shiftcnt - 2*n_shiftcnt + 1;
				err_t1 = L_shl(err_t1, err_t);					
				err_t= -err_t1;	
			}					
			else{
				err_t = MAX_32;
			}

			if ((err_t < min_err)) {
				min_err=err_t;
				pitch_params->lag = i;
				pitch_params->frac = 0;
				minExy = tmpExy;
				minEyy = tmpEyy;
			}  
		}
	}		
	/* BUG FIX - sab Protect against divide by zero */
	if( minEyy < 1 )
	{
		pitch_params->lag = 0;
	}
	if (pitch_params->lag==0) {
		if(e_mem->frame_num > 0)
			pitch_params->frac=0;
		quantize_b(0, &(pitch_params->b), &qcode_b);
		pitch_params->qcode_b=qcode_b;			
	}
	else {
		tmpExy = minExy;
		tmpEyy = minEyy;
		n_shiftcnt = (norm_l(tmpExy) - 1);
		d_shiftcnt = norm_l(tmpEyy);

		s_tmpExy = extract_h(tmpExy << n_shiftcnt);
		s_tmpEyy = extract_h(tmpEyy << d_shiftcnt);

		unquantb = shr(divide_s(s_tmpExy , s_tmpEyy), (3 - (d_shiftcnt - n_shiftcnt)));

		quantize_b(unquantb, &(pitch_params->b), &qcode_b);

		if(pitch_params->b == 0)
			pitch_params->lag = 0;
		pitch_params->qcode_b = qcode_b;
	}
	quantize_lag(&(pitch_params->lag), &(pitch_params->qcode_lag),
			&(pitch_params->frac), &(pitch_params->qcode_frac));		
}
#if 1 // combine two function into one function
void initial_recursive_conv(
		short   *resid,
		short   length,
		short   *impulse_response
                )
{
	short     i, j;
	short	  resid0;
	short     *indata, *outdata;	
	short     curindex;
	for(i = length-1; i>=0; i--)
	{
		resid0 = resid[i];
		if (resid0 == 0)  continue;
		curindex = ((length-i) <= LENGTH_OF_IMPULSE_RESPONSE) ? (length-i) : LENGTH_OF_IMPULSE_RESPONSE;
		outdata = &resid[i] + 1;
		indata = impulse_response + 1;
		for(j = curindex - 1; j; j--)
		{
			*outdata += ((resid0*(*indata) + 8192) >> 14);
			indata++; outdata++; 	
		}
	}
}
#else
void initial_recursive_conv(
		short   *resid,
		short   length,
		short   *impulse_response
		)
{
	int i;
	for (i= length-1; i >= 0; i--) {
		recursive_conv(&resid[i], impulse_response, length-i);
	} 
}
void recursive_conv(
		short   *resid,
		short   *impulse_response,
		short   length 
		)
{
	int	  resid0;
	int   i;
	short *indata, *outdata;	
	int curindex;
	if (resid[0]==0) return;
	curindex = (length <= LENGTH_OF_IMPULSE_RESPONSE) ? length : LENGTH_OF_IMPULSE_RESPONSE;
	resid0 = resid[0];
	indata = impulse_response + 1;
	outdata = resid + 1;
	for(i = curindex - 1; i; i--)
	{
		*outdata += ((resid0*(*indata) + 8192) >> 14);
		indata++; outdata++; 	
	}
}
#endif


//#ifndef ASM_OPT
void recursive_conv_10(
		short   *resid,
		short   *impulse_response,
		short   length 
		)
{
	int	  resid0;
	short *indata, *outdata;
	int curindex;

	if (resid[0]==0) return;

	curindex = (length <= LENGTH_OF_IMPULSE_RESPONSE) ? length : LENGTH_OF_IMPULSE_RESPONSE;

	resid0 = resid[0];
	indata = impulse_response + 1;
	outdata = resid + 1;

	for( ; curindex; )
	{
		*outdata += ((resid0*(*indata) + 8192) >> 14);
		indata++; outdata++; 

		*outdata += ((resid0*(*indata) + 8192) >> 14);
		indata++; outdata++; 

		*outdata += ((resid0*(*indata) + 8192) >> 14);
		indata++; outdata++; 

		*outdata += ((resid0*(*indata) + 8192) >> 14);
		indata++; outdata++; 

		*outdata += ((resid0*(*indata) + 8192) >> 14);
		indata++; outdata++; 

		curindex -= 5;
	}
}
//#endif
