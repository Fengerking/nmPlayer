
#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"


Word32 Get_gain(        /* o  : codebook gain (adaptive or fixed) */
  Word16 x[],           /* i  : target signal                     */
  Word16 y[],           /* i  : filtered codebook excitation      */
  Word16 n,             /* i  : segment length                    */
  Word16 Q,             /* i  L Scaling of x                      */
  /*Word32 *ener_y*/
  Word16 *mant_ener,    /* o  : Energy mantise of y               */
  Word16 *exp_ener      /* o  : Energy exponent of y              */
)
{
  Word32 corr, ener, tcorr, tener, Lgain;
  Word16 i, j, tmp16, *pt_x, *pt_y, e_corr, m_corr, e_ener, m_ener, negative;

  tcorr = 0; move32();
  tener = 0; move32();
  negative = 0; move16();

  pt_x = x;     move16();
  pt_y = y;     move16();

  /* Worst case: n = 1152 */
  for (j = 0;j< n; j += 16)
  { 
    corr = 0; move32();
    ener = 0; move32();
    for (i = 0; i < L_SUBFR/4; i++) 
    {
      tmp16 = mult(*pt_y,16384);
      corr = L_mac(corr, mult(*pt_x, 16384), tmp16);
      ener = L_mac(ener, tmp16, tmp16);
      pt_y++; pt_x++;
    }

    tcorr = L_add(tcorr, L_shr(corr, 2));
    tener = L_add(tener, L_shr(ener, 2));
  }

  test();
  if (tener == 0) 
  {
    tener = 1; move32();
  }
  
  test();
  if (tcorr <= 0)
  { 
    negative = 1;     move16();
    tcorr = L_negate(tcorr);
  }

  
  e_corr = norm_l(tcorr);
  m_corr = extract_h(L_shl(tcorr, e_corr));
  e_corr = sub(30, add(e_corr,Q));

  e_ener = norm_l(tener);
  m_ener = extract_h(L_shl(tener, e_ener));
  e_ener = sub(30, e_ener);
  *mant_ener = m_ener;    move16();
  *exp_ener = e_ener;    move16();

  test();
  if(sub(m_corr,m_ener)>0)
  {
    m_corr = shr(m_corr,1);
    e_corr = add(e_corr,1);
  }
  m_corr = div_s(m_corr, m_ener);
  e_corr = sub(e_corr, e_ener);

  Lgain = L_shl(m_corr, add(e_corr,1));    /* Lgain in Q16 */
  test();
  if (sub(negative,1) == 0)
  {
    Lgain = L_negate(Lgain);    /* Lgain in Q16 */
  }
  return Lgain;
}
