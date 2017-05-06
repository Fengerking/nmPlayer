/*_____________________________________________________________________
 |                                                                     |
 |  FUNCTION NAME segsnr                                               |
 |      Computes the segmential signal-to-noise ratio between the      |
 |      signal x and its estimate xe of length n samples. The segment  |
 |      length is nseg samples.                                        |
 |
 |  INPUT
 |      x[0:n-1]   Signal of n samples.
 |      xe[0:n-1]  Estimated signal of n samples.
 |      n          Signal length.
 |      nseg       Segment length, must be a submultiple of n.
 |
 |  RETURN VALUE
 |      snr        Segmential signal to noise ratio in dB.
 |_____________________________________________________________________|
*/

#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "log2.h"
#include "oper_32b.h"


Word16 Segsnr(Word16 x[],Word16 xe[], Word16 n, Word16 nseg)
{

  Word32 L_snr;
  Word16 m_snr, e_snr, norm_n, norm_nseg, scl;
  Word16 i, j, tmp16, sign;
  Word32 L_signal, L_noise, L_error, L_tmp1, L_tmp2;

  L_snr = 0;      move32();
  for (i = 0; i < n; i += nseg) 
  {
    sign = 0;      move16();
    L_signal = 1;   move32();
    L_noise  = 1;   move32();

    for (j = 0; j < nseg; j++) 
    {
      /*signal += (*x**x);*/
      tmp16 = mult(*x, 4096);
      L_signal = L_mac(L_signal, tmp16, tmp16);
      /*error  = *x++ - *xe++;*/
      L_error = L_mult(*x, 4096);
      L_error = L_msu(L_error, *xe, 4096);
      tmp16 = round(L_error);
      /*noise  += error*error;*/
      L_noise = L_mac(L_noise, tmp16, tmp16);
      x++;xe++;
    }
    /*snr += (float)log10((double)(signal/noise));*/
    Log2(L_signal,&e_snr,&m_snr);
    L_tmp1 = Mpy_32_16(e_snr, m_snr, 12330);  /* x 3.0103 in Q12 (10*log10...)*/
    Log2(L_noise,&e_snr,&m_snr);
    L_tmp2 = Mpy_32_16(e_snr, m_snr, 12330);  /* x 3.0103 in Q12 (10*log10...)*/
    L_snr = L_add(L_snr, L_sub(L_tmp1,L_tmp2));

  }


  /*fac = ((float)(10*nseg))/(float)n;*/
  norm_n = norm_s(n);
  n = shl(n,norm_n);
  norm_nseg = sub(norm_s(nseg),1);
  nseg = shl(nseg,norm_nseg);
  scl = div_s(nseg,n);
  L_snr = L_shl(L_snr,add(sub(norm_n,norm_nseg),11));
  m_snr = mult(round(L_snr),scl);

  test();
  if (sub(m_snr, -25348) < 0)   /*-99   Q8*/
  {
    m_snr = -25348;      move16();
  }

  return(m_snr);
}
