/*------------------------------------------------------------------------*
 *                         HOMING.C                                       *
 *------------------------------------------------------------------------*
 * Performs the homing routines                                           *
 *------------------------------------------------------------------------*/

#include "typedef.h"
#include "cnst_wb_fx.h"
#include "basic_op.h"
#include "bits_fx.h"
#include "count.h"
#include "homing.tab"
extern const Word16 *voAMRWBPDhf[10];
extern const Word16 voAMRWBPDecNb_of_param[];

static Word16 Nb_of_param_first[NUM_OF_SPMODES]=
{
	9,  14, 15,
	15, 15, 19,
	19, 19, 19
};

static Word16 dhf_test(Word16 input_frame[], const Word16 mdhf[], Word16 nparms)
{
    Word16 i, j;
    
     /* check if the parameters matches the parameters of the corresponding decoder homing frame */
    j = 0;
    for (i = 0; i < nparms; i++)
    {
        j = (input_frame[i]^mdhf[i]); 
        if (j)
            break;
    }

    return (Word16) (!j);
}


Word16 voAMRWBPDecHomingTest(Word16 input_frame[], Word16 mode)
{
    /* perform test for COMPLETE parameter frame */
/* perform test for COMPLETE parameter frame */
   if (mode != MODE_24k)
   {
        return dhf_test(input_frame,  &voAMRWBPDhf[mode][0], voAMRWBPDecNb_of_param[mode]);
   }
   else
   {
        Word16 i = 0;
        i = dhf_test(input_frame,  &voAMRWBPDhf[MODE_24k][0], 19)| 
                dhf_test(input_frame+20, &voAMRWBPDhf[MODE_24k][20], 11) |
                    dhf_test(input_frame+32, &voAMRWBPDhf[MODE_24k][32], 11)|
                        dhf_test(input_frame+44, &voAMRWBPDhf[MODE_24k][44], 11);
        return i;
   }

}
Word16 voAMRWBPDecHomingTestFirst(Word16 input_frame[], Word16 mode)
{
    /* perform test for FIRST SUBFRAME of parameter frame ONLY */
    return dhf_test(input_frame, &voAMRWBPDhf[mode][0], Nb_of_param_first[mode]);
}
