/*---------------------------------------------------------------------------*
 *                         LEVINSON.C										 *
 *---------------------------------------------------------------------------*
 *                                                                           *
 *      LEVINSON-DURBIN algorithm in double precision                        *
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        *
 *                                                                           *
 * Algorithm                                                                 *
 *                                                                           *
 *       R[i]    autocorrelations.                                           *
 *       A[i]    filter coefficients.                                        *
 *       K       reflection coefficients.                                    *
 *       Alpha   prediction gain.                                            *
 *                                                                           *
 *       Initialization:                                                     *
 *               A[0] = 1                                                    *
 *               K    = -R[1]/R[0]                                           *
 *               A[1] = K                                                    *
 *               Alpha = R[0] * (1-K**2]                                     *
 *                                                                           *
 *       Do for  i = 2 to M                                                  *
 *                                                                           *
 *            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]                      *
 *                                                                           *
 *            K = -S / Alpha                                                 *
 *                                                                           *
 *            An[j] = A[j] + K*A[i-j]   for j=1 to i-1                       *
 *                                      where   An[i] = new A[i]             *
 *            An[i]=K                                                        *
 *                                                                           *
 *            Alpha=Alpha * (1-K**2)                                         *
 *                                                                           *
 *       END                                                                 *
 *                                                                           *
 * Remarks on the dynamics of the calculations.                              *
 *                                                                           *
 *       The numbers used are in double precision in the following format :  *
 *       A = AH <<16 + AL<<1.  AH and AL are 16 bit signed integers.         *
 *       Since the LSB's also contain a sign bit, this format does not       *
 *       correspond to standard 32 bit integers.  We use this format since   *
 *       it allows fast execution of multiplications and divisions.          *
 *                                                                           *
 *       "DPF" will refer to this special format in the following text.      *
 *       See oper_32b.c                                                      *
 *                                                                           *
 *       The R[i] were normalized in routine AUTO (hence, R[i] < 1.0).       *
 *       The K[i] and Alpha are theoretically < 1.0.                         *
 *       The A[i], for a sampling frequency of 8 kHz, are in practice        *
 *       always inferior to 16.0.                                            *
 *                                                                           *
 *       These characteristics allow straigthforward fixed-point             *
 *       implementation.  We choose to represent the parameters as           *
 *       follows :                                                           *
 *                                                                           *
 *               R[i]    Q31   +- .99..                                      *
 *               K[i]    Q31   +- .99..                                      *
 *               Alpha   Normalized -> mantissa in Q31 plus exponent         *
 *               A[i]    Q27   +- 15.999..                                   *
 *                                                                           *
 *       The additions are performed in 32 bit.  For the summation used      *
 *       to calculate the K[i], we multiply numbers in Q31 by numbers        *
 *       in Q27, with the result of the multiplications in Q27,              *
 *       resulting in a dynamic of +- 16.  This is sufficient to avoid       *
 *       overflow, since the final result of the summation is                *
 *       necessarily < 1.0 as both the K[i] and Alpha are                    *
 *       theoretically < 1.0.                                                *
 *___________________________________________________________________________*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "acelp_fx.h"
#include "count.h"

#define M   16
#define NC  (M/2)


