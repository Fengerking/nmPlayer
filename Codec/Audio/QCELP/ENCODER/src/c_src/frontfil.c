/* frontfil.c - Front end high-pass filter		*/
#include"celp.h"

void front_end_filter(
					  short               *speech,
                      struct ENCODER_MEM  *e_mem )
{
	int i;
	/* see init.c for initialization of filter coefficients */
#ifdef ASM_OPT   // asm opt branch
	do_zero_filter_front_asm(speech,speech, FSIZE,&(e_mem->hipass_z), UPDATE);
#else
	do_zero_filter_front(speech,speech, FSIZE,&(e_mem->hipass_z), UPDATE);
#endif

#ifdef ASM_OPT // asm opt branch 
    do_pole_filter_high_asm(speech, speech, FSIZE,&(e_mem->hipass_p), UPDATE);
#else
	do_pole_filter_high(speech, speech, FSIZE,&(e_mem->hipass_p), UPDATE);
#endif

#define NORM_SCALE  31003   /* 1/1.056920 * (1 << 15) */
	/* normalization so that gain of filter at 1 kHz (PI/4) is 1.0 */
	for(i = 0; i < FSIZE;)
	{
		//speech[i] = mult_r(speech[i], NORM_SCALE); i++;     //lhp		
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		speech[i] =  (((int)speech[i]*NORM_SCALE + 16384) >> 15); i++;
		
	}
}
