/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : sp_enc.h
*      Purpose          : Encoding of one speech frame.
*
*****************************************************************************
*/
#ifndef sp_enc_h
#define sp_enc_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "pre_proc.h"
#include "mode.h"
#include "cod_amr.h"
#include "AMRNB_E_API.h"
#include "basic_op.h"
/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/
typedef struct{
    Pre_ProcessState *pre_state;
    cod_amrState   *cod_amr_state;
    Flag dtx;
    int complexityCounter;   /* Only for complexity computation            */
} Speech_Encode_FrameState;
 
/*
*****************************************************************************
*                         ENCLARATION OF PROTOTYPES
*****************************************************************************
*/
int voAMRNBEnc_Speech_Encode_Frame_init (Speech_Encode_FrameState **st,
                              Flag dtx, VO_MEM_OPERATOR *pMemOP);
/* initialize one instance of the speech encoder
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to voAMRNBEnc_Speech_Encode_Frame in each call.
   returns 0 on success
 */
int voAMRNBEnc_Speech_Encode_Frame_reset (Speech_Encode_FrameState *st);
/* reset speech encoder (i.e. set state memory to zero)
   returns 0 on success
 */
void voAMRNBEnc_Speech_Encode_Frame_exit (Speech_Encode_FrameState **st, VO_MEM_OPERATOR *pMemOP);
/* de-initialize speech encoder (i.e. free status struct)
   stores NULL in *s
 */
int voAMRNBEnc_Speech_Encode_Frame_First (
    Speech_Encode_FrameState *st, /* i/o : post filter states     */
    Word16 *new_speech);          /* i   : speech input           */

int voAMRNBEnc_Speech_Encode_Frame (
    Speech_Encode_FrameState *st, /* i/o : encoder states         */
    enum Mode mode,               /* i   : speech coder mode      */
    Word16 *new_speech,           /* i   : input speech           */
    Word16 *serial,               /* o   : serial bit stream      */
    enum Mode *usedMode           /* o   : used speech coder mode */
);
/*    return 0 on success
 */
#ifdef MMS_IO
Word16 voAMRNBEnc_PackBits(
    enum Mode used_mode,       /* i : actual AMR mode             */
    enum Mode mode,            /* i : requested AMR (speech) mode */
    enum TXFrameType fr_type,  /* i : frame type                  */
    VoiAMRNBFrameType frameType,/*raw/IF1/IF2 */
    Word16 bits[],             /* i : serial bits                 */
    UWord8 packed_bits[]       /* o : sorted&packed bits          */
);
#endif
#endif
