/*-------------------------------------------------------------------*
 *                         DTX.C									 *
 *-------------------------------------------------------------------*
 * DTX functions											         *
 *-------------------------------------------------------------------*/

//#include <stdio.h>
//#include <stdlib.h>

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "cnst_wb_fx.h"
#include "acelp_fx.h"                         /* prototype of functions    */
#include "bits_fx.h"
#include "dtx_fx.h"
#include "count.h"
#include "log2.h"
#include "voAMRWBPMalloc.h"

static void CN_dithering(
     Word16 isf[M],
     Word32 * L_log_en_int,
     Word16 * dither_seed
);

/* excitation energy adjustment depending on speech coder mode used, Q7 */
//static Word16 en_adjust[9] =
//{
//    230,                                   /* mode0 = 7k  :  -5.4dB  */
//    179,                                   /* mode1 = 9k  :  -4.2dB  */
//    141,                                   /* mode2 = 12k :  -3.3dB  */
//    128,                                   /* mode3 = 14k :  -3.0dB  */
//    122,                                   /* mode4 = 16k :  -2.85dB */
//    115,                                   /* mode5 = 18k :  -2.7dB  */
//    115,                                   /* mode6 = 20k :  -2.7dB  */
//    115,                                   /* mode7 = 23k :  -2.7dB  */
//    115                                    /* mode8 = 24k :  -2.7dB  */
//};

/**************************************************************************
 *
 *
 * Function    : dtx_dec_init
 *
 *
 **************************************************************************/
Word16 voAMRWBPDecdtx_dec_init(dtx_decState ** st, Word16 isf_init[])
{
    dtx_decState *s;

    if (st == (dtx_decState **) NULL)
    {
        //AMRWBPLUS_FPRINTF(stderr, "dtx_dec_init: invalid parameter\n");
        return -1;
    }
    *st = NULL;

    /* allocate memory */
    if ((s = (dtx_decState *) voAMRWBPDecAlignedMalloc(sizeof(dtx_decState))) == NULL)
    {
        //AMRWBPLUS_FPRINTF(stderr, "dtx_dec_init: can not AlignedMalloc state structure\n");
        return -1;
    }
    voAMRWBPDecdtx_dec_reset(s, isf_init);
    *st = s;

    return 0;
}

/**************************************************************************
 *
 *
 * Function    : dtx_dec_reset
 *
 *
 **************************************************************************/
Word16 voAMRWBPDecdtx_dec_reset(dtx_decState * st, Word16 isf_init[])
{
    Word16 i;

    
    if (st == (dtx_decState *) NULL)
    {
        //AMRWBPLUS_FPRINTF(stderr, "dtx_dec_reset: invalid parameter\n");
        return -1;
    }
    st->since_last_sid = 0;                
    st->true_sid_period_inv = (1 << 13);     /* 0.25 in Q15 */

    st->log_en = 3500;                     
    st->old_log_en = 3500;                 
    /* low level noise for better performance in  DTX handover cases */

    st->cng_seed = RANDOM_INITSEED;        

    st->hist_ptr = 0;                      

    /* Init isf_hist[] and decoder log frame energy */
    voAMRWBPDecCopy(isf_init, st->isf, M);
    voAMRWBPDecCopy(isf_init, st->isf_old, M);

    for (i = 0; i < DTX_HIST_SIZE; i++)
    {
        voAMRWBPDecCopy(isf_init, &st->isf_hist[i * M], M);
        st->log_en_hist[i] = st->log_en;   
    }

    st->dtxHangoverCount = DTX_HANG_CONST; 
    st->decAnaElapsedCount = 32767;        

    st->sid_frame = 0;                     
    st->valid_data = 0;                    
    st->dtxHangoverAdded = 0;              

    st->dtxGlobalState = SPEECH;           
    st->data_updated = 0;                  

    st->dither_seed = RANDOM_INITSEED;     
    st->CN_dith = 0;

    return 0;
}

/**************************************************************************
 *
 *
 * Function    : dtx_dec_exit
 *
 *
 **************************************************************************/
void voAMRWBPDecdtx_dec_exit(dtx_decState ** st)
{
    if (st == NULL || *st == NULL)
        return;

    /* deallocate memory */
    voAMRWBPDecAlignedFree((void*)*st);
    *st = NULL;

    return;
}

/*
     Table of new SPD synthesis states

                           |     previous SPD_synthesis_state
     Incoming              |
     frame_type            | SPEECH       | DTX           | DTX_MUTE
     ---------------------------------------------------------------
     RX_SPEECH_GOOD ,      |              |               |
     RX_SPEECH_PR_DEGRADED | SPEECH       | SPEECH        | SPEECH
     ----------------------------------------------------------------
     RX_SPEECH_BAD,        | SPEECH       | DTX           | DTX_MUTE
     ----------------------------------------------------------------
     RX_SID_FIRST,         | DTX          | DTX/(DTX_MUTE)| DTX_MUTE
     ----------------------------------------------------------------
     RX_SID_UPDATE,        | DTX          | DTX           | DTX
     ----------------------------------------------------------------
     RX_SID_BAD,           | DTX          | DTX/(DTX_MUTE)| DTX_MUTE
     ----------------------------------------------------------------
     RX_NO_DATA,           | SPEECH       | DTX/(DTX_MUTE)| DTX_MUTE
     RX_SPARE              |(class2 garb.)|               |
     ----------------------------------------------------------------
*/

/**************************************************************************
 *
 *
 * Function    : dtx_dec
 *
 *
 **************************************************************************/
Word16 voAMRWBPDecdtx_dec(
     dtx_decState * st,                    /* i/o : State struct                                          */
     Word16 * exc2,                        /* o   : CN excitation                                          */
     Word16 new_state,                     /* i   : New DTX state                                          */
     Word16 isf[],                         /* o   : CN ISF vector                                          */
     Word16 ** prms
)
{
    Word16 log_en_index;
    Word16 ind[7];
    Word16 i, j;
    Word16 int_fac;
    Word16 gain;

    Word32 L_isf[M], L_log_en_int, level32, ener32;
    Word16 ptr;
    Word16 tmp_int_length;
    Word16 tmp, exp, exp0, log_en_int_e, log_en_int_m, level;

    /* This function is called if synthesis state is not SPEECH the globally passed  inputs to this function
     * are st->sid_frame st->valid_data st->dtxHangoverAdded new_state  (SPEECH, DTX, DTX_MUTE) */
    
    if ((st->dtxHangoverAdded != 0) &&
        (st->sid_frame != 0))
    {
        /* sid_first after dtx hangover period */
        /* or sid_upd after dtxhangover        */

        /* consider  twice the last frame */
        ptr = add(st->hist_ptr, 1);
        
        if (sub(ptr, DTX_HIST_SIZE) == 0)
            ptr = 0;                       

        voAMRWBPDecCopy(&st->isf_hist[st->hist_ptr * M], &st->isf_hist[ptr * M], M);

        st->log_en_hist[ptr] = st->log_en_hist[st->hist_ptr];   

        /* compute mean log energy and isf from decoded signal (SID_FIRST) */
        st->log_en = 0;                    
        for (i = 0; i < M; i++)
        {
            L_isf[i] = 0;                  move32();
        }

        /* average energy and isf */
        for (i = 0; i < DTX_HIST_SIZE; i++)
        {
            /* Division by DTX_HIST_SIZE = 8 has been done in dtx_buffer log_en is in Q10 */
            st->log_en = add(st->log_en, st->log_en_hist[i]);   

            for (j = 0; j < M; j++)
            {
                L_isf[j] = L_add(L_isf[j], L_deposit_l(st->isf_hist[i * M + j]));       move32();
            }
        }

        /* st->log_en in Q9 */
        st->log_en = shr(st->log_en, 1);   

        /* Add 2 in Q9, in order to have only positive values for voAMRWBPDecPow2 */
        /* this value is subtracted back after voAMRWBPDecPow2 function */
        st->log_en = add(st->log_en, 1024);
        
        if (st->log_en < 0)
            st->log_en = 0;                

        for (j = 0; j < M; j++)
        {
            st->isf[j] = extract_l(L_shr(L_isf[j], 3)); move32();  /* divide by 8 */
        }

    }
    
    if (st->sid_frame != 0)
    {
        /* Set old SID parameters, always shift */
        /* even if there is no new valid_data   */

        voAMRWBPDecCopy(st->isf, st->isf_old, M);
        st->old_log_en = st->log_en;       
        
        if (st->valid_data != 0)           /* new data available (no CRC) */
        {
            /* st->true_sid_period_inv = 1.0f/st->since_last_sid; */
            /* Compute interpolation factor, since the division only works * for values of since_last_sid <
             * 32 we have to limit the      * interpolation to 32 frames                                  */
            tmp_int_length = st->since_last_sid;        

            
            if (sub(tmp_int_length, 32) > 0)
            {
                tmp_int_length = 32;       
            }
            
            if (sub(tmp_int_length, 2) >= 0)
            {
                
                st->true_sid_period_inv = div_s(1 << 10, shl(tmp_int_length, 10));
            } else
            {
                st->true_sid_period_inv = 1 << 14;      /* 0.5 it Q15 */
            }

            ind[0] = **prms; (*prms)++; /*Serial_parm(6, prms);*/ 
            ind[1] = **prms; (*prms)++; /*Serial_parm(6, prms);*/ 
            ind[2] = **prms; (*prms)++; /*Serial_parm(6, prms);*/ 
            ind[3] = **prms; (*prms)++; /*Serial_parm(5, prms);*/ 
            ind[4] = **prms; (*prms)++; /*Serial_parm(5, prms);*/ 

            voAMRWBPDecDisf_ns(ind, st->isf);

            log_en_index = **prms; (*prms)++; /*Serial_parm(6, prms);*/     

            /* read background noise stationarity information */
            st->CN_dith = **prms; (*prms)++; /*Serial_parm(1, prms);*/ 

            /* st->log_en = (float)log_en_index / 2.625 - 2.0;  */
            /* log2(E) in Q9 (log2(E) lies in between -2:22) */
            st->log_en = shl(log_en_index, 15 - 6);     

            /* Divide by 2.625  */
            st->log_en = mult(st->log_en, 12483);       
            /* Subtract 2 in Q9 is done later, after voAMRWBPDecPow2 function  */

            /* no interpolation at startup after coder reset        */
            /* or when SID_UPD has been received right after SPEECH */
            
            if ((st->data_updated == 0) ||
                (sub(st->dtxGlobalState, SPEECH) == 0))
            {
                voAMRWBPDecCopy(st->isf, st->isf_old, M);
                st->old_log_en = st->log_en;    
            }
        }                                  /* endif valid_data */
    }                                      /* endif sid_frame */
    
    
    if ((st->sid_frame != 0) && (st->valid_data != 0))
    {
        st->since_last_sid = 0;            
    }
    /* Interpolate SID info */
    int_fac = shl(st->since_last_sid, 10); /* Q10 */
    int_fac = mult(int_fac, st->true_sid_period_inv);   /* Q10 * Q15 -> Q10 */

    /* Maximize to 1.0 in Q10 */
    
    if (sub(int_fac, 1024) > 0)
    {
        int_fac = 1024;                    
    }
    int_fac = shl(int_fac, 4);             /* Q10 -> Q14 */

    L_log_en_int = L_mult(int_fac, st->log_en); /* Q14 * Q9 -> Q24 */

    for (i = 0; i < M; i++)
    {
        isf[i] = mult(int_fac, st->isf[i]);/* Q14 * Q15 -> Q14 */
    }

    int_fac = sub(16384, int_fac);         /* 1-k in Q14 */

    /* ( Q14 * Q9 -> Q24 ) + Q24 -> Q24 */
    L_log_en_int = L_mac(L_log_en_int, int_fac, st->old_log_en);

    for (i = 0; i < M; i++)
    {
        /* Q14 + (Q14 * Q15 -> Q14) -> Q14 */
        isf[i] = add(isf[i], mult(int_fac, st->isf_old[i]));    
        isf[i] = shl(isf[i], 1);           /* Q14 -> Q15 */
    }

    /* If background noise is non-stationary, insert comfort noise dithering */
    if (st->CN_dith != 0)
    {
        CN_dithering(isf, &L_log_en_int, &st->dither_seed);
    }
    /* L_log_en_int corresponds to log2(E)+2 in Q24, i.e log2(gain)+1 in Q25 */
    /* Q25 -> Q16 */
    L_log_en_int = L_shr(L_log_en_int, 9);

    /* Find integer part  */
    log_en_int_e = extract_h(L_log_en_int);

    /* Find fractional part */
    log_en_int_m = extract_l(L_shr(L_sub(L_log_en_int, L_deposit_h(log_en_int_e)), 1));

    /* Subtract 2 from L_log_en_int in Q9, i.e divide the gain by 2 (energy by 4) */
    /* Add 16 in order to have the result of voAMRWBPDecPow2 in Q16 */
    log_en_int_e = add(log_en_int_e, 16 - 1);

    /* level = (float)( pow( 2.0f, log_en ) );  */
    level32 = voAMRWBPDecPow2(log_en_int_e, log_en_int_m); /* Q16 */
    exp0 = norm_l(level32);
    level32 = L_shl(level32, exp0);        /* level in Q31 */
    exp0 = sub(15, exp0);
    level = extract_h(level32);            /* level in Q15 */

    /* generate white noise vector */
    for (i = 0; i < L_FRAME; i++)
    {
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
        exc2[i] = shr(Random(&(st->cng_seed)), 4);      
#elif EVC_AMRWBPLUS
        exc2[i] = shr(Random_evc(&(st->cng_seed)), 4);      
#endif
// end
    }

    /* gain = level / sqrt(ener) * sqrt(L_FRAME) */

    /* energy of generated excitation */
    ener32 = voAMRWBPDecDot_product12(exc2, exc2, L_FRAME, &exp);

    voAMRWBPDecIsqrt_n(&ener32, &exp);

    gain = extract_h(ener32);

    gain = mult(level, gain);              /* gain in Q15 */

    exp = add(exp0, exp);

    /* Multiply by sqrt(L_FRAME)=16, i.e. shift left by 4 */
    exp = add(exp, 4);

    for (i = 0; i < L_FRAME; i++)
    {
        tmp = mult(exc2[i], gain);         /* Q0 * Q15 */
        exc2[i] = shl(tmp, exp);           
    }

    
    if (sub(new_state, DTX_MUTE) == 0)
    {
        /* mute comfort noise as it has been quite a long time since last SID update  was performed                            */

        tmp_int_length = st->since_last_sid;    
        
        if (sub(tmp_int_length, 32) > 0)
        {
            tmp_int_length = 32;           
        }
        
        st->true_sid_period_inv = div_s(1 << 10, shl(tmp_int_length, 10));

        st->since_last_sid = 0;            
        st->old_log_en = st->log_en;       
        /* subtract 1/8 in Q9 (energy), i.e -3/8 dB */
        st->log_en = sub(st->log_en, 64);  
    }
    /* reset interpolation length timer if data has been updated.        */
    
    if ((st->sid_frame != 0) &&
        ((st->valid_data != 0) ||
            ((st->valid_data == 0) && (st->dtxHangoverAdded) != 0)))
    {
        st->since_last_sid = 0;            
        st->data_updated = 1;              
    }
    return 0;
}


void voAMRWBPDecdtx_dec_activity_update(
     dtx_decState * st,
     Word16 isf[],
     Word16 exc[])
{
#if (FUNC_DTX_DEC_ACTIVITY_UPDATE_OPT)
    Word16 i;

    Word32 L_frame_en;
    Word16 log_en_e, log_en_m, log_en;


    st->hist_ptr++;
    if (st->hist_ptr == DTX_HIST_SIZE)
    {
        st->hist_ptr = 0;
    }
    voAMRWBPDecCopy(isf, &st->isf_hist[st->hist_ptr * M], M);

    /* compute log energy based on excitation frame energy in Q0 */
    L_frame_en = 0;
    for (i = 0; i < L_FRAME; i++)
    {
        //L_frame_en = L_mac(L_frame_en, exc[i], exc[i]);
        L_frame_en += exc[i] * exc[i];
    }
    //L_frame_en >>= 1;

    /* log_en = (float)log10(L_frame_en/(float)L_FRAME)/(float)log10(2.0f); */
    voAMRWBPDecLog2(L_frame_en, &log_en_e, &log_en_m);

    /* convert exponent and mantissa to Word16 Q7. Q7 is used to simplify averaging in dtx_enc */
    log_en = shl(log_en_e, 7);             /* Q7 */     //can't be modified
    log_en += log_en_m >> 8;

    /* Divide by L_FRAME = 256, i.e subtract 8 in Q7 = 1024 */
    log_en -= 1024;

    /* insert into log energy buffer */
    st->log_en_hist[st->hist_ptr] = log_en;

    return;
    
#else

    Word16 i;

    Word32 L_frame_en;
    Word16 log_en_e, log_en_m, log_en;


    st->hist_ptr = add(st->hist_ptr, 1);   
    
    if (sub(st->hist_ptr, DTX_HIST_SIZE) == 0)
    {
        st->hist_ptr = 0;                  
    }
    voAMRWBPDecCopy(isf, &st->isf_hist[st->hist_ptr * M], M);

    /* compute log energy based on excitation frame energy in Q0 */
    L_frame_en = 0;                        move32();
    for (i = 0; i < L_FRAME; i++)
    {
        L_frame_en = L_mac(L_frame_en, exc[i], exc[i]);
    }
    L_frame_en = L_shr(L_frame_en, 1);

    /* log_en = (float)log10(L_frame_en/(float)L_FRAME)/(float)log10(2.0f); */
    voAMRWBPDecLog2(L_frame_en, &log_en_e, &log_en_m);

    /* convert exponent and mantissa to Word16 Q7. Q7 is used to simplify averaging in dtx_enc */
    log_en = shl(log_en_e, 7);             /* Q7 */
    log_en = add(log_en, shr(log_en_m, 15 - 7));

    /* Divide by L_FRAME = 256, i.e subtract 8 in Q7 = 1024 */
    log_en = sub(log_en, 1024);

    /* insert into log energy buffer */
    st->log_en_hist[st->hist_ptr] = log_en;

    return;
#endif
}


/*
     Table of new SPD synthesis states

                           |     previous SPD_synthesis_state
     Incoming              |
     frame_type            | SPEECH       | DTX           | DTX_MUTE
     ---------------------------------------------------------------
     RX_SPEECH_GOOD ,      |              |               |
     RX_SPEECH_PR_DEGRADED | SPEECH       | SPEECH        | SPEECH
     ----------------------------------------------------------------
     RX_SPEECH_BAD,        | SPEECH       | DTX           | DTX_MUTE
     ----------------------------------------------------------------
     RX_SID_FIRST,         | DTX          | DTX/(DTX_MUTE)| DTX_MUTE
     ----------------------------------------------------------------
     RX_SID_UPDATE,        | DTX          | DTX           | DTX
     ----------------------------------------------------------------
     RX_SID_BAD,           | DTX          | DTX/(DTX_MUTE)| DTX_MUTE
     ----------------------------------------------------------------
     RX_NO_DATA,           | SPEECH       | DTX/(DTX_MUTE)| DTX_MUTE
     RX_SPARE              |(class2 garb.)|               |
     ----------------------------------------------------------------
*/

Word16 voAMRWBPDecRx_dtx_handler(
     dtx_decState * st,                    /* i/o : State struct     */
     Word16 frame_type                     /* i   : Frame type       */
)
{
    Word16 newState;
    Word16 encState;

    /* DTX if SID frame or previously in DTX{_MUTE} and (NO_RX OR BAD_SPEECH) */
    
    
    
    if ((sub(frame_type, RX_SID_FIRST) == 0) ||
        (sub(frame_type, RX_SID_UPDATE) == 0) ||
        (sub(frame_type, RX_SID_BAD) == 0) ||
        (((sub(st->dtxGlobalState, DTX) == 0) ||
                (sub(st->dtxGlobalState, DTX_MUTE) == 0)) &&
            ((sub(frame_type, RX_NO_DATA) == 0) ||
                (sub(frame_type, RX_SPEECH_BAD) == 0) ||
                (sub(frame_type, RX_SPEECH_LOST) == 0))))
    {
        newState = DTX;                    

        /* stay in mute for these input types */
        
        if ((sub(st->dtxGlobalState, DTX_MUTE) == 0) &&
            ((sub(frame_type, RX_SID_BAD) == 0) ||
                (sub(frame_type, RX_SID_FIRST) == 0) ||
                (sub(frame_type, RX_SPEECH_LOST) == 0) ||
                (sub(frame_type, RX_NO_DATA) == 0)))
        {
            newState = DTX_MUTE;           
        }
        /* evaluate if noise parameters are too old                     */
        /* since_last_sid is reset when CN parameters have been updated */
        st->since_last_sid = add(st->since_last_sid, 1);        

        /* no update of sid parameters in DTX for a long while */
        
        if (sub(st->since_last_sid, DTX_MAX_EMPTY_THRESH) > 0)
        {
            newState = DTX_MUTE;           
        }
    } else
    {
        newState = SPEECH;                 
        st->since_last_sid = 0;            
    }

    /* reset the decAnaElapsed Counter when receiving CNI data the first time, to robustify counter missmatch
     * after handover this might delay the bwd CNI analysis in the new decoder slightly. */
    
    if ((st->data_updated == 0) &&
        (sub(frame_type, RX_SID_UPDATE) == 0))
    {
        st->decAnaElapsedCount = 0;        
    }
    /* update the SPE-SPD DTX hangover synchronization */
    /* to know when SPE has added dtx hangover         */
    st->decAnaElapsedCount = add(st->decAnaElapsedCount, 1);    
    st->dtxHangoverAdded = 0;              

    
    if ((sub(frame_type, RX_SID_FIRST) == 0) ||
        (sub(frame_type, RX_SID_UPDATE) == 0) ||
        (sub(frame_type, RX_SID_BAD) == 0) ||
        (sub(frame_type, RX_NO_DATA) == 0))
    {
        encState = DTX;                    
    } else
    {
        encState = SPEECH;                 
    }

    
    if (sub(encState, SPEECH) == 0)
    {
        st->dtxHangoverCount = DTX_HANG_CONST;  
    } else
    {
        
        if (sub(st->decAnaElapsedCount, DTX_ELAPSED_FRAMES_THRESH) > 0)
        {
            st->dtxHangoverAdded = 1;      
            st->decAnaElapsedCount = 0;    
            st->dtxHangoverCount = 0;      
// @shanrong modify
#if (!REMOVE_COUNT_FUNC)
        } else if (test(), st->dtxHangoverCount == 0)
#else
        } else if (st->dtxHangoverCount == 0)
#endif
// end
        {
            st->decAnaElapsedCount = 0;    
        } else
        {
            st->dtxHangoverCount = sub(st->dtxHangoverCount, 1);        
        }
    }
    
    if (sub(newState, SPEECH) != 0)
    {
        /* DTX or DTX_MUTE CN data is not in a first SID, first SIDs are marked as SID_BAD but will do
         * backwards analysis if a hangover period has been added according to the state machine above */

        st->sid_frame = 0;                 
        st->valid_data = 0;                

        
        if (sub(frame_type, RX_SID_FIRST) == 0)
        {
            st->sid_frame = 1;             
// @shanrong modify
#if (!REMOVE_COUNT_FUNC)
        } else if (test(), sub(frame_type, RX_SID_UPDATE) == 0)
#else
        } else if (sub(frame_type, RX_SID_UPDATE) == 0)
#endif
// end
        {
            st->sid_frame = 1;             
            st->valid_data = 1;            
// @shanrong modify
#if (!REMOVE_COUNT_FUNC)
        } else if (test(), sub(frame_type, RX_SID_BAD) == 0)
#else
        } else if (sub(frame_type, RX_SID_BAD) == 0)
#endif
// end
        {
            st->sid_frame = 1;             
            st->dtxHangoverAdded = 0;      /* use old data */
        }
    }
    return newState;
    /* newState is used by both SPEECH AND DTX synthesis routines */
}

static void CN_dithering(
     Word16 isf[M],
     Word32 * L_log_en_int,
     Word16 * dither_seed
)
{
    Word16 temp, temp1, i, dither_fac, rand_dith;
    Word16 rand_dith2;

    /* Insert comfort noise dithering for energy parameter */
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
    rand_dith = shr(Random(dither_seed), 1);
    rand_dith2 = shr(Random(dither_seed), 1);
#elif EVC_AMRWBPLUS
    rand_dith = shr(Random_evc(dither_seed), 1);
    rand_dith2 = shr(Random_evc(dither_seed), 1);
#endif
// end
    rand_dith = add(rand_dith, rand_dith2);
    *L_log_en_int = L_add(*L_log_en_int, L_mult(rand_dith, GAIN_FACTOR));
    if (*L_log_en_int < 0)
    {
        *L_log_en_int = 0;
    }
    /* Insert comfort noise dithering for spectral parameters (ISF-vector) */
    dither_fac = ISF_FACTOR_LOW;

// @shanrong modified
#ifdef MSVC_AMRWBPLUS
    rand_dith = shr(Random(dither_seed), 1);
    rand_dith2 = shr(Random(dither_seed), 1);
#elif EVC_AMRWBPLUS
    rand_dith = shr(Random_evc(dither_seed), 1);
    rand_dith2 = shr(Random_evc(dither_seed), 1);
#endif
// end
    rand_dith = add(rand_dith, rand_dith2);
    temp = add(isf[0], mult_r(rand_dith, dither_fac));

    /* Make sure that isf[0] will not get negative values */
    if (sub(temp, ISF_GAP) < 0)
    {
        isf[0] = ISF_GAP;
    } else
    {
        isf[0] = temp;
    }

    for (i = 1; i < M - 1; i++)
    {
        dither_fac = add(dither_fac, ISF_FACTOR_STEP);

// @shanrong modified
#ifdef MSVC_AMRWBPLUS
        rand_dith = shr(Random(dither_seed), 1);
        rand_dith2 = shr(Random(dither_seed), 1);
#elif EVC_AMRWBPLUS
        rand_dith = shr(Random_evc(dither_seed), 1);
        rand_dith2 = shr(Random_evc(dither_seed), 1);
#endif
// end
        rand_dith = add(rand_dith, rand_dith2);
        temp = add(isf[i], mult_r(rand_dith, dither_fac));
        temp1 = sub(temp, isf[i - 1]);

        /* Make sure that isf spacing remains at least ISF_DITH_GAP Hz */
        if (sub(temp1, ISF_DITH_GAP) < 0)
        {
            isf[i] = add(isf[i - 1], ISF_DITH_GAP);
        } else
        {
            isf[i] = temp;
        }
    }

    /* Make sure that isf[M-2] will not get values above 16384 */
    if (sub(isf[M - 2], 16384) > 0)
    {
        isf[M - 2] = 16384;
    }
    return;
}
