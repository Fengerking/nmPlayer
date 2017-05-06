//#include <stdlib.h> 
#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"




#define N_MAX       1152
#define ORDER_MAX   7
#define SCALE1      14      /* Scaling factor for FFT9_Fx  (/4) */
#define SCALE1T     8192    /* Scaling factor for FFT9_Fx  (/4) */
#define ISCALE1     1       /* Scaling factor for IFFT9_Fx (/2) */

#define COSOFFSET   288

/*_____________________________________________________________________
|                                                                     |
|                       Constant Definitions                          |
|_____________________________________________________________________|
*/

void Fft3(Word16 X[], Word16 Y[], Word16 n)
{
	Word16   Z[N_MAX/6];

	Word16 *Z0, *Z1, *Z2;
	Word16 *z0, *z1, *z2;
	Word16 *x;
	Word16 *yre, *yim, *zre, *zim, *wre, *wim;
	Word16 m = 0, step = 0, sign, order;
	Word16 i, j, k;
	Word16 ns2, ms2, wtmp1, wtmp2, inc, pos, n3;
	Word16 wSizeByTwo = 0, wNumberStage = 0;
	Word16 phs_tbl[voSIZE/2];
	Word16 ii_table[NUM_STAGE];

	Word32 Lyre, Lyim;

	/* Determine the order of the transform, the length of decimated  */
	/* transforms m, and the step for the sine and cosine tables.     */
	switch(n) {
	case 48:
		order = 4;      
		m     = 16;     
		step  = 24;     
		wSizeByTwo = voSIZE/16;         
		wNumberStage = NUM_STAGE-3; 

		break;
	case 96:
		order = 5;      
		m     = 32;     
		step  = 12;     
		wSizeByTwo = voSIZE/8;         
		wNumberStage = NUM_STAGE-2;
		break;
	case 192:
		order = 6;      
		m = 64;         
		step = 6;       
		wSizeByTwo = voSIZE/4;         
		wNumberStage = NUM_STAGE-1;
		break;
	default:
		//AMRWBPLUS_PRINT(" invalid fft3 size!\n");
		//exit(0);
		break;
	}

	/* Compose decimated sequences X[3i], X[3i+1],X[3i+2] */
	/* compute their FFT of length m.                                 */

	Z0 = &Z[0];   z0 = &Z0[0];      
	Z1 = &Z0[m];  z1 = &Z1[0];      /* Z1 = &Z[ m];     */
	Z2 = &Z1[m];  z2 = &Z2[0];      /* Z2 = &Z[2m];     */

	x  =  &X[0];                               

	n3 = mult_r(n,10923);
	for (i = 0; i < n3; i++)
	{
		*z0++ = *x++;            /* Z0[i] = X[3i];   */	
		*z1++ = *x++;            /* Z1[i] = X[3i+1]; */
		*z2++ = *x++;            /* Z2[i] = X[3i+2]; */
	}

	/* multiply by 2/3 */
	for (i = 0; i < n; i++)
	{ 
		Z[i] = mult(Z[i],21845);  
	}
	init_r_fft_fx(m, wSizeByTwo, wNumberStage, ii_table, phs_tbl);

	r_fft_fx(Z0, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl,1);
	r_fft_fx(Z1, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl,1);
	r_fft_fx(Z2, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl,1);

	/* Compute the DC coefficient and store it into Y[0]. Note that   */
	/* the variables Z0, ..., Z8, z0, ..., z8 are not needed after    */
	/* this.                                                          */

	/**Y = *Z0 + *Z1 + *Z2;*/
	Lyre = L_mult(*Z0, 1);
	Lyre = L_mac(Lyre, *Z1, 1);
	Lyre = L_mac(Lyre, *Z2, 1);
	*Y = round16(L_shl(Lyre, SCALE1-1));

	/* Initialize the index table, which points to the sine and       */
	/* cosine tables.                                                 */
	/* Butterflies of order 3. */
	sign = 32767;                /* 1 in Q15*/
	ns2 = shr(n,1);
	ms2 = shr(m,1);

	wtmp1 = sub(m,1);
	wtmp2 = add(ns2,1);
	zre = &Z[1];   zim = &Z[wtmp1];     
	yre = &Y[1];   yim = &Y[wtmp2];     
	inc = 0;                            
	for (i = 0; i < 3; i++) 
	{

		for (j = 1; j < ms2; j++) 
		{
			wre = &zre[0];
			wim = &zim[0];

			inc = add(inc, step);
			pos = 0;      /* ptr*/

			if(sign>0)
			{
				/**yre =       *wre;*/
				/**yim = sign*(*wim);*/
				Lyre = L_mult(*wre,SCALE1T);        /* *wre/4 stored in high part*/
				Lyim = L_mult(*wim,SCALE1T);
				for (k = 1; k < 3; k++) 
				{
					wre  += m;
					wim  += m;
					pos += inc;
					if (pos >= N_MAX) 
					{
						pos = sub(pos,N_MAX);      /* circular addressing*/
					}

					/* *yre +=  (*wre)*t_cos[*ind] + sign*(*wim)*t_sin[*ind];*/
					Lyre = L_mac(Lyre,*wim,t_sinFxS4[pos]);
					Lyre = L_mac(Lyre,*wre,t_sinFxS4[pos+COSOFFSET]);

					/**yim += -(*wre)*t_sin[*ind] + sign*(*wim)*t_cos[*ind];*/
					Lyim = L_msu(Lyim,*wre,t_sinFxS4[pos]);
					Lyim = L_mac(Lyim,*wim,t_sinFxS4[pos+COSOFFSET]);

				}
				zre ++;
				zim --;
			}
			else
			{
				Lyre = L_mult(*wre,SCALE1T);        /* *wre/4 stored in high part*/
				//Lyim = L_negate(L_mult(*wim,SCALE1T));      /*yim = sign*(*wim);*/               
				Lyim = L_mult(*wim,SCALE1T);
				Lyim = L_negate(Lyim);      /*yim = sign*(*wim);*/               
				for (k = 1; k < 3; k++) 
				{
					wre  += m;
					wim  += m;
					pos += inc;
					
					if (sub(pos,N_MAX) >= 0) 
					{
						pos = sub(pos,N_MAX);      /* circular addressing*/
					}

					/*yre +=  (*wre)*t_cos[pos] + sign*(*wim)*t_sin[pos];*/
					Lyre = L_mac(Lyre,*wre,t_sinFxS4[pos+COSOFFSET]);
					Lyre = L_msu(Lyre,*wim,t_sinFxS4[pos]);         

					/*yim += -(*wre)*t_sin[pos] + sign*(*wim)*t_cos[pos];*/
					Lyim = L_msu(Lyim,*wre,t_sinFxS4[pos]);
					Lyim = L_msu(Lyim,*wim,t_sinFxS4[pos+COSOFFSET]);
				}
				zre--;
				zim++;
			}
			*yre++  = round16(Lyre);
			*yim++  = round16(Lyim);
		}

		wre  = &zre[0];         
		/**yre = *wre;*/
		Lyre = L_mult(*wre,SCALE1T);
		Lyim = 0;               move32();
		pos = 0;                
		inc = add(inc, step);

		/*        *yim = 0.0; */
		for (k = 1; k < 3; k++) 
		{
			wre += m; 
			pos += inc;
			
			if (sub(pos,N_MAX) >= 0) 
			{
				pos = sub(pos,N_MAX);      /* circular addressing*/
			}

			/*yre +=  (*wre)*t_cos[pos];*/
			Lyre = L_mac(Lyre,*wre,t_sinFxS4[pos+COSOFFSET]);

			/*            *yim += -(*wre)*t_sin[*ind]; */
			Lyim = L_msu(Lyim,*wre,t_sinFxS4[pos]);

		}
		sign = negate(sign);
		
		if (sign > 0)
		{
			zre++;
			zim--;
		}
		else
		{
			zre--;
			zim++;
		}
		*yre++ = round16(Lyre);
		
		if (sub(i,2)<0)
		{ 
			*yim++ = round16(Lyim);     
		}

	}

	return;
}

void Ifft3(
		   Word16  Y[],
		   Word16  X[],
		   Word16  n
		   )
{
	Word16 i, k, m = 0, step = 0, order = 0, n3;
	Word16 Z[N_MAX/6];

	Word16 *z, *zre, *zim;
	Word16 *z0, *z1, *z2, *x;
	Word16 *yref, *yreb;  
	Word16 *yimf, *yimb;
	Word16 *yre, *yim;
	Word16 *yr0f,*yr1f,*yi0f,*yi1f;

	Word16 inc, pos, wtmp;
	Word16 ns2, ms2, ns2pm;
	Word32 Ltmp, Ltmp1;
	Word16 wSizeByTwo = 0, wNumberStage = 0;
	Word16 phs_tbl[voSIZE/2];
	Word16 ii_table[NUM_STAGE];

	inc = 0;

	/* Determine the order of the transform, the length of decimated  */
	/* transforms m, and the step for the sine and cosine tables.     */  
	switch(n) {
	case 48:
		order = 4;        
		m     = 16;       
		step  = 24;       
		wSizeByTwo = voSIZE/16;          
		wNumberStage = NUM_STAGE-3; 
		break;
	case 96:    
		order = 5;      
		m     = 32;     
		step  = 12;     
		wSizeByTwo = voSIZE/8;         
		wNumberStage = NUM_STAGE-2;
		break;
	case 192:
		order = 6;      
		m = 64;         
		step = 6;       
		wSizeByTwo = voSIZE/4;         
		wNumberStage = NUM_STAGE-1;
		break;
	default:
		//AMRWBPLUS_PRINT(" invalid fft3 size!\n");
		//exit(0);
		break;
	}

	zre = &Z[0];              
	zim = &Z[(m - 1)];       
	yref = &Y[0];             
	ms2 = shr(m,1);           
	ns2 = shr(n,1);
	ns2pm = add(ns2,m);


	/* Compute the inverse butterflies. */
	/* p = 0*/
	/*zre++ = *yr0f++ + 2*(*yr1f++);*/
	Ltmp = L_mult(*yref, 1);
	yref += m;
	Ltmp = L_mac(Ltmp, *yref,2);  
	*zre++ = round16(L_shl(Ltmp, 14));    

	yreb = &Y[(m-1)];                 
	yr0f = &Y[1];                                               
	yr1f = &Y[m+1];                  

	yimb = &Y[ns2pm-1];            
	yi0f = &Y[ns2+1];                                                    
	yi1f = &Y[ns2pm+1];            

	for (i = 1; i < ms2; i++) 
	{
		/*zre++ = *yr0f++ + *yr1f++ + *yr0b--;*/

		Ltmp = L_mult(*yr0f, 1);
		Ltmp = L_mac(Ltmp, *yr1f, 1);
		Ltmp = L_mac(Ltmp, *yreb, 1);
		*zre = round16(L_shl(Ltmp, 14));
		yr0f++;
		yr1f++;
		yreb--;      

		/*zim-- = *yi0f++ + *yi1f++ - *yi0b--;*/
		Ltmp = L_mult(*yi0f, 1);
		Ltmp = L_mac(Ltmp, *yi1f, 1);
		Ltmp = L_msu(Ltmp, *yimb, 1);
		*zim = round16(L_shl(Ltmp,14));   

		yi0f++;
		yi1f++;
		yimb--;      
		zre++; zim--;
	}
	/*zre = 2*(*yr0f) + (*yr1f);*/
	yref = &Y[ms2];                        
	Ltmp = L_mult(*yr0f,2);
	Ltmp = L_mac(Ltmp, *yr1f,1);
	*zre  = round16(L_shl(Ltmp,14));      

	yre = &Y[m];                        
	yim = &Y[ns2pm];                   

	/* p=1,2 */
	for (k = 1; k < 3; k++) 
	{
		inc = add(inc, (N_MAX/3));       
		pos = inc;   

		z  = &Z[shl(k,order)];     
		/*z  = *yr0;                                 ind = &Ind[1];
		*z += *yr1*t_cos[*ind] - *yi1*t_sin[*ind];  ind++;
		*z += *yr1*t_cos[*ind] + *yi1*t_sin[*ind];*/
		Ltmp = L_shl(Y[0], 14);
		Ltmp = L_mac(Ltmp, *yre,t_sinFxS4[pos+COSOFFSET]);
		Ltmp = L_msu(Ltmp, *yim, t_sinFxS4[pos]);

		pos += inc;
		if (pos >= N_MAX)
		{
			pos = sub(pos,N_MAX);          /* circular addressing*/
		}

		Ltmp = L_mac(Ltmp, *yre,t_sinFxS4[pos+COSOFFSET]);
		Ltmp = L_mac(Ltmp, *yim, t_sinFxS4[pos]);        
		*z = round16(L_shl(Ltmp, 1));

		zre  = &z[1];     zim  = &z[m-1]; 
		pos = 0; 

		for (i = 1; i < ms2; i++) 
		{
			pos = add(pos, step);

			if (sub(pos,N_MAX) >=0)
			{
				pos = sub(pos,N_MAX);          /* circular addressing*/
			}      
			yref = &Y[i];                 
			yreb = &Y[sub(m,i)];              
			yimf = &Y[add(ns2,i)];            
			yimb = &Y[sub(ns2pm,i)];           

			/*zre  = *yr0f*t_cos[*ind] - *yi0f*t_sin[*ind];
			*zim  = *yr0f*t_sin[*ind] + *yi0f*t_cos[*ind];  ind++;*/
			Ltmp = L_mult(*yref,t_sinFxS4[pos+COSOFFSET]);
			Ltmp = L_msu(Ltmp,*yimf,t_sinFxS4[pos]);

			Ltmp1 = L_mult(*yref,t_sinFxS4[pos]);
			Ltmp1 = L_mac(Ltmp1,*yimf,t_sinFxS4[pos + COSOFFSET]);

			yref += m;
			yimf += m;
			pos += inc;
			
			if (sub(pos,N_MAX) >=0)
			{
				pos = sub(pos,N_MAX);          /* circular addressing*/
			}      
			/*zre += *yr1f*t_cos[*ind] - *yi1f*t_sin[*ind];
			*zim += *yr1f*t_sin[*ind] + *yi1f*t_cos[*ind];  ind++;*/
			Ltmp = L_mac(Ltmp, *yref,t_sinFxS4[pos+COSOFFSET]);
			Ltmp = L_msu(Ltmp,*yimf,t_sinFxS4[pos]);

			Ltmp1 = L_mac(Ltmp1, *yref,t_sinFxS4[pos]);
			Ltmp1 = L_mac(Ltmp1,*yimf,t_sinFxS4[pos + COSOFFSET]);

			pos += inc;
			
			if (sub(pos,N_MAX) >=0)
			{
				pos = sub(pos,N_MAX);          /* circular addressing*/
			}      

			/*zre += *yr0b*t_cos[*ind] + *yi0b*t_sin[*ind];
			*zim += *yr0b*t_sin[*ind] - *yi0b*t_cos[*ind];*/

			Ltmp = L_mac(Ltmp, *yreb,t_sinFxS4[pos+COSOFFSET]);
			Ltmp = L_mac(Ltmp,*yimb,t_sinFxS4[pos]);

			Ltmp1 = L_mac(Ltmp1, *yreb,t_sinFxS4[pos]);
			Ltmp1 = L_msu(Ltmp1,*yimb,t_sinFxS4[pos + COSOFFSET]);

			pos += inc;
			
			if (sub(pos,N_MAX) >=0)
			{
				pos = sub(pos,N_MAX);          /* circular addressing*/
			}      

			*zre = round16(L_shl(Ltmp,1));        
			*zim = round16(L_shl(Ltmp1,1));       
			zre++;   zim--;
		}

		pos = add(pos,step);
		
		if (sub(pos,N_MAX) >=0)
		{
			pos = sub(pos,N_MAX);          /* circular addressing*/
		}      

		yref = &Y[i];                   
		yreb = &Y[m-i];            
		yimf = &Y[ns2+i];          
		yimb = &Y[ns2pm-i];        

		/*zre  = *yr0f*t_cos[*ind] - *yi0f*t_sin[*ind];   ind++;*/
		Ltmp = L_mult(*yref,t_sinFxS4[pos+COSOFFSET]);
		Ltmp = L_msu(Ltmp, *yimf,t_sinFxS4[pos]);
		yref+=m;
		pos += inc;
		
		if (sub(pos,N_MAX) >=0)
		{
			pos = sub(pos,N_MAX);          /* circular addressing*/
		}      

		/*zre += *yr1f*(t_cos[*ind] - t_sin[*ind]);       ind++;*/
		Ltmp = L_mac(Ltmp, *yref,t_sinFxS4[pos+COSOFFSET]);
		Ltmp = L_msu(Ltmp, *yref,t_sinFxS4[pos]);
		pos += inc;
		
		if (sub(pos,N_MAX) >=0)
		{
			pos = sub(pos,N_MAX);          /* circular addressing*/
		}      

		/*zre += *yr0b*t_cos[*ind] + *yi0b*t_sin[*ind];*/
		Ltmp = L_mac(Ltmp, *yreb, t_sinFxS4[pos+COSOFFSET]);
		Ltmp = L_mac(Ltmp, *yimb, t_sinFxS4[pos]);

		*zre = round16(L_shl(Ltmp,1));      

		/* Update the table step. */
		/*step =(step+3*(1<<(ORDER_MAX-order))); triple the step if we want to use va's cosine table*/
		wtmp = sub(ORDER_MAX,order);
		wtmp = shl(1,wtmp);
		wtmp = add(wtmp, shl(wtmp,1));
		step = add(step,wtmp);

	}


	/* Compute the inverse FFT for all nine blocks. */

	z0 = &Z[0];      

	z1 = &z0[m];   /* z1 = &Z[ m];     */
	z2 = &z1[m];   /* z2 = &Z[2m];     */

	init_r_fft_fx(m, wSizeByTwo, wNumberStage, ii_table, phs_tbl);

	r_fft_fx(&z0[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl,-1);
	r_fft_fx(&z1[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl,-1);
	r_fft_fx(&z2[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl,-1);

	x = X;             
	n3 = mult_r(n,10923);
	for (i = 0; i < n3; i++) 
	{
		*x++ = *z0++;           
		*x++ = *z1++;           
		*x++ = *z2++;           
	}

}

