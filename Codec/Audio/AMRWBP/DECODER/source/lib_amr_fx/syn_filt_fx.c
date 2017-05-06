/*-------------------------------------------------------------------*
 *                         SYN_FILT.C								 *
 *-------------------------------------------------------------------*
 * Do the synthesis filtering 1/A(z).							     *
 *-------------------------------------------------------------------*/
//#include "stdio.h"
#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "count.h"
#include "acelp_fx.h"
#include "cnst_wb_fx.h"


#if (LARGE_VAR_OPT)
static Word16 y_buf[L_FRAME16k + M16k];
#endif

void voAMRWBPDecSynFilt(
     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients           */
     Word16 m,                             /* (i)     : order of LP filter                       */
     Word16 x[],                           /* (i)     : input signal                             */
     Word16 y[],                           /* (o)     : output signal                            */
     Word16 lg,                            /* (i)     : size of filtering                        */
     Word16 mem[],                         /* (i/o)   : memory associated with this filtering.   */
     Word16 update                         /* (i)     : 0=no update, 1=update of memory.         */
)
{
#if (FUNC_SYN_FILT_OPT)

#if (!LARGE_VAR_OPT)
    Word16 y_buf[L_FRAME16k + M16k];
#endif
    Word16 i, j, a0, s;
    Word32 L_tmp;
    Word16 *yy;
    Word16 *temp_p = NULL;

    yy = y_buf;
    
    /* copy initial filter states into synthesis buffer */
    for (i = 0; i < m; i++)
    {
        *yy++ = mem[i];
    }
    s = norm_s(a[0]) - 2;
    a0 = a[0] >> 1;                     /* input / 2 */

    /* Do the filtering. */

    for (i = 0; i < m; i++)
    {
        L_tmp = x[i] * a0;
        temp_p = yy + i;
        for (j = 1; j <= m; j += 4)
        {
            L_tmp -= a[j    ] * (*(temp_p - j    ))
                   + a[j + 1] * (*(temp_p - j - 1))
                   + a[j + 2] * (*(temp_p - j - 2))
                   + a[j + 3] * (*(temp_p - j - 3));
        }
        L_tmp <<= (4 + s);
        y[i] = yy[i] = (L_tmp + 0x8000) >> 16;
    }
    for (i = m; i < lg; i++)
    {
        L_tmp = x[i] * a0;
        temp_p = yy + i;
        for (j = 1; j <= m; j += 4)
        {
            L_tmp -= a[j    ] * (*(temp_p - j    ))
                   + a[j + 1] * (*(temp_p - j - 1))
                   + a[j + 2] * (*(temp_p - j - 2))
                   + a[j + 3] * (*(temp_p - j - 3));
        }
        L_tmp <<= (4 + s);
        y[i] = yy[i] = (L_tmp + 0x8000) >> 16;
    }

    /* Update memory if required */
    if (update)
    {
        for (i = 0; i < m; i++)
        {
            mem[i] = yy[lg - m + i];
        }
    }

    return;

#else

#if (!LARGE_VAR_OPT)
  Word16 y_buf[L_FRAME16k + M16k];
#endif
    Word16 i, j, a0, s;
    Word32 L_tmp;
    Word16 *yy;

    yy = &y_buf[0];                        

    /* copy initial filter states into synthesis buffer */
    for (i = 0; i < m; i++)
    {
        *yy++ = mem[i];                    
    }
    s = sub(norm_s(a[0]), 2);
    a0 = shr(a[0], 1);                     /* input / 2 */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        L_tmp = L_mult(x[i], a0);

        for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, a[j], yy[i - j]);

        L_tmp = L_shl(L_tmp, add(3, s));

        y[i] = yy[i] = round16(L_tmp);       
    }

    /* Update memory if required */
    
    if (update)
        for (i = 0; i < m; i++)
        {
            mem[i] = yy[lg - m + i];       
        }

    return;
#endif
}


void voAMRWBPDecSyn_filt_32(
     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients */
     Word16 m,                             /* (i)     : order of LP filter             */
     Word16 exc[],                         /* (i) Qnew: excitation (exc[i] >> Qnew)    */
     Word16 Qnew,                          /* (i)     : exc scaling = 0(min) to 8(max) */
     Word16 sig_hi[],                      /* (o) /16 : synthesis high                 */
     Word16 sig_lo[],                      /* (o) /16 : synthesis low                  */
     Word16 lg                             /* (i)     : size of filtering              */
)
{
#if (FUNC_SYN_FILT_32_OPT)
    Word16 i, j, a0, s;
    Word32 L_tmp;
    
    Word16 k;
    Word16 *temp_p, *temp_p_2;

    s = norm_s(a[0]) - 2;
    a0 = a[0] >>  (4 + Qnew);          /* input / 16 and >>Qnew */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        temp_p = sig_lo + i;
        temp_p_2 = sig_hi + i;
#if (MAX_OPT)
        L_tmp = 0;
//        for (j = 1; j <= m; j++)
//        {
//            L_tmp -= (sig_lo[i - j] * a[j]);
//        }
        for (j = 1; j <= m; j += 8)
        {
            L_tmp -= (*(temp_p - j    ) * a[j    ])
                   + (*(temp_p - j - 1) * a[j + 1])
                   + (*(temp_p - j - 2) * a[j + 2])
                   + (*(temp_p - j - 3) * a[j + 3])
                   + (*(temp_p - j - 4) * a[j + 4])
                   + (*(temp_p - j - 5) * a[j + 5])
                   + (*(temp_p - j - 6) * a[j + 6])
                   + (*(temp_p - j - 7) * a[j + 7]);
        }

        L_tmp >>= 11;      /* -4 : sig_lo[i] << 4 */

        L_tmp += (exc[i] * a0) << 1;

        for (j = 1; j <= m; j++)
        {
            L_tmp -= (sig_hi[i - j] * a[j]) << 1;
        }

#else

        L_tmp = 0;
        for (j = 1; j <= m; j += 8)
        {
            L_tmp -= (*(temp_p - j    ) * a[j    ])
                   + (*(temp_p - j - 1) * a[j + 1])
                   + (*(temp_p - j - 2) * a[j + 2])
                   + (*(temp_p - j - 3) * a[j + 3])
                   + (*(temp_p - j - 4) * a[j + 4])
                   + (*(temp_p - j - 5) * a[j + 5])
                   + (*(temp_p - j - 6) * a[j + 6])
                   + (*(temp_p - j - 7) * a[j + 7]);
        }

        L_tmp >>= 11;      /* -4 : sig_lo[i] << 4 */

        L_tmp += (exc[i] * a0) << 1;

        for (j = 1; j <= m; j++)
        {
            L_tmp = L_sub(L_tmp, ((sig_hi[i - j] * a[j]) << 1));
        }
#endif
        
        /* sig_hi = bit16 to bit31 of synthesis */
        //L_tmp = L_shl(L_tmp, (3 + s));           /* ai in Q12 */
        k = s + 3;
        while (k-- > 0)
        {
            if (L_tmp > (Word32) 0X3fffffffL)
            {
                L_tmp = MAX_32;
                break;
            }
            else
            {
                if (L_tmp < (Word32) 0xc0000000L)
                {
                    L_tmp = MIN_32;
                    break;
                }
            }
            L_tmp <<= 1;
        }
        
        sig_hi[i] = L_tmp >> 16;

        /* sig_lo = bit4 to bit15 of synthesis */
        L_tmp >>= 4;           /* 4 : sig_lo[i] >> 4 */
        sig_lo[i] = L_tmp - (sig_hi[i] << 12);
    }

    return;

#else

    Word16 i, j, a0, s;
    Word32 L_tmp;

    s = sub(norm_s(a[0]), 2);
    a0 = shr(a[0], add(4, Qnew));          /* input / 16 and >>Qnew */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        L_tmp = 0;                         move32();
        for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, sig_lo[i - j], a[j]);

        L_tmp = L_shr(L_tmp, 16 - 4);      /* -4 : sig_lo[i] << 4 */

        L_tmp = L_mac(L_tmp, exc[i], a0);

        for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, sig_hi[i - j], a[j]);

        /* sig_hi = bit16 to bit31 of synthesis */
        L_tmp = L_shl(L_tmp, add(3, s));           /* ai in Q12 */
        sig_hi[i] = extract_h(L_tmp);      

        /* sig_lo = bit4 to bit15 of synthesis */
        L_tmp = L_shr(L_tmp, 4);           /* 4 : sig_lo[i] >> 4 */
        sig_lo[i] = extract_l(L_msu(L_tmp, sig_hi[i], 2048));   
    }

    return;
#endif
}

void voAMRWBPDecSyn_filt_s(
  Word16 shift,           /* (i) : scaling to apply                           Q0  */
  Word16 a[],             /* (i) : a[m+1] prediction coefficients             Q12 */
  Word16 m,               /* (i) : order of LP filter                         Q0  */
  Word16 x[],             /* (i) : input signal                               Qx  */
  Word16 y[],             /* (o) : output signal                              Qx-s*/  
  Word16 lg,              /* (i) : size of filtering                          Q0  */
  Word16 mem[],           /* (i/o) :memory associated with this filtering.    Qx-s*/
  Word16 update           /* (i) : 0=no update, 1=update of memory.           Q0  */
)
{
#if (FUNC_SYN_FILT_S_OPT)
  Word16 i, j, a0;
  Word32 L_tmp;
  Word16 *yy;
  Word16 y_buf[M16k << 1];

  Word16 *temp_p = NULL;
  
  yy = y_buf;

  //copy initial filter states into synthesis buffer
//  memcpy(yy, mem, m*sizeof(Word16));
//  yy += m;
  for (i = 0; i < m; i += 8)
  {
    *yy++ = mem[i];
    *yy++ = mem[i + 1];
    *yy++ = mem[i + 2];
    *yy++ = mem[i + 3];
    *yy++ = mem[i + 4];
    *yy++ = mem[i + 5];
    *yy++ = mem[i + 6];
    *yy++ = mem[i + 7];
  }
//  for (i = 0; i < m; i++)
//  {
//    *yy++ = mem[i];
//  }

//  a0 = shr(a[0], shift);           // input / 2^shift
  a0 = (shift >= 0) ? (a[0] >> shift) : (a[0] << (-shift));

  // Do the filtering.

  for (i = 0; i < m; i++)
  //for (i = 0; i < lg; i++)
  {
    L_tmp = x[i] * a0;
    temp_p = yy + i;
//    for (j = 1; j <= m; j++)
//    {
//        L_tmp -= (*(a + j)) * (*(temp_p - j));
//    }
    for (j = 1; j <= m; j += 8)
    {
        L_tmp -= (*(a + j    )) * (*(temp_p - j    ))
               + (*(a + j + 1)) * (*(temp_p - j - 1))
               + (*(a + j + 2)) * (*(temp_p - j - 2))
               + (*(a + j + 3)) * (*(temp_p - j - 3))
               + (*(a + j + 4)) * (*(temp_p - j - 4))
               + (*(a + j + 5)) * (*(temp_p - j - 5))
               + (*(a + j + 6)) * (*(temp_p - j - 6))
               + (*(a + j + 7)) * (*(temp_p - j - 7));
    }

    y[i] = yy[i] = (L_tmp + 0x800) >> 12;
  }
  for (i = m; i < lg; i++)
  {
    L_tmp = x[i] * a0;
    temp_p = y + i;
//    for (j = 1; j <= m; j++)
//    {
//        L_tmp -= (*(a + j)) * (*(temp_p - j));
//    }
    for (j = 1; j <= m; j += 8)
    {
        L_tmp -= (*(a + j    )) * (*(temp_p - j    ))
               + (*(a + j + 1)) * (*(temp_p - j - 1))
               + (*(a + j + 2)) * (*(temp_p - j - 2))
               + (*(a + j + 3)) * (*(temp_p - j - 3))
               + (*(a + j + 4)) * (*(temp_p - j - 4))
               + (*(a + j + 5)) * (*(temp_p - j - 5))
               + (*(a + j + 6)) * (*(temp_p - j - 6))
               + (*(a + j + 7)) * (*(temp_p - j - 7));
    }

    y[i] = (L_tmp + 0x800) >> 12;
  }

  //Update memory if required
  if (update)
  {
//    memcpy(mem, (y + lg - m), m * sizeof(Word16));

    yy = y + lg - m;
    for (i = 0; i < m; i += 8)
    {
        mem[i] = *(yy + i);
        mem[i + 1] = *(yy + i + 1);
        mem[i + 2] = *(yy + i + 2);
        mem[i + 3] = *(yy + i + 3);
        mem[i + 4] = *(yy + i + 4);
        mem[i + 5] = *(yy + i + 5);
        mem[i + 6] = *(yy + i + 6);
        mem[i + 7] = *(yy + i + 7);
    }

//    for (i = 0; i < m; i++)
//    {
//        mem[i] = y[lg - m + i];
//    }
  }

  return;

#else

  Word16 i, j, a0;
  Word32 L_tmp;
  Word16 *yy;
  Word16 y_buf[L_FRAME + M16k];

  yy = &y_buf[0];                        

  /* copy initial filter states into synthesis buffer */
  for (i = 0; i < m; i++)
  {
    *yy++ = mem[i];                    
  }
  a0 = shr(a[0], shift);           /* input / 2^shift */

  /* Do the filtering. */

  for (i = 0; i < lg; i++)
  {
    L_tmp = L_mult(x[i], a0);

    for (j = 1; j <= m; j++)
        L_tmp = L_msu(L_tmp, a[j], yy[i - j]);

    L_tmp = L_shl(L_tmp, 3);
    y[i] = yy[i] = round16(L_tmp);       
  }

  /* Update memory if required */
  
  if (update)
    for (i = 0; i < m; i++)
    {
        mem[i] = yy[lg - m + i];       
    }

  return;
#endif
// end
}
