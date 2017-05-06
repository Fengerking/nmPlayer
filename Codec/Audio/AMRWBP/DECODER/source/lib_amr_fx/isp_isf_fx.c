/*-------------------------------------------------------------------*
 *                         ISP_ISF.C								 *
 *-------------------------------------------------------------------*
 *   Isp_isf   Transformation isp to isf                             *
 *   Isf_isp   Transformation isf to isp                             *
 *                                                                   *
 * The transformation from isp[i] to isf[i] and isf[i] to isp[i] are *
 * approximated by a look-up table and interpolation.                *
 *-------------------------------------------------------------------*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "isp_isf.tab"                     /* Look-up table for transformations */


void voAMRWBPDecIsf_isp(
     Word16 isf[],                         /* (i) Q15 : isf[m] normalized (range: 0.0<=val<=0.5) */
     Word16 isp[],                         /* (o) Q15 : isp[m] (range: -1<=val<1)                */
     Word16 m                              /* (i)     : LPC order                                */
)
{
    Word16 i, ind, offset;
    Word32 L_tmp;

    for (i = 0; i < m - 1; i++)
    {
        isp[i] = isf[i];                   
    }
    isp[m - 1] = shl(isf[m - 1], 1);

    for (i = 0; i < m; i++)
    {
        ind = shr(isp[i], 7);              /* ind    = b7-b15 of isf[i] */
        offset = (Word16) (isp[i] & 0x007f);    logic16();  /* offset = b0-b6  of isf[i] */

        /* isp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 128 */

        L_tmp = L_mult(sub(table[ind + 1], table[ind]), offset);
        isp[i] = add(table[ind], extract_l(L_shr(L_tmp, 8)));   
    }
#if AMR_DUMP
  {
	Dumploop(AMR_DEBUG_Isf_isp,"after Isf_isp",M,isp,d16);
  }
#endif//AMR_DUMP
    return;
}
