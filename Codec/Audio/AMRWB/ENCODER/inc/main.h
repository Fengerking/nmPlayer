
/***********************************************************************
*                                                                      *
*    VisualOn, Inc. Confidential and Proprietary, 2003-2010            *
*                                                                      *
************************************************************************/

/*--------------------------------------------------------------------------*
 *                         MAIN.H	                                    *
 *--------------------------------------------------------------------------*
 *       Main functions							    *
 *--------------------------------------------------------------------------*/

#ifndef __MAIN_H__
#define __MAIN_H__

void coder(
     Word16 * mode,                        /* input :  used mode                             */
     Word16 speech16k[],                   /* input :  320 new speech samples (at 16 kHz)    */
     Word16 prms[],                        /* output:  output parameters           */
     Word16 * ser_size,                    /* output:  bit rate of the used mode   */
     void *spe_state,                      /* i/o   :  State structure                       */
     Word16 allow_dtx                      /* input :  DTX ON/OFF                            */
);



void Reset_encoder(void *st, Word16 reset_all);


Word16 encoder_homing_frame_test(Word16 input_frame[]);

#endif //__MAIN_H__

