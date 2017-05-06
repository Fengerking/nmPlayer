/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-------------------------------------------------------------------*
 * Function  Pred_lt_3()                                             *
 *           ~~~~~~~~~~~                                             *
 *-------------------------------------------------------------------*
 * Compute the result of long term prediction with fractional        *
 * interpolation of resolution 1/3.                                  *
 *                                                                   *
 * On return exc[0..L_subfr-1] contains the interpolated signal      *
 *   (adaptive codebook excitation)                                  *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "tab_ld8a.h"
void Pred_lt_30(
  Word16   exc[],       /* in/out: excitation buffer */
  Word16   T0,          /* input : integer pitch lag */
  Word16   frac,        /* input : fraction of lag   */
  Word16   L_subfr      /* input : subframe size     */
)
{
  Word16   i, j;
  Word16   *x0, *x1, *x2, *c1;
  Word32  s;

  x0 = &exc[-T0];
  c1 = &inter_3l[0];

  for (j=0; j<L_subfr; j++)
  {
    x1 = x0++;
    x2 = x0;
    s = 0;
    i = 0;
    
    s += x1[-i] * c1[0];
    s += (x2[i] + x1[-i-1]) * c1[3];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[6];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[9];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[12];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[15];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[18];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[21];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[24];
    i++;

    s += (x2[i]+ x1[-i-1]) * c1[27];
    i++;

    s += x2[i] * c1[30];

    exc[j] = (Word16)(((s <<1) + 0x8000) >>16);
  }

  return;
}

void Pred_lt_3(
  Word16   exc[],       /* in/out: excitation buffer */
  Word16   T0,          /* input : integer pitch lag */
  Word16   frac,        /* input : fraction of lag   */
  Word16   L_subfr      /* input : subframe size     */
)
{
  Word16   i, j, k;
  Word16   *x0, *x1, *x2, *c1, *c2;
  Word32  s;

  x0 = &exc[-T0];

  frac = -frac;
  if (frac < 0)
  {
    frac = frac + UP_SAMP;
    x0--;
  }

  c1 = &inter_3l[frac];
  c2 = &inter_3l[UP_SAMP - frac];

  for (j=0; j<L_subfr; j++)
  {
    x1 = x0++;
    x2 = x0;
 
    s = 0;
    i = 0;
    k = 0;
    
    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;

    s += x1[-i] * c1[k];
    s += x2[i] * c2[k];
    i++;
    k+=UP_SAMP;
    s += x1[-i] * c1[k]; 
    s += x2[i] * c2[k];
    exc[j] = (Word16)(((s <<1) + 0x8000) >>16);
  }

  return;
}

