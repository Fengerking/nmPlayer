/*--------------------------------------------------------------------------*
 *                         MAIN.H	                                        *
 *--------------------------------------------------------------------------*
 *       Main functions								                        *
 *--------------------------------------------------------------------------*/

void voAMRWBDecMainProcess(
     Word16 mode,                          /* input : used mode                     */
     Word16 prms[],                        /* input : parameter vector                     */
     Word16 synth16k[],                    /* output: synthesis speech              */
     Word16 * frame_length,                /* output:  lenght of the frame         */
     void *spd_state,                      /* i/o   : State structure                      */
     Word16 frame_type                     /* input : received frame type           */
);


void Reset_decoder(void *st, Word16 reset_all);

Word16 voAMRWBDecHomingTest(Word16 input_frame[], Word16 mode);
Word16 voAMRWBDecHomingTestFirst(Word16 input_frame[], Word16 mode);
