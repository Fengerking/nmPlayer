
#include "typedef.h"
#include "basic_op.h"
#include "amr_plus_fx.h"

/*---------------------------------------------------------------------*
 * routine int_lpc()                                                   *
 * ~~~~~~~~~~~~~~~~~                                                   *
 * Find the interpolated LPC parameters in every subframes.            *
 *---------------------------------------------------------------------*/
void Int_lpc(
  Word16 isp_old[],     /* (i)  : Old isp                     Q15 */
  Word16 isp_new[],     /* (i)  : New isp                     Q15 */
  const Word16 frac[],  /* (i)  : Interpolation window        Q15 */
  Word16 Az[],          /* (o)  : Interpolated lpc parameters Q12 */  
  Word16 n_subfr,       /* (i)  : Number of subframe              */
  Word16 m              /* (i)  : LPC Order                       */
)
{
  Word32 L_tmp;
  Word16 isp[M], fac_old, fac_new, i, k, m1;
  m1 = add(m,1); 
  for(k = 0; k < n_subfr; k++)
  {
    fac_new = frac[k];
    fac_old = add(sub(32767 ,fac_new) ,1);   /* 1.0 - fac_new */

    for(i = 0; i < m; i++)
    {
      L_tmp = L_mult(isp_old[i], fac_old);
      L_tmp = L_mac(L_tmp, isp_new[i], fac_new);
      isp[i] = vo_round(L_tmp); 
    }

    Isp_Az(isp, Az, m, 0);
    Az += (m1);
  }

  /* 4th subframe: isp_new (frac=1.0) */
  Isp_Az(isp_new, Az, m, 0);
  return;
}
