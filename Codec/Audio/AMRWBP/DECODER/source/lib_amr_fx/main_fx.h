/*--------------------------------------------------------------------------*
 *                         MAIN.H	                                        *
 *--------------------------------------------------------------------------*
 *       Main functions								                        *
 *--------------------------------------------------------------------------*/

void voAMRWBPDecInit_decoder(void **spd_state);
void voAMRWBPDecClose_decoder(void *spd_state);

void voAMRWBPDecMainProc(
     Word16 mode,                          /* input : used mode                     */
     Word16 prms[],                        /* input : parameter vector                     */
     Word16 synth16k[],                    /* output: synthesis speech              */
     Word16 * frame_length,                /* output:  lenght of the frame         */
     void *spd_state,                      /* i/o   : State structure                      */
     Word16 frame_type                     /* input : received frame type           */
);

void voAMRWBPDecReset_decoder(void *st, Word16 reset_all);
Word16 voAMRWBPDecHomingTest(Word16 input_frame[], Word16 mode);
Word16 voAMRWBPDecHomingTestFirst(Word16 input_frame[], Word16 mode);
