
#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "amr_plus_fx.h"



void Init_decoder_stereo_x(Decoder_State_Plus_fx *st)
{	
	/* Allocate memory for stereo states */
	Set_zero(st->my_old_synth_2k_fx,L_FDEL_2k + D_STEREO_TCX +  2*(D_NC*5)/32);
	Set_zero(st->my_old_synth_hi_fx,2*D_NC);
	Set_zero(st->my_old_synth_fx,2*L_FDEL+20);
	Set_zero(st->mem_left_2k_fx,2*L_FDEL_2k);
	Set_zero(st->mem_right_2k_fx,2*L_FDEL_2k);
	Set_zero(st->mem_left_hi_fx,L_FDEL);
	Set_zero(st->mem_right_hi_fx,L_FDEL);
	Set_zero(st->left.mem_d_tcx_fx,D_NC + (D_STEREO_TCX*32/5));
	Set_zero(st->right.mem_d_tcx_fx,D_NC + (D_STEREO_TCX*32/5));

	Init_dec_hi_stereo(st);

	Set_zero(st->right.wmem_d_nonc,D_NC);
	Set_zero(st->left.wmem_d_nonc,D_NC);

	Init_tcx_stereo_decoder(st);

	st->last_stereo_mode =0;                            
	st->side_rms_fx = 0;                                


	return;
}

/*-----------------------------------------------------------------*
* Funtion decoder_stereo                                          *
* ~~~~~~~~~~~~~~~~~~~~~~                                          *
*   ->Principle stereo decoder routine (working at fs=12.8kHz).   *
*                                                                 *
* Note: HF band are encoded twice (2 channels) using 0.8kbps BWE. *
*       Usage of 2xBWE for stereo provide better time domain      *
*       stereo definition in HF without increasing the bit-rate.  *
*       Another advantage is that the stereo decoder is limited   *
*       to the lower band (fs=12.8kHz) and this reduce the        *
*       overall complexity of the AMR-WB+ codec.  Also, this      *
*       solution is not dependent of the AMR-WB+ mode where many  *
*       different sampling frequencies are used (16, 24, 32 kHz). *
*-----------------------------------------------------------------*/

void Decoder_stereo_x(
					  Word16 param[],       /* (i)  : Codebooks indices		    Q0  */
					  Word16 bad_frame[],   /* (i)  : Bad frame index			    Q0  */
					  Word16 sig_left_[],   /* (o)  : Decoded left channel	  Qsyn ( <= 14 bits)     */
					  Word16 synth_[],      /* (o)  : Decoded right channel	  Qsyn ( <= 14 bits)     */
					  Word16 Az[],          /* (i)  : coefficients NxAz[M+1]  Q12 ( 1.0 == 4096.0)  */
					  Word16 StbrMode,      /* (i)  : Bit rate mode (stereo)  Q0  */              
					  Word16 fscale,        /* (i)  : fscale mode          Q0  */
					  Decoder_State_Plus_fx *st     /* (i/o): Decoder states			*/		
					  )		
{	
	Word16 *my_synth_buf_   = sig_left_; /* Use sig_left as a temp-buffer*/
	Word16 *my_new_synth_   = my_synth_buf_+2*L_FDEL;
	Word16 my_old_synth_2k_[L_FRAME_2k + D_STEREO_TCX + L_FDEL_2k + 2*(D_NC*5)/32];
	Word16 *my_new_synth_2k_= my_old_synth_2k_ + D_STEREO_TCX + L_FDEL_2k + 2*(D_NC*5)/32;
	Word16 *my_old_synth_hi_= synth_; /* Use synth as a temp-buffer*/
	Word16 *my_new_synth_hi_= my_old_synth_hi_+2*D_NC;
	Word16 *my_synth_hi_t0_ = my_old_synth_hi_+2*D_NC+L_BSP+D_BPF; 
	Word16 old_right_2k_[L_FRAME_2k+2*L_FDEL_2k];
	Word16 *new_right_2k_   = old_right_2k_ + 2*L_FDEL_2k;
	Word16 old_left_2k_[L_FRAME_2k+2*L_FDEL_2k];
	Word16 *new_left_2k_  = old_left_2k_ + 2*L_FDEL_2k;

	/* set buffers */

	voAMRWBPDecCopy(st->my_old_synth_fx, my_synth_buf_, 2*L_FDEL+20);															
	voAMRWBPDecCopy(synth_, my_new_synth_+20, L_FRAME_PLUS);																				
	voAMRWBPDecCopy(st->my_old_synth_2k_fx, my_old_synth_2k_, D_STEREO_TCX + L_FDEL_2k + 2*(D_NC*5)/32);						
	voAMRWBPDecCopy(st->my_old_synth_hi_fx, my_old_synth_hi_, 2*D_NC);															
	voAMRWBPDecCopy(st->mem_left_2k_fx, old_left_2k_, 2*L_FDEL_2k);																
	voAMRWBPDecCopy(st->mem_right_2k_fx, old_right_2k_, 2*L_FDEL_2k);																


	/* mono synth band-split*/	
	/* do the lo,hi band-splitting on the mono signal */
	Band_split_taligned_2k(my_new_synth_-2*L_FDEL,my_new_synth_2k_-2*L_FDEL_2k,my_new_synth_hi_,L_FRAME_PLUS);									

	/* Low band */
	if (StbrMode < 0)
	{
		Word16 i;

		for (i=0; i<L_OVLP_2k; i++)
		{
			new_left_2k_[i] = add(my_old_synth_2k_[i], st->mem_stereo_ovlp_fx[i]);     
			new_right_2k_[i] = sub(my_old_synth_2k_[i], st->mem_stereo_ovlp_fx[i]);    
		}
		for (i=L_OVLP_2k; i<L_FRAME_2k; i++)
		{
			new_right_2k_[i] = my_old_synth_2k_[i];
			new_left_2k_[i] = new_right_2k_[i];    
		}

		Init_tcx_stereo_decoder(st);   
		st->mem_stereo_ovlp_size_fx = L_OVLP_2k;   
		st->last_stereo_mode = 0;                 
		st->side_rms_fx = 0;                        
	}
	else
	{
		Dec_tcx_stereo(my_old_synth_2k_,new_left_2k_,new_right_2k_,param+NPRM_STEREO_HI_X*NB_DIV,bad_frame,st);
	}
	/* High band */
	{
		/* Extended with HI_FILT_ORDER in order to use it in dec_hi_stereo */
		Word16 old_right_hi_[L_FRAME_PLUS+L_FDEL+HI_FILT_ORDER]; 
		Word16 *new_right_hi_ = old_right_hi_ + L_FDEL;
		Word16 old_left_hi_[L_FRAME_PLUS+L_FDEL];
		Word16 *new_left_hi_ = old_left_hi_ + L_FDEL;

		/*initialize code books */
		if (StereoNbits_FX[StbrMode] > 304)
		{
			st->Filt_hi_pmsvq_fx = &filt_hi_pmsvq7_fx; 
			st->Gain_hi_pmsvq_fx = &gain_hi_pmsvq5_fx; 
		}
		else 
		{
			st->Filt_hi_pmsvq_fx = &filt_hi_pmsvq4_fx; 
			st->Gain_hi_pmsvq_fx = &gain_hi_pmsvq2_fx; 
		}


		voAMRWBPDecCopy(st->mem_left_hi_fx,old_left_hi_,L_FDEL);																		
		voAMRWBPDecCopy(st->mem_right_hi_fx,old_right_hi_,L_FDEL);																

		if (StbrMode < 0)
		{
			Word16 i;
			for (i=0; i<L_FRAME_PLUS; i++)
			{
				new_right_hi_[i] = my_synth_hi_t0_[i-L_BSP-D_BPF];    
				new_left_hi_[i] = new_right_hi_[i];                   
			}
			Init_dec_hi_stereo(st);
		}
		else
		{
			Dec_hi_stereo(my_synth_hi_t0_,new_right_hi_,new_left_hi_,Az,param,bad_frame,fscale,st);
		}
		/* delay of new_right_hi and new_left_hi is -D_BPF-L_BSP */
		/* we need to delay them by D_NC */
		Delay(new_right_hi_,L_FRAME_PLUS,D_NC,st->right.wmem_d_nonc);																	
		Delay(new_left_hi_,L_FRAME_PLUS,D_NC,st->left.wmem_d_nonc);																

		/*left_hi and right_hi are time aligned here */

		/* synthesis is delayed, so delay the left_hi and right_hi */
		Delay(new_right_hi_,L_FRAME_PLUS,D_NC+(D_STEREO_TCX*32/5),st->right.mem_d_tcx_fx);											
		Delay(new_left_hi_,L_FRAME_PLUS,D_NC+(D_STEREO_TCX*32/5),st->left.mem_d_tcx_fx);											

		/* the whole stereo is delayed by D_STEREO_TCX */

		/* Join low and high frequency band*/
		/* Left channel */
		voAMRWBPDecCopy(&my_synth_buf_[L_FRAME_PLUS],st->my_old_synth_fx,2*L_FDEL+20);														
		Band_join_2k(sig_left_,new_left_2k_, new_left_hi_, L_FRAME_PLUS);														

		/* Right channel */
		voAMRWBPDecCopy(&my_old_synth_hi_[L_FRAME_PLUS],st->my_old_synth_hi_fx,2*D_NC);													
		Band_join_2k(synth_,new_right_2k_, new_right_hi_,L_FRAME_PLUS);														


		voAMRWBPDecCopy(&old_left_hi_[L_FRAME_PLUS],st->mem_left_hi_fx,L_FDEL);															
		voAMRWBPDecCopy(&old_right_hi_[L_FRAME_PLUS],st->mem_right_hi_fx,L_FDEL);															

	}

	voAMRWBPDecCopy(&my_old_synth_2k_[L_FRAME_2k],st->my_old_synth_2k_fx,L_FDEL_2k + D_STEREO_TCX+2*(D_NC*5)/32);					
	voAMRWBPDecCopy(&old_left_2k_[L_FRAME_2k],st->mem_left_2k_fx,2*L_FDEL_2k);													
	voAMRWBPDecCopy(&old_right_2k_[L_FRAME_2k],st->mem_right_2k_fx,2*L_FDEL_2k);													


	return;
}

static Word16  Pack4bits_d(Word16  nbits, Word16 *ptr, Word16  *prm)
{
	Word16  i = 0;
					
	while(nbits > 4)
	{	
		prm[i] = Bin2int(4, ptr);
		ptr += 4;																								
		nbits = nbits - 4;
		i++;
	}
	prm[i] = Bin2int(nbits, ptr);
	i++;	
	return(i);
}



void Dec_prm_stereo_x(
					  Word16 bad_frame[],   /* (i) : bfi for 4 frames (bad_frame[4])  */
					  Word16 serial[],      /* (i) : serial bits stream               */
					  Word16  nbits_pack,   /* (i) : number of bits per packet of 20ms*/
					  Word16  nbits_bwe,    /* (i) : number of BWE bits per 20ms  */
					  Word16  param[],      /* (o) : decoded parameters               */
					  Word16  brMode,
					  Decoder_State_Plus_fx* st)

{
	Word16 nbits, *prm;
	Word16 k,mod_buf[1+NB_DIV];
	Word16 *mod;
	Word16 nbits_AVQ[NB_DIV];
	Word16 prm_AVQ[NBITS_MAX+N_PACK_MAX];
	Word16 *ptr;
	Word16 j,n_pack;
	Word16 hf_bits=6;
	Word16 hiband_mode;


	/*----------------------------------------------------------*
	* Set number of bits used for stereo (per packet of 20 ms) *
	* When stereo is transmitted, the bit ordering is:         *
	* serial: mode (2bits), core, stereo, 2xBWE(2x16bits)      *
	*----------------------------------------------------------*/		
	nbits = (StereoNbits_FX[brMode] + (nbits_bwe << 1)) >> 2;

	hiband_mode = 0;

	if(StereoNbits_FX[brMode] > 304)
	{
		hiband_mode = 1;
	}

	mod = &mod_buf[1];
	mod[-1] = st->last_stereo_mode;         /* previous mode */																


	/*----------------------------------------------------------*
	* decode the high band parameters							*
	*----------------------------------------------------------*/												

	for(k=0;k<NB_DIV;k++) 
	{
		prm = param + k*NPRM_STEREO_HI_X;																		
		ptr = serial + (k+1)*nbits_pack - nbits;					

		if(hiband_mode == 0) 
		{
			prm[0] = Bin2int(4, ptr); ptr += 4;	
			prm[1] = Bin2int(2, ptr); ptr += 2;	
		}
		else
		{
			prm[0] = Bin2int(4, ptr); ptr += 4;	
			prm[1] = Bin2int(3, ptr); ptr += 3;	
			prm[2] = Bin2int(5, ptr); ptr += 5; 
		}
	}
	if(hiband_mode == 0) 
	{
		hf_bits = 4+2; 
	}	
	else
	{
		hf_bits = 7+5; 
	}

	/*----------------------------------------------------------*
	* decode the low band parameters							*
	*----------------------------------------------------------*/

	/* decode the mode */
	sub(hf_bits, nbits); /* initialize ptr */
	for(k=0;k<NB_DIV;k++) 
	{
		ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +1; /* 1 reserved bit*/
		if(bad_frame[k] == 0) 
		{
			mod[k] = Bin2int(2, ptr); 
		}
		else
		{
			mod[k] = -1;   
		}
	}

	/* extrapolate modes in lost packets */						
	if ((mod[0] == 3)||(mod[1] == 3)||(mod[2] == 3)||(mod[3] == 3))
	{																											
		for (k=0; k<NB_DIV; k++) 
		{
			mod[k] = 3;   /* partial loss on tcx 80 ms*/
		}
	}
	else
	{ 																		
	if ((mod[0] == 2)||(mod[1] == 2)) 
	{																		
		for (k=0; k<2; k++) 
		{																								
			mod[k] = 2;      
		}
	}																							
	if ((mod[2] == 2)||(mod[3] == 2)) 
	{
		for (k=2; k<4; k++) 
		{																							
			mod[k] = 2;       /* partial loss on tcx 40 ms	*/
		}
	}																									
	for (k=0; k<NB_DIV; k++) 
	{															
	if (mod[k] < 0)
	{	  																			
	if (mod[k-1] == 0) 
	{																							
		mod[k] = 0;     /* stay tcx 20 ms */
	}
	else 
	{																									
		mod[k] = 1;   /* otherwise it's tcx 20ms */
	}
	}
	}					
	}																											
	for( k = 0;k<NB_DIV;k++) 
	{
		param[NPRM_STEREO_HI_X*NB_DIV+k]=mod[k];													
	}

	k = 0;  

	while(k < NB_DIV)
	{
		/* set pointer to parameters */
		/*prm = (param +4+NPRM_STEREO_HI_X*NB_DIV)+ (k*NPRM_DIV_TCX_STEREO);									*/
		prm = param + add(4+NPRM_STEREO_HI_X*NB_DIV,extract_l(L_mult(k,NPRM_DIV_TCX_STEREO/2)));

		if ((mod[k] == 1)|| (mod[k]==0))
		{
			n_pack =1;																						
			nbits_AVQ[0] = (((StereoNbits_FX[brMode] - 4) >> 2) - (16 + hf_bits));											

			/* decode 20ms TCX */
			/* ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +2+1; */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit */						

			prm[0] = Bin2int(7, ptr); ptr += 7;   
			prm[1] = Bin2int(7, ptr); ptr += 7;   

			Pack4bits_d(nbits_AVQ[0], ptr, prm_AVQ);																

			/* demultiplex and decode */
			AVQ_Demuxdec(n_pack,prm_AVQ, nbits_AVQ, &prm[2], TOT_PRM_20/8, &bad_frame[k]);					

			/* convert to integer */

			k++;
		} /* end of mode 0/1 */
		else if(mod[k] == 2) 
		{
			/* decode and demultiplex a 40 ms frame */
			n_pack = 2;																						
			nbits_AVQ[0] = (((StereoNbits_FX[brMode] - 4) >> 2) - (9 + hf_bits));   
			nbits_AVQ[1] = (((StereoNbits_FX[brMode] - 4) >> 2) - (9 + hf_bits));   

			/* decode first 20ms packet */
			/* ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +2+1; */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit*/						

			prm[0] = Bin2int(7, ptr);    ptr += 7; 

			j = Pack4bits_d(nbits_AVQ[0], ptr, prm_AVQ);													

			/* decode second 20ms packet */
			/* ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +2+1; */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,2),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit*/						


			prm[1] = Bin2int(7, ptr);    ptr += 7; 
			Pack4bits_d(nbits_AVQ[1], ptr, &prm_AVQ[j]);

			/* demultiplex and decode tcx parameters */

			AVQ_Demuxdec(n_pack,prm_AVQ, nbits_AVQ, &prm[2], TOT_PRM_40/8, &bad_frame[k]);

			k+=2;	
		} /* end of mode 2 */
		else if(mod[k] == 3) 
		{
			/* encode and multiplex 80ms TCX */
			n_pack = 4;																				
			nbits_AVQ[0] = (((StereoNbits_FX[brMode] - 4) >> 2) - (9 + hf_bits));   
			nbits_AVQ[1] = (((StereoNbits_FX[brMode] - 4) >> 2) - (2 + hf_bits));      
			nbits_AVQ[2] = (((StereoNbits_FX[brMode] -4) >> 2) - (9 + hf_bits));    
			nbits_AVQ[3] = (((StereoNbits_FX[brMode] -4) >> 2) - (2 + hf_bits));      

			/* set pointer to bit stream */
			/* ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +2+1; */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit*/						

			/* decode first 20 ms frame */
			prm[0] = Bin2int(7, ptr);    ptr += 7;        
			j = Pack4bits_d(nbits_AVQ[0], ptr, prm_AVQ);

			/* set pointer to bit stream */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,2),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit*/						

			/* decode second 20 ms frame */
			j += Pack4bits_d(nbits_AVQ[1], ptr, &prm_AVQ[j]);   

			/* set pointer to bit stream */
			/* ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +2+1; */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,3),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit*/						

			/* decode third 20 ms frame */
			prm[1] = Bin2int(7, ptr);    ptr += 7;  
			j += Pack4bits_d(nbits_AVQ[2], ptr, &prm_AVQ[j]);   


			/* set pointer to bit stream */
			/* ptr = serial + (k+1)*nbits_pack - nbits + hf_bits +2+1; */
			ptr = serial + add(add(sub(extract_l(L_shr(L_mult(add(k,4),nbits_pack),1)) , nbits) , hf_bits), 2+1); /* +2 for the mode + 1 reserved bit*/						


			/* decode forth 20 ms frame */
			j += Pack4bits_d(nbits_AVQ[3], ptr, &prm_AVQ[j]);

			/* demultiplex and decode tcx parameters */
			AVQ_Demuxdec(n_pack,prm_AVQ, nbits_AVQ, &prm[2], TOT_PRM_80/8, bad_frame);

			k+=4;																									
		}
	}	

	return;
}


