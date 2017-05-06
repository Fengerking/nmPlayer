#include "amr_plus_fx.h"

#define MAX_VECT_DIM	16


#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"
#include "log2.h"



void Init_dec_hi_stereo(Decoder_State_Plus_fx *st)
{
  /* initializations for the high encoder*/
  int i;
  
  Cos_window(st->W_window,0,L_SUBFR, 2);
  for(i=0;i<L_SUBFR;i++) 
  {
    st->W_window[i] = mult_r(st->W_window[i],st->W_window[i]);	
  }
  
  Set_zero(st->old_wh_fx,HI_FILT_ORDER);
  Set_zero(st->old_wh2_fx,HI_FILT_ORDER);
  Set_zero(st->old_exc_mono_fx,HI_FILT_ORDER);
  /* init LPC filters	*/
  Set_zero(st->old_AqLF_fx,5*(M+1));

  for(i=0;i<5;i++)
  {
    st->old_AqLF_fx[i*(M+1)]=4096; 
  }

  Set_zero(st->left.mem_synth_hi_fx,M);
  Set_zero(st->right.mem_synth_hi_fx,M);
  Set_zero(st->old_wh_q_fx,HI_FILT_ORDER);
  Set_zero(st->old_gm_gain_fx,2);

  for(i=0;i<4;i++)
  {
    st->old_gain_left_fx[i]=8192; /*Q13*/
    st->old_gain_right_fx[i]=8192; 
  }

}
/*Word16 w_window_fx[L_SUBFR];*/

void Dec_hi_stereo(
  Word16 synth_hi_t0[],   /* Q0 */
  Word16 right_hi[],      /* Qsyn */
  Word16 left_hi[],       /* Qsyn */ 
  Word16 AqLF[],          /* Q12  */
  Word16   param[],
  Word16 bad_frame[],
  Word16 fscale,
  Decoder_State_Plus_fx *st
  )
{
  Word16 *exc_buf = right_hi;  /* Use right_hi as a temp_buffer */
  Word16 *exc_mono=exc_buf+HI_FILT_ORDER;
  Word16 *synth_hi_d;

  Word16 *p_Aq,*p_h;
  Word16 i,k;
  Word16 i_subfr;
  Word16 start;

  Word16 gain_left[NB_DIV+2];
  Word16 gain_right[NB_DIV+2];

  Word16 new_gain_left[NB_DIV];
  Word16 new_gain_right[NB_DIV];

  Word16 *left_exc, *right_exc;

  Word16 side_buf[L_DIV+L_SUBFR];
  Word16 buf[L_SUBFR];

  Word16 wh[NB_DIV*HI_FILT_ORDER];
  Word16 *prm;


  /* initialization of exc_buf and exc_mono */

  left_exc = left_hi; 
  right_exc = right_hi; 

  /*---------------------------------------------------------------------*
   Mismatch between received parameters and signal buffer               
                                                                        
                                                                        
                                                                        
    fscale=0
                                                                        
                    |    20ms   |    20ms   |     20ms  |   20ms    |   
          -|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|   
          <--------  Total speech buffer ---------------->			   
                                                                        
    fscale!=0                                                       
                                                                        
                    |    20ms   |    20ms   |     20ms  |   20ms    |   
       -|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|   
       <--------  Total speech buffer ---------------->                 
   g(-2)    g(-1)       g(0)         g(1)        g(2)        g(3)       
   f(-2)    f(-1)       f(0)         f(1)        f(2)        f(3)       
  ---------------------------------------------------------------------*/ 
  /*   Decode gain and filters               
  ------------------------------------------------------------*/

  /* decoding loop initilize prm */  
  for(i=0;i<NB_DIV;i++)
  {

    prm = param + i*NPRM_STEREO_HI_X;
    
    /* decode the filters */
    Dec_filt(prm,&wh[i*HI_FILT_ORDER],st->old_wh_q_fx, bad_frame[i],st->Filt_hi_pmsvq_fx);
    prm += st->Filt_hi_pmsvq_fx->msvq.nstages;

    /* decode the gains */
    Dec_gain(prm,&new_gain_left[i],&new_gain_right[i],st->old_gm_gain_fx, bad_frame[i],st->Gain_hi_pmsvq_fx);
    prm += st->Gain_hi_pmsvq_fx->msvq.nstages;
    /* Input: Q10;  output should be Q13 not Q14, and saturate */
    new_gain_left[i] = Comp_hi_gain(new_gain_left[i]);      
    new_gain_right[i] = Comp_hi_gain(new_gain_right[i]);    
  }
  /*-----------------------------------------------------------*
     Set correct alignement of parameters
   ------------------------------------------------------------*/

  voAMRWBPDecCopy(st->old_gain_left_fx,gain_left,2);
  voAMRWBPDecCopy(st->old_gain_right_fx,gain_right,2);

  voAMRWBPDecCopy(new_gain_left,gain_left+2,4);
  voAMRWBPDecCopy(new_gain_right,gain_right+2,4);

  voAMRWBPDecCopy(new_gain_left+2,st->old_gain_left_fx,2);
  voAMRWBPDecCopy(new_gain_right+2,st->old_gain_right_fx,2);
  /*-----------------------------------------------------------
     Set correct starting buffer
   -----------------------------------------------------------*/
  synth_hi_d = &synth_hi_t0[-L_BSP-D_BPF]; 
  /*-----------------------------------------------------------*
    Compute left right pseudo-excitation
    Use the mono LPC filter for whitening
   ------------------------------------------------------------*/

  p_Aq = st->old_AqLF_fx; 

  /* first 1/2 subframe */
  voAMRWBPDecResidu(p_Aq,M, synth_hi_d, exc_mono, L_SUBFR/2);

  p_Aq += (M+1);

  /* the 3/4 ramaining subframes */
  for(i_subfr=L_SUBFR/2;i_subfr<3*L_SUBFR+ L_SUBFR/2;i_subfr+=L_SUBFR)
  {
    voAMRWBPDecResidu(p_Aq,M, &synth_hi_d[i_subfr], &exc_mono[i_subfr], L_SUBFR);
    p_Aq += (M+1);
  }
  
  if (fscale != 0) 
  {
    voAMRWBPDecResidu(p_Aq,M, &synth_hi_d[i_subfr], &exc_mono[i_subfr], L_SUBFR); /* 5 AqLF to be saved */
  }

  /* do the rest*/
  p_Aq = AqLF; 
  if(fscale==0) 
  {
    start = 3*L_SUBFR + L_SUBFR/2; 
  }
  else
  {
    start = 4*L_SUBFR + L_SUBFR/2; 
  }
  /* i boundaries check */
  for (i_subfr=start; i_subfr <L_FRAME_PLUS-L_SUBFR/2; i_subfr+=L_SUBFR)
  {
    voAMRWBPDecResidu(p_Aq,M, &synth_hi_d[i_subfr], &exc_mono[i_subfr], L_SUBFR);
    p_Aq += (M+1);
  }

  /* the last half subframe */
  voAMRWBPDecResidu(p_Aq,M, &synth_hi_d[L_FRAME_PLUS-L_SUBFR/2], &exc_mono[L_FRAME_PLUS-L_SUBFR/2], L_SUBFR/2);

  /*-----------------------------------------------------------*
    prepare excitation buffer for filtering	   
   ------------------------------------------------------------*/

  /* fill up the fir filter state past */
  voAMRWBPDecCopy(st->old_exc_mono_fx,exc_buf,HI_FILT_ORDER);
  /* save for next frame  */
  voAMRWBPDecCopy(exc_buf+L_FRAME_PLUS,st->old_exc_mono_fx,HI_FILT_ORDER);

  /*-----------------------------------------------------------*
    synthesise left right pseudo excitation with interpolation 
   ------------------------------------------------------------*/
  if(fscale ==0 ) 
  {
    /*  first the 3.5 subframes */
    Fir_filt(st->old_wh_fx,HI_FILT_ORDER,exc_mono,side_buf,L_DIV-L_SUBFR/2);
    Fir_filt(st->old_wh2_fx,HI_FILT_ORDER,exc_mono,buf,L_SUBFR/2);

    /* window and overlap for the first half subframe */

    Get_exc_win(left_exc, buf,exc_mono,side_buf,&st->W_window[L_SUBFR/2],gain_left, L_SUBFR/2,1);
    Get_exc_win(right_exc,buf,exc_mono,side_buf,&st->W_window[L_SUBFR/2],gain_right,L_SUBFR/2,0);

    /* the 3 subframes */
    Get_exc(&left_exc[L_SUBFR/2],&exc_mono[L_SUBFR/2],&side_buf[L_SUBFR/2],gain_left[1],L_DIV-L_SUBFR,1);
    Get_exc(&right_exc[L_SUBFR/2],&exc_mono[L_SUBFR/2],&side_buf[L_SUBFR/2],gain_right[1],L_DIV-L_SUBFR,0);

    /* compute forward overlap */
    Fir_filt(st->old_wh_fx,HI_FILT_ORDER,&exc_mono[L_DIV-L_SUBFR/2],buf,L_SUBFR);

    /*  then the three L_DIV frames */
    p_h = wh;   
    k = 1;    
    for(i_subfr=L_DIV-L_SUBFR/2;i_subfr<L_FRAME_PLUS-L_SUBFR/2;i_subfr+=L_DIV)
    {
      Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[i_subfr],side_buf,L_DIV);

      /* window and overlap */
      Get_exc_win(&left_exc[i_subfr], buf,&exc_mono[i_subfr],side_buf,st->W_window,&gain_left[k],L_SUBFR,1);
      Get_exc_win(&right_exc[i_subfr],buf,&exc_mono[i_subfr],side_buf,st->W_window,&gain_right[k],L_SUBFR,0);

      /* fill up the three subframes */
      /* compute forward overlap */
      k++;
      Get_exc(&left_exc[i_subfr+L_SUBFR],&exc_mono[i_subfr+L_SUBFR],&side_buf[L_SUBFR],gain_left[k],L_DIV-L_SUBFR,1);
      Get_exc(&right_exc[i_subfr+L_SUBFR],&exc_mono[i_subfr+L_SUBFR],&side_buf[L_SUBFR],gain_right[k],L_DIV-L_SUBFR,0);
      
      if(sub(add(i_subfr, L_DIV + L_SUBFR),L_FRAME_PLUS-L_SUBFR/2) < 0)
      {
        Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[i_subfr+L_DIV],buf,L_SUBFR);
      }
      else 
      {
        Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[i_subfr+L_DIV],buf,L_SUBFR/2);
      }
      p_h += HI_FILT_ORDER;
    }
    /* the last half subframe  */
    Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[L_FRAME_PLUS-L_SUBFR/2],side_buf,L_SUBFR/2);								
    Get_exc_win(&left_exc[L_FRAME_PLUS-L_SUBFR/2], buf,&exc_mono[L_FRAME_PLUS-L_SUBFR/2],side_buf,st->W_window,&gain_left[k],L_SUBFR/2,1);
    Get_exc_win(&right_exc[L_FRAME_PLUS-L_SUBFR/2], buf,&exc_mono[L_FRAME_PLUS-L_SUBFR/2],side_buf,st->W_window,&gain_right[k],L_SUBFR/2,0);
  }
  else 
  {
    /* 1/2 subframe first */
    Fir_filt(st->old_wh2_fx,HI_FILT_ORDER,exc_mono,buf,L_SUBFR/2);
    
    Get_exc(&left_exc[0],&exc_mono[0],&buf[0],gain_left[0],L_SUBFR/2,1);
    Get_exc(&right_exc[0],&exc_mono[0],&buf[0],gain_right[0],L_SUBFR/2,0);

    /* compute 1st L_DIV  */
    Fir_filt(st->old_wh2_fx,HI_FILT_ORDER,exc_mono +L_SUBFR/2,buf,L_SUBFR);
    Fir_filt(st->old_wh_fx,HI_FILT_ORDER, exc_mono + L_SUBFR/2,side_buf,L_DIV);

    Get_exc_win(&left_exc[L_SUBFR/2], buf,&exc_mono[L_SUBFR/2],side_buf,st->W_window,&gain_left[0],L_SUBFR,1);
    Get_exc_win(&right_exc[L_SUBFR/2], buf,&exc_mono[L_SUBFR/2],side_buf,st->W_window,&gain_right[0],L_SUBFR,0);

    Get_exc(&left_exc[L_SUBFR+L_SUBFR/2],&exc_mono[L_SUBFR+L_SUBFR/2],&side_buf[L_SUBFR],gain_left[1],L_DIV-L_SUBFR,1);
    Get_exc(&right_exc[L_SUBFR+L_SUBFR/2],&exc_mono[L_SUBFR+L_SUBFR/2],&side_buf[L_SUBFR],gain_right[1],L_DIV-L_SUBFR,0);

    /* compute forward overlap  */
    Fir_filt(st->old_wh_fx,HI_FILT_ORDER,&exc_mono[L_DIV+L_SUBFR/2],buf,L_SUBFR);
    /* then the  remaining LDIV's */
    p_h = wh;       
    k = 1;          

    Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[L_DIV+L_SUBFR/2],side_buf,L_DIV);
    /* window and overlap  */
    
    Get_exc_win(&left_exc[L_DIV+L_SUBFR/2], buf,&exc_mono[L_DIV+L_SUBFR/2],side_buf,st->W_window,&gain_left[k],L_SUBFR,1);
    Get_exc_win(&right_exc[L_DIV+L_SUBFR/2], buf,&exc_mono[L_DIV+L_SUBFR/2],side_buf,st->W_window,&gain_right[k],L_SUBFR,0);

    /* fill up the three subframes */
    Get_exc(&left_exc[L_DIV+L_SUBFR/2+L_SUBFR],&exc_mono[L_DIV+L_SUBFR/2+L_SUBFR],&side_buf[L_SUBFR],gain_left[k+1],L_DIV-L_SUBFR,1);
    Get_exc(&right_exc[L_DIV+L_SUBFR/2+L_SUBFR],&exc_mono[L_DIV+L_SUBFR/2+L_SUBFR],&side_buf[L_SUBFR],gain_right[k+1],L_DIV-L_SUBFR,0);

    /* compute forward overlap */
    Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[(2*L_DIV)+(L_SUBFR/2)],buf,L_SUBFR);
    p_h += HI_FILT_ORDER;
    k++;

    Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[(2*L_DIV)+L_SUBFR/2],side_buf,L_DIV);
    
    /* window and overlap  */
    Get_exc_win(&left_exc[2*L_DIV+L_SUBFR/2], buf,&exc_mono[2*L_DIV+L_SUBFR/2],side_buf,st->W_window,&gain_left[k],L_SUBFR,1);
    Get_exc_win(&right_exc[2*L_DIV+L_SUBFR/2], buf,&exc_mono[2*L_DIV+L_SUBFR/2],side_buf,st->W_window,&gain_right[k],L_SUBFR,0);

    /* fill up the three subframes */
    Get_exc(&left_exc[2*L_DIV+L_SUBFR/2+L_SUBFR],&exc_mono[2*L_DIV+L_SUBFR/2+L_SUBFR],&side_buf[L_SUBFR],gain_left[k+1],L_DIV-L_SUBFR,1);
    Get_exc(&right_exc[2*L_DIV+L_SUBFR/2+L_SUBFR],&exc_mono[2*L_DIV+L_SUBFR/2+L_SUBFR],&side_buf[L_SUBFR],gain_right[k+1],L_DIV-L_SUBFR,0);

    /*  compute forward overlap */
    Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[(3*L_DIV)+(L_SUBFR/2)],buf,L_SUBFR);
    p_h += HI_FILT_ORDER;
    k++;
    Fir_filt(p_h,HI_FILT_ORDER,&exc_mono[(3*L_DIV)+L_SUBFR/2],side_buf,L_DIV-L_SUBFR/2);

    /* window and overlap  */
    Get_exc_win(&left_exc[3*L_DIV+L_SUBFR/2], buf,&exc_mono[3*L_DIV+L_SUBFR/2],side_buf,st->W_window,&gain_left[k],L_SUBFR,1);
    Get_exc_win(&right_exc[3*L_DIV+L_SUBFR/2], buf,&exc_mono[3*L_DIV+L_SUBFR/2],side_buf,st->W_window,&gain_right[k],L_SUBFR,0);
    
    /* fill up the three subframes - 1/2 subframe */
    Get_exc(&left_exc[3*L_DIV+L_SUBFR/2+L_SUBFR],&exc_mono[3*L_DIV+L_SUBFR/2+L_SUBFR],&side_buf[L_SUBFR],gain_left[k+1],L_DIV-L_SUBFR-L_SUBFR/2,1);
    Get_exc(&right_exc[3*L_DIV+L_SUBFR/2+L_SUBFR],&exc_mono[3*L_DIV+L_SUBFR/2+L_SUBFR],&side_buf[L_SUBFR],gain_right[k+1],L_DIV-L_SUBFR-L_SUBFR/2,0);
    p_h += HI_FILT_ORDER;
  }

  /*-----------------------------------------------------------*
    Save filters in memory for next frame
   ------------------------------------------------------------*/
  voAMRWBPDecCopy(p_h,st->old_wh_fx,HI_FILT_ORDER);
  voAMRWBPDecCopy(p_h-HI_FILT_ORDER,st->old_wh2_fx,HI_FILT_ORDER);

  /*-----------------------------------------------------------*
    Synthesise left right mid band signals
   ------------------------------------------------------------*/

  /*  synthesise left and right signals */

  p_Aq = st->old_AqLF_fx;       
  /*  first 1/2 subframe */
  voAMRWBPDecSyn_filt_s(1, p_Aq,M, &left_exc[0], left_hi, L_SUBFR/2,st->left.mem_synth_hi_fx,1);
  voAMRWBPDecSyn_filt_s(1, p_Aq,M, &right_exc[0], right_hi, L_SUBFR/2,st->right.mem_synth_hi_fx,1);
  p_Aq += (M+1);

  /* the 3 ramaining subframes */
  /* init the counter k*L_SUBFR+L_SUBFR/2 */
  for(k=0;k<3;k++)
  {
    voAMRWBPDecSyn_filt_s(1, p_Aq,M, &left_exc[k*L_SUBFR+L_SUBFR/2], &left_hi[k*L_SUBFR+L_SUBFR/2],                                                      L_SUBFR,st->left.mem_synth_hi_fx,1);
    voAMRWBPDecSyn_filt_s(1, p_Aq,M, &right_exc[k*L_SUBFR+L_SUBFR/2], &right_hi[k*L_SUBFR+L_SUBFR/2], 
                                                        L_SUBFR,st->right.mem_synth_hi_fx,1);
    p_Aq += (M+1);
  }


  if (fscale !=0) 
  {
    voAMRWBPDecSyn_filt_s(1, p_Aq,M, &left_exc[k*L_SUBFR+L_SUBFR/2], &left_hi[k*L_SUBFR+L_SUBFR/2], 
                                                        L_SUBFR,st->left.mem_synth_hi_fx,1);
    voAMRWBPDecSyn_filt_s(1, p_Aq,M, &right_exc[k*L_SUBFR+L_SUBFR/2], &right_hi[k*L_SUBFR+L_SUBFR/2], 
                                                        L_SUBFR,st->right.mem_synth_hi_fx,1);
  }

  if(fscale==0) 
  {
    start = 3*L_SUBFR + L_SUBFR/2;  
  }
  else 
  {
    start = 4*L_SUBFR + L_SUBFR/2;  
  }
  /* do the rest  */
  p_Aq = AqLF;
  /* i_subfr boundaries check */
  for (i_subfr=start; i_subfr <L_FRAME_PLUS-L_SUBFR/2; i_subfr+=L_SUBFR)
  {
    voAMRWBPDecSyn_filt_s(1, p_Aq,M, &left_exc[i_subfr], &left_hi[i_subfr],L_SUBFR,st->left.mem_synth_hi_fx,1);
    voAMRWBPDecSyn_filt_s(1, p_Aq,M, &right_exc[i_subfr], &right_hi[i_subfr],L_SUBFR,st->right.mem_synth_hi_fx,1);
    p_Aq += (M+1);
  }
  /* the last 1/2 subframe */
  voAMRWBPDecSyn_filt_s(1, p_Aq,M, &left_exc[L_FRAME_PLUS-L_SUBFR/2], &left_hi[L_FRAME_PLUS-L_SUBFR/2],
                                                                   L_SUBFR/2,st->left.mem_synth_hi_fx,1);
  voAMRWBPDecSyn_filt_s(1, p_Aq,M, &right_exc[L_FRAME_PLUS-L_SUBFR/2], &right_hi[L_FRAME_PLUS-L_SUBFR/2],
                                                                    L_SUBFR/2,st->right.mem_synth_hi_fx,1);

  /* save LPC filters for the next frame  */
  if(fscale == 0) 
  {
    voAMRWBPDecCopy(p_Aq,st->old_AqLF_fx,4*(M+1));
  }
  else 
  {
    voAMRWBPDecCopy(p_Aq,st->old_AqLF_fx,5*(M+1));
  }
}

