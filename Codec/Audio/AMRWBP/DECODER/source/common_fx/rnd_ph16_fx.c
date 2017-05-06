/*------------------------------------------------------------------*
 * function rnd_ph16                                                *
 * ~~~~~~~~~~~~~~~~~                                                *
 * phase random generator (16 phasef, output lg/2 real+imag)         *
 *------------------------------------------------------------------*/

#include "amr_plus_fx.h"


#include "typedef.h"
#include "basic_op.h"
#include "count.h"


void Rnd_ph16(
  Word16 *seed,   /* (i/o) : Seed generator */
  Word16 *xri,    /* (o)   : Fft coeff      */ 
  Word16 lg,      /* (i)   : Length         */
  Word16 Qifft    /* (i)   : Coeff scaling  */
)
{
  Word16 i, scale;
  UWord16 phase;

  scale  = sub(15,Qifft);
  for (i=0; i<lg; i+=2)
  {
    /* random phase from 0 to 15 */
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
    phase = shr(Random(seed),12) & 0x000f; logic16();
#elif EVC_AMRWBPLUS
    phase = shr(Random_evc(seed),12) & 0x000f; logic16();
#endif
// end

    xri[i] = shr(Sin20[phase+4],scale);    /* Q9ifft*/
    xri[i+1] = shr(Sin20[phase],scale);    /* Q9ifft*/
  }
  
  return;
}




