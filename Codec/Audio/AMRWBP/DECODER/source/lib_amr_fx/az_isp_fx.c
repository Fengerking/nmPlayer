/*-----------------------------------------------------------------------*
 *                         Az_isp.C									     *
 *-----------------------------------------------------------------------*
 * Compute the ISPs from  the LPC coefficients  (order=M)                *
 *-----------------------------------------------------------------------*
 *                                                                       *
 * The ISPs are the roots of the two polynomials F1(z) and F2(z)         *
 * defined as                                                            *
 *               F1(z) = A(z) + z^-m A(z^-1)                             *
 *  and          F2(z) = A(z) - z^-m A(z^-1)                             *
 *                                                                       *
 * For a even order m=2n, F1(z) has M/2 conjugate roots on the unit      *
 * circle and F2(z) has M/2-1 conjugate roots on the unit circle in      *
 * addition to two roots at 0 and pi.                                    *
 *                                                                       *
 * For a 16th order LP analysis, F1(z) and F2(z) can be written as       *
 *                                                                       *
 *   F1(z) = (1 + a[M])   PRODUCT  (1 - 2 cos(w_i) z^-1 + z^-2 )         *
 *                        i=0,2,4,6,8,10,12,14                           *
 *                                                                       *
 *   F2(z) = (1 - a[M]) (1 - z^-2) PRODUCT (1 - 2 cos(w_i) z^-1 + z^-2 ) *
 *                                 i=1,3,5,7,9,11,13                     *
 *                                                                       *
 * The ISPs are the M-1 frequencies w_i, i=0...M-2 plus the last         *
 * predictor coefficient a[M].                                           *
 *-----------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
//#include "stdio.h"
#include "count.h"


