/*--------------------------------------------------------------------------*
 *                         Q_PULSE.C										*
 *--------------------------------------------------------------------------*
 * Coding and decodeing of algebraic codebook                               *
 *--------------------------------------------------------------------------*/

//#include <stdio.h>

#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "q_pulse_fx.h"


#define NB_POS 16                          /* pos in track, mask for sign bit */


Word32 voAMRWBPDecQuant_1p_N1(                        /* (o) return N+1 bits             */
     Word16 pos,                           /* (i) position of the pulse       */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 mask;
    Word32 index;

//    mask = sub(shl(1, N), 1);              /* mask = ((1<<N)-1); */
    mask = (1 << N) - 1;              /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Quantization of 1 pulse with N+1 bits:                *
     *-------------------------------------------------------*/
    index = L_deposit_l((Word16) (pos & mask));
    if ((pos & NB_POS) != 0)
    {
        index += L_deposit_l(1 << N);   /* index += 1 << N; */
    }
    return (index);

#else

    Word16 mask;
    Word32 index;

    mask = sub(shl(1, N), 1);              /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Quantization of 1 pulse with N+1 bits:                *
     *-------------------------------------------------------*/
    index = L_deposit_l((Word16) (pos & mask));
    
    if ((pos & NB_POS) != 0)
    {
        index = L_add(index, L_deposit_l(shl(1, N)));   /* index += 1 << N; */
    }
    return (index);
#endif
}

void voAMRWBPDecDec_1p_N1(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 pos1;
    Word32 mask, i;

    mask = L_deposit_l((1 << N) - 1);   /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Decode 1 pulse with N+1 bits:                         *
     *-------------------------------------------------------*/
    pos1 = extract_l(index & mask) + offset;        /* pos1 = ((index & mask) + offset); */
    i = ((index >> N) & 1L);              /* i = ((index >> N) & 1); */
    if (i == 1)
    {
        pos1 += NB_POS;
    }
    pos[0] = pos1;

    return;

#else

    Word16 pos1;
    Word32 mask, i;

    mask = L_deposit_l(sub(shl(1, N), 1));   /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Decode 1 pulse with N+1 bits:                         *
     *-------------------------------------------------------*/
    pos1 = add(extract_l(index & mask), offset);        /* pos1 = ((index & mask) + offset); */
    i = (L_shr(index, N) & 1L);              /* i = ((index >> N) & 1); */
    
    if (L_sub(i, 1) == 0)
    {
        pos1 = add(pos1, NB_POS);
    }
    pos[0] = pos1;                         

    return;
#endif
}


Word32 voAMRWBPDecQuant_2p_2N1(                       /* (o) return (2*N)+1 bits         */
     Word16 pos1,                          /* (i) position of the pulse 1     */
     Word16 pos2,                          /* (i) position of the pulse 2     */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 mask, tmp;
    Word32 index;

    mask = (1 << N) - 1;              /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Quantization of 2 pulses with 2*N+1 bits:             *
     *-------------------------------------------------------*/
    if (((pos2 ^ pos1) & NB_POS) == 0)
    {
        /* sign of 1st pulse == sign of 2th pulse */
        if (pos1 <= pos2)          /* ((pos1 - pos2) <= 0) */
        {
            /* index = ((pos1 & mask) << N) + (pos2 & mask); */
            index = L_deposit_l(((((Word16)(pos1 & mask)) << N) + ((Word16)(pos2 & mask))));
        } else
        {
            /* ((pos2 & mask) << N) + (pos1 & mask); */
            index = L_deposit_l(((((Word16)(pos2 & mask)) << N) + ((Word16)(pos1 & mask))));
        }
        if ((pos1 & NB_POS) != 0)
        {
            tmp = N << 1;
            index += 1L << tmp;       /* index += 1 << (2*N); */
        }
    } else
    {
        /* sign of 1st pulse != sign of 2th pulse */
        if ((Word16)(pos1 & mask) <= (Word16)(pos2 & mask))
        {
            /* index = ((pos2 & mask) << N) + (pos1 & mask); */
            index = L_deposit_l(((((Word16)(pos2 & mask)) << N) + ((Word16)(pos1 & mask))));
            if ((pos2 & NB_POS) != 0)
            {
                tmp = N << 1;           /* index += 1 << (2*N); */
                index += 1L << tmp;
            }
        } else
        {
            /* index = ((pos1 & mask) << N) + (pos2 & mask);	 */
            index = L_deposit_l(((((Word16)(pos1 & mask)) << N) + ((Word16)(pos2 & mask))));
            if ((pos1 & NB_POS) != 0)
            {
                tmp = N << 1;
                index += 1 << tmp;    /* index += 1 << (2*N); */
            }
        }
    }
    return (index);
    
#else

    Word16 mask, tmp;
    Word32 index;

    mask = sub(shl(1, N), 1);              /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Quantization of 2 pulses with 2*N+1 bits:             *
     *-------------------------------------------------------*/
    logic16();logic16();
    if (((pos2 ^ pos1) & NB_POS) == 0)
    {
        /* sign of 1st pulse == sign of 2th pulse */
        
        if (sub(pos1, pos2) <= 0)          /* ((pos1 - pos2) <= 0) */
        {
            /* index = ((pos1 & mask) << N) + (pos2 & mask); */
            index = L_deposit_l(add(shl(((Word16) (pos1 & mask)), N), ((Word16) (pos2 & mask))));
        } else
        {
            /* ((pos2 & mask) << N) + (pos1 & mask); */
            index = L_deposit_l(add(shl(((Word16) (pos2 & mask)), N), ((Word16) (pos1 & mask))));
        }
        logic16();
        if ((pos1 & NB_POS) != 0)
        {
            tmp = shl(N, 1);
            index = L_add(index, L_shl(1L, tmp));       /* index += 1 << (2*N); */
        }
    } else
    {
        /* sign of 1st pulse != sign of 2th pulse */
        logic16();logic16();
        if (sub((Word16) (pos1 & mask), (Word16) (pos2 & mask)) <= 0)
        {
            /* index = ((pos2 & mask) << N) + (pos1 & mask); */
            index = L_deposit_l(add(shl(((Word16) (pos2 & mask)), N), ((Word16) (pos1 & mask))));       logic16();logic16();
            logic16();
            if ((pos2 & NB_POS) != 0)
            {
                tmp = shl(N, 1);           /* index += 1 << (2*N); */
                index = L_add(index, L_shl(1L, tmp));
            }
        } else
        {
            /* index = ((pos1 & mask) << N) + (pos2 & mask);	 */
            index = L_deposit_l(add(shl(((Word16) (pos1 & mask)), N), ((Word16) (pos2 & mask))));       logic16();logic16();
            logic16();
            if ((pos1 & NB_POS) != 0)
            {
                tmp = shl(N, 1);
                index = L_add(index, L_shl(1, tmp));    /* index += 1 << (2*N); */
            }
        }
    }
    return (index);
#endif
}

void voAMRWBPDecDec_2p_2N1(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 pos1, pos2, tmp;
    Word32 mask, i;

    mask = L_deposit_l((1 << N) - 1); /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Decode 2 pulses with 2*N+1 bits:                      *
     *-------------------------------------------------------*/
    /* pos1 = (((index >> N) & mask) + offset); */
    pos1 = extract_l(((index >> N) & mask) + L_deposit_l(offset));
    tmp = N << 1;
    i = (index >> tmp) & 1L;                /* i = (index >> (2*N)) & 1; */
    pos2 = extract_l(index & mask) + offset;        ;/* pos2 = ((index & mask) + offset); */
    if (pos2 < pos1)               /* ((pos2 - pos1) < 0) */
    {
        if (i == 1L)
        {                                  /* (i == 1) */
            pos1 += NB_POS;      /* pos1 += NB_POS; */
        } else
        {
            pos2 += NB_POS;      /* pos2 += NB_POS;	 */
        }
    } else
    {
        if (i == 1L)
        {                                  /* (i == 1) */
            pos1 += NB_POS;      /* pos1 += NB_POS; */
            pos2 += NB_POS;      /* pos2 += NB_POS; */
        }
    }

    pos[0] = pos1;
    pos[1] = pos2;

    return;
    
#else

    Word16 pos1, pos2, tmp;
    Word32 mask, i;

    mask = L_deposit_l(sub(shl(1, N), 1)); /* mask = ((1<<N)-1); */
    /*-------------------------------------------------------*
     * Decode 2 pulses with 2*N+1 bits:                      *
     *-------------------------------------------------------*/
    /* pos1 = (((index >> N) & mask) + offset); */
    pos1 = extract_l(L_add((L_shr(index, N) & mask), L_deposit_l(offset)));     logic16();
    tmp = shl(N, 1);
    i = (L_shr(index, tmp) & 1L);          logic16();/* i = (index >> (2*N)) & 1; */
    pos2 = add(extract_l(index & mask), offset);        logic16();/* pos2 = ((index & mask) + offset); */
    
    if (sub(pos2, pos1) < 0)               /* ((pos2 - pos1) < 0) */
    {
        
        if (L_sub(i, 1L) == 0)
        {                                  /* (i == 1) */
            pos1 = add(pos1, NB_POS);      /* pos1 += NB_POS; */
        } else
        {
            pos2 = add(pos2, NB_POS);      /* pos2 += NB_POS;	 */
        }
    } else
    {
        
        if (L_sub(i, 1L) == 0)
        {                                  /* (i == 1) */
            pos1 = add(pos1, NB_POS);      /* pos1 += NB_POS; */
            pos2 = add(pos2, NB_POS);      /* pos2 += NB_POS; */
        }
    }

    pos[0] = pos1;                         
    pos[1] = pos2;                         

    return;
#endif
}


Word32 voAMRWBPDecQuant_3p_3N1(                       /* (o) return (3*N)+1 bits         */
     Word16 pos1,                          /* (i) position of the pulse 1     */
     Word16 pos2,                          /* (i) position of the pulse 2     */
     Word16 pos3,                          /* (i) position of the pulse 3     */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 nb_pos;
    Word32 index;

    nb_pos = 1 << (N - 1);            /* nb_pos = (1<<(N-1)); */
    /*-------------------------------------------------------*
     * Quantization of 3 pulses with 3*N+1 bits:             *
     *-------------------------------------------------------*/
    if (((pos1 ^ pos2) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, (N-1)); */
        /* index += (pos1 & nb_pos) << N; */
        index += L_deposit_l((Word16)(pos1 & nb_pos)) << N;
        /* index += voAMRWBPDecQuant_1p_N1(pos3, N) << (2*N); */
        index += L_shl(voAMRWBPDecQuant_1p_N1(pos3, N), (Word16)(N << 1));

    } else if (((pos1 ^ pos3) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos3, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos1, pos3, (N-1)); */
        index += L_deposit_l((Word16)(pos1 & nb_pos)) << N;
        /* index += (pos1 & nb_pos) << N; */
        index += voAMRWBPDecQuant_1p_N1(pos2, N) << (N << 1);
        /* index += voAMRWBPDecQuant_1p_N1(pos2, N) <<
                                                                         * (2*N); */
    } else
    {
        index = voAMRWBPDecQuant_2p_2N1(pos2, pos3, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos2, pos3, (N-1)); */
        /* index += (pos2 & nb_pos) << N;			 */
        index += L_deposit_l((Word16)(pos2 & nb_pos)) << N;
        /* index += voAMRWBPDecQuant_1p_N1(pos1, N) << (2*N);	 */
        index += voAMRWBPDecQuant_1p_N1(pos1, N) << (N << 1);
    }
    return (index);
    
#else

    Word16 nb_pos;
    Word32 index;

    nb_pos = shl(1, sub(N, 1));            /* nb_pos = (1<<(N-1)); */
    /*-------------------------------------------------------*
     * Quantization of 3 pulses with 3*N+1 bits:             *
     *-------------------------------------------------------*/
    logic16();logic16();logic16();logic16();
    if (((pos1 ^ pos2) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, (N-1)); */
        /* index += (pos1 & nb_pos) << N; */
        index = L_add(index, L_shl(L_deposit_l((Word16) (pos1 & nb_pos)), N));  logic16();
        /* index += voAMRWBPDecQuant_1p_N1(pos3, N) << (2*N); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_1p_N1(pos3, N), shl(N, 1)));

    } else if (((pos1 ^ pos3) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos3, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos1, pos3, (N-1)); */
        index = L_add(index, L_shl(L_deposit_l((Word16) (pos1 & nb_pos)), N));  logic16();
        /* index += (pos1 & nb_pos) << N; */
        index = L_add(index, L_shl(voAMRWBPDecQuant_1p_N1(pos2, N), shl(N, 1)));
        /* index += voAMRWBPDecQuant_1p_N1(pos2, N) <<
                                                                         * (2*N); */
    } else
    {
        index = voAMRWBPDecQuant_2p_2N1(pos2, pos3, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos2, pos3, (N-1)); */
        /* index += (pos2 & nb_pos) << N;			 */
        index = L_add(index, L_shl(L_deposit_l((Word16) (pos2 & nb_pos)), N));  logic16();
        /* index += voAMRWBPDecQuant_1p_N1(pos1, N) << (2*N);	 */
        index = L_add(index, L_shl(voAMRWBPDecQuant_1p_N1(pos1, N), shl(N, 1)));
    }
    return (index);
#endif
}

void voAMRWBPDecDec_3p_3N1(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)

    Word16 j, tmp;
    Word32 mask, idx;

    /*-------------------------------------------------------*
     * Decode 3 pulses with 3*N+1 bits:                      *
     *-------------------------------------------------------*/
    tmp = (N << 1) - 1;               /* mask = ((1<<((2*N)-1))-1); */
    mask = (1L << tmp) - 1L;

    idx = index & mask;
    j = offset;
    tmp = (N << 1) - 1;

    if (((index >> tmp) & 1L) != 0L)
    {                                      /* if (((index >> ((2*N)-1)) & 1) == 1){ */
        j += 1 << (N - 1);     /* j += (1<<(N-1)); */
    }
    voAMRWBPDecDec_2p_2N1(idx, (Word16) (N - 1), j, pos);

    mask = (1 << (N + 1)) - 1;      /* mask = ((1<<(N+1))-1); */
    tmp = N << 1;                       /* idx = (index >> (2*N)) & mask; */
    idx = (index >> tmp) & mask;

    voAMRWBPDecDec_1p_N1(idx, N, offset, pos + 2);

#else

    Word16 j, tmp;
    Word32 mask, idx;

    /*-------------------------------------------------------*
     * Decode 3 pulses with 3*N+1 bits:                      *
     *-------------------------------------------------------*/
    tmp = sub(shl(N, 1), 1);               /* mask = ((1<<((2*N)-1))-1); */
    mask = L_sub(L_shl(1L, tmp), 1L);

    idx = index & mask;                    logic16();
    j = offset;
    tmp = sub(shl(N, 1), 1);

    logic16();
    if ((L_shr(index, tmp) & 1L) != 0L)
    {                                      /* if (((index >> ((2*N)-1)) & 1) == 1){ */
        j = add(j, shl(1, sub(N, 1)));     /* j += (1<<(N-1)); */
    }
    voAMRWBPDecDec_2p_2N1(idx, (Word16) (N - 1), j, pos);

    mask = sub(shl(1, add(N, 1)), 1);      /* mask = ((1<<(N+1))-1); */
    tmp = shl(N, 1);                       /* idx = (index >> (2*N)) & mask; */
    idx = L_shr(index, tmp) & mask;        logic16();

    voAMRWBPDecDec_1p_N1(idx, N, offset, pos + 2);    

    return;
#endif
}


Word32 voAMRWBPDecQuant_4p_4N1(                       /* (o) return (4*N)+1 bits         */
     Word16 pos1,                          /* (i) position of the pulse 1     */
     Word16 pos2,                          /* (i) position of the pulse 2     */
     Word16 pos3,                          /* (i) position of the pulse 3     */
     Word16 pos4,                          /* (i) position of the pulse 4     */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 nb_pos;
    Word32 index;

    nb_pos = 1 << (N - 1);            /* nb_pos = (1<<(N-1));  */
    /*-------------------------------------------------------*
     * Quantization of 4 pulses with 4*N+1 bits:             *
     *-------------------------------------------------------*/
    if (((pos1 ^ pos2) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, (N-1)); */
        /* index += (pos1 & nb_pos) << N;	 */
        index += L_deposit_l((Word16)(pos1 & nb_pos)) << N;
        /* index += voAMRWBPDecQuant_2p_2N1(pos3, pos4, N) << (2*N); */
        index += voAMRWBPDecQuant_2p_2N1(pos3, pos4, N) << (N << 1);
    } else if (((pos1 ^ pos3) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos3, sub(N, 1));
        /* index += (pos1 & nb_pos) << N; */
        index += L_deposit_l((Word16) (pos1 & nb_pos)) << N;
        /* index += voAMRWBPDecQuant_2p_2N1(pos2, pos4, N) << (2*N); */
        index += voAMRWBPDecQuant_2p_2N1(pos2, pos4, N) << (N << 1);
    } else
    {
        index = voAMRWBPDecQuant_2p_2N1(pos2, pos3, sub(N, 1));
        /* index += (pos2 & nb_pos) << N; */
        index += L_deposit_l((Word16) (pos2 & nb_pos)) << N;
        /* index += voAMRWBPDecQuant_2p_2N1(pos1, pos4, N) << (2*N); */
        index += voAMRWBPDecQuant_2p_2N1(pos1, pos4, N) << (N << 1);
    }
    return (index);
    
#else

    Word16 nb_pos;
    Word32 index;

    nb_pos = shl(1, sub(N, 1));            /* nb_pos = (1<<(N-1));  */
    /*-------------------------------------------------------*
     * Quantization of 4 pulses with 4*N+1 bits:             *
     *-------------------------------------------------------*/
    logic16();logic16();logic16();logic16();
    if (((pos1 ^ pos2) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, sub(N, 1));    /* index = voAMRWBPDecQuant_2p_2N1(pos1, pos2, (N-1)); */
        /* index += (pos1 & nb_pos) << N;	 */
        index = L_add(index, L_shl(L_deposit_l((Word16) (pos1 & nb_pos)), N));  logic16();
        /* index += voAMRWBPDecQuant_2p_2N1(pos3, pos4, N) << (2*N); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_2p_2N1(pos3, pos4, N), shl(N, 1)));
    } else if (((pos1 ^ pos3) & nb_pos) == 0)
    {
        index = voAMRWBPDecQuant_2p_2N1(pos1, pos3, sub(N, 1));
        /* index += (pos1 & nb_pos) << N; */
        index = L_add(index, L_shl(L_deposit_l((Word16) (pos1 & nb_pos)), N));  logic16();
        /* index += voAMRWBPDecQuant_2p_2N1(pos2, pos4, N) << (2*N); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_2p_2N1(pos2, pos4, N), shl(N, 1)));
    } else
    {
        index = voAMRWBPDecQuant_2p_2N1(pos2, pos3, sub(N, 1));
        /* index += (pos2 & nb_pos) << N; */
        index = L_add(index, L_shl(L_deposit_l((Word16) (pos2 & nb_pos)), N));  logic16();
        /* index += voAMRWBPDecQuant_2p_2N1(pos1, pos4, N) << (2*N); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_2p_2N1(pos1, pos4, N), shl(N, 1)));
    }
    return (index);
#endif
}

void voAMRWBPDecDec_4p_4N1(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 j, tmp;
    Word32 mask, idx;

    /*-------------------------------------------------------*
     * Decode 4 pulses with 4*N+1 bits:                      *
     *-------------------------------------------------------*/
    tmp = (N << 1) - 1;               /* mask = ((1<<((2*N)-1))-1); */
    mask = (1L << tmp) - 1L;
    idx = index & mask;
    j = offset;
    tmp = (N << 1) - 1;

    if (((index >> tmp) & 1L) != 0L)
    {                                      /* (((index >> ((2*N)-1)) & 1) == 1) */
        j += 1 <<(N - 1);     /* j += (1<<(N-1)); */
    }
    voAMRWBPDecDec_2p_2N1(idx, (Word16) (N - 1), j, pos);


    tmp = (N << 1) + 1;               /* mask = ((1<<((2*N)+1))-1); */
    mask = (1L << tmp) - 1L;
    idx = (index >> (N << 1)) & mask;  /* idx = (index >> (2*N)) & mask; */
    voAMRWBPDecDec_2p_2N1(idx, N, offset, pos + 2);   /* voAMRWBPDecDec_2p_2N1(idx, N, offset, pos+2); */

    return;
    
#else

    Word16 j, tmp;
    Word32 mask, idx;

    /*-------------------------------------------------------*
     * Decode 4 pulses with 4*N+1 bits:                      *
     *-------------------------------------------------------*/
    tmp = sub(shl(N, 1), 1);               /* mask = ((1<<((2*N)-1))-1); */
    mask = L_sub(L_shl(1L, tmp), 1L);
    idx = index & mask;                    logic16();
    j = offset;                            
    tmp = sub(shl(N, 1), 1);

    logic16();
    if ((L_shr(index, tmp) & 1L) != 0L)
    {                                      /* (((index >> ((2*N)-1)) & 1) == 1) */
        j = add(j, shl(1, sub(N, 1)));     /* j += (1<<(N-1)); */
    }
    voAMRWBPDecDec_2p_2N1(idx, (Word16) (N - 1), j, pos);


    tmp = add(shl(N, 1), 1);               /* mask = ((1<<((2*N)+1))-1); */
    mask = L_sub(L_shl(1L, tmp), 1L);
    idx = L_shr(index, shl(N, 1)) & mask;  logic16();/* idx = (index >> (2*N)) & mask; */
    voAMRWBPDecDec_2p_2N1(idx, N, offset, pos + 2);     /* voAMRWBPDecDec_2p_2N1(idx, N, offset, pos+2); */

    return;
#endif
}


Word32 voAMRWBPDecQuant_4p_4N(                        /* (o) return 4*N bits             */
     Word16 pos[],                         /* (i) position of the pulse 1..4  */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 i, j, k, nb_pos, mask, n_1, tmp;
    Word16 posA[4], posB[4];
    Word32 index;

    n_1 = (Word16) (N - 1);
    nb_pos = (1 << n_1);                  /* nb_pos = (1<<n_1); */
    mask = (1 << N) - 1;              /* mask = ((1<<N)-1); */

    i = 0;
    j = 0;
    for (k = 0; k < 4; k++)
    {
        if ((pos[k] & nb_pos) == 0)
        {
            posA[i++] = pos[k];
        } else
        {
            posB[j++] = pos[k];
        }
    }

    switch (i)
    {
    case 0:
        tmp = (N << 2) - 3;           /* index = 1 << ((4*N)-3); */
        index = 1L << tmp;
        /* index += voAMRWBPDecQuant_4p_4N1(posB[0], posB[1], posB[2], posB[3], n_1); */
        index += voAMRWBPDecQuant_4p_4N1(posB[0], posB[1], posB[2], posB[3], n_1);
        break;
    case 1:
        /* index = voAMRWBPDecQuant_1p_N1(posA[0], n_1) << ((3*n_1)+1); */
        //tmp = extract_l(L_shr(L_mult(3, n_1), 1)) + 1;
        tmp = extract_l(3 * n_1) + 1;
        index = voAMRWBPDecQuant_1p_N1(posA[0], n_1) << tmp;
        /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1); */
        index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1);
        break;
    case 2:
        tmp = (n_1 << 1) + 1;         /* index = voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1) << ((2*n_1)+1); */
        index = voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1) << tmp;
        /* index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1); */
        index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1);
        break;
    case 3:
        /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << N; */
        index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << N;
        index += voAMRWBPDecQuant_1p_N1(posB[0], n_1);        /* index += voAMRWBPDecQuant_1p_N1(posB[0], n_1); */
        break;
    case 4:
        index = voAMRWBPDecQuant_4p_4N1(posA[0], posA[1], posA[2], posA[3], n_1);
        break;
    default:
        index = 0;
        //AMRWBPLUS_FPRINTF(stderr, "Error in function voAMRWBPDecQuant_4p_4N\n");
    }
    tmp = (N << 2) - 2;               /* index += (i & 3) << ((4*N)-2); */
    index += (L_deposit_l(i) & (3L)) << tmp;

    return (index);
    
#else

    Word16 i, j, k, nb_pos, mask, n_1, tmp;
    Word16 posA[4], posB[4];
    Word32 index;

    n_1 = (Word16) (N - 1);                
    nb_pos = shl(1, n_1);                  /* nb_pos = (1<<n_1); */
    mask = sub(shl(1, N), 1);              /* mask = ((1<<N)-1); */

    i = 0;                                 
    j = 0;                                 
    for (k = 0; k < 4; k++)
    {
        logic16();
        if ((pos[k] & nb_pos) == 0)
        {
            posA[i++] = pos[k];            
        } else
        {
            posB[j++] = pos[k];            
        }
    }

    switch (i)
    {
    case 0:
        tmp = sub(shl(N, 2), 3);           /* index = 1 << ((4*N)-3); */
        index = L_shl(1L, tmp);
        /* index += voAMRWBPDecQuant_4p_4N1(posB[0], posB[1], posB[2], posB[3], n_1); */
        index = L_add(index, voAMRWBPDecQuant_4p_4N1(posB[0], posB[1], posB[2], posB[3], n_1));
        break;
    case 1:
        /* index = voAMRWBPDecQuant_1p_N1(posA[0], n_1) << ((3*n_1)+1); */
        tmp = add(extract_l(L_shr(L_mult(3, n_1), 1)), 1);
        index = L_shl(voAMRWBPDecQuant_1p_N1(posA[0], n_1), tmp);
        /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1); */
        index = L_add(index, voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1));
        break;
    case 2:
        tmp = add(shl(n_1, 1), 1);         /* index = voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1) << ((2*n_1)+1); */
        index = L_shl(voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1), tmp);
        /* index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1); */
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1));
        break;
    case 3:
        /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << N; */
        index = L_shl(voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1), N);
        index = L_add(index, voAMRWBPDecQuant_1p_N1(posB[0], n_1));        /* index += voAMRWBPDecQuant_1p_N1(posB[0], n_1); */
        break;
    case 4:
        index = voAMRWBPDecQuant_4p_4N1(posA[0], posA[1], posA[2], posA[3], n_1);
        break;
    default:
        index = 0;
        //AMRWBPLUS_FPRINTF(stderr, "Error in function voAMRWBPDecQuant_4p_4N\n");
    }
    tmp = sub(shl(N, 2), 2);               /* index += (i & 3) << ((4*N)-2); */
    index = L_add(index, L_shl((L_deposit_l(i) & (3L)), tmp));  logic16();

    return (index);
#endif
}

void voAMRWBPDecDec_4p_4N(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 j, n_1, tmp;

    /*-------------------------------------------------------*
     * Decode 4 pulses with 4*N bits:                        *
     *-------------------------------------------------------*/

    n_1 = (Word16) (N - 1);                
    j = offset + (1 << n_1);          /* j = offset + (1 << n_1); */

    tmp = (N << 2) - 2;
    switch ((index >> tmp) & 3)
    {                                      /* ((index >> ((4*N)-2)) & 3) */
    case 0:
        tmp = (n_1 << 2) + 1;

        if (((index >> tmp) & 1) == 0)
        {                                  /* (((index >> ((4*n_1)+1)) & 1) == 0) */
            voAMRWBPDecDec_4p_4N1(index, n_1, offset, pos);
        } else
        {
            voAMRWBPDecDec_4p_4N1(index, n_1, j, pos);
        }
        break;
    case 1:
        tmp = extract_l(3 * n_1) + 1;       /* voAMRWBPDecDec_1p_N1((index>>((3*n_1)+1)), n_1, offset, pos) */
        voAMRWBPDecDec_1p_N1((index >> tmp), n_1, offset, pos);
        voAMRWBPDecDec_3p_3N1(index, n_1, j, pos + 1);
        break;
    case 2:
        tmp = (n_1 << 1) + 1;         /* voAMRWBPDecDec_2p_2N1((index>>((2*n_1)+1)), n_1, offset, pos); */
        voAMRWBPDecDec_2p_2N1((index >> tmp), n_1, offset, pos);
        voAMRWBPDecDec_2p_2N1(index, n_1, j, pos + 2);
        break;
    case 3:
        tmp = n_1 + 1;                 /* voAMRWBPDecDec_3p_3N1((index>>(n_1+1)), n_1, offset, pos); */
        voAMRWBPDecDec_3p_3N1((index >> tmp), n_1, offset, pos);
        voAMRWBPDecDec_1p_N1(index, n_1, j, pos + 3);
        break;
    }
    return;
    
#else

    Word16 j, n_1, tmp;

    /*-------------------------------------------------------*
     * Decode 4 pulses with 4*N bits:                        *
     *-------------------------------------------------------*/

    n_1 = (Word16) (N - 1);                
    j = add(offset, shl(1, n_1));          /* j = offset + (1 << n_1); */

    tmp = sub(shl(N, 2), 2);
    logic16();
    switch (L_shr(index, tmp) & 3)
    {                                      /* ((index >> ((4*N)-2)) & 3) */
    case 0:
        tmp = add(shl(n_1, 2), 1);

        logic16();
        if ((L_shr(index, tmp) & 1) == 0)
        {                                  /* (((index >> ((4*n_1)+1)) & 1) == 0) */
            voAMRWBPDecDec_4p_4N1(index, n_1, offset, pos);
        } else
        {
            voAMRWBPDecDec_4p_4N1(index, n_1, j, pos);
        }
        break;
    case 1:
        tmp = add(extract_l(L_shr(L_mult(3, n_1), 1)), 1); /* voAMRWBPDecDec_1p_N1((index>>((3*n_1)+1)), n_1, offset, pos) */
        voAMRWBPDecDec_1p_N1(L_shr(index, tmp), n_1, offset, pos);
        voAMRWBPDecDec_3p_3N1(index, n_1, j, pos + 1);
        break;
    case 2:
        tmp = add(shl(n_1, 1), 1);         /* voAMRWBPDecDec_2p_2N1((index>>((2*n_1)+1)), n_1, offset, pos); */
        voAMRWBPDecDec_2p_2N1(L_shr(index, tmp), n_1, offset, pos);
        voAMRWBPDecDec_2p_2N1(index, n_1, j, pos + 2);
        break;
    case 3:
        tmp = add(n_1, 1);                 /* voAMRWBPDecDec_3p_3N1((index>>(n_1+1)), n_1, offset, pos); */
        voAMRWBPDecDec_3p_3N1(L_shr(index, tmp), n_1, offset, pos);
        voAMRWBPDecDec_1p_N1(index, n_1, j, pos + 3); 
        break;
    }
    return;
#endif
}


Word32 voAMRWBPDecQuant_5p_5N(                        /* (o) return 5*N bits             */
     Word16 pos[],                         /* (i) position of the pulse 1..5  */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 i, j, k, nb_pos, n_1, tmp;
    Word16 posA[5], posB[5];
    Word32 index, tmp2;

    n_1 = (Word16) (N - 1);
    nb_pos = (1 << n_1);                  /* nb_pos = (1<<n_1); */

    i = 0;
    j = 0;
    for (k = 0; k < 5; k++)
    {
        if ((pos[k] & nb_pos) == 0)
        {
            posA[i++] = pos[k];
        } else
        {
            posB[j++] = pos[k];
        }
    }

    switch (i)
    {
    case 0:
        tmp = extract_l(5 * N) - 1;        /* ((5*N)-1)) */
        index = 1L << tmp;   /* index = 1 << ((5*N)-1); */
        tmp = (N << 1) + 1;  /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << ((2*N)+1);*/
        tmp2 = voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << tmp;
        index += tmp2;
        index += voAMRWBPDecQuant_2p_2N1(posB[3], posB[4], N);        /* index += voAMRWBPDecQuant_2p_2N1(posB[3], posB[4], N); */
        break;
    case 1:
        tmp = extract_l(5 * N) - 1;        /* index = 1 << ((5*N)-1); */
        index = 1L << tmp;
        tmp = (N << 1) + 1;   /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) <<((2*N)+1);  */
        tmp2 = voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << tmp;
        index += tmp2;
        index += voAMRWBPDecQuant_2p_2N1(posB[3], posA[0], N);        /* index += voAMRWBPDecQuant_2p_2N1(posB[3], posA[0], N); */
        break;
    case 2:
        tmp = extract_l(5 * N) - 1;        /* ((5*N)-1)) */
        index = 1L << tmp;            /* index = 1 << ((5*N)-1); */
        tmp = (N << 1) + 1;           /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << ((2*N)+1);  */
        tmp2 = voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << tmp;
        index += tmp2;
        index += voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], N);        /* index += voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], N); */
        break;
    case 3:
        tmp = (N << 1) + 1;           /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((2*N)+1);  */
        index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << tmp;
        index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], N);        /* index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], N); */
        break;
    case 4:
        tmp = (N << 1) + 1;           /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((2*N)+1);  */
        index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << tmp;
        index += voAMRWBPDecQuant_2p_2N1(posA[3], posB[0], N);        /* index += voAMRWBPDecQuant_2p_2N1(posA[3], posB[0], N); */
        break;
    case 5:
        tmp = (N << 1) + 1;           /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((2*N)+1);  */
        index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << tmp;
        index += voAMRWBPDecQuant_2p_2N1(posA[3], posA[4], N);        /* index += voAMRWBPDecQuant_2p_2N1(posA[3], posA[4], N); */
        break;
    default:
        index = 0;
        //AMRWBPLUS_FPRINTF(stderr, "Error in function voAMRWBPDecQuant_5p_5N\n");
    }

    return (index);
    
#else

    Word16 i, j, k, nb_pos, n_1, tmp;
    Word16 posA[5], posB[5];
    Word32 index, tmp2;

    n_1 = (Word16) (N - 1);                
    nb_pos = shl(1, n_1);                  /* nb_pos = (1<<n_1); */

    i = 0;                                 
    j = 0;                                 
    for (k = 0; k < 5; k++)
    {
        logic16();
        if ((pos[k] & nb_pos) == 0)
        {
            posA[i++] = pos[k];            
        } else
        {
            posB[j++] = pos[k];            
        }
    }

    switch (i)
    {
    case 0:
        tmp = sub(extract_l(L_shr(L_mult(5, N), 1)), 1);        /* ((5*N)-1)) */
        index = L_shl(1L, tmp);   /* index = 1 << ((5*N)-1); */
        tmp = add(shl(N, 1), 1);  /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << ((2*N)+1);*/
        tmp2 = L_shl(voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1), tmp);
        index = L_add(index, tmp2);
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posB[3], posB[4], N));        /* index += voAMRWBPDecQuant_2p_2N1(posB[3], posB[4], N); */
        break;
    case 1:
        tmp = sub(extract_l(L_shr(L_mult(5, N), 1)), 1);        /* index = 1 << ((5*N)-1); */
        index = L_shl(1L, tmp);
        tmp = add(shl(N, 1), 1);   /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) <<((2*N)+1);  */
        tmp2 = L_shl(voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1), tmp);
        index = L_add(index, tmp2);
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posB[3], posA[0], N));        /* index += voAMRWBPDecQuant_2p_2N1(posB[3], posA[0], N); */
        break;
    case 2:
        tmp = sub(extract_l(L_shr(L_mult(5, N), 1)), 1);        /* ((5*N)-1)) */
        index = L_shl(1L, tmp);            /* index = 1 << ((5*N)-1); */
        tmp = add(shl(N, 1), 1);           /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1) << ((2*N)+1);  */
        tmp2 = L_shl(voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1), tmp);
        index = L_add(index, tmp2);
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], N));        /* index += voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], N); */
        break;
    case 3:
        tmp = add(shl(N, 1), 1);           /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((2*N)+1);  */
        index = L_shl(voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1), tmp);
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], N));        /* index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], N); */
        break;
    case 4:
        tmp = add(shl(N, 1), 1);           /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((2*N)+1);  */
        index = L_shl(voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1), tmp);
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posA[3], posB[0], N));        /* index += voAMRWBPDecQuant_2p_2N1(posA[3], posB[0], N); */
        break;
    case 5:
        tmp = add(shl(N, 1), 1);           /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((2*N)+1);  */
        index = L_shl(voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1), tmp);
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posA[3], posA[4], N));        /* index += voAMRWBPDecQuant_2p_2N1(posA[3], posA[4], N); */
        break;
    default:
        index = 0;
        //AMRWBPLUS_FPRINTF(stderr, "Error in function voAMRWBPDecQuant_5p_5N\n");
    }

    return (index);
#endif
}

void voAMRWBPDecDec_5p_5N(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 j, n_1, tmp;
    Word32 idx;

    /*-------------------------------------------------------*
     * Decode 5 pulses with 5*N bits:                        *
     *-------------------------------------------------------*/

    n_1 = (Word16) (N - 1);
    j = offset + (1 << n_1);          /* j = offset + (1 << n_1); */
    tmp = (N << 1) + 1;               /* idx = (index >> ((2*N)+1)); */
    idx = index >> tmp;
    tmp = extract_l(5 * N) - 1;    /* ((5*N)-1)) */

    if (((index >> tmp) & 1) == 0)      /* ((index >> ((5*N)-1)) & 1)  */
    {
        voAMRWBPDecDec_3p_3N1(idx, n_1, offset, pos);
        voAMRWBPDecDec_2p_2N1(index, N, offset, pos + 3);  
    } else
    {
        voAMRWBPDecDec_3p_3N1(idx, n_1, j, pos);
        voAMRWBPDecDec_2p_2N1(index, N, offset, pos + 3);  
    }
    return;

#else
    Word16 j, n_1, tmp;
    Word32 idx;

    /*-------------------------------------------------------*
     * Decode 5 pulses with 5*N bits:                        *
     *-------------------------------------------------------*/

    n_1 = (Word16) (N - 1);                
    j = add(offset, shl(1, n_1));          /* j = offset + (1 << n_1); */
    tmp = add(shl(N, 1), 1);               /* idx = (index >> ((2*N)+1)); */
    idx = L_shr(index, tmp);
    tmp = sub(extract_l(L_shr(L_mult(5, N), 1)), 1);    /* ((5*N)-1)) */

    logic16();
    if ((L_shr(index, tmp) & 1) == 0)      /* ((index >> ((5*N)-1)) & 1)  */
    {
        voAMRWBPDecDec_3p_3N1(idx, n_1, offset, pos);
        voAMRWBPDecDec_2p_2N1(index, N, offset, pos + 3);  
    } else
    {
        voAMRWBPDecDec_3p_3N1(idx, n_1, j, pos);
        voAMRWBPDecDec_2p_2N1(index, N, offset, pos + 3);  
    }
    return;
#endif
}


Word32 voAMRWBPDecQuant_6p_6N_2(                      /* (o) return (6*N)-2 bits         */
     Word16 pos[],                         /* (i) position of the pulse 1..6  */
     Word16 N)                             /* (i) number of bits for position */
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 i, j, k, nb_pos, n_1;
    Word16 posA[6], posB[6];
    Word32 index;

    /* !!  N and n_1 are constants -> it doesn't need to be operated by Basic Operators */

    n_1 = (Word16) (N - 1);
    nb_pos = (1 << n_1);                  /* nb_pos = (1<<n_1); */

    i = 0;
    j = 0;
    for (k = 0; k < 6; k++)
    {
        if ((pos[k] & nb_pos) == 0)
        {
            posA[i++] = pos[k];
        } else
        {
            posB[j++] = pos[k];
        }
    }

    switch (i)
    {
    case 0:
        index = 1L << (Word16)(6 * N - 5);        /* index = 1 << ((6*N)-5); */
        index += voAMRWBPDecQuant_5p_5N(posB, n_1) << N; /* index += voAMRWBPDecQuant_5p_5N(posB, n_1) << N; */
        index += voAMRWBPDecQuant_1p_N1(posB[5], n_1);        /* index += voAMRWBPDecQuant_1p_N1(posB[5], n_1); */
        break;
    case 1:
        index = 1L << (Word16)(6 * N - 5);        /* index = 1 << ((6*N)-5); */
        index += voAMRWBPDecQuant_5p_5N(posB, n_1) << N; /* index += voAMRWBPDecQuant_5p_5N(posB, n_1) << N; */
        index += voAMRWBPDecQuant_1p_N1(posA[0], n_1);        /* index += voAMRWBPDecQuant_1p_N1(posA[0], n_1); */
        break;
    case 2:
        index = 1L << (Word16)(6 * N - 5);        /* index = 1 << ((6*N)-5); */
        /* index += voAMRWBPDecQuant_4p_4N(posB, n_1) << ((2*n_1)+1); */
        index += voAMRWBPDecQuant_4p_4N(posB, n_1) << (Word16)(2 * n_1 + 1);
        index += voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1);      /* index += voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1); */
        break;
    case 3:
        index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << (Word16)(3 * n_1 + 1);    /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((3*n_1)+1); */
        index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1);     /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1); */
        break;
    case 4:
        i = 2;
        index = voAMRWBPDecQuant_4p_4N(posA, n_1) << (Word16)(2 * n_1 + 1);  /* index = voAMRWBPDecQuant_4p_4N(posA, n_1) << ((2*n_1)+1); */
        index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1);      /* index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1); */
        break;
    case 5:
        i = 1;
        index = voAMRWBPDecQuant_5p_5N(posA, n_1) << N;       /* index = voAMRWBPDecQuant_5p_5N(posA, n_1) << N; */
        index += voAMRWBPDecQuant_1p_N1(posB[0], n_1);        /* index += voAMRWBPDecQuant_1p_N1(posB[0], n_1); */
        break;
    case 6:
        i = 0;
        index = voAMRWBPDecQuant_5p_5N(posA, n_1) << N;       /* index = voAMRWBPDecQuant_5p_5N(posA, n_1) << N; */
        index += voAMRWBPDecQuant_1p_N1(posA[5], n_1);        /* index += voAMRWBPDecQuant_1p_N1(posA[5], n_1); */
        break;
    default:
        index = 0;
        //AMRWBPLUS_FPRINTF(stderr, "Error in function voAMRWBPDecQuant_6p_6N_2\n");
    }
    index += (L_deposit_l(i) & 3L) << (Word16)(6 * N - 4);   /* index += (i & 3) << ((6*N)-4); */

    return (index);
    
#else

    Word16 i, j, k, nb_pos, n_1;
    Word16 posA[6], posB[6];
    Word32 index;

    /* !!  N and n_1 are constants -> it doesn't need to be operated by Basic Operators */

    n_1 = (Word16) (N - 1);                
    nb_pos = shl(1, n_1);                  /* nb_pos = (1<<n_1); */

    i = 0;                                 
    j = 0;                                 
    for (k = 0; k < 6; k++)
    {
        logic16();
        if ((pos[k] & nb_pos) == 0)
        {
            posA[i++] = pos[k];            
        } else
        {
            posB[j++] = pos[k];            
        }
    }

    switch (i)
    {
    case 0:
        index = L_shl(1L, (Word16) (6 * N - 5));        /* index = 1 << ((6*N)-5); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_5p_5N(posB, n_1), N)); /* index += voAMRWBPDecQuant_5p_5N(posB, n_1) << N; */
        index = L_add(index, voAMRWBPDecQuant_1p_N1(posB[5], n_1));        /* index += voAMRWBPDecQuant_1p_N1(posB[5], n_1); */
        break;
    case 1:
        index = L_shl(1L, (Word16) (6 * N - 5));        /* index = 1 << ((6*N)-5); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_5p_5N(posB, n_1), N)); /* index += voAMRWBPDecQuant_5p_5N(posB, n_1) << N; */
        index = L_add(index, voAMRWBPDecQuant_1p_N1(posA[0], n_1));        /* index += voAMRWBPDecQuant_1p_N1(posA[0], n_1); */
        break;
    case 2:
        index = L_shl(1L, (Word16) (6 * N - 5));        /* index = 1 << ((6*N)-5); */
        /* index += voAMRWBPDecQuant_4p_4N(posB, n_1) << ((2*n_1)+1); */
        index = L_add(index, L_shl(voAMRWBPDecQuant_4p_4N(posB, n_1), (Word16) (2 * n_1 + 1)));
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1));      /* index += voAMRWBPDecQuant_2p_2N1(posA[0], posA[1], n_1); */
        break;
    case 3:
        index = L_shl(voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1), (Word16) (3 * n_1 + 1));    /* index = voAMRWBPDecQuant_3p_3N1(posA[0], posA[1], posA[2], n_1) << ((3*n_1)+1); */
        index = L_add(index, voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1));     /* index += voAMRWBPDecQuant_3p_3N1(posB[0], posB[1], posB[2], n_1); */
        break;
    case 4:
        i = 2;                             
        index = L_shl(voAMRWBPDecQuant_4p_4N(posA, n_1), (Word16) (2 * n_1 + 1));  /* index = voAMRWBPDecQuant_4p_4N(posA, n_1) << ((2*n_1)+1); */
        index = L_add(index, voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1));      /* index += voAMRWBPDecQuant_2p_2N1(posB[0], posB[1], n_1); */
        break;
    case 5:
        i = 1;                             
        index = L_shl(voAMRWBPDecQuant_5p_5N(posA, n_1), N);       /* index = voAMRWBPDecQuant_5p_5N(posA, n_1) << N; */
        index = L_add(index, voAMRWBPDecQuant_1p_N1(posB[0], n_1));        /* index += voAMRWBPDecQuant_1p_N1(posB[0], n_1); */
        break;
    case 6:
        i = 0;                             
        index = L_shl(voAMRWBPDecQuant_5p_5N(posA, n_1), N);       /* index = voAMRWBPDecQuant_5p_5N(posA, n_1) << N; */
        index = L_add(index, voAMRWBPDecQuant_1p_N1(posA[5], n_1));        /* index += voAMRWBPDecQuant_1p_N1(posA[5], n_1); */
        break;
    default:
        index = 0;
        //AMRWBPLUS_FPRINTF(stderr, "Error in function voAMRWBPDecQuant_6p_6N_2\n");
    }
    index = L_add(index, L_shl((L_deposit_l(i) & 3L), (Word16) (6 * N - 4)));   logic16();/* index += (i & 3) << ((6*N)-4); */

    return (index);
#endif
}

void voAMRWBPDecDec_6p_6N_2(Word32 index, Word16 N, Word16 offset, Word16 pos[])
{
#if (FILE_Q_PULSE_FX_OPT)
    Word16 j, n_1, offsetA, offsetB;

    n_1 = (Word16) (N - 1);
    j = offset + (1 << n_1);          /* j = offset + (1 << n_1); */


    /* !!  N and n_1 are constants -> it doesn't need to be operated by Basic Operators */

    offsetA = offsetB = j;
    if (((index >> (Word16)(6 * N - 5)) & 1L) == 0)
    {                                      /* if (((index >> ((6*N)-5)) & 1) == 0) */
        offsetA = offset;
    } else
    {
        offsetB = offset;
    }

    switch ((index >> (Word16)(6 * N - 4)) & 3)
    {                                      /* (index >> ((6*N)-4)) & 3 */
    case 0:
        voAMRWBPDecDec_5p_5N((index >> N), n_1, offsetA, pos);  /* voAMRWBPDecDec_5p_5N(index>>N, n_1, offsetA, pos); */
        voAMRWBPDecDec_1p_N1(index, n_1, offsetA, pos + 5);
        break;
    case 1:
        voAMRWBPDecDec_5p_5N((index >> N), n_1, offsetA, pos);  /* voAMRWBPDecDec_5p_5N(index>>N, n_1, offsetA, pos); */
        voAMRWBPDecDec_1p_N1(index, n_1, offsetB, pos + 5);
        break;
    case 2:
        voAMRWBPDecDec_4p_4N((index >> (Word16)(2 * n_1 + 1)), n_1, offsetA, pos); /* voAMRWBPDecDec_4p_4N(index>>((2*n_1)+1 ), n_1, offsetA, pos); */
        voAMRWBPDecDec_2p_2N1(index, n_1, offsetB, pos + 4);
        break;
    case 3:
        voAMRWBPDecDec_3p_3N1((index >> (Word16)(3 * n_1 + 1)), n_1, offset, pos); /* voAMRWBPDecDec_3p_3N1(index>>((3*n_1)+ 1), n_1, offset, pos); */
        voAMRWBPDecDec_3p_3N1(index, n_1, j, pos + 3);
        break;
    }
    return;
    
#else

    Word16 j, n_1, offsetA, offsetB;

    n_1 = (Word16) (N - 1);                
    j = add(offset, shl(1, n_1));          /* j = offset + (1 << n_1); */


    /* !!  N and n_1 are constants -> it doesn't need to be operated by Basic Operators */

    offsetA = offsetB = j;                 
    logic16();
    if ((L_shr(index, (Word16) (6 * N - 5)) & 1L) == 0)
    {                                      /* if (((index >> ((6*N)-5)) & 1) == 0) */
        offsetA = offset;                  
    } else
    {
        offsetB = offset;                  
    }

    logic16();
    switch (L_shr(index, (Word16) (6 * N - 4)) & 3)
    {                                      /* (index >> ((6*N)-4)) & 3 */
    case 0:
        voAMRWBPDecDec_5p_5N(L_shr(index, N), n_1, offsetA, pos);  /* voAMRWBPDecDec_5p_5N(index>>N, n_1, offsetA, pos); */
        voAMRWBPDecDec_1p_N1(index, n_1, offsetA, pos + 5);        
        break;
    case 1:
        voAMRWBPDecDec_5p_5N(L_shr(index, N), n_1, offsetA, pos);  /* voAMRWBPDecDec_5p_5N(index>>N, n_1, offsetA, pos); */
        voAMRWBPDecDec_1p_N1(index, n_1, offsetB, pos + 5);        
        break;
    case 2:
        voAMRWBPDecDec_4p_4N(L_shr(index, (Word16) (2 * n_1 + 1)), n_1, offsetA, pos); /* voAMRWBPDecDec_4p_4N(index>>((2*n_1)+1 ), n_1, offsetA, pos); */
        voAMRWBPDecDec_2p_2N1(index, n_1, offsetB, pos + 4);       
        break;
    case 3:
        voAMRWBPDecDec_3p_3N1(L_shr(index, (Word16) (3 * n_1 + 1)), n_1, offset, pos); /* voAMRWBPDecDec_3p_3N1(index>>((3*n_1)+ 1), n_1, offset, pos); */
        voAMRWBPDecDec_3p_3N1(index, n_1, j, pos + 3);
        break;
    }
    return;
#endif
}
