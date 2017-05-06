#include <float.h>
#include <stdlib.h>
#include <stdio.h>

#include "amr_plus_fx.h"
#include "util_stereo_x_fx.h"
#include "typedef.h"
#include "count.h"
#include "basic_op.h"

Word16 Unpack4bits(Word16 nbits, Word16 *prm, Word16 *ptr);

/*-----------------------------------------------------------------*
* Funtion  init_coder_stereo                                      *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~                                      *
*   ->Initialization of variables for the stereo coder.           *
*-----------------------------------------------------------------*/
void Init_coder_stereo_x(Coder_State_Plus_fx *st)
{
	Init_cod_hi_stereo(st);
	Init_tcx_stereo_encoder(st);
	return;
}
/*-----------------------------------------------------------------*
* Funtion coder_stereo                                            *
* ~~~~~~~~~~~~~~~~~~~~                                            *
*   ->Principle stereo coder routine (working at fs=12.8kHz).     *
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
void Coder_stereo_x(
  Word16 AqLF[],            /* (i)  : Mono frequencies coefficient  */
  Word16 brMode,            /* (i)  : Stereo bit rate index         */
  Word16 param[],			/* (o)	: Encoded parameters            */	
  Word16 fscale,            /* (i)  : Internal Frequency scaling    */
  Coder_State_Plus_fx *st,  /* i/o  : Encoder states	            */
  Word16 wspeech_hi[],	    /* (i)	: Mixed channel, hi	            */
  Word16 wchan_hi[],		/* (i)	: Right channel, hi	            */
  Word16 wspeech_2k[],	    /* (i)	: Mixed channel, lo             */
  Word16 wchan_2k[]		    /* (i)	: Right channel, lo	            */
)		
{	
    Word16 mod[4];
    Word16 i;

    /* initialize code books */
    test();
    if (sub(StereoNbits_FX[brMode],300+4) > 0)
    {
        st->filt_hi_pmsvq = &filt_hi_pmsvq7_fx;     move16();
        st->gain_hi_pmsvq = &gain_hi_pmsvq5_fx;     move16();
    }
    else 
    {
        st->filt_hi_pmsvq = &filt_hi_pmsvq4_fx;     move16();
        st->gain_hi_pmsvq = &gain_hi_pmsvq2_fx;     move16();
    }

    /* encode the high band */
    Cod_hi_stereo(AqLF,param,st, wspeech_hi, wchan_hi);
    /* encode the low band */
    Cod_tcx_stereo(wspeech_2k, wchan_2k, &param[4+NPRM_STEREO_HI_X*NB_DIV], brMode, mod, fscale, st);
    /* transmitt the mode in the parameters buffer*/
    for(i=0;i<4;i++)
    {
        param[i+NPRM_STEREO_HI_X*NB_DIV] = mod[i];    move16();
    }
    return;
}
void Enc_prm_stereo_x(
  Word16 sparam[],          /* (i) : parameters                       */
  Word16 sserial[],         /* (o) : serial bits stream               */
  Word16 nbits_pack,        /* (i) : number of bits per packet of 20ms*/
  Word16 nbits_bwe,	        /* (i) : number of BWE bits per 20ms  */
  Word16 brMode             /* (i)  : Stereo bit rate index         */
)
{
  Word16 k,j;

  Word16 i;
  Word16 mod[NB_DIV];
  Word16 mode,n_pack;
  Word16 hf_bits=0;
  Word16 hiband_mode;
  Word16 nbits,*prm, *ptr;
  Word16 nbits_AVQ[NB_DIV];
  Word16 prm_AVQ[(NBITS_MAX/4)+N_PACK_MAX];

  /*----------------------------------------------------------*
  * Set number of bits used for stereo (per packet of 20 ms) *
  * When stereo is transmitted, the bit ordering is:         *
  * serial: mode (2bits), core, stereo, 2xBWE(2x16bits)      *
    *----------------------------------------------------------*/
    nbits = shr(add(StereoNbits_FX[brMode],shl(nbits_bwe,1)),2);

    hiband_mode = 0;      move16();
    test();
    if (sub(StereoNbits_FX[brMode],300+4) > 0) 
    { 
      hiband_mode = 1;      move16();
    }

    /*----------------------------------------------------------*
    * Encode the high band parameters	 		   *
    *----------------------------------------------------------*/

    move16(); /* prm init */
    for(k=0;k<NB_DIV;k++) 
    {
      prm = sparam + k*NPRM_STEREO_HI_X;        move16();

      ptr = sserial + sub(extract_l(L_shr(L_mult((k+1),nbits_pack),1)), nbits);     move16();
      test();
      if(hiband_mode == 0) 
      {
        Int2bin(prm[0],4, ptr); ptr += 4;			
        Int2bin(prm[1],2, ptr); ptr += 2;			

      }
      else 
      {
        Int2bin(prm[0],4, ptr); ptr += 4 ;		
        Int2bin(prm[1],3, ptr); ptr += 3 ;		
        Int2bin(prm[2],5, ptr); ptr += 5 ;		

      }
    }

    test();
    if(hiband_mode == 0)
    {
      hf_bits = 4+2;      move16();
    }
    else
    {
      hf_bits = 7+5;      move16();
    }


    /*----------------------------------------------------------*
    * Encode the low band parameters							*
    *----------------------------------------------------------*/

    /* fill up the mode */
    for(i=0;i<NB_DIV;i++) 
    {
      mod[i] = sparam[i+NPRM_STEREO_HI_X*NB_DIV];    move16();
    }


    k = 0;                                          move16();
    while (k < NB_DIV)
    {	
      mode = mod[k];                                move16();
      /* set pointer to parameters */
      prm = (sparam +4+NPRM_STEREO_HI_X*NB_DIV)+ (k*NPRM_DIV_TCX_STEREO);   move16();
      test();test();test();test();
      if ((sub(mode,1) == 0) || (mode == 0))
      {
        /* encode 20ms TCX */
        n_pack = 1;             move16();

        nbits_AVQ[0] = sub(shr(sub(StereoNbits_FX[brMode],4),2), add(7+2+7,hf_bits));    move16();
        AVQ_Encmux(n_pack, &prm[2], prm_AVQ, nbits_AVQ, TOT_PRM_20/8);


        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits), hf_bits);      move16();
        *ptr = 0;        move16(); 
        ptr++;
        /* encode the mode */
        Int2bin(mode, 2, ptr);       ptr += 2;
        Int2bin(prm[0], 7, ptr);     ptr += 7;
        Int2bin(prm[1], 7, ptr);     ptr += 7;
        Unpack4bits(nbits_AVQ[0], prm_AVQ, ptr);
        ptr += nbits_AVQ[0];

        k ++;
      } /* end of mode 0/1 */
      else if (sub(mode,2) == 0) 
      {
        /* encode and multiplex 40ms TCX */

        n_pack = 2;       move16();

        nbits_AVQ[0] = sub(shr(sub(StereoNbits_FX[brMode],4),2), add(2+7,hf_bits));      move16();
        nbits_AVQ[1] = nbits_AVQ[0];    move16();
        AVQ_Encmux(n_pack, &prm[2], prm_AVQ, nbits_AVQ, TOT_PRM_40/8);

        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits), hf_bits);       move16();
        *ptr = 0;  ptr++;       move16();


        /* encode first 20 ms frame */
        Int2bin(mode, 2, ptr);     ptr += 2;
        Int2bin(prm[0], 7, ptr);     ptr += 7;
        j= Unpack4bits(nbits_AVQ[0], prm_AVQ, ptr);


        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult((k+2),nbits_pack),1)), nbits), hf_bits);      move16();
        *ptr = 0;  ptr++;       move16();

        /* encode second 20 ms frame */
        Int2bin(mode, 2, ptr);     ptr += 2;
        Int2bin(prm[1], 7, ptr);     ptr += 7;
        Unpack4bits(nbits_AVQ[1], &prm_AVQ[j], ptr);

        k += 2;

      } /* end of mode 2 */
      else if (sub(mode,3) == 0) 
      {
        /* encode and multiplex 80ms TCX */
        n_pack = 4;       move16();

        nbits_AVQ[0] = sub(shr(sub(StereoNbits_FX[brMode],4),2), add(2+7,hf_bits));    move16();
        nbits_AVQ[1] = sub(shr(sub(StereoNbits_FX[brMode],4),2), add(2,hf_bits));      move16();
        nbits_AVQ[2] = nbits_AVQ[0];                                            move16();
        nbits_AVQ[3] = nbits_AVQ[1];                                            move16();

        AVQ_Encmux(n_pack, &prm[2], prm_AVQ, nbits_AVQ, TOT_PRM_80/8);

        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult(add(k,1),nbits_pack),1)), nbits), hf_bits);      move16();
        *ptr = 0;  ptr++;         move16();


        /* encode first 20 ms frame */
        Int2bin(mode, 2, ptr);     ptr += 2;
        Int2bin(prm[0], 7, ptr);     ptr += 7;
        j= Unpack4bits(nbits_AVQ[0], prm_AVQ, ptr);

        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult(add(k,2),nbits_pack),1)), nbits), hf_bits);      move16();
        *ptr = 0;  ptr++;       move16();


        /* encode second 20 ms frame */
        Int2bin(mode, 2, ptr);     ptr += 2;
        j = add(j,Unpack4bits(nbits_AVQ[1], &prm_AVQ[j], ptr));

        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult(add(k,3),nbits_pack),1)), nbits), hf_bits);      move16();
        *ptr = 0;  ptr++;       move16();


        /* encode third 20 ms frame */	
        Int2bin(mode, 2, ptr);     ptr += 2;
        Int2bin(prm[1], 7, ptr);     ptr += 7;
        j = add(j,Unpack4bits(nbits_AVQ[2], &prm_AVQ[j], ptr));

        /* set pointer to bit stream */
        ptr = sserial + add(sub(extract_l(L_shr(L_mult(add(k,4),nbits_pack),1)), nbits), hf_bits);      move16();
        *ptr = 0;  ptr++;      move16();

        /* encode forth 20 ms frame */
        Int2bin(mode, 2, ptr);     ptr += 2;
        Unpack4bits(nbits_AVQ[3], &prm_AVQ[j], ptr);

        k +=4;
      }  /* end of mode 3 */
    } /* end of while k < NB_DIV */
  return;
}
