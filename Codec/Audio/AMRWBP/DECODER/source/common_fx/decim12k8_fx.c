
/*-------------------------------------------------------------------*
 * Function decim_12k8() and oversamp_12k8()                         *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                         *
 * decim_12k8    : decimation from fs to 12.8kHz.                    *
 * oversamp_12k8 : oversampling from 12.8kHz to fs.                  *
 *                                                                   *
 * fs = 16/22/24/28.8/32/44/48 kHz.                                  *
 *-------------------------------------------------------------------*/

//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>

#include "amr_plus_fx.h"
#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#define L_FRAME_MAX   (L_FRAME24k)
#define L_FILT_MAX    (L_FILT24k)

#define L_SUBFR16k   80       /* Subframe size at 16kHz           */


void Decim_12k8_p(
  Word16 sig_fs[],     /* (i)  : signal to decimate      */
  Word16 lg,           /* (i)  : length of input         */
  Word16 sig12k8[],    /* (o)  : decimated signal        */
  Word16 mem[],        /* (i/o): memory (2*L_FILT_FS)    */
  Word16 band)         /* (i)  : 0=0..6.4k, 1=6.4..10.8k */
{
  Word16 i, ncoef = 0;
  Word16 signal[(2*L_FILT_MAX)+L_FRAME_MAX];
  const Word16 *filter = 0;
  Word16 gain;
  Word16 fac_up = 0, fac_down = 0, fac_down_up = 0, ncoef2;

  gain = 1;  
  switch (lg) {

  case L_FRAME16kPLUS:

    ncoef = ((NCOEF_32k+1)>>1); 
    fac_up = FAC1_32k*2; 
    fac_down = FAC2_32k; 
    fac_down_up = 1;         /*5/(2*2)*/

    if (band == 0) 
    {
      filter = Filter_32k;
    }
    else 
    {
      filter = Filter_32k_7k;
    }    
    break;

  case L_FRAME24k:
    ncoef = ((NCOEF_48k+1)>>1);    
    fac_up = FAC1_48k*2;
    fac_down = FAC2_48k; 
    fac_down_up =  1;                   /*15/ (4*2) */

    if (band == 0) 
    {
      filter = Filter_48k; 
    }
    else 
    {
      filter = Filter_48k_hf;
    }

    break;

  case L_FRAME8k: /* This mode actually upsamples rather than down */
    ncoef = (((NCOEF_32k>>2)+1)>>1); 
    fac_up = FAC1_32k*4;
    fac_down = FAC2_32k;
    fac_down_up = 0;                          /*5/ (2*2*2)*/
    filter = Filter_8k;       
    gain = add(gain,1);                                                        
    break;

  default:
    //AMRWBPLUS_PRINT("wrong frame size in decim_12k8!\n");
    //exit(0);
	break;
  }      

  ncoef2 = shl(ncoef,1);
  for (i=0; i<ncoef2; i++)
  {
    signal[i] = mem[i];         
  }
  
  for (i=0; i<lg; i++) 
  {
    signal[i + ncoef2] = sig_fs[i];   
  }

  voAMRWBPDecInterpol(signal+ncoef, sig12k8, L_FRAME_PLUS, filter, ncoef, 
           fac_up,fac_down_up, fac_down, gain, 0);

  for (i=0; i<ncoef2; i++) 
  {
    mem[i] = signal[i+lg]; 
  }

  return;
}

